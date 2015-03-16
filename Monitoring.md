# Monitoring #

Part of the simulation design process is also deciding which results of the simulation should be captured with a monitoring statement. Not all the data produced by the simulation is needed to interpret the results: mosquito-to-human transmission intensity, parasite density per infection per human together with human infectiousness to mosquitoes, bouts of sickness and treatments affecting each human, etc.  A summary of those events over all the simulated population will be more telling than the detailed experience of each simulated individual.

The monitoring method is described by an XML element of the form below. The `continuous` element is optional and describes continuous reporting, while the other three elements are required and describe surveys.

```
  <monitoring name="(some description or name)" cohortOnly="false">
    <continuous ... />
    <SurveyOptions ... />
    <surveys ... />
    <ageGroup ... />
  </monitoring>
```

`cohortOnly` may be `true` or `false` or, if not using cohorts, omitted entirely. See below.

## Surveys ##

Surveys are periodic summaries at predefined time-steps.  At those time-steps, information is saved about the quantity of interest,  this could be a measurable quantity such as the EIR, or number of events since the last survey time point.  Where this data concerns humans, it can be segregated into age groups, and can be sampled either across all simulated individuals or from a selected cohort.

### Survey measures ###

Exactly which measures will be output by a simulation depends on the `SurveyOptions` element. This contains a list of options, for example:

```
    <SurveyOptions>
      <option name="nHost" value="true"/>
      <option name="nPatent" value="true"/>
      <option name="nMassVaccinations"/>
    </SurveyOptions>
```

(The `value` attribute is optional and can be omitted as in the last example. It can also be explicitly set to "false" which is the same as omitting the option entirely.)

A list of all outputs currently implemented can be found [here](XmlMonitoring#Survey_measures.md).

### Cohort-modes ###

#### From version 32 ####

Multiple cohort outputs possible 

&lt;TODO&gt;



#### Prior to version 32 ####

By default, survey data comes from the whole simulated population. In order to restrict human measures of all surveys to only a cohort, set `cohortOnly="true"`:

```
<monitoring name="..." cohortOnly="true">
  ...
</monitoring>
```

If `cohortOnly` is set to `true`, then the output of most measures are restricted to the cohort (although not quite all; see the [list of measures](XmlMonitoring.md) for details); if it is set to `false` or omitted, then these measures act on the entire simulated population. It is currently not possible to get outputs for both the cohort and the entire population except by running the simulation twice; this will be addressed in the future.

A cohort must also be selected. To do this, (pseudo-)interventions have been added to select individuals either from a proportion of the population at some time point, and to continuously select some proportion of individuals as they reach a set age. See [cohort-selecting pseudo interventions](ModelIntervMisc#Cohort_selection.md).

Note: as of schema 32, multiple cohorts may be used, but the output format has not been updated to handle per-cohort outputs. In this version, `cohortOnly="true"` means output comes from humans enrolled in _any cohort_.

#### Time to first event ####

Three options exist to allow measuring _time-to-first-event_:

| firstBoutOnly | Remove individuals from the cohort at the start of their first episode (clinical bout) | Intended for measuring time to first episode with active case detection |
|:--------------|:---------------------------------------------------------------------------------------|:------------------------------------------------------------------------|
| firstTreatmentOnly | Remove individuals from the cohort when they first seek treatment | Intended for measuring time to first episode with passive case detection |
| firstInfectionOnly | Remove individuals from the cohort at completion of the first survey in which they present with a patent infection | Intended for measuring time to first infection, using active case detection |

These are enabled by adding an attribute to the `monitoring` element, for example (here also enabling `cohortOnly`):

```
<monitoring name="..." cohortOnly="true" firstBoutOnly="true">
  ...
</monitoring>
```

### Survey time points ###

Surveys can take place at any time point, starting from the beginning of the intervention period. Surveys only report events which happened from the beginning of the time-step of the last survey until the end of the time-step _before_ the current survey time-step, and measures of the current state (such as the number of patent hosts) from the _beginning_ of the survey time-step.

The timing of these is described as in the XML fragment below. The first valid time-point for a survey is time-step 0; however, any events happening before time-step 0 are not reported, so measures of events (such as infectious bites received) will be zero.

```
<surveys detectionLimit="40">
  <surveyTime>18</surveyTime>
  <surveyTime>37</surveyTime>
  <surveyTime>55</surveyTime>
  <surveyTime>73</surveyTime>
</surveys>
```

This example describes four quarterly surveys (assuming a 5-day time-step); these are reported in the [output.txt](OutputFiles#Surveys.md) file as surveys 1, 2, 3 and 4. (The `detectionLimit` attribute describes the minimum parasite density (parasites per µl/microlitre) at which a human is counted as having a patent infection.)

Timestep 0 will always correspond to the first of January (up to 5th Jan with a 5-day timestep), except that, prior to schema 22, when the maximumAgeYrs attribute of demography was not a whole number of years timestep values may well have been offset. Years are always modelled as 365 days long, hence (n **365 / days\_per\_timestep) is always the beginning of the n-th year of the intervention period (excepting above offset).**

### Survey age groups ###

The `ageGroup` element describes which age-groups human-specific data is segregated into. Examples:

```
<ageGroup lowerbound="0.0">
      <group upperbound="0.25"/>
      <group upperbound="0.5"/>
      <group upperbound="0.75"/>
      <group upperbound="1"/>
      <group upperbound="1.5"/>
      <group upperbound="2"/>
      <group upperbound="3"/>
      <group upperbound="4"/>
      <group upperbound="5"/>
      <group upperbound="6"/>
      <group upperbound="7"/>
      <group upperbound="8"/>
      <group upperbound="9"/>
      <group upperbound="10"/>
      <group upperbound="12"/>
      <group upperbound="14"/>
      <group upperbound="16"/>
      <group upperbound="18"/>
      <group upperbound="20"/>
      <group upperbound="25"/>
      <group upperbound="30"/>
      <group upperbound="35"/>
      <group upperbound="40"/>
      <group upperbound="45"/>
      <group upperbound="50"/>
      <group upperbound="55"/>
      <group upperbound="60"/>
      <group upperbound="65"/>
      <group upperbound="70"/>
      <group upperbound="99"/>
    </ageGroup>
```
```
    <ageGroup lowerbound="0.0">
      <group upperbound="99"/>
    </ageGroup>
```
Here the upper bound is inclusive, so exact age 1 is included under age group 4 (with upper bound 1), but exact age 0 is always included in the first age group. Note also that this is no implicit catch-all last age group, so if the upper bound on the last age group is less than the oldest age allowed, then humans who are older than the last upper bound will simply not be included in surveys.

## Continuous ##

Whereas survey reporting is designed to aggregate data into configurable-size lumps, the continuous reporting mechanism is designed to report some data at high frequencies (but generally without segregation by age group and with less configuration potential).

To enable continuous reporting, add a `continuous` sub-element to the `monitoring` element, of the following form.
```
<continuous duringInit="false" period="1">
  <option name="input EIR" value="true"/>
  <option name="simulated EIR" value="true"/>
  <option name="human infectiousness" value="true"/>
</continuous>
```
The `period` attribute specifies the number of time-steps between reports. `duringInit` is mostly used for debugging and can be omitted entirely; if set to true it enables reporting during the warm-up period and an extra column in the output (simulation time, the time-step counting from the beginning of the simulation rather than the beginning of the intervention period).

A list of available outputs is [here](XmlMonitoring#Continuous_measures.md).