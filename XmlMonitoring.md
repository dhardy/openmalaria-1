([Back to scenario file description.](XmlScenario.md))



# Monitoring options #

The monitoring system is described [here](Monitoring.md) and within the [scenario configuration reference](XmlScenario#Monitoring_element:_monitoring.md).

## Units ##

### Special output values ###

Some outputs are the result of averaging a measure over the number of simulated individuals for a specified duration. If the denominator number of individuals in this average is zero, the resulting value isn't valid, so an output value of **nan** (or "-nan" - short for Not A Number) is reported.
To make detection of computation errors easier, we try to avoid outputting **nan** values in discrete survey outputs. The same is not true for continuous outputs: several of these may output **nan** values at the beginning of the simulation or other times.

For the drug costing measures no value is listed at all for drugs never prescribed since the last survey.

### EIR ###

The units we generally use for EIR are _entomological inoculations per time period as measured in adults_ where the time period may be days, years, or the model's time step (currently 1 or 5 days).

Since children are smaller than adults our model assumes they are less likely to be bitten by mosquitoes. However, most information we have on EIR tends to be measured in adults. We try to make our units equivalent, either by only considering bites in adults or by scaling the mean availability of all people in the population (children and adults) compared to a population composed only of adults (this mean relative availability is the "human availability" output available in continuous-reporting mode).

## Continuous measures ##

This output format was introduced in schema version 19; some outputs were added later. There's no central list of these outputs in the code; instead search for calls to `Continuous::registerCallback` -- it's first argument should correspond to the output identifier in the table below.

The following outputs are currently available, and can be enabled by adding a sub-element to `continuous` within the monitoring section of the XML, for example:
```
<continuous period="1">
  <option name="input EIR" value="true"/>
</continuous>
```

All options not otherwise specified are point-time outputs. The 'versions' column lists the OpenMalaria versions for which this output is available, going back as far as version 30 (previous history was not checked; this was last updated for version 32rc2).

| output | versions | description |
|:-------|:---------|:------------|
| N\_v0 | 30 - 32 | The number of mosquitoes that emerge and survive to first host seeking, per day (mosquito emergence rate) |
| N\_v | 30 - 32 | The total number of host seeking mosquitoes |
| O\_v | 30 - 32 | The number of infected host seeking mosquitoes |
| S\_v | 30 - 32 | The number of infectious host seeking mosquitoes |
| P\_A | 30 - 32 | The probability that a mosquito doesn't find a host and doesn't die on given night |
| P\_df | 30 - 32 | The probablity that a mosquito finds a host on a given night, feeds and survives to return to the host-seeking population |
| P\_dif | 30 - 32 | The probability that a mosquito finds a host on a given night, feeds, gets infected with _P. falciparum_ and survives to return to the host-seeking population |
| alpha | 30.1 - 32 | The availability rate of humans to mosquitoes (averaged across human population); units: humans/day (I think) |
| P\_B | 30.1 - 32 | The probability of a mosquito successfully biting a human after choosing, averaged across humans |
| P\_C\*P\_D | 30.1 - 32 | The probability of a mosquito successfully escaping a human and resting after biting, averaged across humans |
| input EIR | 30 - 32 | Requested entomological infection rate. This is a fixed periodic value, for comparison with simulated EIR. Units (from schema version 24): inoculations per adult per timestep. |
| simulated EIR | 30 - 32 | EIR acting on simulated humans. Units: from schema version 26, inoculations per adult per timestep, previously inoculations per person per timestep. |
| hosts | 30 - 32 | Total number of human hosts (fixed) |
| host demography | 30 - 32 | Number of humans less than 1, 5, 10, 15, and 25 years of age respectively |
| recent births | 30 - 32 | Number of new humans since last report |
| patent hosts | 30 - 32 | Number of humans with detectible parasite density |
| human infectiousness | 30 - 32 | Infectiousness of humans to mosquitoes, also known as kappa. This is the probability that a mosquito becomes infected at any single feed on a human. |
| human age availability | 30 - 32 | Mean age-based availability of humans to mosquitoes relative to a human adult (doesn't include any other availability factors, such as vector-model rate or intervention protections). |
| immunity h | 30 - 32 | Average of `_cumulativeh` parameter across humans, which is the cumulative number of infections received since birth |
| immunity Y | 30 - 32 | Average (mean) of `_cumulativeY` parameter across humans, which is the cumulative parasite density since birth |
| median immunity Y | 30 - 32 | Average (median) of `_cumulativeY` parameter across humans, which is the cumulative parasite density since birth |
| new infections | 30 - 32 | Number of new infections since last report, including super infections as with survey measure 43. |
| num transmitting humans | 30 - 32 | Number of humans who are infectious to mosquitoes |
| nets owned | 30 - 31 | Number of people owning a bed net. Note that people cannot own more than one of a single type of net, so this is usually also the number of nets owned. For version 32, use ITN coverage instead. |
| ITN coverage | 32 | The number of people owning any type of net divided by the population size. This does not count nets parameterised with the "GVI" model, only those using the "ITN" model. |
| IRS coverage | 32 | The number of people currently protected by any type of IRS divided by the population size. It does not count IRS configured with the "GVI" model. |
| GVI coverage | 32 | The number of people currently protected by any GVI (generic vector intervention) divided by the population size. Note that even if "GVI" is used to model two very different interventions (e.g. deterrents and nets), this is the coverage by "at least one of" these interventions, not separate coverage levels. This includes nets, IRS and other interventions modelled using the "GVI" intervention model but not those using the separate "ITN" or "IRS" models. |
| mean hole index | 30 - 31 | Average hole-index of all nets (will be not-a-number when no nets are owned) |
| mean insecticide content | 30 - 31 | Average insecticide content of all nets in mg/m² (will be not-a-number when no nets are owned) |
| IRS insecticide content | 30.1 - 31 | Average insecticide content of hut walls over all houses ([new IRS model](ModelIntervVector#Newer_model.md) version 2 only); added in schema 30 |
| IRS effects | 30.1 - 31 | Average effect of IRS on the following three factors: availablity to mosquitoes, preprandial killing, postprandial killing; mean across all humans; both [IRS models](ModelIntervVector#Indoor_Residual_Spraying.md) version 1 and 2; added in schema 30 |
| resource availability | 30.3 - 32 | Mean larval resources over a time-step (1/γ for [these models](ModelMosqPopDynamics.md)) |
| requirements availability | 30.3 - 32 | Only for an as-yet unavailable [mosquito population dynamics model](ModelMosqPopDynamics.md) |

## Survey measures ##

The `SurveyOptions` element controls which survey measures are [output](OutputFiles#measures.md). Omitting an option has the same effect as listing it with value "false". The "measure" in the table should be input into the XML to enable an output, which will appear in the `output.txt` with a numeric "id" (also below). Configuration example:
```
<SurveyOptions>
  <option name="nHost" value="true"/>
  <option name="nPatent" value="true"/>
</SurveyOptions>
```

These tables document the values used in the code. These are defined in [model/mon/OutputMeasures.hpp](https://code.google.com/p/openmalaria/source/browse/model/mon/OutputMeasures.hpp) in recent versions of OpenMalaria (from schema 33) and [include/Monitoring/SurveyMeasure.h](https://code.google.com/p/openmalaria/source/browse/include/Monitoring/SurveyMeasure.h?name=schema-32) in older versions.

The "second column" column indicates the meaning of the contents
of the second column of [output files](OutputFiles.md). The "cohort" column
indicates whether, when per-cohort outputs are enabled, this output measure is
reported seperately for each cohort set output or only once (if a number, it
lists either the first or the first and last OpenMalaria versions for which the
output is per cohort).


| id | measure | second column | cohort | description |
|:---|:--------|:--------------|:-------|:------------|
|  0 | nHost  | age group | y | Total number of humans. **Note**: when using the `IPTI_SP_MODEL` option, humans not at risk of a further episode due to having had a recent episode within the health-system-memory period are subtracted from this and several other outputs (only applies to the `IPTI_SP_MODEL`). |
|  1 | nInfect  | age group | y | The number of human hosts with an infection (patent or not) on the reporting timestep  |
| 2 | nExpectd  | age group | y |  expected number of infected hosts |
| 3 | nPatent  | age group | y | The number of human hosts whose total (blood-stage) parasite density is above the detection threshold |
| 4 | sumLogPyrogenThres | age group | y |  Sum of the log of the pyrogen threshold |
| 5 | sumlogDens  | age group | y|  Sum (across hosts) of the natural logarithm of the parasite density of hosts with detectable parasite density (patent according to the monitoring diagnostic) |
| 6 | totalInfs  | age group | y| The sum of the all infections (liver stage and blood stage, detectable or not) across all human hosts |
| 7 | nTransmit | (unused) |n | Infectiousness of human population to mosquitoes: sum(p(transmit\_i)) across humans i, weighted by availability to mosquitoes. Single value, not per age-group. |
| 8 | totalPatentInf  | age group |y | The sum of all detectable infections (where blood stage parasite density is above the detection limit) across all human hosts.  Includes super-infections.|
| 9 |  |  |  | (removed) |
| 10 | sumPyrogenThresh  | age group |y |  Sum of the pyrogenic threshold |
| 11 | nTreatments1  | age group |24| number of treatments (1st line) (added to 1-day model in 24.1) |
| 12 | nTreatments2  | age group |24| number of treatments (2nd line) (added to 1-day model in 24.1) |
| 13 | nTreatments3  | age group |24| number of treatments (inpatient) (added to 1-day model in 24.1) |
| 14 | nUncomp  | age group |24|  number of episodes (uncomplicated) An episode of uncomplicated malaria is a period during which an individual has symptoms caused by malaria parasites present at the time of illness, where the symptoms do not qualifying as severe malaria.  The maximum length of the period of an episode is referred to as the health system memory and is generally set to 30 days: illness recurring within this period counts as the same episode.  Illness recurring over a longer duration than this is counted as more than one episode.   An illness caused by a pathogen other than malaria does not count as a malaria episode even if there is incidental parasitemia |
| 15 | nSevere  | age group |24|  number of episodes (severe) Severe malaria is a potentially life-threatening  disease, diagnosable by clinical or laboratory evidence of vital organ dysfunction, requiring in-patient care. An episode of severe malaria is a period during which an individual has symptoms, qualifying as severe malaria, caused by malaria parasites present at the time of illness.  As with uncomplicated malaria, the maximum duration of an episode is set to the health system memory: illness recurring over a longer duration than this is counted as more than one episode.   |
| 16 | nSeq  | age group |24|  recovered cases with sequelae |
| 17 | nHospitalDeaths  | age group |24|  deaths in hospital Malaria hospitalisations in OpenMalaria are severe malaria episodes simulated as receiving in-patient care. |
| 18 | nIndDeaths  | age group |24|  number of deaths (indirect) These are deaths that occur because of malaria infection but that do not satisfy the definition of direct malaria deaths.  These comprise neonatal deaths secondary to malaria in pregnancy, and deaths resulting from interactions between pathogens where malaria plays an essential role, but the terminal illness does not satisfy the definition of severe malaria. |
| 19 | nDirDeaths  | age group |24|  number of deaths (direct).  This is the number of severe malaria episodes that result in death. |
| 20 | nEPIVaccinations | age group |24|  number of EPI vaccine doses given |
| 21 | allCauseIMR | (unused) | n | all cause infant mortality rate (returned as a single number over whole intervention period, instead of from a survey interval) |
| 22 | nMassVaccinations  | age group |24|  number of Mass / Campaign vaccine doses given |
| 23 | nHospitalRecovs  | age group |24|  recoveries in hospital without sequelae |
| 24 | nHospitalSeqs  | age group |24|  recoveries in hospital with sequelae  |
| 25 | nIPTDoses  | age group |24| number of IPT Doses (removed in version 32) |
| 26 | annAvgK   | (unused) | n| Annual Average Kappa. The  probability that a mosquito bite results in the mosquito becoming infected, averaged over all mosquito bites. Calculated once a year. Note: the actual calculation weights by human body size and by the _input_ (pre-intervention) EIR to approximate the number of biting mosquitoes). |
| 27 | nNMFever  | age group |24|  Number of episodes of non-malaria fever |
| 28 |  |  |  | (removed) |
| 29 |  |  |  | (removed) |

| id | measure | second column | cohort | description |
|:---|:--------|:--------------|:-------|:------------|
| 30 | innoculationsPerAgeGroup | age group |n | The total number of inoculations per age group, summed over the reporting period. (Units are not adjusted to account for reduced child availability to mosquitoes.) |
| 31 | Vector\_Nv0 | vector species | n | Number of emerging mosquitoes that survive to the first feeding search per day at this time-step (mosquito emergence rate). |
| 32 | Vector\_Nv  | vector species | n |  Host seeking mosquito population size at this time step. |
| 33 | Vector\_Ov  | vector species | n |  Number of infected host seeking mosquitoes at this time step. |
| 34 | Vector\_Sv  | vector species | n |  Number of infectious host seeking mosquitoes at this time step. |
| 35 | inputEIR   | (unused)       | n | (Previously Vector\_EIR\_Input.) Input EIR (rate entered into scenario file for vector/non-vector model). Units (schema 24 and later): average inoculations per adult over the time period since the last survey measured in infectious bites per person per time step. |
| 36 | simulatedEIR | (unused)     | n | (Previously Vector\_EIR\_Simulated.) EIR generated by transmission model as measured by inoculations recieved by adults. Units as above (output 35). |
| 37 |  |  |  | (removed) |
| 38 |  |  |  | (removed) |
| 39 | Clinical\_RDTs  | (unused) | 24-31 |  Number of Rapid Diagnostic Tests used |
| 40 | Clinical\_DrugUsage  | drug ID | 24-31 | Quantities of oral drugs used, per active ingredient abbreviation (mg) |
| 41 | Clinical\_FirstDayDeaths  | age group | 24 |  Direct death before treatment takes effect |
| 42 | Clinical\_HospitalFirstDayDeaths  | age group | 24 |  Direct death before treatment takes effect; hospital only |
| 43 | nNewInfections | age group | y | The number of infections introduced since the last survey, per age group. This counts super-infections without the usual limit of 21 concurrent infections, so in some ways is similar to introduction of infections in an infinite population. |
| 44 | nMassITNs | age group |24| The number of ITNs delivered by mass distribution since last survey. |
| 45 | nEPI\_ITNs | age group | 24 | The number of ITNs delivered through EPI since last survey. |
| 46 | nMassIRS | age group | 24 | The number of people newly protected by IRS since last survey. |
| 47 | nMassVA | age group | 24 | The number of people newly protected by a vector-availability intervention since the last survey. |
| 48 | Clinical\_Microscopy  | (unused) | 24-31 | Number of microscopy tests used |
| 49 | Clinical\_DrugUsageIV | drug ID | 24-31 | Quantities of intravenous drugs used, per active ingredient abbreviation (mg) |
| 50 | nAddedToCohort | age group | 32 | Number of individuals added to any cohort. Note that the reporting happens _after_ updating membership details (Removed in version 32). |
| 51 | nRemovedFromCohort | age group | 32 | Number of individuals removed from any cohort. Note that the reporting happens _before_ updating membership details (Removed in version 32). |  |
| 52 | nMDAs | age group | y | Number of drug doses given via mass deployment (MDA or screen&treat) (where configured as screen&treat, etc., this only reports treatments actually prescribed). As of schema 26, this output only reports anything on a 1-day time-step; in the future it will also work on a 5-day timestep. |
| 53 | nNmfDeaths | age group | y | Direct deaths due to non-malaria fevers |
| 54 | nAntibioticTreatments | age group | y | Report number of antibiotic treatments administered |
| 55 | nMassScreenings | age group | y | Report number of screenings used by MDA/MSAT when deployed via timed deployment |

#### Added in schema 32 ####

| id | measure | second column | cohort | description |
|:---|:--------|:--------------|:-------|:------------|
| 56 | nMassGVI | age group | y | Report the number of mass deployments of [generic vector interventions](ModelIntervVector#Generic_vector_intervention_(GVI).md) |
| 57 | nCtsIRS | age group | y | Report the number of [IRS](ModelIntervVector#Indoor_residual_spraying_(IRS).md) deployments via age-based deployment |
| 58 | nCtsGVI | age group | y | Report the number of [GVI](ModelIntervVector#Generic_vector_intervention_(GVI).md) deployments via age-based deployment |
| 59 | nCtsMDA | age group | y | Report the number of [MDA](ModelIntervHuman#Mass_Drug_Administration.md) deployments via age-based deployment |
| 60 | nCtsScreenings | age group | y | Report the number of screenings used by MDA/MSAT when deployed via age-based deployment |
| 61 | nSubPopRemovalTooOld | age group | y | Number of removals from a sub-population due to expiry of duration of membership (e.g. intervention too old). |
| 62 | nSubPopRemovalFirstEvent | age group | y | Number of removals from a sub-population due to first infection/bout/treatment (see onFirstBout & co). |
| 63 | nPQTreatments | age group | y | Number of treatments given with primaquine (P. vivax model only). |
| 64 | nTreatDiagnostics | age group | y | Number of diagnostic tests performed (if in the health system description, useDiagnosticUC="true"). |