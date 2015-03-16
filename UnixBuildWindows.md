# Introduction #

This is a as-I-work-out how to do it documentation of my progress in cross-compiling OpenMalaria.


# Details #

## Set-up ##

Obviously you can't use linux libraries here.. I'm not entirely clear whether MSVC-compiled libs will work (there's been some conflicting information), but I'm going for compiling most bits myself with mingw.

  1. Install [mingw](http://packages.debian.org/search?suite=default&section=all&arch=any&searchon=names&keywords=mingw32)
  1. Create a working dir, with a copy of OpenMalaria sources. Mine looks something like:
```
  # ls -1 i586-mingw32msvc/
  boinc/
  download/
  gsl-1.12/
  include/
  lib/
  openmalaria/
  openmalaria-build/
  share/
  Toolchain-i586-mingw32.cmake
  xerces-c-src_2_8_0/
  xsd -> xsd-3.2.0-i686-windows
  xsd-3.2.0-i686-windows/
  zlib-1.2.3/
```

## zlib ##

Grab and extract the sources.
```
cd zlib-1.2.3
CC=/usr/bin/i586-mingw32msvc-cc AR="/usr/bin/i586-mingw32msvc-ar rc" RANLIB=/usr/bin/i586-mingw32msvc-ranlib ./configure --prefix=/home/USERNAME/i586-mingw32msvc
make test
make install
```
That may be enough, or you may need to copy the library to openmalaria/lib.

## xerces-c ##

(ver 2.8; probably 3.0 should work too.) Extract the sources, then:
```
cd xerces-c-src_2_8_0
export XERCESCROOT=`pwd`
cd src/xercesc
./runConfigure -p mingw-msys -c i586-mingw32msvc-gcc -x i586-mingw32msvc-g++ -P /home/cyborg/STI/i586-mingw32msvc -C --host=i586-mingw32msvc -s
```
You should edit Makefile.incl (once; it's not generated):
  * substitute 'ar' by 'i586-mingw32msvc-ar'
  * substitute 'ranlib' by 'i586-mingw32msvc-ranlib'
  * substitute 'dllwrap' by 'i586-mingw32msvc-dllwrap'
```
make
make install
```
(Instructions ripped from the [clam wiki](http://clam-project.org/wiki/Devel/Windows_MinGW_cross_compile#Xerces-C).)

This should successfully install the library, however I still get some missing xerces symbols when linking OpenMalaria.

## XSD ##

The nice thing is you don't have to compile this.
  * make sure you can execute xsd (just install xsdcxx since you want a _linux_ binary to run at _compile_ time).
  * make the headers available (e.g. make /usr/i586-mingw32msvc/include/xsd a sym-link to /usr/include/xsd)

## GSL ##

After extracing sources,
```
cd gsl-1.12
CC=/usr/bin/i586-mingw32msvc-cc AR="/usr/bin/i586-mingw32msvc-ar" RANLIB=/usr/bin/i586-mingw32msvc-ranlib ./configure --enable-static=yes --enable-shared=no --prefix=/home/USERNAME/i586-mingw32msvc/ --build=mingw32
make -j 2
make install
```

## Compiling OpenMalaria ##

You should now have libraries for the above available in /home/USERNAME/i586-mingw32msvc/lib and headers in /home/USERNAME/i586-mingw32msvc/include (or something similar).

Create a Toolchain file, to tell cmake about the platform we're cross-compiling for (see [here](http://www.cmake.org/Wiki/CmakeMingw) for more info). Mine looks like:
```
# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER i586-mingw32msvc-gcc)
SET(CMAKE_CXX_COMPILER i586-mingw32msvc-g++)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  /usr/i586-mingw32msvc /home/USERNAME/i586-mingw32msvc )

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

Create the openmalaria-build folder somewhere, run cmake-gui and set the source and build dirs, tell it to configure and (being a new build folder) it'll ask for setup details. You can then tell it to cross-compile with the above tool-chain file. Check the lib and include paths are OK; if not it's easiest to set them manually. Then configure & generate.

You might have to temporarily copy the openmalaria-build/xsdcxx/scenario.?xx from somewhere else; there shouldn't be any problem building them but I had a configure/generate problem with cmake without these.

Now compile with make. That should be it if everything's ready... I still get link errors.