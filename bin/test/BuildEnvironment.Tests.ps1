param([switch]$Integration)

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot '..\bin.ps1')

function Assert-True {
    param([bool]$Condition, [string]$Message)
    if (-not $Condition) { throw $Message }
}

function Assert-Throws {
    param([scriptblock]$Action, [string]$Pattern)
    $failure = $null
    try { & $Action | Out-Null }
    catch { $failure = $_.Exception.Message }
    Assert-True ($null -ne $failure -and $failure -like $Pattern) "Expected '$Pattern', received '$failure'."
}

$originalRoot = $env:BUILD_HOME
$originalXml = $Global:componentXMLPath
$nativePowerShell = (Get-Process -Id $PID).Path
$loggingOutput = @(Invoke-BuildTool $nativePowerShell @('-NoProfile', '-Command', @'
[Console]::Out.WriteLine('Compiling Core.cpp')
[Console]::Out.WriteLine('Core.cpp(12): warning C4100: unused parameter')
[Console]::Error.WriteLine('CMake Warning at CMakeLists.txt:10 (message):')
[Console]::Out.WriteLine('    0 Error(s)')
[Console]::Out.WriteLine('    2 Warning(s)')
exit 0
'@) 6>&1)
Assert-True ($loggingOutput.Count -eq 5) 'All stdout and stderr lines must be logged exactly once.'
Assert-True (@($loggingOutput | Where-Object { $_.MessageData.Message -like '[[]WARNING[]]*' }).Count -eq 2) 'Compiler and CMake warnings must use LogWarning.'
$defaultHostRecord = & { Write-Host 'Default color' } 6>&1
foreach ($message in @('Compiling Core.cpp', '    0 Error(s)', '    2 Warning(s)')) {
    $record = $loggingOutput | Where-Object { $_.MessageData.Message -eq $message }
    Assert-True ($null -ne $record -and $record.MessageData.ForegroundColor -eq $defaultHostRecord.MessageData.ForegroundColor) 'Ordinary output and summaries must use the default color.'
}
$script:loggingFailure = $null
$loggingOutput = @(& {
    try {
        Invoke-BuildTool $nativePowerShell @('-NoProfile', '-Command', @'
[Console]::Error.WriteLine('Core.cpp(15): error C2143: syntax error')
[Console]::Out.WriteLine('LINK : fatal error LNK1120: unresolved externals')
[Console]::Error.WriteLine('CMake Error at CMakeLists.txt:20 (message):')
exit 7
'@)
    }
    catch { $script:loggingFailure = $_.Exception.Message }
} 6>&1)
Assert-True ($loggingFailure -like '*failed with exit code 7*') 'Native failures must retain their exit code.'
Assert-True ($loggingOutput.Count -eq 3) 'All error lines must be logged before throwing.'
Assert-True (@($loggingOutput | Where-Object { $_.MessageData.Message -like '[[]ERROR[]]*' }).Count -eq 3) 'Compiler, linker and CMake errors must use LogError.'
Assert-True ($ErrorActionPreference -eq 'Stop') 'Native logging must restore the caller error preference.'
Assert-Throws { Invoke-BuildTool 'missing-build-tool-fixture.exe' @() } '*missing-build-tool-fixture.exe*'
if ($Integration) {
    $integrationCMake = Get-CMakePath
    $invokeNative = ${function:Invoke-BuildTool}
}
$fixtureRoot = Join-Path ([System.IO.Path]::GetTempPath()) ('engine build tests ' + [guid]::NewGuid())
try {
    $null = New-Item -ItemType Directory -Path (Join-Path $fixtureRoot 'src\Core') -Force
    $null = New-Item -ItemType Directory -Path (Join-Path $fixtureRoot 'src\App') -Force
    $env:BUILD_HOME = $fixtureRoot
    $Global:componentXMLPath = Join-Path $fixtureRoot 'component.xml'
    $fixture = @'
<components>
  <component id="1" name="Core" rootdir="Core" template="library"><dependencies /></component>
  <component id="2" name="App" rootdir="App" template="application"><dependencies><dependency id="1" /></dependencies></component>
</components>
'@
    function Reset-Fixture {
        [xml]$xml = $fixture
        $xml.Save($Global:componentXMLPath)
    }
    Reset-Fixture
    $definitions = @(Get-BuildComponents)
    Assert-True ($definitions.Count -eq 2) 'Expected two components.'
    Assert-True ($definitions[0].dependencies.Count -eq 0) 'Empty dependencies must remain an empty array.'
    Assert-True ($definitions[1].dependencies[0] -eq 'Core') 'Dependency ID was not resolved to its target name.'

    [xml]$xml = $fixture
    $xml.components.component[1].dependencies.dependency.id = '99'
    $xml.Save($Global:componentXMLPath)
    Assert-Throws { Get-BuildComponents } '*Unknown dependency ID*'

    [xml]$xml = $fixture
    $xml.components.component[1].id = '1'
    $xml.Save($Global:componentXMLPath)
    Assert-Throws { Get-BuildComponents } '*unique*'

    [xml]$xml = $fixture
    $xml.components.component[1].rootdir = '..\escape'
    $xml.Save($Global:componentXMLPath)
    Assert-Throws { Get-BuildComponents } '*under src*'

    [xml]$xml = $fixture
    $xml.components.component[1].template = 'invalid'
    $xml.Save($Global:componentXMLPath)
    Assert-Throws { Get-BuildComponents } '*Unknown template*'

    [xml]$xml = $fixture
    $xml.components.component[1].template = 'library'
    $dependency = $xml.CreateElement('dependency')
    $dependency.SetAttribute('id', '2')
    $null = $xml.components.component[0].SelectSingleNode('dependencies').AppendChild($dependency)
    $xml.Save($Global:componentXMLPath)
    Assert-Throws { Get-BuildComponents } '*Dependency cycle*'

    Reset-Fixture
    Assert-Throws { build Missing } '*Unknown component*'
    Assert-Throws { executeUT Core } '*No tests found*'
    Assert-Throws { build generate Core } '*without component names*'

    $script:toolCalls = New-Object System.Collections.Generic.List[object]
    $script:applicationLaunches = New-Object System.Collections.Generic.List[object]
    $script:failBuild = $false
    function Get-CMakePath { return 'C:\mock tools\cmake.exe' }
    function Invoke-BuildTool {
        param([string]$Executable, [string[]]$Arguments)
        $script:toolCalls.Add([pscustomobject]@{ Executable = $Executable; Arguments = $Arguments })
        if ($script:failBuild -and $Arguments[0] -eq '--build') { throw 'Fixture build failed.' }
    }
    function Start-Process {
        param([string]$FilePath, [string]$WorkingDirectory, [string]$ErrorAction)
        $script:applicationLaunches.Add([pscustomobject]@{ FilePath = $FilePath; WorkingDirectory = $WorkingDirectory })
    }
    foreach ($configurationName in @('Debug', 'Release')) {
        $applicationOutput = Join-Path $fixtureRoot "out\vs-x64\bin\$configurationName"
        $null = New-Item -ItemType Directory -Path $applicationOutput -Force
        foreach ($applicationName in @('App', 'Tool')) {
            [System.IO.File]::WriteAllText((Join-Path $applicationOutput "$applicationName.exe"), '')
        }
    }
    build core App -Configuration Release
    Assert-True ($toolCalls.Count -eq 2) 'Expected generation and build calls.'
    $buildArguments = $toolCalls[1].Arguments
    Assert-True (($buildArguments -join '|') -like '*--config|Release|--parallel|--target|Core|App') 'Build arguments lost configuration or canonical component names.'
    Assert-True ($applicationLaunches.Count -eq 1) 'Only applications should launch after a successful build.'
    Assert-True ($applicationLaunches[0].FilePath -eq (Join-Path $fixtureRoot 'out\vs-x64\bin\Release\App.exe')) 'Must launch the selected build configuration.'
    Assert-True ($applicationLaunches[0].WorkingDirectory -eq (Join-Path $fixtureRoot 'src')) 'Applications must run from src for asset paths.'
    $manifestPath = Join-Path $fixtureRoot 'out\vs-x64\components.json'
    $manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
    Assert-True ($manifest[0].dependencies -is [array] -and $manifest[0].dependencies.Count -eq 0) 'JSON must preserve empty dependency arrays.'
    Assert-True ($manifest[1].dependencies[0] -eq 'Core') 'JSON must contain resolved dependencies.'

    $applicationLaunches.Clear()
    $script:failBuild = $true
    Assert-Throws { build App } '*Fixture build failed*'
    $script:failBuild = $false
    Assert-True ($applicationLaunches.Count -eq 0) 'A failed build must never launch an executable, even when an older one exists.'
    build Core
    build generate
    Assert-True ($applicationLaunches.Count -eq 0) 'Library builds and generation must never launch applications.'

    $toolCalls.Clear()
    build release
    Assert-True (($toolCalls[1].Arguments -join '|') -like '*--config|Release|--parallel|--target|Core|App') 'Release shorthand must build all components.'
    Assert-True ($applicationLaunches.Count -eq 1) 'Release shorthand must launch the built application.'

    $null = New-Item -ItemType Directory -Path (Join-Path $fixtureRoot 'src\Core\test')
    [System.IO.File]::WriteAllText((Join-Path $fixtureRoot 'src\Core\test\Example.cpp'), 'int main() { return 0; }')
    $toolCalls.Clear()
    $applicationLaunches.Clear()
    executeUT Core
    Assert-True ($toolCalls.Count -eq 3) 'Expected generation, test build, and CTest calls.'
    Assert-True ($toolCalls[1].Arguments[-1] -eq 'Core_tests') 'Must build only selected test targets.'
    Assert-True ($toolCalls[2].Executable -like '*ctest.exe') 'Must execute CTest.'
    Assert-True ($toolCalls[2].Arguments[-1] -eq '^(Core)\.') 'CTest must select only the requested component.'
    Assert-True ($applicationLaunches.Count -eq 0) 'Unit tests must not launch applications.'

    [xml]$xml = $fixture
    $secondApplication = $xml.components.component[1].CloneNode($true)
    $secondApplication.id = '3'
    $secondApplication.name = 'Tool'
    $secondApplication.rootdir = 'Tool'
    $null = $xml.components.AppendChild($secondApplication)
    $xml.Save($Global:componentXMLPath)
    $null = New-Item -ItemType Directory -Path (Join-Path $fixtureRoot 'src\Tool')
    $toolCalls.Clear()
    build App Tool
    Assert-True (($toolCalls[1].Arguments -join '|') -like '*--target|App|Tool') 'Each application must be independently selectable.'
    Assert-True ($applicationLaunches.Count -eq 2) 'Each requested application must launch after a successful build.'
    Assert-True ($applicationLaunches[0].FilePath -like '*\bin\Debug\App.exe' -and $applicationLaunches[1].FilePath -like '*\bin\Debug\Tool.exe') 'Must launch the matching Debug executables.'
    foreach ($applicationName in @('App', 'Tool')) {
        $generated = Get-Content -LiteralPath (Join-Path $fixtureRoot "out\vs-x64\cmake\$applicationName\CMakeLists.txt") -Raw
        Assert-True ($generated.Contains("add_executable($applicationName ")) "Missing executable definition for $applicationName."
        Assert-True (-not $generated.Contains('@COMPONENT_')) 'Template placeholders must be replaced.'
    }
    $library = Get-Content -LiteralPath (Join-Path $fixtureRoot 'out\vs-x64\cmake\Core\CMakeLists.txt') -Raw
    Assert-True ($library.Contains('add_library(Core STATIC ')) 'The library template must still produce a static library.'

    if ($Integration) {
        [System.IO.File]::WriteAllText((Join-Path $fixtureRoot 'src\Core\Core.cpp'), 'int fixtureValue() { return 42; }')
        foreach ($applicationName in @('App', 'Tool')) {
            [System.IO.File]::WriteAllText((Join-Path $fixtureRoot "src\$applicationName\main.cpp"), 'int fixtureValue(); int main() { return fixtureValue() == 42 ? 0 : 1; }')
        }
        $binaryRoot = Join-Path $fixtureRoot 'out\vs-x64'
        & $invokeNative $integrationCMake @('-S', $originalRoot, '-B', $binaryRoot, '-G', 'Visual Studio 17 2022', '-A', 'x64', "-DCOMPONENT_MANIFEST=$manifestPath")
        & $invokeNative $integrationCMake @('--build', $binaryRoot, '--config', 'Debug', '--parallel', '--target', 'App', 'Tool')
        foreach ($applicationName in @('App', 'Tool')) {
            $executable = Join-Path $binaryRoot "bin\Debug\$applicationName.exe"
            Assert-True (Test-Path -LiteralPath $executable) "Missing executable $applicationName.exe."
            & $invokeNative $executable @()
        }
        Write-Host 'Both generated application executables built and ran successfully.' -ForegroundColor Green
    }

    Write-Host 'Build environment regression checks passed.' -ForegroundColor Green
}
finally {
    $env:BUILD_HOME = $originalRoot
    $Global:componentXMLPath = $originalXml
    if (Test-Path -LiteralPath $fixtureRoot) { Remove-Item -LiteralPath $fixtureRoot -Recurse -Force }
}