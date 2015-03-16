# Clinical models and health systems #

We have two different [clinical models](ModelClinical.md) implemented in OpenMalaria, one for a 1-day timestep and one for a 5-day timestep. The clinical models encapsulate clinical fever, diagnosis and treatment (excluding drug action) of fevers, and clinical outcomes. Parameters unaffected by the health system along with the "healthSystemMemory" parameter (which affects reporting) are part of the ["model" element](ScenarioDesign#Model.md) of the XML while all other clinical parameters are configured by the "healthSystem" element (and possibly reconfigured by "changeHS" intervention elements).

Parameters unaffected by the health system include the [incidence of malaria fevers](ModelPathogenesis.md) and the incidence and severity of non-malaria fevers (model to be documented).

The "healthSystemMemory" parameter is the maximum age (units: time-steps) at which a previous bout of fever/sickness is considered part of the current episode: 0 means the memory covers a single timestep (the reporting one), 1 means the memory extends back to events from the previous timestep (i.e. memory is 2×length-of-timestep), and negative values mean truely no memory. (This controls whether second-line treatment is used and whether a new episode is reported.)

## Common data ##

Both models require input of the probability of a case fatality and of sequelae developing for severe malaria in inpatients. (Data for outpatients is simply estimated from these figures.) A template health-system element, including our standard data:

```
<healthSystem>
  ...
  <CFR>
    <group lowerbound="0" value="0.09189"/>
    <group lowerbound="0.25" value="0.0810811"/>
    <group lowerbound="0.75" value="0.0648649"/>
    <group lowerbound="1.5" value="0.0689189"/>
    <group lowerbound="2.5" value="0.0675676"/>
    <group lowerbound="3.5" value="0.0297297"/>
    <group lowerbound="4.5" value="0.0459459"/>
    <group lowerbound="7.5" value="0.0945946"/>
    <group lowerbound="12.5" value="0.1243243"/>
    <group lowerbound="15" value="0.1378378"/>
  </CFR>
  <pSequelaeInpatient interpolation="none">
    <group lowerbound="0.0" value="0.0132"/>
    <group lowerbound="5.0" value="0.005"/>
  </pSequelaeInpatient>
</healthSystem>
```
The `...` should be replaced with either an `ImmediateOutcomes` or an `EventScheduler` element. (This is for schema version 24. For compatibility with older versions, remove the `interpolation="none"` attribute.)

The data here describes the case-fatality rates of hospitalised cases receiving successful treatment per age-group (or rather, a CFR-per-age function is constructed using step-wise linear interpolation of the (lowerbound, cfr) pairs).

Since collecting fatality-rate data for cases outside of hospital, the "community death rate" (used for cases not receiving hospital care, or, in the `ImmediateOutcomes` model, ineffective treatment) is constructed from the hospital CFR data by multiplying the odds by a parameter (exp of the LOG\_ODDS\_RATIO\_CF\_COMMUNITY parameter).

In practice, we always use the same data-set here. Some graphs, showing non-interpolated data in green and a linear interpolation in blue:

<a href='Hidden comment: 
Graphs generated with the trunk/unittest/AgeGroupInterpolationSuite.py script.
'></a>
![http://openmalaria.googlecode.com/svn/wiki/graphs/CFR_age.png](http://openmalaria.googlecode.com/svn/wiki/graphs/CFR_age.png)
![http://openmalaria.googlecode.com/svn/wiki/graphs/prob_sequelae_age.png](http://openmalaria.googlecode.com/svn/wiki/graphs/prob_sequelae_age.png)

## 5-day timestep: `ImmediateOutcomes` ##

When the simulation uses a granularity of 5-day timesteps, fevers can be considered to occur entirely within a single timestep, and thus it is reasonable to decide parasitological, clinical and cost outcomes within a single timestep — hence this model was named the clinical immediate outcome model.

Example XML element:
```
<ImmediateOutcomes name="example name">
  <drugRegimen firstLine="ACT" inpatient="QN" secondLine="ACT"/>
  <initialACR>
    <ACT value="1.0"/>
    <QN value="1.0"/>
    <selfTreatment value="0.0"/>
  </initialACR>
  <compliance>
    <ACT value="1.00"/>
    <selfTreatment value="1.0"/>
  </compliance>
  <nonCompliersEffective>
    <ACT value="0"/>
    <selfTreatment value="0"/>
  </nonCompliersEffective>
  <treatmentActions> <!-- only from schema 32 -->
    <ACT name="clear blood-stage infections">
      <clearInfections stage="blood" timesteps="1"/>
    </ACT>
    <QN name="clear blood-stage infections">
      <clearInfections stage="blood" timesteps="1"/>
    </QN>
  </treatmentActions>
  <pSeekOfficialCareUncomplicated1 value="0.04"/>
  <pSelfTreatUncomplicated value="0.0"/>
  <pSeekOfficialCareUncomplicated2 value="0.04"/>
  <pSeekOfficialCareSevere value="0.48"/>
</ImmediateOutcomes>
```

First, the drugs used for `firstLine`, `secondLine` and `inpatient` treatment are defined in the `drugRegimen` element. Drug codes may be one of `CQ`, `SP`, `AQ`, `SPAQ`, `ACT`, `QN`, or `selfTreatment`.

Then, `compliance` (adherence rate), `initialACR` (initial cure-rate for compliers) and `nonCompliersEffective` (effective cure-rate for non-compliers) are specified per drug (must be described for self-treatment but optional for other drugs; values default to 0). Setting 'compliance' does not affect 'inpatient' treatment.

Additionally, the following (fairly self-explanetory) elements must be described: `pSeekOfficialCareUncomplicated1`, `pSelfTreatUncomplicated`, `pSeekOfficialCareUncomplicated2`, `pSeekOfficialCareSevere`, and `pSequelaeInpatient`.

### Treatment actions ###

From schema 32, the action of treatments can be configured. Prior to this, treatment action (on effective treatment) is to clear all infections (including liver stage) retrospectively. This is the same as the [MDA treatment description](ModelIntervHuman#Treatment.md) except without multiple options. From schema 32:
```
<treatmentActions>
  <!-- for each drug used (e.g. ACT, CQ, QN): -->
  <QN name="short description">
    <clearInfections stage="..." timesteps="..."/>
  </QN>
</treatmentActions>
```
Different drug descriptions can be used for first-line, second-line and inpatient. Each description may have a descriptive `name`, as well as any number of `clearInfections` elements (zero elements implies the treatment does nothing, but since treatment rates can be set to zero and this is intended as a descriptor of effective treatment, doing this is discouraged; two elements may be useful to describe liver- and blood-stage effects separately).

The `clearInfections` element has two parameters: `stage` (must be one of `blood`, `liver` or `both`) and `timesteps` (`-1`, `1` or a greater value). Infections are considered liver-stage on their first timestep (assumes 5-day timestep) and blood-stage thereafter. `-1` implies retroactive treatment: clear infections after having updated densities and simulated sickness events based on these densities, but before reporting patent hosts, sum-log-parasite density, etc; this action is a legacy of the model's history. `1` clears infections on the next update, before calculating the new densities, but after reporting patent hosts etc. for the previous timestep, and is recommended for short-action drugs. Larger values allow model of prophylactic step longer than the timestep and specify the number of future updates at which infections should be cleared: e.g. `2` clears for one extra timestep (thus ten days, for a 5-day timestep model, in which infections are cleared).

## PK/PD Models ##

From Schema v33 the 5-day timestep model can be used to simulate pharmacokinetics and pharmacodynamics. This allows simulation of effects of immunity on drug action and selection of resistant parasites.  It entails [tracking of drug levels](ModelDrug.md) and consequences of treatment on parasite densities and hence clinical outcomes and transmission.

Limitations:
  * this functions only with the [Molineaux within-host model](ModelWithinHost.md).
  * only treatment of uncomplicated malaria is included in the PK/PD modeling.  Hospital treatment and outcomes of severe disease are handled as in earlier versions.


## 1-day timestep: `EventScheduler` ##

The event-scheduler model encapsulates a case-management model and schedules sickness events (fever start/end, recovery/death outcomes). An example XML element:

```
<EventScheduler>
  <!-- Simple partially-realistic (poorly researched) health system description (DH). -->
  <uncomplicated>
    <decisions>
      <decision depends="case,p" name="test" values="none,microscopy,RDT">
        case(UC1){
          p(.5): none
          p(.3): microscopy
          p(.2): RDT
        }
        case(UC2){
          p(.4): none
          p(.3): microscopy
          p(.3): RDT
        }
      </decision>
      <decision depends="result,p" name="treatment" values="MQ_treat,no_treat">
        result(positive){
          p(.9): MQ_treat
          p(.1): no_treat
        }
        result(negative): no_treat
        result(none) {
          p(.5): MQ_treat
          p(.5): no_treat
        }
      </decision>
      <decision depends="age" name="bodyMass" values="5,20,60">
        age(0-2): 5
        age(2-6): 20
        age(6-inf): 60
      </decision>
    </decisions>
    <treatments>
      <treatment name="MQ_treat">
        <schedule>
          <medicate drug="MQ" hour="0" mg="20"/>
        </schedule>
        <modifier decision="bodyMass">
          <multiplyQty effect="MQ(5)" value="5"/>
          <multiplyQty effect="MQ(20)" value="20"/>
          <multiplyQty effect="MQ(60)" value="60"/>
        </modifier>
      </treatment>
      <treatment name="no_treat">
        <schedule/>
      </treatment>
    </treatments>
  </uncomplicated>
  <complicated>
    <decisions>
      <decision depends="result,p" name="treatment" values="AS_sev,none">
        result(positive) {
          p(.9): AS_sev
          p(.1): none
        }
        result(negative): none
        result(none): none
      </decision>
      <decision depends="" name="hospitalisation" values="none,delayed,immediate">
        immediate
      </decision>
      <decision depends="p" name="test" values="none,microscopy,RDT">
        p(.3): none
        p(.5): RDT
        p(.2): microscopy
      </decision>
      <decision depends="age" name="bodyMass" values="5,20,60">
        age(0-2): 5
        age(2-6): 20
        age(6-inf): 60
      </decision>
    </decisions>
    <treatments>
      <treatment name="AS_sev">
        <schedule>
          <medicate drug="AS" hour="0" mg="2.4"/>
          <medicate drug="AS" hour="18" mg="2.4"/>
          <medicate drug="AS" hour="36" mg="2.4"/>
          <medicate drug="AS" hour="54" mg="2.4"/>
        </schedule>
        <modifier decision="bodyMass">
          <multiplyQty effect="AS(5)" value="5"/>
          <multiplyQty effect="AS(20)" value="20"/>
          <multiplyQty effect="AS(60)" value="60"/>
        </modifier>
      </treatment>
      <treatment name="none">
        <schedule/>
      </treatment>
    </treatments>
  </complicated>
  <ClinicalOutcomes>
    <maxUCSeekingMemory>3</maxUCSeekingMemory>
    <uncomplicatedCaseDuration>3</uncomplicatedCaseDuration>
    <complicatedCaseDuration>6</complicatedCaseDuration>
    <complicatedRiskDuration>6</complicatedRiskDuration>
    <pImmediateUC>.8</pImmediateUC>
  </ClinicalOutcomes>
</EventScheduler>
```

### Case management: `uncomplicated` and `complicated` ###

The case-management part consists of decision trees and treatment schedules, split into two cases: `uncomplicated` for non-malarial and malarial fevers not requiring hospitalisation, and `complicated` for severe malarial fevers or those worsened through a further insult. Apart from a few decision rules, the `uncomplicated` and `complicated` elements have the same format: a `decisions` element and a `treatments` element.

#### Example: uncomplicated case treatment ####

As an example of how these decision trees work, lets consider a patient with uncomplicated clinical malaria. The first decision we come to depends on "case" (first-line or second-line) and "p" (indicates a random decision). Assuming the patient has not had another malaria case in recent history, he will recieve first-line ("UC1") treatment, and thus has only a 50% chance of having a parasite test — lets assume has gets tested via microscopy. Like "case", the "result" decision is built-in and indicates the outcome of a malaria parasite diagnostic, based on the outcome of the "test" decision. Lets assume our guy's test came back positive; thus, according to the "treatment" decision he has a 90% chance of getting treated with MQ (mefloquine). Unlike these, "bodyMass" decision is deterministic — in fact "age" decisions may not depend on "p" or any other parameter (if necessary a second decision may do so). Having determined all decisions, we get to the treatments section. If our patient recieved "MQ\_treat", he will immediately recieve a single oral dose including 20 times 5, 20 or 60 miligrams of MQ.

#### `decisions` ####

The `decisions` element describes a model of treatment-seeking and health-system decisions, using a list of `decision` sub-elements.

A decision is described by:
  * a name (an identifier of alpha-numeric characters plus `.` and `_`)
  * a list of possible outcomes (decision outcomes are often referred to using the format `NAME(OUTCOME)`)
  * a list of other decisions this decision depends upon in order to be determined
  * some description of how an outcome is derived

A few built-in decisions exist, and a few particular decisions must be provided, but otherwise the number of decisions used and potential names are not fixed. The built-in and required decisions are:

| name | outcomes | | description |
|:-----|:---------|:|:------------|
| `case` | `UC1`, `UC2` | built-in, `uncomplicated` only | Case is second-line (`UC2`) if, within the [health-system memory](XmlScenario#clinical.md), a fever occurred where anti-malarials were prescribed, otherwise the case is `UC1`. |
| `result` | `none`, `negative`, `positive` | built-in | If the `test` decision has an outcome other than `none`, a parasite-density test is performed using the current density of the simulated case; otherwise the outcome is `none`. The number of RDT tests used is reported for costing purposes. |
| `test` | `none`, `microscopy`, `RDT` | required | Determines whether and which type of malarial diagnostic is used |
| `treatment` |  | required | Selects which treatment to medicate. Outcomes are not restricted, but must match exactly the treatments available (so probably a treatment called "none" will need to be added). |
| `hospitalisation` | `none`,`immediate`,`delayed` | required for `complicated` | Determines whether the individual goes to hospital or not, and whether a 1-day delay to hospital entry occurs. Hospitalisation affects fatality rate, reporting, and protects patient from mosquito bites. A delay causes the afore-mentioned effects to be delayed as expected, but does not delay treatments — this allows immediate suppositories to be given, but means that hospital-administered drugs should be delayed by a day in the XML (using the "delay" modifier). |

Regarding costing, sometimes in severe cases 2 tests would be used (by a referring health centre, and by the hospital accepting the patient). One method of dealing with this, assuming the in-hospital test result has no impact on treatment decisions, is to assume all patients get an RDT test in hospital (by adding the number of hospital recoveries+deaths+sequelae to the number of RDTs reported), and use the `test` decision to describe whether an out-of-hospital test occurs.

Decisions described in the XML are all given as a `decision` element who's content is some form of tree; there are currently two varieties as in the table below. The syntax for a tree is:

```
// a symbol is one or more characters in the set: a-z A-Z 0-9 . _
symbol = ( alnum | '.' | '_' )+
// double_ means a floating-point number; a range looks like: x-y
range = double_ '-' double_

// types of values: a symbol, a number, or a numeric range
value_symbol = symbol
value_double = double_
value_range = range

// an outcome either looks like { <tree> } (where <tree> is another whole tree)
// or like :<symbol> (where <symbol> is some symbol)
outcome = ( '{' tree '}' ) | ( ':' symbol )

// a branch looks like: X(Y)
// if X is "p" we have a probability branch; if it's "age" we have an age-range;
// otherwise X should be the name of a decision and Y one of the decision's outcomes
branch_p = "p" '(' value_double ')' outcome
branch_age = "age" '(' value_range ')' outcome
branch_decision = symbol '(' value_symbol ')' outcome

// A branch set is a list of one of more branches (all of the
// same type and, if decision branches, for the same decision).
// In whichever relevant way the set must be complete: probabilities
// add up to 1 or all outcomes of some decision be represented.
branch_set = ( branch_p | branch_age | branch_decision ) +

// A tree is either a branch set, or simply an outcome.
// This is the "entry point" -- describing the whole decision --
// but can also occur as a sub-tree.
tree = branch_set | symbol
```

Besides the special "age" and "p" cases (see table below), this allows us to determine an outcome based on another decision (this decision must, then, be listed under `depends`). Note that a decision used as a branch point (or "p" or "age") may be used again in a sub-tree (in particular, using multiple random trees may simplify writing decisions).

| age-dependent | The tree must have exactly one dependency: `age`. Branches should be identified with the syntax `age(a-b)` where a and b are in the range `[0,inf]` (allowing the special value `inf`). In a branch-set these ranges must not overlap and must cover the entire range `[0,inf]` (i.e. lowest bound must be `0` and highest `inf` with matching boundaries). Age ranges are considered half-open, open at the top (i.e. an age on a boundary matches the higher age-group). |
|:--------------|:---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| stochastic | A generic decision whose outcomes are determined by input decisions and/or random samples. Unless purely deterministic, `p` must be used as in input dependency and then probability branches may be specified using `p(x)` where `x` is the probability of the branch being chosen. All probabilities in a branch-set must add up to 1. |

#### `treatments` ####

The `treatments` element consists of a list of `treatment` elements. Each `treatment` element has a `name` attribute (which must correspond to one of the outcomes of the `treatments` decision), a `schedule` element and a list of zero-or-more `modifier` elements.

The `schedule` element describes the base schedule of medications taken: a list of `medicate` elements describing each drug dose given, with a `drug` code and the `hour` at which it is given (from 0, start of timestep on which case management occurs). If `duration` is zero or not specified, the dose is assumed to be taken orally, and the size of the dose is given in `mg` of active ingredient. If a non-zero `duration` is specified (units: hours), the dose is administered via IV (uses different equation and separate costing from oral drugs), and the dose is instead specified as mg active ingredient per kg weight of the patient (again using the "mg" attribute). Example of IV drug:
```
medicate drug="XX" hour="0" mg="1" duration="8"/>
```

Drug codes available are those specified in the [drug library](ScenarioDesign#Drug_library.md).

Since one base schedule may be well or poorly adhered to in several variations, we have `modifier` elements. Each modifier corresponds to one `decision` and has one of three types (see table below). Each of these consists of a list of sub-elements, each of which have attributes `value` (the decision's outcome), `effect` (effect of the modifier per drug-code prescribed; of the form `DRUG1(x),DRUG2(y)` where x and y are effects as described below), and optionally the boolean attribute `affectsCost` (whether costs are modified in the same way as the doses administered; defaults to one if not present; can be used to simulate low-quality drugs at standard cost and vomited doses).

| sub-element name | description |
|:-----------------|:------------|
| `multiplyQty` | multiplies dose size; useful for age-specific dosing and poor-quality drugs |
| `delay` | delays each dose by the given number of hours; can be used for delayed arrival to a health facility but this won't affect the start/end time of the bout of illness |
| `selectTimeRange` | Causes doses outside the time range given to be missed (also not costed unless `affectsCost` is set false). The range `x-y` is interpreted as a half-open interval; for dose at time `t` to be taken we must have `x <= t < y`. |

### Severe-fever outcomes: `ClinicalOutcomes` ###

The `EventScheduler` element also has a `ClinicalOutcomes` element, describing:

| name | type | description |
|:-----|:-----|:------------|
| maxUCSeekingMemory | int | Maximum number of timesteps (including first of case) an individual will remember they are sick before resetting. |
| uncomplicatedCaseDuration | int | Fixed length of an uncomplicated case of malarial/non-malarial sickness (from treatment seeking until return to life-as-usual). Usually 3. |
| complicatedCaseDuration | int | Fixed length of a complicated/severe case of malaria (from treatment seeking until return to life-as-usual). |
| complicatedRiskDuration | int | Number of days for which humans are at risk of death during a severe or complicated case of malaria. Cannot be greater than the duration of a complicated case or less than 1 day. |
| pImmediateUC | double | Probability that, when sick with uncomplicated fever, treatment seeking will be done immediately, on the second, or on the third day when sick. |

### Non-malaria fevers ###

By default, no Non-Malaria Fevers (NMFs) are modelled. If the option `NON_MALARIA_FEVERS` is enabled (1-day time-step model only), incidence, treatment and outcomes of non-malaria fevers are modelled. The only outcomes reported are the numbers of NMF deaths and of antibiotic treatments taken.

Two categories of NMF are considered: bacterial/viral infections which are self-limiting, never fatal and therefore require no treatment, and bacterial infections which may lead to death and thus should be treated.

Incidence of NMF and the probability that the NMF requires treatment are described within the ["model" element](ScenarioDesign#Model.md):
```
<model>
  <clinical healthSystemMemory="14">
    <NonMalariaFevers>
<!-- rate of incidence of clinical fever per person per year by age -->
      <incidence>
        <group lowerbound="0" value="0.1"/>
        <group lowerbound="1" value="0.1"/>
      </incidence>
<!-- probability that a case requires treatment by age -->
      <prNeedTreatment>
        <group lowerbound="0" value="0.1"/>
        <group lowerbound="10" value="0.1"/>
      </prNeedTreatment>
    </NonMalariaFevers>
  </clinical>
  <ModelOptions>
    <option name="NON_MALARIA_FEVERS"/>
  </ModelOptions>
  ...
</model>
```

Treatment and outcomes of NMFs are described as part of the health-system model. The uncomplicated decision tree is followed as for uncomplicated malaria, the only difference being that malaria diagnostics are likely to give a negative result unless the patient also carries malaria parasites.

Two additional decisions are made for NMFs: whether antibiotics are administered and whether the fever is fatal. (Antibiotics are never administered for malaria fevers.) These decisions are described by the parameters below. (TODO: describe parameters — model.)
```
<healthSystem>
  <EventScheduler>
    <uncomplicated ... />
    <complicated ... />
    <ClinicalOutcomes ... />
    <NonMalariaFevers>
      <prTreatment>0.7</prTreatment>
      <effectNegativeTest>0</effectNegativeTest>
      <effectPositiveTest>0</effectPositiveTest>
      <effectNeed>0</effectNeed>
      <CFR>
        <group lowerbound="0" value="0.1"/>
        <group lowerbound="10" value="0.1"/>
      </CFR>
      <TreatmentEfficacy>0.6</TreatmentEfficacy>
    </NonMalariaFevers>
  </EventScheduler>
</healthSystem>
```