# Human-stage interventions #

The following table lists interventions which have been implemented in OpenMalaria, and which deployment methods are available:

| intervention | applicable | timed deployment | age-based deployment |
|:-------------|:-----------|:-----------------|:---------------------|
| [changeHS](ModelIntervHuman#Change_heath_system.md) | all versions | yes | no |
| [vaccine](ModelIntervHuman#Vaccines.md) | all versions | yes | yes ([see notes](ModelIntervHuman#Doses_and_schedules.md)) |
| [IPT](ModelIntervHuman#Intermittent_Preventive_Treatment_(IPT).md) | up to version 31 | yes | yes |
| [MDA](ModelIntervHuman#Mass_Drug_Administration.md) | up to version 32 | yes | from version 32 |
| [MDA1D](ModelIntervHuman#MDA1D.md) | up to version 32, 1 day time step only | yes | yes |
| [treatSimple](ModelIntervHuman#treatSimple.md) | from version 33 | yes | yes |
| [treatPKPD](ModelIntervHuman#treatPKPD.md) | from version 33 | yes | yes |
| [screen](ModelIntervHuman#screen.md) | from version 33 | yes | yes |
| [decisionTree](ModelIntervHuman#decisionTree.md) | from version 33 | yes | yes |

## Change heath system ##

This intervention updates the health-system by providing an entirely new description; e.g. it can be used to simulate the effects of improved access to treatment. It is described like:

```
<interventions>
  <changeHS name="name of new health system">
    <timedDeployment time="0">
      <ImmediateOutcomes .../> <!-- or EventScheduler -->
      <CFR .../>
      <pSequelaeInpatient .../>
    </timedDeployment>
  </changeHS>
  ...
</interventions>
```

Any number of `timedDeployment` elements may be used, each of which specifies a `time` of deployment (in timesteps) and has the same type of content as the [healthSystem](ScenarioHealthSystem.md) element used to describe the initial health system.


## Vaccines ##

These are [human-targeting interventions](ModelInterventions#Human_targeting_interventions.md).

Three vaccine types are simulated. Prior to schema 32, all configured vaccine types are administered simultaneously; from version 32, multiple descriptions for each vaccine type are allowed and each vaccine description may be administered independently. Types of vaccine:

  * Pre-erythrocytic vaccine (vaccineType `PEV`): prevents a proportion of infections from commencing.
  * Blood-stage vaccine (vaccineType `BSV`): acts as a killing factor on blood-stage parasites
  * Transmission-blocking vaccine (vaccineType `TBV`): one minus this scales the probability of transmission to mosquitoes

### Description in XML ###

For each of these vaccine types, any positive number of doses can be simulated.  Vaccine efficacy is assigned to the host depending on how many previous vaccinations they have received and the time since the last of these vaccinations (with efficacy decaying as described in [ModelDecayFunctions](ModelDecayFunctions.md)).

Vaccines need to be configured per-scenario. From schema 32, each vaccine effect is described separately, then deployed within one or more interventions:
```
  <interventions name="...">
    <human>
      ...
      <component id="my_TBV_effect">
        <TBV>
          <decay L="10.0" function="exponential"/>
          <efficacyB value="10"/>
          <initialEfficacy value="0.512"/>
          <initialEfficacy value="0.64"/>
          <initialEfficacy value="0.8"/>
        </TBV>
      </component>
      <component id="some_PEV">
        <PEV>
          <decay L="10.0" function="exponential" mu="0" sigma="0.2"/>
          <efficacyB value="10"/>
          <initialEfficacy value="0.512"/>
          <initialEfficacy value="0.64"/>
          <initialEfficacy value="0.8"/>
        </PEV>
      </component>
      ...
      <deployment>
        <component id="my_TBV_effect"/>
        <component id="some_PEV"/>
        <continuous>
          <deploy coverage="0.95" targetAgeYrs="0.0833" vaccMaxCumDoses="1" vaccMinPrevDoses="0"/>
          <deploy coverage="0.95" targetAgeYrs="0.17" vaccMaxCumDoses="2" vaccMinPrevDoses="1"/>
          <deploy coverage="0.95" targetAgeYrs="0.25" vaccMaxCumDoses="3" vaccMinPrevDoses="2"/>
        </continuous>
        <timed>
          <deploy coverage="0.95" time="1" vaccMaxCumDoses="3"/>
          <deploy coverage="0.95" time="7" vaccMaxCumDoses="3"/>
          <deploy coverage="0.95" time="13" vaccMaxCumDoses="3"/>
          <deploy coverage="0.95" time="366" vaccMaxCumDoses="3"/>
          <deploy coverage="0.95" time="731" vaccMaxCumDoses="3"/>
          <deploy coverage="0.95" time="1096" vaccMaxCumDoses="3"/>
        </timed>
      </deployment>
      ...
    </human>
  </interventions>
```

For schemas (?) up to 31, a `description` element is given for each enabled vaccine type within the `vaccine` element:

```
    <vaccine>
      <description vaccineType="TBV">
        <decay L="0.5" function="exponential"/>
        <efficacyB value="10"/>
        <initialEfficacy value="0.4"/>
        <initialEfficacy value="0.46"/>
        <initialEfficacy value="0.52"/>
      </description>
      <description vaccineType="BSV">
        <decay L="0.5" function="exponential"/>
        <efficacyB value="10"/>
        <initialEfficacy value="0.4"/>
        <initialEfficacy value="0.46"/>
        <initialEfficacy value="0.52"/>
      </description>
      <description vaccineType="PEV">
        <decay L="0.5" function="exponential"/>
        <efficacyB value="10"/>
        <initialEfficacy value="0.4"/>
        <initialEfficacy value="0.46"/>
        <initialEfficacy value="0.52"/>
      </description>
      <continuous coverage="0.95" targetAgeYrs="0.0833"/>
      <continuous coverage="0.95" targetAgeYrs="0.17"/>
      <continuous coverage="0.99" targetAgeYrs="0.25"/>
      <continuous coverage="0.99" targetAgeYrs="1.25"/>
      <continuous coverage="0.99" targetAgeYrs="2.25"/>
      <continuous coverage="0.99" targetAgeYrs="3.25"/>
      <continuous coverage="0.99" targetAgeYrs="4.25"/>
    </vaccine>
```

Note: prior to schema version 25, the vaccine type codes were 1, 2, 3 instead of `PEV, BSV, TBV`.

### Efficacy ###

Vaccine efficacy is calculated as `initialEff * decayFactor`, where `initialEff` is sampled from a Beta distribution, and `decayFactor` is described by the `decay` element and time since last deployment (see `DecayFunction` [configuration](ModelDecayFunctions#Configuration.md)).

The Beta distribution takes two parameters: β, set by `efficacyB`, and α, set as "β × μ / (1 - μ)". `μ` is set to one of the given `initialEfficacy` values: the first value the first time this vaccine component is administered, the second the second time, etc. (repeating the last value if necessary). Any number of `initialEfficacy` values can be given.

Note: the mean initial efficacy is μ (`initialEfficacy`), and the variance is "m × (1 - m)^2 / (1 - m + β)" (thus a large β=efficacyB can be used to minimise variance).

### Doses and schedules ###

Use standard [continuous](ModelInterventions#Continuous_descriptions.md) and [mass deployment](ModelInterventions#Timed_descriptions.md) descriptions.

Prior to schema 32, mass deployments select a different portion of the population each time according to the coverage given, as with any other intervention. Continuous deployments work in the same way, except that after selecting individuals according to the coverage described, any individuals who either missed a previous continuous dose (unless receiving a campaign dose to catch up) or have already received this scheduled dose (through campaign) will not receive their dose. (Thus true coverage may be less than the coverage requested.)

From schema 32, both mass and continuous deployments act, by default, like any other intervention: select the target population randomly according to coverage, then deliver. This behaviour can be changed, though (to simulate the old behaviour or do something else) by specifying `vaccMaxCumDoses` and/or `vaccMinPrevDoses`. These attributes are specified per deployment. After selecting an individual, the vaccine is administered only if the total number of doses will not exceed `vaccMaxCumDoses` and the number of doses prior to this administration is at least `vaccMinPrevDoses`. If not specified, `vaccMinPrevDoses` defaults to 0 and `vaccMaxCumDoses` to a very large number (> 10^9), thus have no effect. Use of these attributes can cause true coverage to be less than the coverage requested.


## Mass Drug Administration ##

This is a [human-targeting intervention](ModelInterventions#Human_targeting_interventions.md). It models some type of mass deployment of drugs, with or without screening; i.e. MDA, MSAT (Mass Screen And Treat), T&T (Test and Treat).

### Components for versions >= 33 ###

These are applicable starting from schema version 33. For rough equivalents in older versions, see below.

#### treatSimple ####

This component uses a simple treatment model to clear infections from humans to whom it is deployed. Example:
```
      <component id="MDA">
        <treatSimple durationBlood="1t" durationLiver="0"/>
      </component>
```

[Description of duration parameters](ScenarioHealthSystem#Treatment_actions.md) (note that the syntax is slightly different).

#### treatPKPD ####

This component uses [PK & PD modeling of treatment](ScenarioHealthSystem#PK/PD_Models.md) (only available on a 1-day time step). Example:
```
      <component id="MDA">
        <treatPKPD schedule="MQ1" dosage="MQ1"/>
      </component>
```

#### screen ####

This component does not directly affect humans, but simulates usage of a diagnostic then deploys one or more other components dependent on the outcome. Example:
```
      <component id="treat">
        <!-- legacy effect -->
        <treatSimple durationLiver="-1t" durationBlood="-1t"/>
      </component>
      <component id="MSAT">
        <screen diagnostic="deterministic">
          <positive id="treat"/>
        </screen>
      </component>
```
To deploy more than one component, or deploy on negative outcome:
```
<screen ...>
  <positive id="componentA"/>
  <positive id="componentB"/>
  <negative id="componentC"/>
  <negative id="componentD"/>
</screen>
```

#### decisionTree ####

This is similar to the MDA1D component described below, but with different syntax and also available on a 5-day time step (so long as PK/PD treatment is not used). Example:
```
      <component id="MDA">
        <decisionTree>
          <diagnostic diagnostic="RDT">
            <positive>
              <treatPKPD schedule="AL_treat" dosage="AL_dosages"/>
            </positive>
            <negative>
              <noTreatment/>
            </negative>
          </diagnostic>
        </decisionTree>
      </component>
```

Note that this example is no more complex than that done with a `screen` component above, but that many more decisions are available. TODO: link to description of trees.

### MDA (versions <= 32) ###

Applicable to 5-day and 1-day time step models, up to schema version 32.

#### Diagnostic ####

From schema version 29, a diagnostic can be used with the 5-day time-step model to simulate mass screen-and-treat. For example, the below uses a diagnostic simulating an RDT for Plasmodium falciparum in Africa [source: Murray et al (Clinical Microbiological Reviews, Jan. 2008)](data.md):
```
<component id="mda" name="example with screening">
  <MDA>
    <diagnostic>
      <stochastic dens_50="50" specificity="0.942"/>
    </diagnostic>
    <effects .../>
  </MDA>
</component>
```
Some parameters for simulating microscopy (source: expert opinion): `dens_50="20" specificity="0.75"`. Deterministic tests are also supported:
```
<diagnostic>
  <deterministic minDensity="40"/>
</diagnostic>
```
Here the test outcome is positive if the density is at least the minimum given (in parasites/µL, relative to malaria theropy data rather than densities estimated from Garki/non-Garki slides). If a minimum density of 0 is given, the test outcome is always positive.

#### Treatment ####

From schema 32, treatment is configurable. This allows the same actions as [health system treatment](ScenarioHealthSystem#Treatment_actions.md), but adding stochastic selection from multiple options. A simple example:
```
<component id="mda" name="example with screening">
  <MDA>
    <!-- optional diagnostic here (see above) -->
    <effects>
      <option pSelection="1">
        <clearInfections timesteps="1" stage="blood"/>
      </option>
    </effects>
  </MDA>
</component>
```
Stage should be one of:
  * _blood_: clear infections which are at least one 5-day timestep old (this may include pre-patent infections, since the latent period as set by `latentp` is usually 15 days)
  * _liver_: clear infections during their first 5-day timestep
  * _both_: clear all infections
**Note**: it remains to be determined how blood or liver stage clearance applies when the timestep is not 5 days.
The `timesteps` attribute can be:
  * -1: retrospectively clear infections
  * 1: clear infections on the next update (recommended over -1)
  * _n_: clear infections on the next _n_ updates (where _n_ ≥ 1)
Prior to schema 32 this configuration was not available, and treatment acted as if `timesteps="-1" stage="both"` was used.

As the `pSelection` parameter hints, multiple options can be used with different probabilities, e.g. to have a chance of no action. A (silly) example (showing what's possible):
```
<component id="mda" name="example with screening">
  <MDA>
    <!-- optional diagnostic here (see above) -->
    <effects>
      <option pSelection="0.2">
        <clearInfections timesteps="1" stage="blood"/>
        <clearInfections timesteps="3" stage="liver"/>
      </option>
      <option pSelection="0.3">
      </option>
      <option pSelection="0.5">
        <clearInfections timesteps="10" stage="both"/>
      </option>
    </effects>
  </MDA>
</component>
```

Since less configuration was available prior to schema 32, the `<MDA>` element would just contain deployment times and optionally a diagnostic:
```
    <MDA>
      <!-- optionally a diagnostic here -->
      <timed coverage="1" maxAge="999" minAge="0" time="0"/>
      <timed coverage="1" maxAge="999" minAge="0" time="14"/>
      <timed coverage="1" maxAge="999" minAge="0" time="28"/>
    </MDA>
```

### MDA1D ###

This is a [human-targeting intervention](ModelInterventions#Human_targeting_interventions.md). It uses 1-day health system code to simulate MDA, thus allowing decision trees and PK/PD deployment. The description here is for version 32, but the intervention was available before this (see note below).

With the 1-day time-step model MDA interventions are more flexible and can, for example, be configured as short-action anti-malarial deployment, deployment of SP-like drugs with a long prophylactic effect and mass screen and treat.

Configuration: an MDADescription element must be included, as, for example, below:
```
<component id="mda" name="example of MSAT with a 1-day health system model configuration">
  <MDA1D>
    <decisions>
      <decision depends="result" name="treatment" values="MQ,none">
        result(negative): none
        result(none): none
        result(positive): MQ
      </decision>
      <decision depends="" name="test" values="none,microscopy,RDT">
        RDT
      </decision>
    </decisions>
    <treatments>
      <treatment name="MQ">
        <schedule>
          <medicate drug="MQ" hour="0" mg="1200"/>
        </schedule>
      </treatment>
      <treatment name="none">
        <schedule/>
      </treatment>
    </treatments>
  </MDA1D>
</component>
```

This has the same format as part of the [EventScheduler complicated/uncomplicated trees](ScenarioHealthSystem#1-day_timestep:_EventScheduler.md). The above example is fairly minimal: the `test` decision indicates that no malaria diagnostic is used, and the `treatment` decision indicates that the treatment schedule called `1` is used. Note that it may be preferable to specify dosing by age as with health-system treatment of uncomplicated malaria.

Note: prior to schema version 32, the `<MDA1D>` element was just `<MDA>`:
```
    <MDA>
      <description>
        <decisions>
          ...
        </decisions>
        <treatments>
          ...
        </treatments>
      </description>
      <timed coverage="0.6" time="160"/>
      <timed coverage="0.6" time="860"/>
    </MDA>
```

## Intermittent Preventive Treatment (IPT) ##

This model is no longer available from schema version 32.

IPTi and IPTc: Intermittent Preventive Treatment for infants or children.
Antimalarial drugs are given at specified ages in infants according to the routine vaccination schedule (IPTi) or at regular intervals in children for whom delivery may be timed to coincide with the malaria season (IPTc).

IPT is compatible with the 5-day timestep model. The drug action is similar to that of the mass drug administration but can have a longer lasting (prophylactic) effect and can affect individual infections with different levels of drug resistance differently.
There is currently no equivalent for 1-day time-step models, since the PKPD model used for mass-drug administration already allows prophylactic effects and the 1-day within-host models allow more sophisticated effects of the drugs on parasite dynamics.

```
    <IPT>
      <description iptiEffect="18">
        <infGenotype ACR="1.0" atten="1" freq="0.8" name="wt" proph="10" tolPeriod="4"/>
        <infGenotype ACR="1.0" atten="1" freq="0.1" name="double" proph="2" tolPeriod="4"/>
        <infGenotype ACR="0.5" atten="1" freq="0.1" name="triple" proph="0" tolPeriod="4"/>
      </description>
      <continuous coverage="0.95" targetAgeYrs="0.25"/>
      <continuous coverage="0.95" targetAgeYrs="0.33"/>
      <continuous coverage="0.95" targetAgeYrs="0.75"/>
      <timed time="7" coverage=".8"/>
      <timed time="79" coverage=".8"/>
    </IPT>
```

In this example, IPTi is given at 2, 3 and 9 months of age with a coverage of 95% for each dose.  The drug in this example is sulphadoxine-pyrimethamine (SP) and new infections are randomly assigned one of three specified genotypes (`wt`,`108`,`triples`) which correspond to different degrees of SP resistance and have frequencies `freq`. Existing wildtype infections (`wt`) are cleared with a cure rate (`ACR`) of 100%, and incoming wildtype infections are cleared for a prophylactic period (`proph`) of 10\*5 days. The intervention can also be timed to simulate seasonal delivery. `iptieffect` refers to whether the IPT or placebo group should be simulated (>=10 are IPT groups) and the choice of treatment drug in those who present with malaria fevers. `atten` and `tolperiod` are not used except with alternative models for IPT described in Ross et al, 2008 PLoS ONE e2661.


