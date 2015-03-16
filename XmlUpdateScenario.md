# Automatic schema version update #

If you use some scenarios working with an older schema version, these can be updated with the schema translator. The translation may not work for all scenarios, but it will point out the area where it detects an  ambiguous situation, by either asking you to specify the value of a certain flag and then rerun the translator, or by pointing out where a manual edit in the scenario is required.

The schema translator can be used within [openmalariaTools](openmalariaTools.md) or as a stand-alone tool. Alternately ask your local software developer for help.

## Stand-alone ##

You need Java version 7 or later (open a command prompt: does `java -version` or `java.exe -version` work and tell you it is version 7 (or 1.7) or later?). Then either [checkout the OpenMalaria code](http://code.google.com/p/openmalaria/source/checkout) and go to the util/SchemaTranslator directory or download the jar files from [this directory](http://code.google.com/p/openmalaria/source/browse/#git%2Futil%2FSchemaTranslator%2Fbin) (optionally also a schema (.xsd) file). Create a `scenarios` directory and put the XML files there. Then run something like `java -jar bin/schema-translator.jar --help`.

You will likely need to either use `--no-validation` or `--schema-folder PATH` (substituting PATH with the location of the directory where the XSD file is). You may be asked to specify other options when you try running the tool; if so just add them to the command line and try again (no need to remove existing translated scenarios first).

## Build and Install ##

Building the schema translator from sources:

  1. Check if JDK of at least version 5 is installed.
    * If it's not the case, download OpenJDK or Oracle's JDK
  1. Install the Kotlin compiler (version 0.5.748 in my case)
  1. Find the sources.
    * Clone the OpenMalaria source code, then find the schema translator source files in `util/SchemaTranslator`
  1. Build the sources.
    * in the sources folder do `./compile`

## Usage ##

  1. Put the scenarios to be converted in the `scenarios` folder (sub-folders are fine too — directory structures are preserved)
  1. Run the program from terminal or command line with the command `java SchemaTranslator`
    * If running from another location, use `java -classpath path_to_schematranslator_folder SchemaTranslator`
    * other options can be used if necessary, as below
  1. Check warning/error messages in the program's output
  1. Use updated scenarios from the `translatedScenarios` folder

If you don't add any options, then the scenarios will be converted to the latest schema version.

### Options ###

Below are some of the most useful options. To see a full list, run the schema translator with the help option: `java SchemaTranslator --help`

  * `--required_version XX`: The version number to update the scenarios to. By default, the latest schema version will be used.
  * `--current-schema --schema-folder ../../build/schema`: update to the latest development schema, which is at `../../build/schema/scenario_current.xsd`
  * `--no-validation`: if no xsd file is available, scenarios can still be translated with this option but standard XML schema checks can't be done
  * `--maxDensCorrection BOOL`: Update 12->13 requires this sometimes: set true to include bug fix, false to explicitly exclude it.
  * `--iptiSpOptionWithoutInterventions`: For scenarios with iptiDescription but without interventions, assume usage of the IPTI model was (t) intended or (f) a mistake.

