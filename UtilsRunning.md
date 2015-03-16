# Introduction #

OpenMalaria comes with several [test scenarios](http://code.google.com/p/openmalaria/source/browse/#git%2Ftest) which can be used both as template scenarios and to check that OpenMalaria is working as usual. This page describes how one can run these tests and run other scenarios.  **Note that the test scenarios include many non-default options which will mostly need to be removed if the XML is used a template for simulating malaria in the real world**. Some further
[examples](http://openmalaria.googlecode.com/svn/application_deployment/examples/) can be used. The [centre scenario](http://openmalaria.googlecode.com/svn/application_deployment/examples/centre_scenario.xml) from [Briët et al., 2012](http://www.malariajournal.com/content/11/1/20) is a good starting point.

## Schema versions ##

OpenMalaria's [scenarios](ScenarioDesign.md) are described in an XML document, which must conform to the executable's XSD (Xml Schema Document) version (as of recently running OpenMalaria with the `--version` command-line option will print the current schema version together with the oldest version which would potentially be compatible). In any case, the schema document relevant to the current scenario (e.g. `scenario_23.xsd`) must be present, usually in the working directory, when running a scenario.

Usually new schema versions introduce new features, and in some cases no changes (other than increasing the version number) is needed when updating scenarios to a later schema version, while in other cases further changes are necessary. In any case, scenarios can be updated with the [SchemaTranslator](XmlUpdateScenario.md) tool.

## Running scenarios ##

### Test scenarios ###

If you have built OpenMalaria from source, all of its test scenarios can be run via the `RUN_TESTS` build target (IDEs), by running `make test`, with `ctest`, or with custom settings via the `test/run.py` script. These tools are mostly aimed at testing the OpenMalaria executable and are [documented on the BuildSystem page](BuildSystem#Running_the_tests.md).

### General scenarios ###

Individual and batches of scenarios can be run using

  * By renaming scenarioX.xml, placing in the `test` dir, and using [run.py](BuildSystem#Running_the_tests.md) (sometimes useful, such as when forcing checkpointing or using GDB)
  * By running OpenMalaria directly from a command-line or shell script, as below
  * [OpenMalariaTools](openmalariaTools.md) (this tool needs updating manually)

In theory, one could also just (double) click on `openMalaria[.exe]` to run it. This, however, has some problems:
  * If errors occur, you can't see the error messages can't do any more than guess at the problem
  * It always uses the `scenario.xml` input file
  * Output always goes to the same file (output.txt or output.txt.gz) and if this file already exists OpenMalaria won't run.

#### Script files ####

OpenMalaria can be launched using a simple script file.

On Windows, create a text file called `runOpenMalaria.bat` (make sure you enable explorer options to show file extension, and that it does actually end with `.bat`), and add the following contents (using Notepad or a [better text editor](UtilsWindows.md)):
```
del output.txt
openMalaria.exe --scenario scenario.xml
pause
```

On Linux, the following might work (although running from a command-line as below might be easier). Create a file called `runOpenMalaria.sh` (name isn't important), change the properties to make it executable, and add the following content:
```
#!/bin/sh
rm -f output.txt
./openMalaria --scenario scenario.xml
read
```

I should also explain what those scripts do, and how you can edit them. The first line in the .sh file (`#!/bin/sh`) is required to say its a shell script (run by /bin/sh). The next line deletes the old output file using `del`/`rm` (we use the `-f` flag with `rm` to _force_ it to remove the old file, and not prompt "do you really want to delete ..."). Then we run OpenMalaria: note the name is different, and with unix .sh files we need to prepend it with `./`. We give it one option: `--scenario` which takes one argument, the file name of the input scenario document (you can change the name here; see below for more options). Finally, we use `pause`/`read` to make the terminal window running the script stay open until you press _return_ so that you can read any output. A summary of commands:

| .sh | .bat | Description |
|:----|:-----|:------------|
| ls | dir | Prints a list of files & directories in the current directory (useful from a command-line) |
| cd | cd | Change directory |
| rm | del | Delete a file |
| read | pause | Something to wait for user input |

You may also want to know what options you can use with OpenMalaria. The following is what it prints with the --help option (actually, more options have been added recently; run with --help yourself to see):
```
./openMalaria -h
OpenMalaria simulator of malaria epidemiology and control, schema version 33
(oldest compatible: 32). For more information, see
http://code.google.com/p/openmalaria/

OpenMalaria is copyright © 2005-2010 Swiss Tropical Institute and Liverpool
School Of Tropical Medicine.
OpenMalaria comes with ABSOLUTELY NO WARRANTY. This is free software, and you
are welcome to redistribute it under certain conditions. See the file COPYING
or http://www.gnu.org/licenses/gpl-2.0.html for details of warranty or terms of
redistribution.

Usage: ./openMalaria [options]

Options:
 -p --resource-path     Path to look up input resources with relative URLs (defaults to
                        working directory). Not used for output files.
 -s --scenario file.xml Uses file.xml as the scenario. If not given, scenario.xml is used.
                        If path is relative (doesn't start '/'), --resource-path is used.
 -o --output file.txt   Uses file.txt as output file name. If not given, output.txt is used.
    --ctsout file.txt   Uses file.txt as ctsout file name. If not given, ctsout.txt is used.
 -n --name NAME         Equivalent to --scenario scenarioNAME.xml --output outputNAME.txt \
                        --ctsout ctsoutNAME.txt
    --validate-only     Initialise and validate scenario, but don't run simulation.
    --deprecation-warnings
                        Warn about the use of features deemed error-prone and where
                        more flexible alternatives are available.

Debugging options:
 -m --print-model       Print all model options with a non-default value and exit.
    --print-EIR         Print the annual EIR (of each species in vector mode) and exit.
    --print-interventions                      
                        Print intervention deployment details and exit.
    --sample-interpolations
                        Output samples of all used age-group data according to active
                        interpolation method and exit.
    --checkpoint=t      Forces a checkpoint a simulation time t (time steps). May
                        be specified more than once. Overrides --checkpoint option.
 -c --checkpoint        Forces a checkpoint during each simulation
                        period, exiting after completing each
                        checkpoint. Doesn't require BOINC to do the checkpointing.
 -d --checkpoint-duplicates
                        Write a checkpoint immediately after reading, which should be
                        identical to that read.
    --compress-checkpoints=boolean
                        Set checkpoint compression on or off. Default is on.
    --debug-vector-fitting
                        Show details of vector-parameter fitting. The fitting methods used
                        aren't guaranteed to work. If they don't, this output should help
                        work out why.
 -v --version           Display the current schema version of OpenMalaria.
 -h --help              Print this message.

Printed help; exiting...
```
(Probably the --scenario option is the only one you'll want to use.)

#### Command line ####

The above commands are relevant for the command-lines too (the script files are, in fact, just a list of commands used by the shell/command prompt). On Windows this is usually not a convenient option, but on Unix is often the easiest way of doing things. To run OpenMalaria from a terminal on Linux or Mac,

  * Open a terminal (xterm, konsole, iterm, etc.)
  * `cd` to a directory containing the scenario file, `.xsd` file, and auxiliary files (`densities.csv`, etc)
  * Check there is no existing `output.txt`/`output.txt.gz` or `ctsout.txt` file
  * Run `./openMalaria --scenario MyScenario.xml` (or with another path)

(If you don't know how to do the above, a [bash](http://www.google.com/search?q=bash+tutorial) or [shell](http://www.google.com/search?q=shell+tutorial) tutorial may be a good starting point.)