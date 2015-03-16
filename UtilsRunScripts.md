# Util scripts #

We have developed a few scripts to ease running and analysing the output of individual or small numbers of scenarios:

  * [run.py](UtilsRunScripts#run.py.md)
  * [compareOutput.py](UtilsRunScripts#compareOutput.py.md)
  * [compareCtsout.py](UtilsRunScripts#compareCtsout.py.md)
  * [plotResult.py](UtilsRunScripts#plotResult.py.md)

The first of these is used by [ctest](http://cmake.org/) to [run the built-in tests](http://code.google.com/p/openmalaria/wiki/BuildSystem#Running_the_tests).


# run.py #

Code: [test/run.py](http://code.google.com/p/openmalaria/source/browse/trunk/test/run.py)

This script can run one or more scenarios in a temporary directory with various options and test the output is as expected. It is designed for running the built-in test scenarios and aiding further analysis when these do not work as expected, and thus is sometimes more convenient than [running scenarios directly](UtilsRunning.md).

Scenarios to be run must be in the `test` directory and named `scenarioX.xml` for some `X`. Passing run.py the argument `ABC` will then run `scenarioABC.xml` etc. — multiple names may be passed, or none, in which case run.py will attempt to run all XML files of this form in the `test` dir.

The run.py script in the test directory cannot be run directly but is translated by cmake into the build directory. Thus, after [building OpenMalaria](BuildSystem.md), from the build directory, run.py can be run as

```
# run scenarios 1, 2 and 3.
test/run.py 1 2 3
```

Options can be passed to OpenMalaria by first specifying -- (which marks the end of options passed to the script); examples:

```
test/run.py VecTest -- --print-EIR
test/run.py 1 -- --checkpoint
```

In the last case, OpenMalaria exits after each checkpoint is written, and run.py starts OpenMalaria again (until either output.txt is written or no new checkpoints appear to have been written).

Various options can also be given to run.py:

```
  -h, --help            show this help message and exit
  -q, --quiet           Turn off console output from this script
  -n, --dry-run         Don't actually run OpenMalaria, just output the
                        commandline.
  -c, --dont-cleanup    Don't clean up expected files from the temparary dir
                        (checkpoint files, schema, etc.)
  -d, --diff            Launch a diff program (kdiff3) on the output if
                        validation fails
  --valid, --validate   Validate the XML file(s) using xmllint and the latest
                        schema.
  -g, --gdb             Run OpenMalaria through gdb.
  --valgrind            Run OpenMalaria through valgrind (using memcheck
                        tool).
  --valgrind-track-origins
                        As --valgrind, but pass --track-origins=yes option
                        (1/2 performance).
  --callgrind           Run OpenMalaria through valgrind using callgrind tool.
  --cachegrind          Run OpenMalaria through valgrind using cachegrind
                        tool.
```

As an example, `test/run.py Cohort -c` runs the Cohort scenario, but doesn't delete expected temporary files from the output directory so you can inspect what's there (or run OpenMalaria directly from there). Note that any unexpected files and output files which are different from expectation are not deleted in any case.

After running scenarios, the generated output ([output.txt](http://code.google.com/p/openmalaria/wiki/OutputFiles#Surveys) and [ctsout.txt](http://code.google.com/p/openmalaria/wiki/OutputFiles#Continuous_monitoring)) is compared against that in [`OMROOT/test/expected`](http://code.google.com/p/openmalaria/source/browse/#svn%2Ftrunk%2Ftest%2Fexpected) using the compareOutput.py and compareCtsout.py scripts in `OMROOT/util`. See below for how to interpret their output.

## compareOutput.py ##

Code: [util/compareOutput.py](http://code.google.com/p/openmalaria/source/browse/trunk/util/compareOutput.py)

This script is designed to test whether two output.txt files are identical, similar or completely different. It is used in testing, usually through `run.py`.

When the output files are not identical, the script parses the output files (can take a while when they are large). It may be that although the output files are not byte-for-byte identical, the output is never-the-less exactly or to a high degree of precision equivalent. In these cases not much output is printed.

When significant differences are discovered, a line is printed for each measure with significant differences, as well as a summary. The per-measure lines look like:

```
for measure M:  sum(1st file):X  sum(2nd file):Y  diff/sum: U  (abs diff)/sum:V
```

where M is the measure's numeric identifier (see table in SurveyMeasure.h or on
wiki), X and Y are the sums of all values for this measure where a value is
given in both files, U is the sum of signed differences between values from the
first and second file for this measure divided by X, and V is the same accept
that the absolute value of the differences is taken before summing.

U will be positive if Y>X, negative if X>Y, zero if X=Y, assuming X!=0. If X=0
both U and V will be nan. The absolute value of U should never be larger than
V.

If abs(U) is much less than 1 (say under 0.1, or 10%) it indicates X and Y are similar: the total of the measure (across age groups and time points) is similar. If V is also small it gives a stronger indication: that the values at each (age) group and time-point are similar. Of course if measuring low frequency discrete random events and 2 occur in one simulation while none occur in another, it doesn't indicate there's much difference between scenarios. Similarly if 200 random events are reported in the whole simulation but split between 100 per-group, per-time-point records, it shouldn't be surprising if V is large even if U is small and the scenarios are very similar. It is therefore not really possible to conclude that two scenarios are similar based on their outputs unless the population size is quite large.

As summary, this script reports the "total relative diff". This is the some of a difference metric for each pair of values which can never be greater than 1 (assuming no special values like infinity are input) and will be less than the relative precision (typically 10¯⁶<a href='Hidden comment: 10^(-6)'></a>) for each pair considered approximately equal.


## compareCtsout.py ##

Code: [util/compareCtsout.py](http://code.google.com/p/openmalaria/source/browse/trunk/util/compareCtsout.py)

This functions similarly to compareOutput.py but reads the ctsout.txt files. It needs [matplotlib](http://matplotlib.sourceforge.net/) installed to work.

# plotResult.py #

Code: [util/plotResult.py](https://code.google.com/p/openmalaria/source/browse/util/plotResult.py)

This is a command-line script designed to produce rapid visualisations of output.txt files. A couple of examples:

![http://openmalaria.googlecode.com/svn/wiki/graphs/plotResults-example1.png](http://openmalaria.googlecode.com/svn/wiki/graphs/plotResults-example1.png)

![http://openmalaria.googlecode.com/svn/wiki/graphs/plotResults-example2.png](http://openmalaria.googlecode.com/svn/wiki/graphs/plotResults-example2.png)

The simplest way to plot outputs from a single scenario is:

```
python plotResult.py output.txt
```

There are quite a few options controlling how the plots are generated. A useful way of comparing output measures 14 and 15 between two output files:

```
python plotResult.py outputA.txt outputB.txt -f line -a -e "m in (14,15)"
```

Many more options are available. Run with `-h` to get a list:
```
python plotResult.py -h
Usage: plotResult.py [options] FILES                                                                                      
                                                                                                                          
Plots results from an OpenMalaria (surveys) output file by time. Currently no                                             
support for simultaeneously handling multiple files or plotting according to                                              
age group.  Valid targets for plotting keys are: none (key is aggregated),                                                
x-axis, plot, line. If no key is set to the x-axis, the first unassigned of                                               
survey, group, cohort, genotype, file will be assigned to the x-axis.                                                     
                                                                                                                          
Options:                                                                                                                  
  --version             show program's version number and exit                                                            
  -h, --help            show this help message and exit
  -e FILTEREXPR, --filter=FILTEREXPR
                        Filter entries read according to this rule (i.e.
                        values are included when this returns true).
                        Parameters available: f, m, s, g, c, gt, c. Examples:
                        'True', 'm!=0' (default), 'm in [11,12,13]', 's > 73
                        and m!=0'.
  --debug-filter        Each time FILTEREXPR is called, print input values and
                        output. Warning: will print a lot of data!
  -a, --no-auto-measures
                        Don't automatically put similar measures on the same
                        plot.
  -s S, --survey=S      How to plot surveys
  -g G, --group=G       How to plot (age) groups
  -c C, --cohort=C      How to plot cohorts
  --genotype=GT         How to plot genotypes
  -f F, --file=F        How to plot outputs from different files
  -n, --file-names      Use full file names instead of replacing with short
                        versions
  -l, --no-legends      Turn off legends (sometimes they hide parts of the
                        plots)
  -L LABELS, --labels=LABELS
                        Show [t]itle, [x]-axis and/or [y]-axis labels: txy tx
                        ty t xy x y or (none); default is xy
  -b, --vertical-stack  Where bar-plots need sub-divisions, use a vertical
                        stack instead of horizontal sub-bars
  --scale=SCALE         Set y-axis scale to linear, log or auto (currently
                        auto does nothing)
```

Note that this script can be useful when comparing outputs for significant differences, as in:
```
util/plotResult.py outputA.txt outputB.txt -e 'm in [11,12,13]' -g x-axis -f line -a
util/plotResult.py outputA.txt outputB.txt -e 'm in [11,12,13]' -s x-axis -f line -a
```

This script can be run directly from the source code directory, or you can download [plotResult.py](https://openmalaria.googlecode.com/git/util/plotResult.py) and [readOutput.py](https://openmalaria.googlecode.com/git/util/readOutput.py) then run with [Python](https://www.python.org/) (use Python 2.x, not 3.x).