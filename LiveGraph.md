# Introduction #

[LiveGraph](http://www.live-graph.org/) is a useful graph-plotting program that can directly plot OpenMalaria's [continuous output](Monitoring#Continuous.md) files (other than this, it has nothing to do with OpenMalaria).


## Installing ##

LiveGraph is written in java, so should be platform independent.

Download the binary "Plotter application" of the latest version from [this page](http://www.live-graph.org/download.html). Extract the archive wherever you want to install livegraph.

Run it by executing the `LiveGraph.*.jar` file or using a script like that below. A java run-time environment must be installed.

```
#!/bin/sh
cd /home/dhardy/download/LiveGraph.2.0.beta01.Complete
java -jar LiveGraph.2.0.beta01.Complete.jar
```

Note that configuration files are stored in the current directory when livegraph is run, hence the `cd` in the above script.

## Usage ##

Make sure the scenario of interest is generating continuous output (requires schema version >= 19; see [configuration](XmlScenario#Continuous_reporting.md)).

Open the `ctsout.txt` file; for the test scenarios run with the `run.py` script these should be in the `build/test/NAME-RAND` directory where the scenario file is called `scenarioNAME.xml` and `RAND` is a random sequence of characters. (This can be read live while the simulation is running, though normally no data is output during initialization which accounts for the majority of the run-time.)

Simulation time or time-step is the first output and can be used as the x-axis data series. I recommend not graphing many series at once, due to the completely different scales.

Graphs may be saved as images from the "Plot" menu (graph window or log-messages window).

Observed limitations: when reading a large amount of data, livegraph only uses a sub-sample of this data, which can result in missing peaks of periodic data such as EIR (primarily happens when data for initialization period is included, since this adds another 90 or so years of data). Plotting only the "tail data" avoids this issue, while restricting the output timespan.