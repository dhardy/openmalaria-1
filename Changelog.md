# OpenMalaria releases #

Below is a list of the most significant changes in each release, together with the date and revision number of initial release. Releases are available as tags in the subversion repository: [http://openmalaria.googlecode.com/svn/tags](http://code.google.com/p/openmalaria/source/browse/#svn%2Ftags)

## Current development ##

[Development code](http://code.google.com/p/openmalaria/source/browse/?name=develop)

## Schema version 33 ##

[Schema 33 code — released December 2014](https://code.google.com/p/openmalaria/source/browse/?name=schema-33)

[supplementary changelog](https://code.google.com/p/openmalaria/source/browse/doc/changelog.txt?name=develop)

### Schema changes requiring updates to scenario XMLs ###

#### Mass drug deployment interventions (MDA/MSAT/T&T) ####

> The ["MDA1D"](ModelIntervHuman#MDA1D.md) component has been replaced with ["decisionTree"](ModelIntervHuman#decisionTree.md), a component
> which runs a case management decision tree on deployment; this is now
> usable with 5-day timestep models and gives a flexible way to implement T&T
> or similar campaigns.

> The ["MDA"](ModelIntervHuman#MDA_(versions_<=_32).md) component has been replaced with ["screen"](ModelIntervHuman#screen.md), ["treatSimple"](ModelIntervHuman#treatSimple.md) and
> ["treatPKPD"](ModelIntervHuman#treatPKPD.md) components. The first of these uses a diagnostic and optionally
> triggers deployment of one or more other components on positive and/or
> negative outcomes while the latter offer two ways of treating infections.

### Backwards-compatible schema changes ###

Some of these changes offer additional functionality and some are recommended
but optional changes to enhance clarity of the XML or improve models. Running
OpenMalaria with the `--deprecation-warnings` option will print messages
recommending possible changes in XML documents.

#### Time units (improved clarity): ####

> Previously dates and durations were given in days, time steps or years with
> only a little documentation to indicate which unit should be used. Now
> most of these inputs accept units of days, steps or years with an explicit
> unit, and in the case of absolute times (i.e. not durations), dates.

#### Diagnostics (bug fixes, new capabilities): ####

> Previously the diagnostic used for monitoring purposes was deterministic
> with a specified detection limit, interpreted as being derived from either
> Garki or common (non-Garki) microscopy methods, while diagnostics used by
> the health system and intervention deployment could be either stochastic or
> deterministic with configurable limits but did not adjust these for the
> density estimation method (bug). Now all diagnostics can be specified with
> Garki, common or Malaria-therapy detection limits as deterministic or
> stochastic tests. Further, the diagnostic used by the neonatal mortality
> model may be independant of that used in monitoring.

#### Parasite genetics (new model): ####

> A new model has been introduced for selecting parasite genetics, with
> specification of initial genotype frequency and fitness, and (vector
> transmission model only) optional tracking of genotype frequency.

> This replaces the simplistic genotype model previously embedded within the
> PK/PD model and allows modeling the spread of drug resistance in
> OpenMalaria.

#### Monitoring (more options, some non-backwards-compatible changes): ####

> Much of the monitoring code was rewritten to be easier to extend. XML
> configuration and output files remain backwards compatible but with new
> options: output identifier is configurable, measures can be configured to
> aggregate over some dimensions (age group, cohort, species, genotype)
> to reduce output size, new measures (for tracking parasite genetics)
> are available.

> Vector species was previously indicated via name in output files but is
> now numeric.

> Some measures have been disabled (which?).

#### Health system (clarity, new capabilities): ####

> A new health system model is available (essentially a merge of the old
> ImmediateOutcomes and unused EventScheduler) models. This specifies
> access and severe outcomes as the ImmediateOutcomes model but uses
> decision trees to describe uncomplicated treatment. As with the
> ImmediateOutcomes model, this is only functional on a 5-day timestep.

#### Molineaux infection model (bug fixes): ####

> The Molineaux infection model has been improved slightly. Most notably,
> blood volume is derived from human age (giving infants lower volume and
> higher parasite density elimination thresholds), and parameter sampling
> for new infections has been fixed (non-pairwise-sampling mode).

> New testing code has also been written for the model.

### Tool changes ###

The plotResults.py script was updated to handle new outputs and allow plotting
results by cohort and genotype (on x-axis as separate lines).

## Schema version 32 ##

[Schema-32 code - released 24th June 2014](http://code.google.com/p/openmalaria/source/browse/?name=schema-32)

[supplementary changelog](https://code.google.com/p/openmalaria/source/browse/doc/changelog.txt?name=schema-32)

  * Many changes to human-targeting intervention descriptions as well as a few changes elsewhere in the XML
  * [Human-targeting interventions can have multiple descriptions and be deployed simultaneously with other human-targeting interventions](ModelInterventions#Human_targeting_interventions.md)
  * Sub-populations replace "cohort" for intervention deployment purposes
  * Sub-population removal replaces cohort on-first-event and cumulative intervention deployment's age; resetting of health-system memory works slightly differently
  * [Monitoring of multiple cohorts](Monitoring#Cohort-modes.md) (derived from sub-populations)
  * More generic vector interventions targeting humans: the new [Generic Vector Intervention (GVI)](ModelIntervVector#Generic_vector_intervention_(GVI).md) model replaces the old vector deterrent and IRS v1, [what was IRS v2 is now just IRS](ModelIntervVector#Indoor_residual_spraying_(IRS).md)
  * New non-PK/PD treatment model for 5-day timestep health system and for MDA/MSAT. Allows delayed treatment, treating only liver and only blood stages, and prophylactic effects.
  * Case management can use diagnostics before treating patients
  * Case management can trigger deployment of any intervention
  * New 'screen' intervention component which uses a diagnostic, then deploys interventions from a list depending on outcome (replaces MDA's diagnostic)
  * Removed the [IPT model](ModelIntervHuman#Intermittent_Preventive_Treatment_(IPT).md) (the treatment model mentioned above allows a prophylactic effect which was the main thing IPT code was still used for; there is however no current replacement for modelling resistant infections which IPT code had a very basic model for).
  * [Vaccines: vaccine effects (PEV, BSV, TBV) are now described independently](ModelIntervHuman#Vaccines.md)
  * [Vaccine schedules: removed the implicit schedules used previously by all age-based deployments and added explicit schedules](ModelIntervHuman#Doses_and_schedules.md)
  * Bugfix: [treat and apply indirect mortality correctly](http://code.google.com/p/openmalaria/source/browse/include/util/ModelOptions.h?name=schema-32#244) (this is an option, enabled by default).
  * The [R\_0 special intervention](ModelIntervMisc#Insert_R_0_case.md) has been disabled (purely to avoid maintenance work; it can be re-enabled if needed)
  * [New output/monitoring measures](XmlMonitoring#Added_in_schema_32.md) (56 - 60)
  * Removed some "continuous" monitoring measures
  * Included simple _P. vivax_ model (originally developed on top of version 31)
  * Explicit namespace in XML: [scenario → om:scenario and XML header changes](ScenarioDesign#XML.md)
  * [Clarified the order in which intervention effects are deployed](ModelInterventions#Order_of_deployments.md), [making sure that cohort selection happens first](http://code.google.com/p/openmalaria/source/detail?r=dd7506e85d878f6578f9d3ba48bb8cdb8a88716a).
  * _Reorganise intervention code: most now lives in the [interventions folder](http://code.google.com/p/openmalaria/source/browse/#git%2Finclude%2Finterventions) and namesake namespace; details commonly needed elsewhere in the code now live in [Interfaces.hpp](http://code.google.com/p/openmalaria/source/browse/include/interventions/Interfaces.hpp)._
  * _Reorganise XML loading code: what was in inputData.h/cpp has moved to [Parameters.h](http://code.google.com/p/openmalaria/source/browse/include/Parameters.h)/[.cpp](http://code.google.com/p/openmalaria/source/browse/model/Population.cpp) and [util::DocumentLoader.h](http://code.google.com/p/openmalaria/source/browse/include/util/DocumentLoader.h)/[.cpp](http://code.google.com/p/openmalaria/source/browse/model/util/DocumentLoader.cpp)_
  * _Partial implementation of a Vivax model (which would be used instead of any Falciparum infection models in the simulator). Not yet usable but motivated some code changes: the old WithinHostModel class has been replaced by [WHInterface](http://code.google.com/p/openmalaria/source/browse/include/WithinHost/WHInterface.h) and [WHFalciparum](http://code.google.com/p/openmalaria/source/browse/include/WithinHost/WHFalciparum.h) and the [Pathogenesis](http://code.google.com/p/openmalaria/source/browse/include/WithinHost/Pathogenesis/PathogenesisModel.h), [Diagnostic](http://code.google.com/p/openmalaria/source/browse/include/WithinHost/Diagnostic.h) and [(new, non-PK/PD) Treatments](http://code.google.com/p/openmalaria/source/browse/include/WithinHost/Treatments.h) models have moved within the `WithinHost` namespace/directory._
  * An anonymous conservation society decided that [elimination of P. falciparum isn't always desirable](http://code.google.com/p/openmalaria/source/detail?r=7a346a6501ace06dc7d24178cdb98d16c1a6c491). [Some](http://code.google.com/p/openmalaria/source/detail?r=39577fe93e9c7d91c10d404b0579e719e2f0b141) [other](http://code.google.com/p/openmalaria/source/detail?r=cf6d4d4654820b8f99562879bd510d96a418cd4a) [changes](http://code.google.com/p/openmalaria/source/detail?r=bcd337bcb0d1c6634b859f68f6f2cf32d071d92a) [to](http://code.google.com/p/openmalaria/source/detail?r=99f859b880271ad955db7aee1c1f1e98e6df4b41&name=develop) [tests](http://code.google.com/p/openmalaria/source/detail?r=281e573285ab42c22bbc28dd5fb6d2268397184c&name=develop).

Updating scenarios from schema 31:

  * Any human-targeting interventions need to be updated to the [new syntax](ModelInterventions#Human_targeting_interventions.md)
  * The health system needs a description of [treatment actions](ScenarioHealthSystem#Treatment_actions.md) (5-day timestep)

There is [a tool to do this in an automated fashion](XmlUpdateScenario.md). Note that the tool will ask what behaviour to use for health system treatment actions, but just not MDA treatment actions. You may want to review these after the update.

## Schema Version 31 ##

[Released 8th July 2013](http://code.google.com/p/openmalaria/source/browse/?name=schema-31)

  * Simple mosquito population dynamics model: model allowing reduced numbers of adult mosquitoes to reduce the numbers of eggs laid.
  * Enabled INNATE\_MAX\_DENS by default
  * Included seekingDeathRateIncrease and probDeathOvipositing interventions
  * Allow multiple descriptions of the larviciding intervention as well as the new interventions
  * ITN and IRS tweaks
  * New `mod` and `mod_nn` functions to avoid undefined behaviour of `x%y`
  * Lots of bug fixes and code maintenance

## Schema Version 30.4 ##

[Parallel development in July 2013 to add new models to the fitting runs](http://code.google.com/p/openmalaria/source/browse/?name=schema-30.4)

  * Make the _sigma_ parameter of the Penny infection model fittable
  * `PENNY_JULY_2013` variant of the Penny 1-day-timestep model

## Schema Version 30 - 30.3 ##

Released 12th April 2012 as [r2258](https://code.google.com/p/openmalaria/source/detail?r=2258)

  * Renamed Vector\_EIR\_Input and Vector\_EIR\_Simulated to inputEIR and simulatedEIR respectively.
  * Increased delay in human infectiousness model from the perspective of the non-vector model and several summaries: it appears it was one time-step short.
  * Bug fix in REPORT\_ONLY\_AT\_RISK: after a case at time 0, times 2-5 were considered not-at-risk. This corrects those timesteps to 1-4.
  * `mode` parameter of `<entomology>` element now takes keyword value: `dynamic` or `forced`.
  * Vector model XML parameters now largely use elements instead of being one long string of attributes.
  * Vector parameters `α_i`, `P_B_i`, `P_C_i` and `P_D_i` (rate of encountering hosts and probabilities of biting, escaping and resting respectively) may now vary across humans to simulate human heterogeneity.
  * NC has extended the vector model with simulation of the egg, larval and pupal life-stages.
  * A larviciding using the vector life-cycle model will be included.
  * **Vector-model inputs will be changed:** transmission input will be in the form of a seasonality profile and IBPAPA (infectious bites per adult per annum), where the seasonality profile can be that of transmission (as before) or that of vector densities. Fitting of internal parameters to reproduce this seasonality _should_ be better than before (in progress).
  * SchemaTranslator is now compiled by cmake (output is _build_/util/SchemaTranslator/SchemaTranslator.jar)
  * Seasonality parameterisation of per-vector transmission is now input differently to allow other data sources to be used as seasonality inputs in the future.
  * Added an output: `nMassScreenings` (number of diagnostics used by MSAT, or number of recipients of MDA if no diagnostic is used)
  * New IRS model & parameterisation (to be documented)
  * Old IRS model can also have a preprandial effect
  * Heterogeneity of decay in the ITN model has been made per-human rather than per net (mistake in previous release)
  * New continuous output options


## Schema Version 29 ##

Released on 21st June as [r1902](https://code.google.com/p/openmalaria/source/detail?r=1902).

  * New ITN model
  * MSAT for 5-day timestep.


## Schema Version 28 ##

Released on 24th May 2011 as [r1836](https://code.google.com/p/openmalaria/source/detail?r=1836).

Big rearrangements of interventions content, moving from grouping by description
and deployment type to grouping by intervention. This should make experiment
design easier. Each intervention can optionally have a name attribute.

Changed some names:
  * drugDescription -> pharmacology
  * entoData -> entomology
  * eir -> EIR
  * monthlyEir -> monthlyEIR

Added optional name attributes to intervention descriptions.

New implementation of imported infections "intervention".

Larviciding intervention removed; it will be replaced by a more accurate model
later.


## Schema Version 27 ##

Released on 6th May 2011 as [r1811](https://code.google.com/p/openmalaria/source/detail?r=1811).

  * Fix for Penny's infection model (code)
  * There should no longer be NaNs in output files when the model runs successfully
  * new continuous reporting option: median immunity Y
  * descriptions of vector interventions are no longer required in cases where the  intervention is never deployed
  * bug fix: initialize microscopy count
  * made transmission warm-up fitting problems interrupt the program again


## Schema Version 26 ##

Released on 23rd February 2011 as [r1704](https://code.google.com/p/openmalaria/source/detail?r=1704).

  * Transmission model's initialization strategies updated.
  * Simulated EIR outputs changed to only be outputs from adults.
  * Implementation of Penny infection model and unittest against Matlab densities.
  * Decay functions updated with "step" function and optional heterogeneity.
  * 1-day time-step case-management: treatment-seeking delays for UC cases has new description.
  * Non-malaria morbidity: separate P(need antibiotic treatment) for fevers classified as malarial.


## Schema Version 25 ##

Released on 24th Janruary 2011 as [r1622](https://code.google.com/p/openmalaria/source/detail?r=1622).

  * Non-malaria fever modelling implemented
  * Cumulative intervention deployment: bring coverage up to a target level
  * MDA intervention may now be configured as "screen & treat" (1-day time-step)
  * Some IV-drug corrections and (hopefullly) a speed-up
  * Decay of vector-stage interventions and vaccines can now be modelled using several different functions
  * Vaccine codes are now PEV/BSV/TBV instead of 1/2/3
  * Deterministic case management trees no longer make an unnecessary random-number generator call
  * Time-steps encapsulated in a new class for added type-saftey


## Schema Version 24 ##

Released on 27th December 2010 as [r1578](https://code.google.com/p/openmalaria/source/detail?r=1578).

  * units of EIR input changed to be adult-averaged in all cases
  * vector fitting revision: use a single 5-year period
  * drug code for intravenous administration of drugs (where duration of administration is significant)
  * proposed cohort implementation completed: options for removal and all outputs restricted to cohorts
  * generic handling of data per age-group and interpolation methods
  * heterogeneity in human weight included


## Schema Version 23 ##

Released on November 4th 2010 as [r1420](https://code.google.com/p/openmalaria/source/detail?r=1420), tagged openmalaria-649.