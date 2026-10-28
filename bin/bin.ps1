param(
    [Parameter(Position = 0)]
    [ValidateSet('build', 'executeUT', 'help')]
    [string]$Command,
    [Parameter(Position = 1, ValueFromRemainingArguments = $true)]
    [string[]]$Components,
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug',
    [ValidateSet('x64', 'Win32')]
    [string]$Platform = 'x64'
)

$env:BUILD_HOME = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$Global:componentXMLPath = Join-Path $PSScriptRoot 'component.xml'
$Global:powerShellPluginsPath = Join-Path $PSScriptRoot 'powershellPlugins'
$Global:BuildingCommands = @('generate', 'release')

function LogError {
    param([string]$Text)
    Write-Host "[ERROR] $Text" -ForegroundColor Red
}

function LogWarning {
    param([string]$Text)
    Write-Host "[WARNING] $Text" -ForegroundColor Yellow
}

function LogSuccess {
    param([string]$Text)
    Write-Host $Text -ForegroundColor Green
}

function Get-BuildComponents {
    [xml]$definition = Get-Content -LiteralPath $Global:componentXMLPath -Raw -ErrorAction Stop
    $byId = @{}
    $byName = @{}
    $sourceRoot = [System.IO.Path]::GetFullPath((Join-Path $env:BUILD_HOME 'src')) + '\'
    $result = @(
        foreach ($component in $definition.components.component) {
            $name = [string]$component.name
            $id = [string]$component.id
            if ($name -notmatch '^[A-Za-z][A-Za-z0-9_]*$' -or $name -in @('generate', 'release', 'all', 'ALL_BUILD', 'ZERO_CHECK', 'RUN_TESTS', 'INSTALL', 'PACKAGE', 'test')) {
                throw "Invalid or reserved component name '$name'."
            }
            if (-not $id -or $byId.ContainsKey($id) -or $byName.ContainsKey($name)) {
                throw "Component IDs and names must be nonempty and unique: '$name' ($id)."
            }
            if ($component.template -notin @('application', 'library')) {
                throw "Unknown template '$($component.template)' for '$name'."
            }
            if (-not $component.rootdir) { throw "Missing rootdir for '$name'." }
            $root = [System.IO.Path]::GetFullPath((Join-Path $sourceRoot ([string]$component.rootdir)))
            if (-not $root.StartsWith($sourceRoot, [System.StringComparison]::OrdinalIgnoreCase) -or -not (Test-Path -LiteralPath $root -PathType Container)) {
                throw "Component '$name' must have an existing rootdir under src."
            }
            $entry = [pscustomobject]@{
                id = $id
                name = $name
                template = [string]$component.template
                root = $root.Replace('\', '/')
                dependencies = @($component.dependencies.dependency | Where-Object { $null -ne $_ } | ForEach-Object { [string]$_.id })
            }
            $byId[$id] = $entry
            $byName[$name] = $entry
            $entry
        }
    )
    if (-not $result.Count) { throw 'component.xml contains no components.' }
    foreach ($entry in $result) {
        $entry.dependencies = @(
            foreach ($dependencyId in $entry.dependencies) {
                if (-not $byId.ContainsKey($dependencyId)) {
                    throw "Unknown dependency ID '$dependencyId' in '$($entry.name)'."
                }
                $dependency = $byId[$dependencyId]
                if ($dependency.template -ne 'library') {
                    throw "'$($entry.name)' cannot link application '$($dependency.name)'."
                }
                $dependency.name
            }
        )
    }
    $visiting = @{}
    $visited = @{}
    $visit = {
        param($entry)
        if ($visiting[$entry.name]) { throw "Dependency cycle involving '$($entry.name)'." }
        if ($visited[$entry.name]) { return }
        $visiting[$entry.name] = $true
        foreach ($dependencyName in $entry.dependencies) { & $visit $byName[$dependencyName] }
        $visiting.Remove($entry.name)
        $visited[$entry.name] = $true
    }
    foreach ($entry in $result) { & $visit $entry }
    return $result
}

function Get-CMakePath {
    $cmake = Get-Command cmake.exe -ErrorAction SilentlyContinue
    if ($cmake) { return $cmake.Source }
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (Test-Path -LiteralPath $vswhere) {
        $installation = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        if ($installation) {
            $bundled = Join-Path $installation 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
            if (Test-Path -LiteralPath $bundled) { return $bundled }
        }
    }
    throw 'CMake 3.21+ is required. Install CMake or the Visual Studio C++ CMake tools.'
}

function Invoke-BuildTool {
    param([string]$Executable, [string[]]$Arguments)
    $tool = Get-Command $Executable -CommandType Application -ErrorAction Stop
    $previousErrorActionPreference = $ErrorActionPreference
    $PSNativeCommandUseErrorActionPreference = $false
    try {
        $ErrorActionPreference = 'Continue'
        & $tool.Source @Arguments 2>&1 | ForEach-Object {
            $line = [string]$_
            if ($line -match '\b(?:fatal\s+)?error(?:\s+[A-Z]+\d+)?\s*:|\b(?:CMake|MSBUILD)\s+Error\b|^\s*(?:\d+>)?\s*(?:fatal\s+)?error\b|^\s*Build FAILED\.') {
                LogError $line
            }
            elseif ($line -match '\bwarning(?:\s+[A-Z]+\d+)?\s*:|\bCMake Warning\b|^\s*(?:\d+>)?\s*warning\b') {
                LogWarning $line
            }
            else {
                Write-Host $line
            }
        }
    }
    finally {
        $ErrorActionPreference = $previousErrorActionPreference
    }
    if ($LASTEXITCODE -ne 0) {
        throw "'$Executable' failed with exit code $LASTEXITCODE."
    }
}

function Invoke-TestTool {
    param([string]$Executable, [string[]]$Arguments)
    $tool = Get-Command $Executable -CommandType Application -ErrorAction Stop
    $previousErrorActionPreference = $ErrorActionPreference
    $PSNativeCommandUseErrorActionPreference = $false
    try {
        $ErrorActionPreference = 'Continue'
        & $tool.Source @Arguments 2>&1 | ForEach-Object {
            $line = [string]$_
            if ($line -match '^(?<prefix>\s*(?:\d+:\s*)?\[\s*Summary\s*\]\s*)(?<passed>\d+) passed, (?<failed>\d+) failed\s*$') {
                Write-Host $matches.prefix -NoNewline
                Write-Host "$($matches.passed) passed" -ForegroundColor Green -NoNewline
                Write-Host ', ' -NoNewline
                if ([int]$matches.failed -gt 0) {
                    Write-Host "$($matches.failed) failed" -ForegroundColor Red
                }
                else {
                    Write-Host "$($matches.failed) failed"
                }
            }
            elseif ($line -match '^\s*\d+% tests passed, (?<failed>\d+) tests failed out of (?<total>\d+)\s*$') {
                $failed = [int]$matches.failed
                $total = [int]$matches.total
                Write-Host 'CTest summary: ' -NoNewline
                Write-Host "$($total - $failed) passed" -ForegroundColor Green -NoNewline
                Write-Host ', ' -NoNewline
                if ($failed -gt 0) {
                    Write-Host "$failed failed" -ForegroundColor Red -NoNewline
                }
                else {
                    Write-Host "$failed failed" -NoNewline
                }
                Write-Host " out of $total tests"
            }
            elseif ($line -match '^\s*(?:\d+:\s*)?\[\s*OK\s*\]' -or $line -match '^\s*\d+/\d+\s+Test\s+#\d+:.*\bPassed\b') {
                LogSuccess $line
            }
            elseif ($line -match '^\s*(?:\d+:\s*)?\[\s*FAILED\s*\]' -or $line -match '^\s*\d+/\d+\s+Test\s+#\d+:.*(?:\*\*\*|Not Run)' -or $line -match '^\s*(?:The following tests FAILED:|Errors while running CTest|No tests were found)' -or $line -match '^\s*\d+\s+-\s+.*\((?:Failed|Not Run|Timeout|SEGFAULT)[^)]*\)\s*$') {
                LogError $line
            }
            else {
                Write-Host $line
            }
        }
    }
    finally {
        $ErrorActionPreference = $previousErrorActionPreference
    }
    if ($LASTEXITCODE -ne 0) {
        throw "Unit tests failed with exit code $LASTEXITCODE."
    }
}

function New-BuildProjects {
    param([object[]]$Definitions, [string]$Platform)
    $output = Join-Path $env:BUILD_HOME "out\vs-$Platform"
    $null = New-Item -ItemType Directory -Path $output -Force -ErrorAction Stop
    $manifest = Join-Path $output 'components.json'
    $json = ConvertTo-Json -InputObject @($Definitions) -Depth 5
    [System.IO.File]::WriteAllText($manifest, $json, (New-Object System.Text.UTF8Encoding($false)))
    foreach ($component in $Definitions) {
        $templatePath = Join-Path $PSScriptRoot "templates\$($component.template).CMakeLists.txt"
        $template = Get-Content -LiteralPath $templatePath -Raw -ErrorAction Stop
        $content = $template.Replace('@COMPONENT_NAME@', $component.name).Replace('@COMPONENT_ROOT@', $component.root)
        $componentOutput = Join-Path $output "cmake\$($component.name)"
        $null = New-Item -ItemType Directory -Path $componentOutput -Force -ErrorAction Stop
        [System.IO.File]::WriteAllText((Join-Path $componentOutput 'CMakeLists.txt'), $content, (New-Object System.Text.UTF8Encoding($false)))
    }
    Invoke-BuildTool (Get-CMakePath) @('-S', $env:BUILD_HOME, '-B', $output, '-G', 'Visual Studio 17 2022', '-A', $Platform, "-DCOMPONENT_MANIFEST=$manifest")
    return $output
}

function setup {
    $null = Get-BuildComponents
    foreach ($commandName in @('build', 'executeUT')) {
        Register-ArgumentCompleter -CommandName $commandName -ParameterName Components -ScriptBlock {
            param($commandName, $parameterName, $wordToComplete, $commandAst, $fakeBoundParameters)
            $names = @((Get-BuildComponents).name)
            if ($commandName -eq 'build') { $names += $Global:BuildingCommands }
            $names | Where-Object { $_ -like "$wordToComplete*" } | ForEach-Object {
                [System.Management.Automation.CompletionResult]::new($_, $_, 'ParameterValue', $_)
            }
        }
    }
}

<#
.SYNOPSIS
Displaying help for commands
.PARAMETER -original
Optional to get powershell help
#>
function help {
    param (
        [Parameter(ValueFromRemainingArguments = $true)]
        [string[]]$Args
    )

    if ($Args.Count -gt 0 -and $Args[0] -eq '-original') {
        Write-Host "Original help command"
        $filteredArgs = $Args | Where-Object { $_ -ne '-original' }
        Write-Host "$filteredArgs"

        if ($filteredArgs.Count -eq 0) {
            # Show default help summary
            Get-Help
        } else {
            # Show help for specific command
            Get-Help @filteredArgs
        }
    } else {
        CustomHelp
    }
}

function CustomHelp {
    $callerScript = $PSCommandPath
    $scriptContent = Get-Content $callerScript -Raw

    # Match all functions with comment-based help
    $helpBlocks = [regex]::Matches($scriptContent, '(?ms)^<#[\r\n]+(.*?)[\r\n]+#>\s*^function\s+(\w+)\s*\{')

    foreach ($match in $helpBlocks) {
        $helpBlock = $match.Groups[1].Value -split "`n"
        $funcName = $match.Groups[2].Value

        $synopsis = $helpBlock | Where-Object { $_ -match '\.SYNOPSIS' } | ForEach-Object {
            $index = $helpBlock.IndexOf($_)
            if ($index -ge 0 -and ($index + 1) -lt $helpBlock.Count) {
                $helpBlock[$index + 1].Trim()
            }
        }
        
        # Extract .PARAMETER entries
        $parameters = @{}
        for ($i = 0; $i -lt $helpBlock.Count; $i++) {
            if ($helpBlock[$i] -match '\.PARAMETER\s+(\S+)') {
                $paramName = $matches[1]
                $description = ""
                if ($i + 1 -lt $helpBlock.Count) {
                    $description = $helpBlock[$i + 1].Trim()
                }
                $parameters[$paramName] = $description
            }
        }

        # Build output
        $paramList = ($parameters.Keys | ForEach-Object { "<$_>" }) -join ' '
        Write-Host "$funcName $paramList" -ForegroundColor White -BackgroundColor DarkBlue
        $message = "    $synopsis"

        foreach ($param in $parameters.Keys) {
            $message += "`n    [$param] : $($parameters[$param])"
        }

        Write-Host "$message`n"

    }
}

<#
.SYNOPSIS
Build a component or project
.PARAMETER -Components
The components to build.
.PARAMETER -Configuration
The build configuration (Debug or Release).
.PARAMETER -Platform
The target platform (x64 or Win32).
#>
function build {
    param(
        [Parameter(Mandatory = $true, Position = 0, ValueFromRemainingArguments = $true)]
        [string[]]$Components,
        [ValidateSet('Debug', 'Release')]
        [string]$Configuration = 'Debug',
        [ValidateSet('x64', 'Win32')]
        [string]$Platform = 'x64'
    )
    $definitions = @(Get-BuildComponents)
    $targets = @($Components)
    if ($targets[0] -eq 'generate') {
        if ($targets.Count -ne 1) { throw 'Use build generate without component names.' }
        $null = New-BuildProjects $definitions $Platform
        LogSuccess "Generated out/vs-$Platform/GameEngine.sln"
        return
    }
    if ($targets[0] -eq 'release') {
        $Configuration = 'Release'
        $targets = @($targets | Select-Object -Skip 1)
        if (-not $targets.Count) { $targets = @($definitions.name) }
    }
    $targets = @(
        foreach ($name in $targets) {
            $match = $definitions | Where-Object { $_.name -eq $name }
            if (-not $match) { throw "Unknown component '$name'. Available: $($definitions.name -join ', ')." }
            $match.name
        }
    )
    $output = New-BuildProjects $definitions $Platform
    Invoke-BuildTool (Get-CMakePath) (@('--build', $output, '--config', $Configuration, '--parallel', '--target') + $targets)
    LogSuccess "Built $($targets -join ', ') ($Configuration/$Platform)."
    foreach ($name in ($targets | Select-Object -Unique)) {
        $component = $definitions | Where-Object { $_.name -eq $name }
        if ($component.template -ne 'application') { continue }
        $executable = Join-Path $output "bin\$Configuration\$name.exe"
        if (-not (Test-Path -LiteralPath $executable -PathType Leaf)) {
            throw "Built application executable not found: '$executable'."
        }
        Start-Process -FilePath $executable -WorkingDirectory (Join-Path $env:BUILD_HOME 'src') -ErrorAction Stop | Out-Null
    }
}

<#
.SYNOPSIS
Execute unit tests for specified components
.PARAMETER -Components
The components to test.
.PARAMETER -Configuration
The build configuration (Debug or Release).
.PARAMETER -Platform
The target platform (x64 or Win32).
#>
function executeUT {
    param(
        [Parameter(Mandatory = $true, Position = 0, ValueFromRemainingArguments = $true)]
        [string[]]$Components,
        [ValidateSet('Debug', 'Release')]
        [string]$Configuration = 'Debug',
        [ValidateSet('x64', 'Win32')]
        [string]$Platform = 'x64'
    )
    $definitions = @(Get-BuildComponents)
    $testComponents = @(
        foreach ($name in $Components) {
            $match = $definitions | Where-Object { $_.name -eq $name }
            if (-not $match) { throw "Unknown component '$name'." }
            $testRoot = Join-Path $match.root 'test'
            if (-not (Test-Path -LiteralPath $testRoot) -or -not @(Get-ChildItem -LiteralPath $testRoot -Filter '*.cpp' -File).Count) {
                throw "No tests found for '$name' in '$testRoot'."
            }
            $match.name
        }
    )
    $cmake = Get-CMakePath
    $ctest = Join-Path (Split-Path $cmake) 'ctest.exe'
    $output = New-BuildProjects $definitions $Platform
    $targets = @($testComponents | ForEach-Object { "${_}_tests" })
    Invoke-BuildTool $cmake (@('--build', $output, '--config', $Configuration, '--parallel', '--target') + $targets)
    $pattern = '^(' + (($testComponents | ForEach-Object { [regex]::Escape($_) }) -join '|') + ')\.'
    Invoke-TestTool $ctest @('--test-dir', $output, '-C', $Configuration, '--verbose', '--no-tests=error', '-R', $pattern)
    LogSuccess "Tests passed for $($testComponents -join ', ') ($Configuration/$Platform)."
}

try {
    setup
    if ($Command) {
        if ($Command -eq 'help') { help }
        else { & $Command -Components $Components -Configuration $Configuration -Platform $Platform }
    }
    elseif ($MyInvocation.InvocationName -ne '.') {
        Set-Location -LiteralPath $env:BUILD_HOME
        help
    }
}
catch {
    if ($Command) {
        LogError $_.Exception.Message
        exit 1
    }
    throw
}