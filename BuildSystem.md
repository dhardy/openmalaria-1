## Building OpenMalaria ##
Instructions are included for building OpenMalaria on three operating systems:

  * [Linux](UnixBuildOpenMalaria.md) (and Unix in general)
  * [Windows](WindowsBuildOpenMalaria.md)
  * [Mac OS X](MacBuildOpenMalaria.md)

### Configuration details ###
Regardless of operating system, building OpenMalaria requires some configuration of the build tool. Below we give some detail on the most relevant build configuration options.

#### General configuration settings ####

  * `CMAKE_BUILD_TYPE`
> Options:
    * `None` (`CMAKE_CXX_FLAGS` or `CMAKE_C_FLAGS` used)
    * `Debug` (compile in debugging symbols)
    * `Release` (compile with optimizations)
    * `RelWithDebInfo` (both optimize and compile in debugging symbols; but if debugging note that code may not execute in the order expected due to optimizations)
  * `OM_BOINC_INTEGRATION`
> BOINC integration adds reporting of progress and completion compatible with the BOINC client; it is required for volunteer computing on http://www.malariacontrol.net or another BOINC server. Messages normally sent to the standard error stream are redirected to `stderr.txt`. Additionally, this reduces the verbosity of reporting on the command-line: some messages are abbreviated, and progress reporting on the command-line is turned off.
  * `OM_BOXTEST_ENABLE`
> Enables what I refer to as OpenMalaria's "black-box" tests in ctest: full scenarios which are run and who's output is checked. These are the primary tests that everything is working, though when they fail it is not always clear why. Can be run with and without checkpointing to also test the checkpointing system (ctest normally runs them with checkpointing). Sometimes during development these get broken, so if they fail it is not necessarily your build that is at fault.
  * `OM_CXXTEST_ENABLE`
> Enables OpenMalaria's (few) unit tests. These should test individual components of OpenMalaria, however not that many have been written.
  * `OM_CXXTEST_GUI`
> A GUI progress bar is shown while the unit tests are run. Not that useful since the unit tests are pretty quick in comparison to the black box tests.

#### Further configuration settings, needed less often ####

  * `XXX_LIB`, `YYY_INCLUDE_DIR`
> Path to library XXX, include file directory for YYY. On windows it is sometimes necessary to set these paths manually, on linux they are usually found automatically when the needed packages are installed.
  * `OM_CXXTEST_USE_PYTHON`
> Use Python instead of Perl to generate the unit tests. Either can be used, and this option should be automatically selected.
  * `PYTHON_EXECUTABLE` / `PERL_EXECUTABLE`
> Path to either the perl or the python program, needed to run the unit tests (see OM\_CXXTEST\_ENABLE). May need to be set manually in windows (usually Python since this is also needed by the black-box tests).
  * `OM_STATICALLY_LINK` (GCC), `OM_USE_LIBCMT` (MSVC)
> Options to control/reduce the run-time dependencies needed by the resultant binary. May be useful when compiling a binary which will be distributed to other computers.
  * `CMAKE_CXX_FLAGS`
> Can be used if extra build flags need to be added.
  * `CMAKE_VERBOSE_MAKEFILE`
> To try to solve build/link errors it is sometimes useful to know the full options sent to the compiler/linker. This option enables this, at the cost of a very verbose build log.
  * `OM_STREAM_VALIDATOR`
> Tool to aid analysis of why a black-box test is failing. See note in `include/util/StreamValidator.h` for usage.