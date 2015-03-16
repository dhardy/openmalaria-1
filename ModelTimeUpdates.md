

## Basic premises ##

  * time steps are one or five days long (or both: the vector model and certain within host models use a one day time step internally even when the main simulation uses a five day step)
  * a year is exactly 365 (= 5 × 73) days; leap years are not modeled
  * time 0 usually corresponds to 1st January, though up to version 32 this is neither explicitly set nor required
  * the maximum human age is constant, defined in the scenario XML document (`maximumAgeYrs` attribute of the scenario's `<demography>` element)
  * there are two measures of time in the simulator: time since the start of the simulation (used internally but not seen in scenario XML or outputs) and time since the start of the "intervention period"


# Simulation phases #

The simulation proceeds roughly as follows:

  * the XML document is read, model constants are set and some variables initialised
  * an initial human population is created
  * a warm-up period of one human life span is run
  * some extra simulation to initialise the transmission model may happen
  * the intervention phase starts

## Warm-up ##

The simulation is run with enforced transmission up to time **`t = tWL`**, where `tWL` is at least as long as the human life span and a whole number of years.

The purpose of this period is to give all simulated humans a life-time's history of immunity. It has been speculated that this could probably be optimised away via some model for allocating initial immunity, but no work has been done to develop such a model (which must account for EIR and human heterogeneity at least, and probably several model variants and parameters).

## Transmission initialization iteration ##

At this point, the transmission model's **`initIterate()`** method is called. This method reports either that the transmission model is sufficiently well initialized (in which case the next phase is immediately started) or gives a length of time to continue the simulation for before this method is called again.

In the [non-vector model](ModelTransmission#Non-vector_transmission.md),  **`initIterate()`** prepares the model to switch to dynamic transmission mode (if enabled) and indicates the next phase should be started.

In the [vector transmission model](XmlEntoVector.md), **`initIterate()`** checks whether or not the simulated EIR could significantly better reproduce the EIR requested in the scenario file or not. If so, it makes some adjustments and indicates the simulation should continue for six years before calling **`initIterate()`** again.  Once it is confirmed that the simulated EIR sufficiently approximates the requested EIR, tje simulator indicates the simulation should be run for another year to let mosquito parameters stabilize before indicating that the next phase, dynamic transmission, may begin.

In the event that the the model has attempted to initiate itself 10 times without success, the simulation exits with an error.

## Intervention period ##

Finally, the so called "intervention period" starts: the period during which interventions may be deployed and measurements of the simulation may be taken. All times seen in the XML document and monitoring outputs are relative to the start of this period.

Only once the intervention periods starts are the constants **`tI₀`** and **`tEnd`** known for certain. At this point the **`tI`** time measure starts being useful.


# Time steps #

The model runs in terms of _time steps:_ periods of fixed length over which updates happen. During a step update, the two main times of interest are the start and the end of this step. From schema version 33, these are available as `sim::ts0()` and `sim::ts1()` respectively (from `include/SimTime.h`); before this only the end time was directly available, as `TimeStep::simulation` (from `include/util/TimeStep.h`).

In OpenMalaria, process updates (natural inception and progression of infections, sickness events, case management and resulting outcomes, etc.) happen during a time step update. Monitoring surveys and intervention deployment, however, are modelled as happening instantaneously, between these updates (e.g. a survey at time 365 days reports on the state after the update step ending at 365 days (e.g. patency) and events happening since the previous survey or time 0 (e.g. number of deaths due to malaria)).

These times are best thought of as being midday (e.g. time 0 being noon on 1st January) and step updates as progressing from midday to midday (over one or five nights). This way monitoring, intervention deployments and on a one-day step case management all happen at midday.

Summary: at time **`t`** (`sim::now()` from version 33), continuous outputs and surveys due at this time are finalised, then interventions due at this time are deployed. To update from time **`t`** (`sim::ts0()`) to **`t+1`** (`sim::ts1()`), the human population and the transmission model are updated.

## `Population::update1` ##

This function updates humans and the transmission model:

  * neonatal mortality is updated based on the previous time-step's patent hosts
  * the transmission model is updated based on previous infectiousness of humans
  * **`Human::update`** is called for each human
  * population age structure is maintained by out migration
  * population size is maintained by adding newborns
  * transmission-specific summary statistics are updated

## `Human::update` ##

This function updates a single human:

  * number of _P.f._ innoculations is calculated and new infections are  started
  * the parasite densities are of infected humans are updated. If the 1-day time step model is used, these densities may be modified by the drugs given to the patient
  * indirect deaths (neonatal and otherwise) occur
  * pathogenesis state is updated based on the new parasite densities (_i.e._, new illnesses start)
  * In the 5-day time-step model, diagnosis, medication and treatment occurs
  * infant death statistics are updated
  * infectiousness of human is calculated
  * Humans who reach the end of the human life span (see calculation of **`tWL`** above) die at the end of the timestep (after continuous and discrete survey measures are taken)

Note that in five-day step models parasite density, ensuing disease, treatment seeking and most outcomes are all simulated as happening during the same step. [This may hide high parasite densities from the monitoring system by treating or killing the host before monitoring happens, at the end of the step.](ScenarioHealthSystem#Treatment_actions.md)

# Specifying times #

Up to version 32, all durations and times are specified in relative terms, often in units of time steps. In the case of intervention deployment and survey times, the time is relative to the start of the intervention period.

From version 33, the unit of a time duration may be specified:

| 0 | zero-length duration (unit not required) |
|:--|:-----------------------------------------|
| 1t | one time step (i.e. 1 or 5 days) |
| 15d | fifteen days (i.e. 3 or 15 steps) |
| 1.2y | one plus one fifth years (i.e. 1.2 × 365 days, rounded to the nearest step) |

Intervention deployment and survey times may instead be specified via dates. This is done by defining the date of the start of the intervention period:

```
  <monitoring name="..." startDate="2000-01-01">
```

and then the deployment date (again in the format `yyyy-m-d`). For example:

```
    <deploy time="2000-4-11"/>
```

corresponds to a relative deployment time of 100 days or 20 steps (31 + 28 + 31 + 11 - 1 = 100). Note that February always has 28 days in OpenMalaria.


# Terminology #

Used on this page:

| variable | name in code | description |
|:---------|:-------------|:------------|
| **`t`** | TimeStep::simulation | Timestep being updated or last complete (see [#Timestep\_updates](#Timestep_updates.md)) |
| **`tI`**| TimeStep::interventionPeriod | Before the intervention period, **`tI < 0`**; during this period **`tI = t + tI₀`** |
| **`tWL`** | humanWarmupLength | Length of first phase and time of its end |
| **`tI₀`** | (none) | **`t`** at the start of the intervention period (only known when **`t ≥ tI₀ `**) |
| **`tEnd`** | totalSimDuration | Time at which the simulation ends (only known when **`t ≥ tI₀`**; Note, this value is determined as **`tI₀`** + largest surveyTime element value - [see description of surveyTime element](XmlScenario.md)) |

In the code (`include/util/TimeStep.h` or `include/SimTime.h`, depending on version):

| <= 32 | >= 33 | applicability | description |
|:------|:------|:--------------|:------------|
| N/A | `sim::ts0()` | step update | Time at the start of an update step |
| `TimeStep::simulation` | `sim::ts1()` | step update | Time at the end of an update step |
| `TimeStep::simulation` | `sim::now()` | monitoring/deployment | Time between updates used for monitoring and interventions |
| `TimeStep::interventionPeriod` | `sim::intervNow()` | monitoring/deployment | Time between updates measured relative to the intervention period |