# Set up #

Create a working directory. Checkout OpenMalaria into a sub-folder (see the [Source](http://code.google.com/p/openmalaria/source/checkout) tab). Perhaps use TortoiseSvn for this...

# Dependencies #
## Tools ##
Building OpenMalaria requires the following tools:
  * A build system:
    * Visual Studio (at least version 8.0 (2005), 2012 Express under testing, both 32-bit and 64-bit work)
    * Code::Blocks, along with some compiler (recommended:  [install the version with mingw](http://www.codeblocks.org/downloads/5#windows)) (not actually tested, but should work if you can build/get libraries)
    * Maybe Cygwin (if you know what you are doing)
  * CMake (oldest supported is 2.4) - use the installer from [here](http://www.cmake.org/cmake/resources/software.html).
  * Python 2.x (not 3.x) - download from [here](http://www.python.org).
  * Git - download the basic version at [git-scm](http://git-scm.com/) or download a GUI (e.g. SourceTree)

### Common steps ###

Create a directory for project sources, e.g. `om` in your Documents folder or `C:\develop`. This is where you should clone repositories to with Git.

You probably also need an install directory, e.g. `C:\install`. This is because the CMake install step (run from Visual Studio for some libraries) cannot write to `C:\Program Files`.

To configure a project with CMake:
  1. open the GUI
  1. select the source directory (where the first CMakeLists.txt file is found)
  1. select a build directory (this can be anywhere; usually I create one called 'build' inside the source directory)
  1. click configure (first time you do this you must choose a compiler)
  1. adjust configuration as necessary, then click configure again
  1. click generate
  1. open the build directory, then open the PROJECT.sln file in Visual Studio (or the project file for Code Blocks if using that, or run make)

Building in Visual Studio:
  1. choose the build type (`Release` for an optimised build, `Debug` if you need to do debugging, or perhaps `Static Release`)
  1. select the target (e.g. `ALL_BUILD`) and build
  1. you may wish to run other targets, e.g. "build" the `RUN_TESTS` or `INSTALL` targets

Note: often with MSVC code compiled in debug mode can only be used with libraries built in debug mode, and maybe even building a static binary requires libraries to be built as static. So if you want to change compile modes later you may have to recompile all libraries (though usually the different builds can co-exist once built).

## Libraries ##

### zlib ###

Option 1: download [a fork of zlib with CMake config files](https://github.com/commontk/zlib), configure with CMake (change the install directory to somewhere writable, e.g. C:\install\ZLIB), build with Visual Studio, then install (choose the `INSTALL` target in Visual Studio and click build).

Option 2: get the DLL download from [www.zlib.net here], extract to the working directory above the OpenMalaria source code and rename to "zlib".

### GSL ###

Option 1: download [a fork of GSL with CMake config files](https://github.com/ampl/gsl), then configure and build as above for z-lib. Then go [thank vitaut](http://stackoverflow.com/a/14467869/314345).

Option 2: follow the instructions below, search around for a compatible build, or use Cygwin...

Get it from [here](ftp://sunsite.cnlab-switch.ch/mirror/gnu/gsl) (oldest supported is 1.13); extract to your working directory above the OpenMalaria code and rename to "gsl". This should put the sources within WORKSPACE/gsl/gsl/.

Now build the libraries: open the gsl/gsl/gsl.sln file select the "Release MT"/"Debug MT" version you want to use (you'll have to stick with one), and build. Now find the libraries: they should be under gsl/gsl/win32/lib/. Copy/move the lib folder up two directories (so you have WORKSPACE/gsl/lib), go into it and rename the files: remove the "_mt" (release) or "_mtd" (debug) suffix (so you have gs.lib, cblas.lib, etc.). (If you want to change between debug and release later, replace this set of files with the other generated version.)

That should be it. It's a pain, yes, but (a) pre-built libraries are likely to cause you problems and (b) it's better to explicitly choose to use the release or debug libraries since libraries generated in release mode and ones generated in debug mode aren't necessarily compatible with one another (with MSVC).

### XSD and Xerces-C ###

Run the .msi installer from [here](http://codesynthesis.com/products/xsd/download.xhtml).

Xerces-C versions 2.8 and 3.0+ should both work; 3.x is included with the XSD installer for windows. This ought to work if doing a 32-bit build, but it seems no 64-bit build is included.

If the above doesn't work, try building xerces yourself. Download the [xerces-c source code](http://xerces.apache.org/xerces-c/download.cgi), extract, then open `xerces-c-3.x.x\projects\Win32\VC10\xerces-all\xerces-all.sln` and build. When configuring OpenMalaria, set `XERCES_INCLUDE_DIRS` to `xerces-c-3.x.x\src` and `XERCES_LIB_XXX` to `xerces-c-3.x.x/Build/Win64/VC10/XXX/xerces-c_XXX.lib`.

Note: if not using a static xerces library, you need to copy `xerces-c_x_x.dll` to the same directory as openMalaria.exe before running OM.

> Here we assume you have installed Xerces along with XSD (the default option when installing XSD. Find the xerces-all.sln file in you Xerces install directory (the default location will be something like C:\Program Files\CodeSynthesis XSD 3.2\xerces-c-3.0.1\projects\Win32\VC8\xerces-all for the VS version 8 solution file), and open it with VS. Choose the "Release" build configuration. Build the XercesLib build target. This will create a file C:\Program Files\CodeSynthesis XSD 3.2\xerces-c-3.0.1\Build\Win32\VC8\Static Release\xerces-c\_3.lib (Optional: Build using the "Static Release" target to creat a xerces-c\_static\_3.lib library, then follow the instructions below to create OpenMalaria with Xerces statically linked).

### Boost ###

Just download the source (you don't need to compile anything, just extract and set `Boost_INCLUDE_DIR` in the OpenMalaria CMake config to the base directory).

I don't know which is the oldest compatible version; I know 1.41.0 and most newer versions work. Newer versions may need to have their version number added in openmalaria/CMakeLists.txt (search for Boost\_ADDITIONAL\_VERSIONS).


# Build #

Clone OpenMalaria with git, then follow the CMake instructions [above](WindowsBuildOpenMalaria#Common_steps.md). You _will_ need to tweak the configuration to point out various libraries and headers.

Build in Visual Studio. Fix bugs in the source (most testing happens on Linux, so inevitably a few little fixes are needed).

Run the tests. If any fail, check the log (probably build/Testing/Temporary/LastTest.log). Try to work out whether failed tests are due to on-going development work or platform differences (usually these are very minor).

You can probably stop reading here; the text below may be outdated.

## Compiling with Code::Blocks or Visual Studio ##

Navigate to your build directory and open "OpenMalaria.cbp" or "OpenMalaria.sln". Build like normal.

Note: if you get told that the project has been modified externally, you must reload it. Any time you click build and any of the CMakeLists.txt files have changed, cmake will regenerate the project file (so any custom settings will be lost). If you want to change any cmake settings, run cmake-gui; if you just want to rebuild the project file, you could do this or you could build the "rebuild\_cache" build-target (in Code::Blocks anyway).

You can select the Debug/Release/... build mode, but it seems you have to rebuild when changing this (in Visual Studio).

If on Visual Studio 2005 (and maybe others), when inside VS5, open the Properties of the project OpenMalaria, Check Configuration Properties/Linker/Input. If Ignore Specific Library has LIBCMT, replace with MSVCRT. If you don't do this change, OpenMalaria will compile but not run.


# Compiling without a .net dependency and with Xerces statically linked #
(Ignore this section unless you **know** you want to do this. It's mostly notes to make a BOINC release version with minimal dependencies.)

Please remember that you have to use the xerces static library, which is not given precompiled in the xsd install.

Further explanations in chapter [Dependencies](Dependencies.md).

TODO: Add XERCES\_STATIC\_LIBRARY to OpenMalaria compile targets.

In the cmake configuration for OpenMalaria:
  * set OM\_USE\_LIBCMT to true
  * edit `CMAKE_CXX_FLAGS_*` (whichever build mode you're interested in): change "/MD" to "/MT" (or "/MDd" to "/MTd")
  * edit `CMAKE_CXX_FLAGS`: add "/D XERCES\_STATIC\_LIBRARY"

# Running Simulations #

If you want to run the tests,
  * Visual Studio: build the "RUN\_TESTS" target
  * Code::Blocks: change the build-target to "test" and click build. Unfortunately you can't cancel the test, and need to change the build-target back to "all" (or whichever you want) afterwards.
  * From a command-line (should work whichever build-tool you use): navigate to the build directory, and run "ctest". You can also just run some tests like this, e.g. "ctest -I 5,6".

If you want to run individual simulations using your own scenario.xml file then use the command line from build directory or create a .bat file in the build directory.


# Troubleshooting #

## '_read_ : is not a member of 'std::basic\_istream<_Elem,_Traits>' ##

Update BOINC (this is caused by a wierd macro in BOINC).

## openssl dependency ##

Checkout http://boinc.berkeley.edu/svn/trunk/boinc_depends_win_vs2005 into the same directory as boinc is in.