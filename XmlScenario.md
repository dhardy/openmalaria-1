Note: the following is an older piece of documentation, kept in case it is still of use. It has not been updated to describe the latest scenario format.

# Introduction #

The main input to the simulator is an [XML](http://www.w3schools.com/xml/default.asp) file generally called `scenario.xml` (though it may be named something different if the `--scenario FILE` command-line option is passed to OpenMalaria). The XML file has several sections corresponding to specific model blocks.   If you are new to XML, there is a nice tutorial of the XSD schema  [here](http://www.w3schools.com/schema/default.asp). You can view our base scenario [here](http://code.google.com/p/openmalaria/source/browse/trunk/schema/scenario.xsd).


## Scope of documentation ##

This documentation is intended to aid individuals modifying existing scenario files, either by hand or with the scenario editor currently under development.

## Contents ##



# Scenario element: `scenario` #

The primary element in the file is called "scenario" (this encapsulates everything else in the file, except the XML header line).

It has the following attributes:

| attribute         | data type | purpose                                         |
|:------------------|:----------|:------------------------------------------------|
| schemaVersion     | int       | Version of XSD schema to use; should correspond to one of the `scenario_VER.xsd` files in [test](http://code.google.com/p/openmalaria/source/browse/#svn/trunk/test) |
| analysisNo        | int       |                                                 |
| name              | string    | user-friendly name for the scenario             |
| wuID              | int       | BOINC workunit number (automatically set when needed) |
| assimMode         | 0 or 1    | removes 3 columns from output.txt for BOINC usage (use 0 for local scenarios) |

## Demography element: `demography` ##

Describes the population size and age structure. Most of our scenarios use the same age-structure data from Ifakara, Tanzania. Adjusting the population size is useful, since it has a major impact on run-time and stochasticity of results (1000-100000 individuals are often used in experiments; testing scenarios often use less).

## Monitoring element: `monitoring` ##

The `monitoring` element is used to describe two types of output: continuous (or at least regular) reporting of single values, and survey data, which may be irregular and is usually sub-divided by age-group or another differentiator.

### Measures: `continuous`, `SurveyOptions` ###

The `continuous` and `SurveyOptions` elements describe which outputs to enable for the two outputs, using a list of sub-elements of the form `<option name="NAME" value="true"/>`; lists of valid options are below.

[Tables of continuous and survey measures.](XmlMonitoring.md)

### Continuous reporting: `continuous` ###

Continuous reporting, when enabled (a `continuous` element is described and `period` is positive), outputs data every `period` days (`period` is an attribute of the `continuous` element).

This output normally starts from the beginning of the intervention period, but for diagnostic purposes it can be enabled from the beginning of initialization by including a `duringInit="true"` attribute. This has two additional side effects: an extra column, `simulation time`, is output as the first column, and where `period` is greater than the length of a timestep, reporting (during intervention period) may happen on different timesteps.

### Surveys: `surveys`, `ageGroup` ###

The `surveys` element has a list of `surveyTime` sub-elements, each with the timestep of a survey, starting from 0, the start of the intervention period (numbers match timesteps at which interventions can be deployed). A survey at time `x` takes place at the beginning of that timestep, so, for example, if the first survey has time 73 with a 5-day timestep, it covers exactly the first year of the intervention period.

Timestep 0 will always correspond to the first of January (up to 5th Jan with a 5-day timestep), except that, prior to schema 22, when the `maximumAgeYrs` attribute of `demography` was not a whole number of years timestep values may well have been offset. Years are always modelled as 365 days long, hence `(n * 365 / days_per_timestep)` is always the beginning of the n-th year of the intervention period (excepting above offset).

Note: The largest `surveyTime` sub-element value determines the length of the intervention period (and thus the number of post-warmup time steps that the simulation will run; see [Model Time Updates.](ModelTimeUpdates.md)).

The `ageGroup` element contains a list of `group` sub-elements, each of which has an `upperbound` attribute. These (and the `lowerbound` attribute of `ageGroup`) define the age-groups which reports are divided into. The age-group specified in output data is the index within these groups, ordered by age (youngest to oldest), with the first age-group having index `1`.

## Deployment of interventions: `interventions` ##

The `interventions` element is organized informally into three sections, first descriptions of interventions (without associating times), second continuous interventions, and third timed interventions.

### Descriptions ###

Intervention descriptions are, at this time, all direct sub-elements of `interventions` and not some other element.

The `MDADescription` element is only required when using _Mass Drug Administration_ interventions on a one-day timestep. It can then take a `schedule` sub-element describing a treatment dose or schedule taken by individuals covered by the administration campaign. There are currently no adherence/dose size/quality effects varying this; i.e. individuals either are not covered by the campaign and receive nothing or are and receive the full schedule.

The `vaccineDescription` element describes vaccines.

The `iptiDescription` describes _Intermittent Preventive Treatment for Infants_ . One important point: including this element also enables the IPT drug/within-host model, which adds a protective effect to every treatment. (This behaviour could be changed in the future to use a separate model-option switch.)

The `anopheles` element, if used, should be listed for each mosquito species (using the `mosquito` attribute to associate with one of the mosquito species specified in entomological data). Each may contain descriptions for _Insecticide Treated Net_ (`ITNDescription`), _Indoor Residual Spraying_ (`IRSDescription`) and a mosquito repellant (`VADescription` — _Vector Availability_) intervention. Theses have sub-elements describing a parameter as a Weibull decay curve. All three have a `deterrency` parameter; additionally `ITNDescription` has `preprandialKillingEffect` and `postprandialKillingEffect` (chance of killing a mosquito before or after feeding) and `IRSDescription` has `killingEffect` (chance of killing mosquito while resting) parameters.

### Continuous: `continuous` ###

Interventions with continuous deployment are described by a `continuous` element. This accepts sub-elements `vaccine`, `ITN` and `ipti` (see descriptions above); these may occur more than once. (They must be listed in order of type; that is all `vaccine` elements before any `ITN` elements, etc.) Each has the same type with the following attributes:

| `targetAgeYrs` | Intervention has a chance of being given to the human at this exact point of age. |
|:---------------|:----------------------------------------------------------------------------------|
| `coverage` | Chance of human recieving intervention at given point of age. |
| `begin` | First timestep at which this item becomes active (defaults to 0, the start of the intervention period). |
| `end` | First timestep at which this item becomes deactive (defaults to the maximum value for an int, 2<sup>31</sup>-1). |

Multiple elements may be used for each intervention, e.g. to administer at several time points or make the intervention active for multiple independant time periods (if end-time = begin-time for two items, they meet with no overlap).

### Timed: `timed` ###

Timed interventions correspond to mass deployment at a specific time point. The `timed` element accepts a list of `intervention` sub-elements, each of which describes interventions occuring at a time specified by its `time` attribute. The following interventions may occur (and should be listed in this order within each `intervention`):

| `changeHS` | Replaces the current health-system description with a new one. Must be a complete description of the health system (contain an `ImmediateOutcomes` or `EventScheduler` element, and a `CFR` element, as with the usual `healthSystem` element) |
|:-----------|:-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `changeEIR` | Replaces EIR data with new, daily, data described here. Data must be sufficient to last until the end of the simulation. |
| `importedInfectionsPerThousandHosts` | Not an intervention, but deployed the same way: a floating-point number describing the chance each individual recieves an infection (hence number introduced is unlikely to be exactly the number specified) |
| `MDA` | Mass drug administration (clears infections directly for models with a 5-day timestep and administers drugs for models with a 1-day timestep) |
| `vaccinate` | Mass vaccination campaign |
| `ITN` | Mass ITN-deployment |
| `IRS` | IRS spray round |
| `VectorAvailability` (VectorDeterrent) | Mass mosquito-repellent deployment (e.g. house spraying) |
| `ipti` | Mass IPTi campaign |
| `larviciding` | Mosquito emergence-rate reduction intervention. Has an `anopheles` sub-element for each mosquito sub-species, containing the attributes: `mosquito` (name of sub-species), `effectiveness` (proportional reduction in mosquito emergence), `duration` (number of days the intervention is effective for). |

The `MDA` through to `ipti` interventions above are all mass deployment. Each has its deployment described by the following attributes: `maxAge` and `minAge` describe the age-range applicable in years (optional; they default to 100 and 0 respectively), and `coverage` specifies the chance that each individual receives the intervention.

## Drug parameters: `drug` ##

This describes PK and PD parameters of drugs. In the future initial levels of resistance need to be described (in fact, these are to some extent now, but the schema for entering this data may change). For now, just copy this section from another scenario which describes the desired drugs.

## Model parameters: `model` ##

Finally, `scenario` contains a `model` element, grouping some of the data describing the "model" together.

### `ModelOptions` ###

The `ModelOptions` sub-element is a list of binary options, in the same format as `SurveyOptions` (except here, bug-fix options default to true if not specified, while the rest still default to false). The available options are:



&lt;placeholder&gt;



(For now, see [ModelOptions.hpp](https://code.google.com/p/openmalaria/source/browse/include/util/ModelOptions.h).)

### `clinical` ###

The `clinical` element currently simply has a `healthSystemMemory` attribute: if, when a sickness bout occurs, the previous episode was less than or equal to this many time-steps ago, the new bout is considered a second case (second-line treatment may be used and the two bouts will likely be reported as a single episode). The `healthSystemMemory` is defined in time steps. In the 5 day model, a value of 6 corresponds to 35 days (_not the 30 days previously claimed_).

(The main health-system description is [above](XmlScenario#Health_system.md).)

### `parameters` ###

The `parameters` element describes a list of parameters which are fitted. Usually, one can just copy this section from an _appropriate_ scenario. Varying the length of the timestep or changing several of the model's options will affect the ideal values of these parameters. Ideally we should make a list of all fit parameter sets, together with the conditions under which they are valid, publically available somewhere.

A list of which parameters may be specified can be found here: [inputData.h](http://code.google.com/p/openmalaria/source/browse/trunk/include/inputData.h) (the numbers, rather than names, are used to specify each parameter in scenario files).

This element also contains some attributes (placed _here_ for historical reasons):

  * `interval` — number of days in a time-step
  * `iseed` — number to seed random-number-generator with
  * `delta` — unused and removed in schema version 19
  * `latentp` — number of timesteps by which blood-stage infection is delayed from biting