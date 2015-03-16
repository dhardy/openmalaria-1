# Transmission #

The transmission sub-models within **OpenMalaria** describe inter-human transmission of malaria, taking modeled values of human infectiousness to mosquitoes as input and translating this into subsequent human exposure to infectious mosquitoes (as inoculations per person per year, denoted EIR: Entomological Infection Rate).  The human's relative availability to mosquitoes is taken into account in both cases.  The simulator is designed to simulate seasonally forced transmission,_i.e._, we assume that in the absence of interventions the EIR recurs annually with the same seasonal pattern.

**OpenMalaria** currently supports two distinct sets of algorithms for transmission models:  vector transmission and non-vector transmission.

## Vector transmission ##

_An explicit population model of vectors, including sub-populations infected by and infectious with malaria parasites. Allows modelling of vector interventions._

The vector transmission model used by **OpenMalaria** is required for modeling interventions that have effects on mosquitoes, and hence change the vectorial capacity.

Simulations that include non-periodic changes in the vectorial capacity use a seasonally forced version of the difference equation model for vector dynamics of [Chitnis et al (2008)Journal of Biological Dynamics Vol. 2, No. 3, July 2008, 259–285](http://dx.doi.org/10.1080/17513750701769857), further described in [Chitnis et al (2010) American Journal of Tropical Medicine and Hygiene Vol. 83, No. 2, 230--240](http://www.ajtmh.org/cgi/content/full/83/2/230).

XML parameters for specifying this sub-model are described [here](XmlEntoVector.md).

### Mosquito Population Dynamics ###

Extensions to the vector model, described [here](ModelMosqPopDynamics.md).

## Non-vector transmission ##

_A simple "non vector" model: this assumes a fixed seasonal vectorial capacity, and either forces infection rates (EIR) or makes EIR dependent on human infectiousness to vectors while forcing vectorial capacity. Does not allow interventions that modify vectorial capacity._

When the non-vector model is used, there is no explicit modeling of the mosquito population. Initial exposure of humans to infectious mosquito bits is input and any intervention effects on transmission to the mosquito translate into proportionate effects on transmission back to the human.   The 'Non-vector transmission' sub-model can be used only when there are no interventions affecting the vectorial capacity and is described in:
[Smith et al, 2006.](http://www.ajtmh.org/cgi/content/abstract/75/2_suppl/11)

This model assumes that the mosquito population acts to distribute infections at random to the human population (with human exposure proportionate to availability).  There is a fixed time lag between infection of the mosquito and transmission to humans, corresponding to the duration of the sporogonic cycle in the mosquito.  The effect on transmission of interventions on the human population is simulated by changing the infectiousness to mosquitoes.

The average human infectiousness to mosquitoes, denoted `kappa`, is measured by the function `updateKappa`, according to the formula
```
kappa = sum_{i in pop}( alpha_i * kappa_i ) / sum_i( alpha_i )
```
where `pop` is the human population, `alpha_i` is the availability of human `i` relative to the population's mean availability, and `kappa_i` is the infectiousness of human `i` (units?).

The Entomological Infection Rate [EIR](EIR.md) can be calculated in three modes: `equilibriumMode`, where the daily EIR from the scenario file is used directly, `transientEIRknown`, where, similarly, EIR data from a `changeEIR` intervention in scenario file is used directly, and `dynamicEIR`. The `equilibriumMode` is always used for the initialisation phase, during which `kappa` data per day of year is collected and stored in `initialKappa`. When commencing the intervention phase of the simulation, the mode is usually switched to `dynamicEIR`, which uses the same EIR data as in `equilibriumMode` but scales this by `kappa_d / initialKappa_d` where `d` is the day-of-year.

#### Notes ####

Simulated EIR is delayed relative to changes in `kappa_d / initialKappa_d` by the entomological innoculation duration, usually parameterised in our simulations with the non-vector model as 10 days. However, a bug introduced in schema version 26 reduced this lag to 1 time-step (5-days for the 5-day time-step model). This bug should be fixed in version 30.