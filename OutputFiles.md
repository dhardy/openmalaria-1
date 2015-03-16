# Output of simulations #

Output from OpenMalaria simulations is currently collected using two methods. The [monitoring](Monitoring.md) page describes how these methods work and are configured, while the [monitoring measures](XmlMonitoring.md) page lists what data these methods are able to output.

## Surveys ##

Output from the [surveys](Monitoring#Surveys.md) system is written at the end of the simulation to the `output.txt` (or when compiled in BOINC mode `output.txt.gz`) file. The contents are formatted as in a database, with tab-separated columns and one record per line. Each line has four columns with the following meaning:

| survey number | third dimension | measure | value |
|:--------------|:----------------|:--------|:------|

The survey number starts from one and corresponds to the [survey time point](Monitoring#Survey_time_points.md). (Exception: measure 21 has one record from the end of the simulation and does not use the survey number or third dimension columns.)

Output can be associated with several different [measures](Monitoring#Survey_measures.md); the code under the label "id" in the first column of the [survey measures table](XmlMonitoring#Survey_measures.md) appears in the third column of output.

The "third dimension" (in the second column for historical reasons) specifies another dimension of the output. For many measures it identifies the human [age group](Monitoring#Survey_age_groups.md), for a few measures it is unused, and for some it holds a [mosquito species](XmlEntoVector.md) or a [drug identifier](ModelDrug.md).

Example:
```
1       15      2       4
```
Measure 2 is the "expected number of infected hosts", so this record specifies that during the first survey, the expected number of infected hosts within the fifteenth age group was 4.

## Continuous monitoring ##

Some properties of the simulation can also be monitored continuously. The [continuous monitoring system](Monitoring#Continuous.md) writes output to the `ctsout.txt` file in real-time (while the simulation is still running). This file can be monitored while a simulation is running from LiveGraph and read from most spread-sheet programs.

This file starts with a line specifying the column separator (a tab) to make the files readable by [LiveGraph](LiveGraph.md). The next line is a header, giving a title for each column (note: spaces may occur within column names, whereas tab characters separate columns). Following this, each line corresponds to a time point.

Available outputs are listed [here](XmlMonitoring#Continuous_measures.md); note that some of these output a single value while others output multiple values (using titles of the format "measure (subcategory)").

## Checksum ##

When compiled in BOINC mode, an additional file is output: `scenario.sum`, which is a checksum of the input XML file.