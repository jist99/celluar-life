# Cellular Life
An implementation of Particle Life using Cells

# Building
## Supported Platforms
We aim to support the main 3 desktop platforms:
* Windows
* Linux (Precompiled binary for School of Computing Machines included)
* MacOS

## Note for all Platforms
* To compile the program in release mode for optimal performance, use 'make config=release_x64' instead of 'make'

## VSCode Users (all platforms)
*Note* You must have a compiler toolchain installed in addition to vscode.

* Clone the repo
* Open the folder in VSCode
* Run the build task ( CTRL+SHIFT+B or F5 )
* You are good to go

## Windows Users
There are two compiler toolchains available for windows, MinGW-W64 (a free compiler using GCC), and Microsoft Visual Studio
### Using MinGW-W64
* Double click the `build-MinGW-W64.bat` file
* CD into the folder in your terminal
* run `make`
* You are good to go

#### Note on MinGW-64 versions
Make sure you have a modern version of MinGW-W64 (not mingw).
The best place to get it is from the W64devkit from
https://github.com/skeeto/w64devkit/releases
or the version installed with the raylib installer
#### If you have installed raylib from the installer
Make sure you have added the path

`C:\raylib\w64devkit\bin`

To your path environment variable so that the compiler that came with raylib can be found.

DO NOT INSTALL ANOTHER MinGW-W64 from another source such as msys2, you don't need it.

### Microsoft Visual Studio
* Run `build-VisualStudio2022.bat`
* double click the `.sln` file that is generated
* develop your game
* you are good to go

## Linux Users
* CD into the build folder
* run `./premake5 gmake2`
* CD back to the root
* run `make`
* you are good to go

## Note on School of Computing machines
* We have included a precompiled binary to run the application of SoC machines, availible from Soc_LinuxBinaries/cellular-life, simply execute this to run the program
* If you wish to compile on SoC machines: premake will NOT automatically download raylib, you must:
* Download raylib from https://github.com/raysan5/raylib/archive/refs/heads/master.zip
* Unzip this file to build/external
* Then you can follow the usual Linux Users Instructions

## MacOS Users
* CD into the build folder
* run `./premake5.osx gmake2`
* CD back to the root
* run `make`
* you are good to go

## Output files
The built code will be in the bin dir

## Building for other OpenGL targets
If you need to build for a different OpenGL version than the default (OpenGL 3.3) you can specify an OpenGL version in your premake command line. Just modify the bat file or add the following to your command line

### For OpenGL 1.1
`--graphics=opengl11`

### For OpenGL 2.1
`--graphics=opengl21`

### For OpenGL 4.3
`--graphics=opengl43`

### For OpenGLES 2.0
`--graphics=opengles2`

### For OpenGLES 3.0
`--graphics=opengles3`

# License

*We used the raylib quickstart template, it's license is below:*

Copyright (c) 2020-2025 Jeffery Myers

This software is provided "as-is", without any express or implied warranty. In no event 
will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial 
applications, and to alter it and redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not claim that you 
  wrote the original software. If you use this software in a product, an acknowledgment 
  in the product documentation would be appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be misrepresented
  as being the original software.

  3. This notice may not be removed or altered from any source distribution.
