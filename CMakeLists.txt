# Root CMake configuration for OpenMalaria
# Copyright © 2005-2013 Swiss Tropical Institute and Liverpool School Of Tropical Medicine
# Licence: GNU General Public Licence version 2 or later (see COPYING)

cmake_minimum_required (VERSION 3.0)

project (OpenMalaria CXX)

# The FindXXX.cmake files included are not quite the same as standard ones.
# They abort if not found, and look in a few extra places (e.g. ${CMAKE_SOURCE_DIR}/lib).
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

# -----  Compile options  -----

if (NOT MSVC)
  # Enable builds on Raspberry pi (although without optimizations)
  if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else (${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -msse2 -mfpmath=sse")
  endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm")
  # We almost always want optimisations enabled:
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O2")
endif (NOT MSVC)

set (OM_STD_LIBS )

# Statically link libgcc; isn't going to work when other C++ libraries are dynamically linked
if( ${CMAKE_COMPILER_IS_GNUCXX} )
  # This doesn't show up in the cache, but it is passed to gcc:
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

  option (OM_COVERAGE "Produce coverage into binary.")
  if (OM_COVERAGE)
    # Add coverage
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
  endif (OM_COVERAGE)
  
  option (OM_STATICALLY_LINK "Attempt to link libraries, including stdc++, statically.")
  if (OM_STATICALLY_LINK)
    link_directories (${CMAKE_SOURCE_DIR}/lib)
    # Note: -static-libstdc++ is only available since GCC 4.5
    # See: http://www.trilithium.com/johan/2005/06/static-libstdc/
    add_definitions(-static-libgcc -static-libstdc++)
    set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -static-libgcc -static-libstdc++")
  endif (OM_STATICALLY_LINK)
endif( ${CMAKE_COMPILER_IS_GNUCXX} )

option (BUILD_SHARED_LIBS "Link xsdcxx, model, etc. libraries dynamically instead of statically (almost certainly not wanted)." OFF)
mark_as_advanced (BUILD_SHARED_LIBS)

if (MSVC)
  # We almost always want optimisations enabled:
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /O2")
  # This is now the OM_STATICALLY_LINK for windows
  option (OM_USE_LIBCMT "Use LIBCMT.lib instead of MSVCRT[D].lib when linking, and link statically. This removes a dependency on a .net library, but usually requires manually building dependency libraries." OFF)
  # set a temporary variable, since cache variables don't get rewritten
  if (OM_USE_LIBCMT)
    #piece of code from http://stackoverflow.com/questions/1618927/cmake-microsoft-visual-studio-and-monolithic-runtimes
    #Note: doesn't update cmake cache (but still works)
    #We statically link to reduce dependancies
    foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
      if(${flag_var} MATCHES "/MDd")
        string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
      endif(${flag_var} MATCHES "/MDd")
      if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
      endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
    
    set (OM_LINK_FLAGS "/NODEFAULTLIB:MSVCRT /NODEFAULTLIB:MSVCRTD /NODEFAULTLIB:library")
    set (OM_COMPILE_FLAGS "/DXERCES_STATIC_LIBRARY")
  else (OM_USE_LIBCMT)
    set (OM_LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
    set (OM_COMPILE_FLAGS)
  endif (OM_USE_LIBCMT)
  set (OM_COMPILE_FLAGS "${OM_COMPILE_FLAGS} /D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS")
endif (MSVC)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")
  # These are actually only needed when using "clang" not "clang++" (same with
  # "gcc"), but adding here saves a user error:
  set (OM_STD_LIBS m stdc++)
endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

# -----  Find dependencies  -----

set (CMAKE_LIBRARY_PATH /usr/lib64)

find_package (XSD)
find_package (XERCESC)
find_package (GSL)
find_package (Z)
find_package (Python)

if (NOT PYTHON_EXECUTABLE)
    message (SEND_ERROR "Unable to find a Python interpreter (required).")
endif (NOT PYTHON_EXECUTABLE)


# Find Boost:

# suggested on windows: install boost such that paths are:
# C:\Program Files\boost\boost_VER\boost\config.hpp
# VER looks like 1_38_0 NOT 1.38.0
# Add additional versions here when needed:
# set (Boost_ADDITIONAL_VERSIONS "1.54.0" "1.55.0" "1.56.0" "1.57.0" "1.58.0")
#set (Boost_DEBUG)
# We require 1.54: first version including nonfinite_num_facets.hpp
find_package( Boost 1.54.0 )
if(NOT Boost_FOUND)
    message (FATAL_ERROR "Unable to find boost headers! Please install libboost-dev or boost from boost.org (preferably at least version 1.41)")
endif(NOT Boost_FOUND)


# -----  Compile-time optional features  -----
# (must come before add_subdirectory (model))

option (OM_STREAM_VALIDATOR "Compile in StreamValidator (see model/util/StreamValidator.h for usage notes)" OFF)
if (OM_STREAM_VALIDATOR)
  add_definitions (-DOM_STREAM_VALIDATOR)
endif (OM_STREAM_VALIDATOR)


# -----  Compile code  -----

add_subdirectory (contrib)
add_subdirectory (schema)
add_subdirectory (model)
add_subdirectory (util/SchemaTranslator)
add_dependencies (model schema)

# -----  generate openMalaria  -----

if (UNIX)
  find_library (PTHREAD_LIBRARIES pthread PATHS ${CMAKE_SOURCE_DIR}/lib /usr/lib /usr/local/lib)
  if (NOT PTHREAD_LIBRARIES)
    message (SEND_ERROR "PThread library not found")
  endif (NOT PTHREAD_LIBRARIES)
endif (UNIX)
mark_as_advanced (PTHREAD_LIBRARIES)

include_directories (SYSTEM
  ${XSD_INCLUDE_DIRS}
  ${XERCESC_INCLUDE_DIRS}
  ${GSL_INCLUDE_DIRS}
  ${Z_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/contrib
)
include_directories (
  ${CMAKE_SOURCE_DIR}/model ${CMAKE_BINARY_DIR}
)

add_executable (openMalaria model/openMalaria.cpp)

target_link_libraries (openMalaria
  model
  schema
  contrib
  ${GSL_LIBRARIES}
  ${XERCESC_LIBRARIES}
  ${Z_LIBRARIES}
  ${PTHREAD_LIBRARIES}
  ${OM_STD_LIBS}
)

if (MSVC)
  set_target_properties (openMalaria PROPERTIES
    LINK_FLAGS "${OM_LINK_FLAGS}"
    COMPILE_FLAGS "${OM_COMPILE_FLAGS}"
  )
endif (MSVC)

# Dependencies from outside this repository are linked dynamically. Since we
# cannot be sure deployment systems have the same versions, we copy the ones
# from the build system, and add an rpath entry to link from the current dir.
option (OM_COPY_LIBS "Copy linked library files")
if (OM_COPY_LIBS)
  set_target_properties (openMalaria PROPERTIES
    LINK_FLAGS "-Wl,-rpath,\"\$ORIGIN\"")
  foreach (lib ${GSL_LIBRARIES} ${XERCESC_LIBRARIES} ${Z_LIBRARIES})
    configure_file (${lib} ${CMAKE_BINARY_DIR} COPYONLY)
  endforeach (lib)
endif (OM_COPY_LIBS)

# add_executable (pVivax model/Pv_mod/Source.cpp)
# target_link_libraries (pVivax libpvivax)


# -----  OM_BOXTEST - black-box & unit testing  -----

option(OM_CXXTEST_ENABLE "Enable lower-level unittests using cxx (use 'make test' or Visual Studio build target)test" ON)
if (OM_CXXTEST_ENABLE)
  enable_testing()
  add_subdirectory (unittest)
endif (OM_CXXTEST_ENABLE)

option(OM_BOXTEST_ENABLE "Enable black-box testing of openMalaria (use 'make test' or Visual Studio build target)" ON)
if (OM_BOXTEST_ENABLE)
  enable_testing()
  add_subdirectory (test)
endif (OM_BOXTEST_ENABLE)
