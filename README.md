Project on Visual Studio 2022.

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


For now we are loading a world from a file and displaying it in a window.
Also the camera is managed and can be controlled by keyboard/mouse inputs.



Starting code is in Viewer/Viewer/main.cpp



WHAT's new :

World serialization and deserialization.



TO BE DONE :

* The first new executable project will be the world editor : displaying world, can add items from assets.
* Add an item drop down button. It will appear with a right click on the window, it will be able to create any item in the asset manager at a position.
* Add bounding rect Object that can be linked to an object and modify its position and size.
* Left click will make appear a bounding rect linked to the item clicked on.
* Far later : should be able to modify object properties ( animated frames, button callback, text label)
