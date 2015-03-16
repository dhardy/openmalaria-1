# Interventions #

An intervention is a method used to bring about a _change_ in the state of the simulation (e.g. to reduce transmission). Some are designed to simulate a real-world malaria control device (e.g. a bed net) but there are also several with no real-world equivalent (e.g. "uninfect vectors").

The initial case-management system functions during the warm-up period of the simulation and therefore does not bring about a _change_ in this sense, however, a change to the health system is considered to be an intervention.

Some models can be used to simulate established strategies (e.g. mass drug administration), while some have effects that do not correspond to any currently known intervention (e.g. "uninfect vectors"). Some do not directly affect malaria dynamics, but instead change the status of the simulated hosts in some other way (e.g. sub-population recruitment).

Intervention models can be usefully divided into three groups:
  * [those targeting individual humans](ModelInterventions#Human_targeting_interventions.md),
  * [those that act at the human population level with effects specific to the vector type](ModelInterventions#Host-independent_vector_interventions.md),
  * [the rest](ModelInterventions#Other_interventions.md).
These are specified as follows.

```
<interventions name="example of intervention descriptions">
  <human>
    ...
  </human>
  <vectorPop>
    ...
  </vectorPop>
  ...
</interventions>
```

#### Order of deployments ####

In most cases where multiple interventions are deployed at the same time, the exact order in which they are deployed does not matter, but in some cases it does, e.g. where intervention deployment is restricted to members of a sub-population who are recruited on the same timestep.

Timed intervention deployments always happen before continuous intervention deployments. Within each of these sets, deployments happen in the order the `<deploy>` elements occur in the XML. Each deployment ("campaign" or ...) may include multiple intervention components (e.g. ITN, vaccine). The [order in which these components are deployed is fixed](http://code.google.com/p/openmalaria/source/browse/include/interventions/Interfaces.hpp), but note that selection happens before any effects are deployed.

## Human targeting interventions ##

From schema 32, human-targeting interventions are described in terms of "components" and "deployment programmes". Terminology:

  * A "component" is one of an intervention (e.g. LLIN), a component of an intervention (e.g. the blood-stage effect of a vaccine), or a psuedo-intervention (e.g. recruitment); it encompasses an effect of the intervention, the decay of the intervention (component), and meta-data such as the sub-population name and coverage status
  * A "deployment programme" describes how one or more components are deployed; these components are deployed simultaneously within the campaign; it is however possible for a separate programme to deploy any of the same components independently

These are described in the XML like this, where all `component` elements must come before `deployment` elements:

```
<human>
  <component id="effect1">...</component>
  <component id="effect2">...</component>
  <component id="effect3">...</component>
  ...
  <deployment name="some deployment programme">
    <!-- deploy these components: -->
    <component id="effect1"/>
    <component id="effect3"/>
    <!-- via these deployment lists: -->
    <continuous>...</continuous>
    <timed>...</timed>
  </deployment>
  ...
</human>
```

### Parameterisation of components ###

Each `component` element contains necessary parameters (since other components may use the same intervention model, e.g. GVI, but with different parameters). It also contains an `id` attribute (a textual identifier), and may contain a `name` (for documentation purposes).

```
<component id="imm-suppress" name="Model complete loss of immunity">
  <clearImmunity/>
</component>
```

The models available are described on these pages (along with interventions not targeting individual humans). Multiple parameterisations of a single model (using a different `component` element and `id`) are allowed:

  * [human-stage interventions](ModelIntervHuman.md): mass drug deployment (MDA/MSAT), vaccines (PEV/BSV/TBV)
  * [mosquito-stage interventions](ModelIntervVector.md): bed nets (ITN/LLIN), indoor spraying (IRS), generic (GVI)
  * [special/pseudo interventions](ModelIntervMisc.md): recruitment only, clear immunity

### Deployment programmes ###

`deployment` elements describe the deployment of a set of intervention components. Deployment happens by selecting a set of humans on age criteria or at a certain time, then deploying components to all selected humans. The elements look something like:

```
<deployment name="example intervention">
  <component id="effect1"/>
  <component id="effect3"/>
  <continuous>...</continuous>
  <timed>...</timed>
</deployment>
```

where:

  * `component` elements list the components which are deployed when a deployment happens; the `id` references a component described above
  * `continuous` elements control the selection of humans for deployment by potentially recruiting humans who reach a certain exact age
  * `timed` elements control the selection of humans for deployment by potentially recruiting humans at a certain time during the simulation
  * `component` elements must come before `continuous` elements which must come before `timed` elements

Deployment times and coverage details are described by the `continuous` and `timed` elements. Multiple `continuous` and `timed` elements may be used (useful if not all deployments should use the same sub-population or cumulative coverage rules), but all `continuous` elements must come first. These elements take the following form:

```
<continuous>
  <restrictToSubPop id="subpop1"/> <!-- optional -->
  <deploy targetAgeYrs="0.833" coverage="0.8"/>
  <deploy targetAgeYrs="0.166" coverage="0.7"/>
  <deploy targetAgeYrs="0.25" coverage="0.7"/>
</continuous>
<timed>
  <restrictToSubPop id="subpop2"/> <!-- optional -->
  <cumulativeCoverage .../> <!-- optional -->
  <deploy time="0" coverage="0.6"/>
  <deploy time="73" coverage="0.6"/>
</timed>
```

**Timed** (mass-deployment) campaigns: at each time given (units: time steps from start of intervention period), humans are selected from the target population with probability `coverage`. Age-based **continuous** deployment: from the start of the intervention period, humans are selected from the target population at exact age `targetAgeYrs` (years, converted to timesteps, rounding down (i.e. 0.5 years becomes 36 timesteps)) with probability `coverage`. Selected humans are then deployed all effects listed (with one exception: [vaccine schedules may block deployment of vaccines](ModelIntervHuman#Doses_and_schedules.md), though not of other effects deployed by the intervention). Note that at each time step timed deployments happen before continuous deployments, and that it is technically possible for a single human to be selected and receive deployments two or more times in a single timestep.

The target population is normally the entire simulated population, but attributes of the `deploy` elements and the `restrictToSubPop` element (see below) can change this. Attributes allowed in `deploy` elements:

| attribute | unit (type) | description | applicability |
|:----------|:------------|:------------|:--------------|
| `begin` | timestep (integer) | Time from the beginning of the intervention period at which deployment starts | continuous |
| `end` |  timestep (integer) | Time from the beginning of the intervention period at which deployment stops, plus 1 (previous timestep is the last on which deployment occurs) | continuous |
| minAge | years (floating-point) | Minimum age (inclusive) at which humans are considered eligible in years | timed |
| maxAge | years (floating-point) | Maximum age (exclusive) at which humans are considered eligible in years | timed |
| vaccMinPrevDoses | doses (integer) | See [vaccine schedules](ModelIntervHuman#Doses_and_schedules.md); only affects vaccines | both |
| vaccMaxCumDoses | doses (integer) | See [vaccine schedules](ModelIntervHuman#Doses_and_schedules.md); only affects vaccines | both |

`restrictToSubPop` and `cumulativeDeployment`: see below. Note that both apply to all `deploy` times/ages within the `continuous`/`timed` list they appear, but that multiple `continuous` and `timed` lists may be used.


## Subpopulations ##

This is a [human-targeting intervention](ModelInterventions#Human_targeting_interventions.md).

### Motivations ###

**Cohort studies**: field trials of interventions generally enroll only a defined sub-group of the human population (a cohort) into a study, and consider only data collected on those individuals.  **Heterogeneity of deployment**: when multiple interventions are deployed at a given coverage level or are renewed at a later date, one should consider whether the same subset of the population or a different, randomly chosen subset receive the intervention.

### Concepts ###

To address these issues, OpenMalaria (from version 32) features the following concepts:

  * **sub-population**: one can target the sub-population of humans which has received some intervention recently, as well as the inverse
  * **cohort**: survey outputs can be divided between cohort sets (and combinations and inverses)

### Sub-populations, recruitment and expiry ###

Every [human-targeting intervention component](https://code.google.com/p/openmalaria/wiki/ModelInterventions#Human_targeting_interventions) defines a sub-population: the sub-population is identified by the same `id` (identifier) as the intervention component.

The members of a sub-population are determined by recruitment and expiry rules; each sub-population initially has no members. _Recruitment_ is tied to intervention deployment: each human receiving this intervention automatically becomes a member of the associated sub-population. A human's membership to the sub-population _expires_ when any of the following happens:
  * Attrition of bed-nets: when owners dispose of nets using the [ITN model](https://code.google.com/p/openmalaria/wiki/ModelIntervVector#Insecticide_treated_nets_%28ITNs%29), they also lose membership in the linked sub-population
  * Intervention age: if the time since the last deployment of the associated intervention component exceeds a given number of years, membership expires. This only applies if configured for the intervention component.
  * On first bout/treatment/infection: an intervention component can also be configured to expire membership the first time some event happens to the target human since intervention deployment.

Note however that _expiry of sub-population membership_ does not imply _cessation of intervention effect_ (excepting the attrition rule above).

#### Configuration of expiry ####

Of the above membership expiry rules, only the attrition rule is active by default. The others require configuration, attached to the `<component .../>` configuration in the XML:

```
<component id="xyz">
  <PEV>...</PEV> <!-- or whatever -->
  <subPopRemoval afterYears="1" onFirstBout="true" onFirstTreatment="true" onFirstInfection="true"/>
</component>
```

Following the intervention model parameters (in this case, `PEV`, but the same applies to all intervention models), the `<subPopRemoval>` element can appear with configuration applying to this component `id`. Any of the attributes (`afterYears`, `onFirst...`) can be omitted. If present (and "true" in the case of the latter attributes), the have the following effects:

  * _After x years:_ remove individuals from the sub-population when the time since the last deployment of this cohort to this human exceeds x years.
  * _First bout only:_ remove individuals from the sub-population at the start of the first episode (start of a clinical bout) since they were recruited into the sub-population. This is intended for cohort studies that intend to measure time to first episode, using active case detection.
  * _First treatment only:_ remove individuals from the sub-population when they first seek treatment since they were recruited into the sub-population. This is intended for cohort studies that intend to measure time to first episode, using passive case detection.
  * _First infection only:_ remove individuals from the sub-population at completion of the first survey in which they present with a patent infection since they were recruited into the sub-population. This intended for cohort studies that intend to measure time to first infection, using active case detection.

None of these options prevent re-recruitment in the case that recruitment settings could conceivably recruit the same individual twice.

### Restriction of intervention deployment ###


`restrictToSubPop` can be used to restrict deployment to a sub-population. Example:

```
<continuous>
  <restrictToSubPop id="subpop1" complement="false"/>
  <deploy targetAgeYrs="0.833" coverage="0.8"/>
  <deploy targetAgeYrs="0.166" coverage="0.7"/>
  <deploy targetAgeYrs="0.25" coverage="0.7"/>
</continuous>
```

Here, only members of `subpop1` will receive the intervention, with coverage specified relative to the sub-population (i.e. only 80% or 70% of the members of the sub-population in this example). **Negation**: in this case `complement="false"` and deployment is to members of the `subpop1` sub-population (it is the same if `complement` is not specified); if instead `complement="true"` is specified, then deployment is to the complement of `subpop1` (i.e. all humans who are not members).

`cumulativeCoverage` can be used with timed deployment to bring the overall coverage of some intervention up to some target level; normally `coverage` specifies the portion of the target population selected randomly at _each_ deployment, but with this option `coverage` instead specifies a target coverage level. If coverage is already above the target level, nothing is done; if coverage is below, the necessary increase in coverage is calculated and this is used to randomly select unprotected humans. The element has one attribute: `component`, which should be the `id` of a sub-population (respectively an intervenvion component) for which membership is tested.

```
<component id="LLIN">
  <ITN>...</ITN>
  <subPopRemoval afterYears="3"/><!-- membership expires 3 years after deployment -->
</component>
<deployment name="LLIN programme">
  <component id="LLIN"/>
  <timed>
    <cumulativeCoverage component="LLIN"/><!-- calculate coverage by membership in the LLIN sub-pop -->
    <deploy time="0" coverage="0.8"/>
    <deploy time="73" coverage="0.8"/>
    <deploy time="146" coverage="0.8"/>
    <deploy time="219" coverage="0.8"/>
  </timed>
</deployment>
```


### Survey outputs by cohort ###

TODO

[cohort](Monitoring#Cohort-modes.md)

## Host-independent vector interventions ##

Certain vector interventions not targeting individual human hosts can be described as `vectorPop` interventions. This element takes one or more `intervention` elements:

```
<interventions>
  ...
  <vectorPop>
    <intervention .../>
    <intervention .../>
  </vectorPop>
  ...
</interventions>
```

Each `intervention` element describes effects per species and deployment times, as follows:
```
<intervention name="some name">
  <description>
    <anopheles mosquito="mosq id">
      ...
    </anopheles>
    ...
  </description>
  <timed>
    <deploy time="0"/>
    <deploy time="18"/>
    ...
  </timed>
</intervention>
```

Deployment times are given in time steps; unlike human targeting deployments there are no additional details like coverage levels or age-based restrictions to include.

Effects are described per anopheles species; there must be exactly one `anopheles` element for each `anopheles` described in the vector model. The effects may be described at most once per intervention, may be described for some anopheles sub-species but not others, and may be described within more than one intervention. Where not described, the effect is nil.

[Several effects are available: adjust seeking death rate, the probability of death while ovipositing, and emergence reduction.](ModelIntervVector#Vector_population_interventions.md) An example of their configuration is below:

```
<anopheles mosquito="arabiensis">
  <!-- the following elements are all optional and can appear in any order -->
  <seekingDeathRateIncrease initial="0.2">
    <decay .../>
  </seekingDeathRateIncrease>
  <probDeathOvipositing initial="0.6">
    <decay .../>
  </probDeathOvipositing>
  <emergenceReduction initial="0.1">
    <decay .../>
  </emergenceReduction>
</anopheles>
```

## Other interventions ##

Besides `vectorPop` and `human`, the following interventions are available:

  * [Change the health system](ModelIntervHuman#Change_heath_system.md)
  * [Change EIR (non-vector model)](ModelIntervVector#Change_EIR.md)
  * [Import infections](ModelIntervMisc#Import_Infections.md)
  * [Special R\_0 intervention](ModelIntervMisc#Insert_R_0_case.md)
  * [Uninfect vectors](ModelIntervVector#Uninfect_vectors.md)