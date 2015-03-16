# OpenMalaria simulator #

The simulator can be:

  * run via the command line (download [below](Binaries#Releases.md) then see [UtilsRunning](UtilsRunning.md)
  * used with a graphical user interface (outdated; see [OpenMalariaTools](openmalariaTools.md) )
  * complied from source and run via command line, see [BuildSystem](BuildSystem.md)
  * run over the BOINC volunteer computing network, see [malariacontrol.net ](http://malariacontrol.net/)

We can, upon request, create packages with binaries of the **OpenMalaria** simulator and a few examples for win32/OS X/linux 32/64-bit platforms. Mac users are invited to follow the build instructions for their operating system. Please ask us for more information if none of the links above provide what you need.

## Releases ##

Here are links to pre-compiled binaries. For each release, the
following may be added (contact us if something you need is missing):

```
source.zip      A copy of source code for this version (including test scenarios)
win32           Binaries plus a test scenario and batch file to run OM on 32-bit Windows (compatible with XP and probably newer versions)
linux64         Binaries for reasonably modern 64-bit linux versions
mac os x        Mac OS X binaries
scenario_XX.xsd Schema for the relevant version
scenarios/      A directory with some test scenarios
```

A [list of archives](http://code.google.com/p/openmalaria/source/browse?repo=wiki#git%2Fdownload%2Freleases) is available. These contain:

  * Schema 33 (Windows 64-bit)
  * Schema 32 (Linux 64-bit, Mac 32-bit and Windows 64-bit builds)
  * Schema 31 (Linux 64-bit and Windows 32-bit)
  * Schema 30.4 (Linux 64-bit)
  * Schema 30.3 (Linux 64-bit and Windows 32-bit)
  * Schema 29 (Linux 32-bit)


## Troubleshooting ##

Note that, for Windows, in addition to the other [Dependencies](Dependencies.md), the Microsoft .net framework is a prerequisite. If you don't have .net, it can be downloaded from [Microsoft](http://www.microsoft.com/net/).

If you get an error message like "The Program can't start because MSVCR110.dll is missing from your computer.", install it for your system x64 or x86 (for 32 bit) from http://www.microsoft.com/en-us/download/confirmation.aspx?id=30679.


Still have problems? We want to know! Send us an e-mail, or, better yet open an [issue](http://code.google.com/p/openmalaria/issues/list). However you contact us, please include a copy of the error message that is generated.


# Experiment creator #

The experiment creator tool is designed to produce a full set of XML files for an experiment from a set of sweeps (factors) each with one or more arms (values). [more ... ](ExperimentDesign.md)


# Schema translator #

The scenario updater tool, SchemaTranslator, is integrated into OpenMalariaTools and used automatically when running old scenario files. [more ... ](XmlUpdateScenario.md)