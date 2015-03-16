# Scenarios and Experiments #

OpenMalaria can simulate only a single scenario at once; each is defined by one XML document. Using models to make predictions usually involves running many distinct simulations to compare the effects of changes in model structure or parameters.  This section considers only how to set up a single simulation (_scenario_).  If you want to design an experiment evaluating the impact of one of more factors, and hence running many _scenarios_, you may want in addition to read about [designing experiments](ExperimentDesign.md).

## Scenarios in OpenMalaria ##

Each scenario is defined by one input file [written in XML](ScenarioXML.md). This input file describes:

  * Fixed model parameters, usually derived from some study or a model fitting run. To some extent parameters may be adapted to local situations but many parameters are fairly broad in applicability (e.g. to _P. falciparum_ or _Anopheles Gambiae_)
  * Local transmission data
  * Local health system data
  * Local interventions — e.g. changes to the health system, net deployment campaigns, larviciding interventions
  * Monitoring configuration — what data is returned, how it is categorised and when surveys are collected

### Time line ###

OpenMalaria can simulate a steady-state of malaria transmission (with or without health system coverage), followed by one or more interventions and monitoring over a follow-up period. It is less good at simulating low levels of transmission and when historical transmission does not follow some consistent seasonal pattern.

Simulations have essentially two phases: a **warm-up phase** which is used to initialise human-immunity models (by simulating a whole lifespan of transmission) as well as transmission models, and an **intervention phase** during which interventions may be deployed and the results monitored. OpenMalaria automatically handles the warm-up phase, while events in the intervention phase are determined by configuration of interventions and monitoring surveys. [More details on time...](ModelTimeUpdates.md)


## Design of scenario files (XML) ##

#### Snippets ####

A [library of XML snippets](snippets.md) has been started to collect templates and parameters useful in creating scenarios.

#### XML overview ####

(If you are not familiar with XML, consult the [XML basics](ScenarioXML.md) page.) Copy one of the `scenario.xml` documents from the snippet library linked above (e.g. [v32](https://code.google.com/p/openmalaria/source/browse/v32/scenario.xml?repo=snippets)). It should have this layout:

```
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<om:scenario name="example" ...>
  <demography .../>
  <monitoring .../>
  <interventions .../>
  <healthSystem .../>
  <entomology .../>
  <parasiteGenetics .../> <!-- only in v33+ when modeling drug resistance -->
  <diagnostics .../> <!-- only in v33+ and not required -->
  <pharmacology .../> <!-- only needed when modeling drugs with 1-day time-step models -->
  <model>
    <ModelOptions .../>
    <clinical .../>
    <human .../>
    <parameters interval="5" iseed="0" latentp="3">
      ...
    </parameters>
  </model>
</om:scenario>
```

Most of the details are not shown. It should be noted that `model/parameters/interval` is the time step (in days) to be used; only 1 and 5 are supported and each implies further restrictions on which models can be used. Further, `model/parameters/iseed` is the seed used to initialise the psuedo-random number generator; usually this should be different for each scenario in an experiment and a prime number.

### Within-host models ###

### Other models ###

The following pages describe other models used:

  * [Demography model](ModelDemography.md): configures population size and age distribution
  * When designing a scenario, you will need to think about what kind of output is required. See [Monitoring](Monitoring.md) and the [list of available measures](XmlMonitoring.md).
  * [Interventions](ModelInterventions.md): Enact some change in the simulation (e.g. administering vaccines, deploying bed nets or changing the health system). Also some "psuedo interventions" such recruit cohort members or uninfecting vectors.
  * [Health system](ScenarioHealthSystem.md): configures treatment seeking behaviour, diagnosis and delivery, as well as some health outcomes. Two models are available, depending on the length of time-step used ([descriptions](ModelClinical.md)).
  * [Transmission model](ModelTransmission.md): configures the model of the vector(s) carrying infections. [(Configuration.)](ScenarioTransmission.md)
  * [Pharmacology data](ScenarioPharmacology.md): library of drug model parameters for use in simulation of antimalarial drugs (1-day timestep only)


## Model of Natural History ##

The `model` element of he scenario file defines the model of the natural history of an individual malaria infection. A critical element here is the `parameters` element which has an `interval="5"` attribute; this specifies the length (in days) of the fundamental time-step of the simulator. Some models of natural history work on a 5-day time-step, and some on a 1-day time-step (which has extra capabilities but results in a more complex and slower model). Note that several other parts of the configuration are entered using units of the length of a time-step.

The main choices to make with regards to the model to use is the [within-host / infection model](ModelWithinHost#Asexual_infection_models.md):
  * "Descriptive" model: 5-day time-step; several parameterizations have been fit for this model
  * "Empirical" model: 1-day time-step; currently has not been parameterized
  * Molineaux model: 1-day time-step; parameter fitting is currently in progress

This will also determine which [clinical / health system model](ModelClinical.md) may be used:
  * 5-day timestep model with simple cure/fail response to treatment and immediately determined clinical outcome
  * 1-day timestep model: treatments use full PKPD modelling with a daily effect on parasite densities; clinical outcome determined by both the parasite density and clinical decisions

After selecting the within-host model to use, an appropriate `<model>...</model>` element should be copied. Note that while the sub-elements of `model` (in particular `ModelOptions`) _can_ be changed, in theory they have an effect on the optimal paramerization(s), so changing them would result in an invalid model.

The `model` element also includes data for some [small human-related models](ModelHumanMisc.md).

Our standard set of 14 parameterisations for the 5-day timestep can be downloaded from the link below (models-VER-5-day.7z where VER is the [schema version](Changelog.md)). There are some example experiments listed alongside. You will need a [7-zip archive program](http://7-zip.org/).

http://code.google.com/p/openmalaria/source/browse?repo=svn-archive#git%2Fdownload%2Fexperiments

The `pharmacology` element is only required when a 1-day time-step is used, and is essentially a library of parameters for the drug model. It should therefore not need to be edited (just copied from a source).