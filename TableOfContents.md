### contents ###

  * **[Introduction](Start.md)**
  * **[Selected Papers](References.md)**
  * **[Simulations](OMUsage.md)**
    * [Getting started (local simulations)](SetupOverview.md)
      * [Compiled executables](Binaries.md)
      * [Building from source](BuildSystem.md)
    * [Scenario design](ScenarioDesign.md)
      * [XML basics](ScenarioXML.md)
      * [Monitoring systems](Monitoring.md)
      * [Health systems](ScenarioHealthSystem.md)
      * [Pharmacology data](ScenarioPharmacology.md)
      * [Transmission model](ScenarioTransmission.md)
        * [Vector-transmission](XmlEntoVector.md)
      * [Interventions](ModelInterventions.md)
    * [Experiments](ExperimentDesign.md)
    * [Output formats](OutputFiles.md)
    * [Monitoring measures](XmlMonitoring.md)
    * [XML snippets](snippets.md)
    * Tools
      * [Windows utilities](UtilsWindows.md)
      * [Scripts for running and analysing scenarios](UtilsRunScripts.md)
      * [Updating schema version](XmlUpdateScenario.md)
      * LiveGraph
    * Technical details
      * [Scenario XML](XmlScenario.md)
      * [Running scenarios](UtilsRunning.md)
      * [Terminology](Terminology.md)
      * [How to apply OpenMalaria to field sites](Instructions.md)
  * [XML syntax (generated doc)](https://github.com/vecnet/om_schema_docs/wiki/GeneratedSchemaDocIndex)
  * **[Model descriptions](ModelsOverview.md)**
    * [Sequencing and time steps](ModelTimeUpdates.md)
    * [Human demography](ModelDemography.md)
    * [Miscellaneous human models](ModelHumanMisc.md)
    * [Transmission models](ModelTransmission.md)
    * [Within-host models](ModelWithinHost.md)
      * [Drug (PKPD) action & resistance models](ModelDrug.md)
    * [Clinical (illness) models](ModelClinical.md)
      * [Morbidity (pathogenesis) models](ModelPathogenesis.md)
      * [Neonatal mortality](ModelNeonatal.md)
    * [Interventions](ModelInterventions.md)
      * [Human-stage](ModelIntervHuman.md)
      * [Mosquito-stage](ModelIntervVector.md)
      * [Special interventions](ModelIntervMisc.md)
      * [Decay of intervention effects](ModelDecayFunctions.md)
  * **Code development**
    * [Changelog](Changelog.md)
    * [Source code](Code#Source_code.md)
    * [Building and Testing](BuildSystem.md)
      * [Building on Linux](UnixBuildOpenMalaria.md)
      * [Building on Mac](MacBuildOpenMalaria.md)
      * [Building on Windows](WindowsBuildOpenMalaria.md)
      * [Testing your build](TestSystem.md)