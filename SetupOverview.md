# Participate in the volunteer computing effort! #

If you would like to help the project by offering down-time on your computer to run simulations, join us at [malariacontrol.net](http://www.malariacontrol.net)

# Running your own simulations #

If you would like to run your own simulations you have the following options:

  * run your own simulation scenarios directly from the command line as described on the [UtilsRunning](UtilsRunning.md) page.

  * install the compiled tool with GUI [OpenMalariaTools](openmalariaTools.md).  This includes facilities for updating configuration files and running small simulation experiments comprising multiple scenarios.

The [UtilsRunning](UtilsRunning.md) page also explains how to continue once you have installed **OpenMalaria** and to test small numbers of scenarios.

The [Binaries](Binaries.md) page gives general information on how and where to install pre-built executables of **OpenMalaria**.

# Contributing to **OpenMalaria** development #

  * If you want to join the development effort, you can find the source code [here](Code#Source_code.md).
  * To build the OpenMalaria executable from source, go [here](BuildSystem.md)

# Tools #

The configuration files for **OpenMalaria** are in XML format, which allows them to be edited by a text editor.  We recommend the following:

  * [Windows](UtilsWindows.md) — a convenient text editor, and archive tool
  * For MacOSX — handles archives fine but not necessarily XML files; dashcode (presumably part of [xcode](http://developer.apple.com/tools/xcode/)) can edit XML files but struggles with scripts. There are a number of other MacOSX editors [search for one](http://www.google.com/search?q=macosx+text+editor)
  * [run.py script](UtilsRunScripts#run.py.md) — sometimes useful for running test scenarios
  * [plotResult.py script](UtilsRunScripts#plotResult.py.md) — provides a quick way to plot outputs from one or a small number of scenarios

## Resources ##

  * [XML snippet library](snippets.md)
  * [Monitoring measures](XmlMonitoring.md)

Schema (XSD) files are also available. Files with names like `scenario_32.xsd` are the complete schema for some version while files like `scenario.xsd`, `demography.xsd` contain the latest version in multiple parts.

  * [Schema v30](https://code.google.com/p/openmalaria/source/browse/?name=schema-30.4#git%2Fschema)
  * [Schema v32](https://code.google.com/p/openmalaria/source/browse/?name=schema-32#git%2Fschema)
  * [Schema latest code (development version)](https://code.google.com/p/openmalaria/source/browse/?name=develop#git%2Fschema)