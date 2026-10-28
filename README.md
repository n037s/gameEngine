## Build Environment

Install Visual Studio 2022 with **Desktop development with C++**, the Windows SDK,
and CMake 3.21 or newer (on PATH or bundled with Visual Studio). Windows
PowerShell 5.1 is sufficient. No administrator privileges or module installation
are needed.

From the repository root, open the interactive build shell:

```bat
bin\bin.bat
```

### Component Definitions

`bin/component.xml` contains all definition for components. 
Each component holds and id, a name, a rootdir relative to src and a template. 
A template library produce a static library, a template application produce an excutable. 
All components defines their dependencies referencing ids of their dependencies. 

`build generate` creates `out/vs-x64/cmake/<name>/CMakeLists.txt` for every
component using `bin/templates/application.CMakeLists.txt` or
`bin/templates/library.CMakeLists.txt`, according to the XML `template` attribute.
The root `CMakeLists.txt` loads these generated component definitions, and CMake
generates `out/vs-x64/GameEngine.sln`, `.vcxproj`, and `.filters` files.

Executables are placed in `out/vs-x64/bin/Debug` or `bin/Release`; static libraries
are in `out/vs-x64/lib/<configuration>`. SDL DLLs are copied next to executables.
Each application component produces its own `<name>.exe`. Add more application
entries to the XML with their own source directories and library dependencies,
then run `build generate` followed by `build <name>` to build each one. The
generated solution uses the first application in the XML as its startup project.

### Tests

`executeUT` builds only the selected components' test targets, then runs CTest
with failure output and a 60-second timeout per test. Missing tests are errors;
build and test failures return a nonzero exit code from the batch launcher.
The existing ControlManager camera test currently reports an isometric-angle
expectation mismatch; the runner surfaces that failure rather than hiding it.

### Scenario Environment

`UnitTestEnvironment` is a reusable C++17 test component. CMake links it to test
executables only; production components do not depend on it. Put test sources in
`src/<component>/test`, include `UnitTestEnvironment.h`, and register named
scenarios:

```cpp
#include "UnitTestEnvironment.h"

struct CounterFixture : UnitTest::Fixture
{
	int value{0};
	void setUp() override { value = 42; }
	void tearDown() override {}
};

int main()
{
	UnitTest::Environment tests("Counter");
	tests.addScenario<CounterFixture>("Starts at 42",
		[](UnitTest::Scenario& scenario, CounterFixture& fixture) {
			UT_REQUIRE(scenario, fixture.value == 42);
			++fixture.value;
			UT_EXPECT(scenario, fixture.value == 43);
		});
	return tests.run();
}
```

Each fixture-backed scenario gets a fresh fixture. `tearDown` runs even when
`setUp`, the scenario body, or a required assertion throws, provided fixture
construction completed.
`UT_EXPECT` records a failure and continues; `UT_REQUIRE` records a failure and
stops that scenario. Both report the expression, source file, and line.
Unhandled C++ exceptions fail only the current scenario; native crashes are not
recoverable. The suite returns a nonzero exit code if any scenario fails or no
scenarios were registered. Scenario names must be nonempty and unique.

For scenarios without fixtures, use `tests.addScenario("Name", body)` with a
callable accepting `UnitTest::Scenario&`. Register parameter combinations in a
loop with distinct names, as in `ContextualObjectMenuTests.cpp`.

Run the runner self-tests and editor scenarios with:


WHAT's new :

World serialization and deserialization.
WorldManager to move objects, delete them, modify them. 
Left click select the object, right click make appear a contextual menu to modify the object. 
Added an isotropic view available with key "c". 
Objects can be stready or flat. 



TO BE DONE :
* Should be able to modify object properties ( button callback, text label )
* Introduce 3D Objects
* Player should better handle its animation 
* Camera should follow the player
* Player move could be nicer, it could be great to give it a speed and not move each timeout. 
