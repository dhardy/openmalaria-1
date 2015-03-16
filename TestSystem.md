## Testing OpenMalaria ##
Once your installation is built, there are is a suite of regression and unit tests that should be run to test that nothing has gone awry.

### Running the tests ###
CMake creates a build target which is never actually built, but "building" this target will run any integrated tests which are enabled. You can run this by:
  * _(Makefiles)_ Running **make test**
  * _(IDEs)_ Building the **RUN\_TESTS** target
  * _(CTest)_ Running **ctest**

CTest (part of [cmake.org CMake]) has the advantage that you can use options like `-R` to select individual tests to run and (CMake 2.8 and later) `-j2` to run 2 or more jobs in parallel.

Most likely, you'll pass with flying colors. In the event that a test fails, look at the `BUILD_DIR/Testing/Temporary/LastTest.log` file, or call [run.py](UtilsRunScripts#run.py.md) directly. Example:
```
cd build
test/run.py 2 IPT -- --checkpoint
```

> ## Running the included tests ##

Several unit and regression tests have been built for OpenMalaria. These should be run after every build, to confirm that nothing has gone amiss in the build process.


On unix, from the build directory, run ctest (can pass the `-j 2` option to speed up):
```
$ ctest
Start processing tests
Test project /home/gouigoui/workspace/OpenMalaria_readOnly/build
  1/ 13 Testing 1                                Passed
  2/ 13 Testing 2                                Passed
  3/ 13 Testing 3                                Passed
  4/ 13 Testing 4                                Passed
  5/ 13 Testing 5                                Passed
  6/ 13 Testing 6                                Passed
  7/ 13 Testing 9                                Passed
  8/ 13 Testing 11                              Passed
  9/ 13 Testing 12                              Passed
 10/ 13 Testing IPT                            Passed
 11/ 13 Testing VecTest                     Passed
 12/ 13 Testing VecFullTest                Passed
 13/ 13 Testing unittest                      Passed
100% tests passed, 0 tests failed out of 13
```

Alternately, you can use the test/run.py script directly:
```
# (from build folder)
test/run.py 1 2 3
test/run.py VecTest -- --checkpoint
test/run.py Empirical --gdb
test/run.py --help
```