# Mosquito-stage interventions #

The following table lists interventions which have been implemented in **OpenMalaria**, and which deployment method(s) are available for them:

| Intervention | Deployment method | [Transmission model(s)](ModelTransmission.md) |
|:-------------|:------------------|:----------------------------------------------|
| [changeEIR](ModelIntervVector#Change_EIR.md) | timed | [non-vector](ModelTransmission#Non-vector_transmission.md) |
| [GVI](ModelIntervVector#Generic_vector_intervention_(GVI).md) | [human targeting](ModelInterventions#Human_targeting_interventions.md) | [vector](ModelTransmission#Vector_transmission.md) |
| [ITN](ModelIntervVector#Insecticide_treated_nets_(ITNs).md) | [human targeting](ModelInterventions#Human_targeting_interventions.md) | [vector](ModelTransmission#Vector_transmission.md) |
| [IRS](ModelIntervVector#Indoor_residual_spraying_(IRS).md) | [human targeting](ModelInterventions#Human_targeting_interventions.md) | [vector](ModelTransmission#Vector_transmission.md) |
| [uninfectVectors](ModelIntervVector#Uninfect_vectors.md) | timed | both |
| [vectorPop](ModelIntervVector#Vector_population_interventions.md) | [vector population](ModelInterventions#Host-independent_vector_interventions.md) | [vector](ModelTransmission#Vector_transmission.md) |

## Change EIR ##

The entomological innoculation rate (EIR)  intervention is used to override the default transmission settings. It can be used, for example, to switch to EIR settings recorded during a trial. It can only be used with the non-vector transmission model.

This intervention updates the health-system by providing an entirely new description; e.g. it can be used to simulate the effects of improved access to treatment. It is described like:

```
<interventions>
  <changeEIR name="name of new transmission settings">
    <timedDeployment eipDuration="10" time="0">
        <EIRDaily origin="interpolate">0.00219</EIRDaily>
        <EIRDaily origin="interpolate">0.00210</EIRDaily>
        <EIRDaily origin="interpolate">0.00202</EIRDaily>
        ...
      </timedDeployment>
  </changeEIR>
  ...
</interventions>
```

The new EIR is always matched to the whole intervention period, thus must contain enough entries to cover from the _beginning_ of the intervention period to the _end_, even if not deployed at time 0 or replaced before the end of the simulation. Values are, however, only used from the time of deployment (with some values potentially going unused).

Any number of `timedDeployment` elements may be used, each of which specifies a `time` of deployment (in timesteps) and has the same type of content as the [nonVector](ScenarioTransmission#Non-vector_parameters:_nonVector.md) element used to describe the initial transmission.

Deployming a changeEIR intervention changes the transmission `mode` parameter to force EIR (human infectiousness to mosquitoes has no further effect on EIR).

## Net / indoor spraying interventions ##

The following interventions all work by multiplying [vector model](XmlEntoVector.md) parameters affecting individual humans:

  * `deterrency` describes a parameter _x_ such that the availability of a protected host to mosquitoes becomes (1 - _x_) × _preInterventionAvailability_
  * `preprandialKillingEffect` describes a parameter _y_ such that the probability of a mosquito successfully biting a chosen protected host (instead of dying) becomes (1 - _y_) × _preInterventionProbability_
  * `postprandialKillingEffect` describes a parameter _z_ such that the probability of a mosquito successfully escaping from a protected human after feeding (instead of dying) becomes (1 - _z_) × _preInterventionProbability_

The interventions use the generic [decay functions framework](ModelDecayFunctions.md) to describe decays — see [configuration](ModelDecayFunctions#Configuration.md) (`DecayFunctionValue` elements).

They are deployed as [human-targeting interventions](ModelInterventions#Human_targeting_interventions.md).

### Generic vector intervention (GVI) ###

This model can simulate the effects bed nets, IRS, deterrents, and maybe even ivermectin can have on mosquitoes. The dedicated [ITN](ModelIntervVector#Insecticide_treated_nets_(ITNs).md) and [ModelIntervVector#Indoor\_residual\_spraying\_(IRS)](ModelIntervVector#Indoor_residual_spraying_(IRS).md) models interact with the vector transmission model in the same places but include more detailed models of decay of effects.

GVI effects are described like this:

```
<GVI name="DDT test" id="DDTtest">
  <decay L="0.5" function="exponential"/>
  <anophelesParams mosquito="gambiae_ss" propActive="1">
    <deterrency value="0.56"/>
    <preprandialKillingEffect value="0"/>
    <postprandialKillingEffect value="0.24"/>
  </anophelesParams>
</GVI>
```

GVI is a new name introduced in schema 32; prior to this IRS (v1) did the same thing and `vectorDeterrent` a sub-set of this. Both are now replaced by GVI. Examples of the old configuration:

```
    <IRS name="DDT test">
      <description>
        <decay L="0.5" function="exponential"/>
        <anophelesParams mosquito="gambiae_ss" propActive="1">
          <deterrency value="0.56"/>
          <preprandialKillingEffect value="0"/>
          <postprandialKillingEffect value="0.24"/>
        </anophelesParams>
        <anophelesParams mosquito="funestus" propActive="1">
          <deterrency value="0.56"/>
          <preprandialKillingEffect value="0"/>
          <postprandialKillingEffect value="0.24"/>
        </anophelesParams>
        <anophelesParams mosquito="arabiensis" propActive="1">
          <deterrency value="0.56"/>
          <preprandialKillingEffect value="0"/>
          <postprandialKillingEffect value="0.24"/>
        </anophelesParams>
      </description>
      <timed>
        <deploy coverage="0.95" time="7"/>
      </timed>
    </IRS>

    <vectorDeterrent name="hypothetical deterrency">
      <decay L="4" function="exponential"/>
      <anophelesParams mosquito="gambiae_ss">
        <deterrency value="0.2"/>
      </anophelesParams>
      <timed coverage="0.7" time="7"/>
    </vectorDeterrent>
```

The preprandialKillingEffect is only available as of schema 30, the other two previously (but the XML has a slightly different structure).

All effects decay to zero as described by the [decay function](ModelDecayFunctions.md) described (in the first case, exponential decay with a half-life of 0.5 years).

From schema 30 onwards, the propActive option indicates the proportion of bites for which the intervention has any action whatsoever on the mosquito. This is helpful for accounting for preferential biting times and location (indoors vs outdoors) (see also under ITNs).

### Insecticide treated nets (ITNs) ###

ITNs act via the same [three factors described above](ModelIntervVector#Net_/_indoor_spraying_interventions.md), and are also deployed as [human-targeting interventions](ModelInterventions#Human_targeting_interventions.md).

From schema 29 onwards, the ITN physical state (measured through a hole index) and chemical state (measured through insecticide concentration) decay with time. The hole index does this through formation of holes, the insecticide concentration through wear and aging. The effects of the net, depending on its state, are calculated using functions describing the relationship between effect and state.

The XML description looks like:

```
<component id="ITN" name="Extended ITNs">
  <ITN>
    <usage value="0.8"/>
    <holeRate mean=".9" sigma=".8"/>
    <ripRate mean=".7" sigma=".8"/>
    <ripFactor value="0.4"/>
    <initialInsecticide mu="70" sigma="20"/>
    <insecticideDecay L="2.2" function="exponential" mu="-0.32" sigma="0.8"/>
    <attritionOfNets L="12" function="smooth-compact" k="2"/>
    <anophelesParams mosquito="gambiae_ss" propActive="1">
      <deterrency holeFactor="0.5" holeScalingFactor="0.1" insecticideFactor="0.67" insecticideScalingFactor="0.1" interactionFactor="1.492537"/>
      <preprandialKillingEffect baseFactor="0.09" holeFactor="0.57" holeScalingFactor="0.1" insecticideFactor="0.604" insecticideScalingFactor="1" interactionFactor="-0.424"/>
      <postprandialKillingEffect baseFactor="0.10" holeFactor="0" holeScalingFactor="0.1" insecticideFactor="0.55" insecticideScalingFactor="0.1" interactionFactor="0"/>
    </anophelesParams>
    <anophelesParams mosquito="funestus" ...
  </ITN>
</component>
```

TODO: existing parameterisations?

Description of parameters:

| Parameter | Unit/type | description |
|:----------|:----------|:------------|
| usage: value | A proportion between 0 and 1 | Usage represents the proportion of time during the night that a net is used by the person. Because in the current model all mosquito species bite homogeneously throughout the night, usage value can also be seen as the πi value, the proportion of encounters out of all encounters with mosquitoes that an unprotected subject has during the time they would be under an ITN if they used one. If this value differs between mosquito species because of different preferential biting times and location (indoors vs outdoors), instead of through the `usage value` the πi has to be specified through the `propActive` for each species (available from schema 30 onwards, see below). |
| holeRate: mean | Holes per annum; min: 0; no max | Each new net is assigned a fixed rate of hole formation. Holes are formed according to a Poisson process with this rate as (only) parameter. |
| holeRate: sigma | Type: real; min: 0; no max | The value of the rate of hole formation is varied among nets by multiplying with a distribution factor which is log normally distributed. For the distribution factor, a sample is taken from a normal distribution with mean=0 and var=1. The sample is multiplied by sigma and summed with mu=-0.5sigma<sup>2</sup>, prior to exponentiation. Note that with mu=-0.5sigma<sup>2</sup>, then the mean of the lognormal distribution =1. |
| ripRate: mean | Units: rips per existing hole per annum; min: 0; no max | Each existing hole has a probability of being ripped bigger according to a Poisson process with this rate as (only) parameter. |
| ripRate: sigma | Type: real; min: 0; no max | See first under holeRate "sigma". The normal sample is the same as for holeRate distribution factor, sigma of the lognormal can be varied. |
| ripFactor: value | Type: real; min: 0; no max | This factor expresses how important rips are in increasing the hole. The hole index of a net is the count of holes plus the "ripFactor" multiplied with the cumulative number of rips. |
| initialInsecticide: mu, sigma | Units: mg/m²; min: 0 |
The insecticide concentration of new nets is Gaussian distributed. Negative sampled values are set to 0. The `insecticideScalingFactor` under `anophelesParams` should be adjusted depending on the size of mu. The deltamethrin declared content is 1.8 g/kg corresponding to 68.4 mg/msq for long-lasting (incorporated into filaments) insecticidal nets according to [WHO interim specification 333/LN/3 (August 2010)](http://www.who.int/whopes/quality/Deltamethrin_LN_incorporated_into_filaments_WHO_spec_eval_Aug_2010.pdf) 
| insecticideDecay | Units: years | A [decay function](ModelDecayFunctions.md) describing the decay of insecticide. "exponential" is suggested as a decay function. |
| attritionOfNets | Units: years | A [decay function](ModelDecayFunctions.md) describing how nets are discarded by users. Note also that humans who are ["outmigrated" by the demography model](ScenarioDesign#Demography.md) and humans who die **also** remove nets from the population since there is no redistribution or sharing of nets in the model. Currently, this is independent of net state, and the time until disappearance is predefined according to the decay function when a net is first distributed. |

Parameters described per anopheles species/subspecies:

| propActive | Proportion between 0 and 1 | This indicates the proportion of bites, when the intervention is in use, for which the intervention has any action whatsoever on the mosquito. This is helpful for accounting for preferential biting times and location (indoors vs outdoors) and factoring in the πi value for different species (see under 'usage value' above and under deterrency described below). |
|:-----------|:---------------------------|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

```
          <deterrency holeFactor="0.5" holeScalingFactor="0.10" insecticideFactor="0.67" insecticideScalingFactor="0.10" interactionFactor="1.492537"/>
```
`RelativeAttractiveness=exp(log(holeFactor)*exp(-HoleIndex*holeScalingFactor)+log(insecticideFactor)*(1-exp(-InsecticideConcentration*insecticideScalingFactor)))+log(interactionFactor)*exp(-HoleIndex*holeScalingFactor)*(1-exp(-InsecticideConcentration*insecticideScalingFactor))))`

NOTE: The exponential function prevents the RelativeAttractiveness from becoming negative. The `interactionFactor` can be calculated from experimental hut data: observed RelativeAttractiveness value for intact treated divided by the product of the observed RelativeAttractiveness values for intact untreated (`holeFactor`) and holed treated (`insecticideFactor`). If the `holeFactor`=1 `insecticideFactor`=1 and `interactionFactor`=1, the RelativeAttractiveness=1 and there is 0 effect of the net.

If the experimental hut data had subjects under ITNs during the whole biting period, and if the the πi value is to be factored in for each mosquito species (thus not via the `usage value`), the factors for `deterrency` change as follows:

`holeFactor=1-πi * (1-RelativeAttractiveness values for intact untreated nets)`

`insecticideFactor=1-πi * (1-RelativeAttractiveness values for holed treated nets)`

`interactionFactor=(1-πi * (1-RelativeAttractiveness values for intact treated nets))/holeFactor * insecticideFactor`

```
          <preprandialKillingEffect baseFactor="0.09" holeFactor="0.57" holeScalingFactor="0.10" insecticideFactor="0.604" insecticideScalingFactor="0.10" interactionFactor="-0.424"/>
```
`Relative pre-prandial survival=(1-(baseFactor+insecticideFactor * (1-exp(-InsecticideConcentration * insecticideScalingFactor))	  +holeFactor * exp(-HoleIndex.holeScalingFactor)+interactionFactor * exp(-HoleIndex * holeScalingFactor) * (1-exp(-InsecticideConcentration * insecticideScalingFactor))))/(1-baseFactor)`

Note: The factors can be calculated from data as follows:

`baseFactor = proportion killed by a badly holed untreated net`

`holeFactor = proportion killed by an intact untreated net - baseFactor`

`insecticideFactor = proportion killed by a badly holed treated net - baseFactor`

`interactionFactor = proportion killed by an intact treated net - baseFactor - holeFactor - insecticideFactor`

If the `holeFactor`=0 `insecticideFactor`=0 and `interactionFactor`=0, the relative pre-prandial survival=1 and there is 0 effect of the net.

If the experimental hut data had subjects under ITNs during the whole biting period, and if the the πi value is to be factored in for each mosquito species (thus not via the `usage value`), the factors for `preprandialKillingEffect` change as follows:

`holeFactor=πi * (proportion killed by intact untreated nets -baseFactor)`

`insecticideFactor=πi * (proportion killed by badly holed treated nets -baseFactor)`

`interactionFactor=πi * (proportion killed by intact treated nets)-baseFactor-holeFactor-insecticideFactor)`


```
          <postprandialKillingEffect baseFactor="0.10" holeFactor="0" holeScalingFactor="0.10" insecticideFactor="0.55" insecticideScalingFactor="0.10" interactionFactor="0"/>
```
The relative post-prandial survival is calculated similarly to the pre-prandial survival.
```
        </anophelesParams>
      </description>
        <continuous coverage="0.95" targetAgeYrs="0.0833"/>
```
Note that at a very young age (under 1 year), the current models may force unrealistically high outmigration (see also comment in demographics section). If the amount of nets allocated is relevant (e.g. for costing), this has to be taken into consideration, especially when nets are distributed continuously at a very young age.
The minimum targetAgeYrs has to be > timestep/365 in order to correspond to timestep=1, because timestep=0 is not allowed for continuous interventions.
```
      <timed coverage="0.785258112" maxAge="5" minAge="0" time="364"/>
     </ITN>
```


### Indoor residual spraying (IRS) ###

IRS acts via the same [three factors described above](ModelIntervVector#Net_/_indoor_spraying_interventions.md), and is also deployed as a [human-targeting intervention](ModelInterventions#Human_targeting_interventions.md).

IRS can be modelled via a simple model (see [GVI](ModelIntervVector#Generic_vector_intervention_(GVI).md)) or a model with indirect decay, similar to the ITN model but without hole factors. This is a description of the latter, previously called IRS version 2.

The description is as follows:
```
      <component id="IRS" name="new param">
        <IRS>
          <!-- Initial insecticide: units are μg/cm²; no variance here -->
          <initialInsecticide mu="118" sigma="0"/>
          <insecticideDecay L="0.15" function="exponential"/>
          <anophelesParams mosquito="gambiae_ss">
            <deterrency insecticideFactor="0.12" insecticideScalingFactor="0.05"/>
            <preprandialKillingEffect baseFactor="0.01" insecticideFactor="0.25" insecticideScalingFactor="0.008"/>
            <postprandialKillingEffect baseFactor="0.01" insecticideFactor="0.5" insecticideScalingFactor="0.006"/>
          </anophelesParams>
        </IRS>
      </component>
```
Prior to schema 32, the description looked like:
```
    <IRS name="DDT test v2">
      <description_v2>
        <!-- Initial insecticide: units are μg/cm²; no variance here -->
        <initialInsecticide mu="118" sigma="0"/>
        <insecticideDecay L="0.15" function="exponential"/>
        <anophelesParams mosquito="gambiae_ss">
          <deterrency insecticideFactor="0.12" insecticideScalingFactor="0.05"/>
          <preprandialKillingEffect baseFactor="0.01" insecticideFactor="0.25" insecticideScalingFactor="0.008"/>
          <postprandialKillingEffect baseFactor="0.01" insecticideFactor="0.5" insecticideScalingFactor="0.006"/>
        </anophelesParams>
      </description_v2>
      <timed>
        <deploy coverage="0.05775" cumulativeWithMaxAge="0.5" time="18"/>
        <deploy coverage="0.1155" cumulativeWithMaxAge="0.5" time="19"/>
        <deploy coverage="0.17325" cumulativeWithMaxAge="0.5" time="20"/>
        ...
      </timed>
    </IRS>
```
Insecticide content decays according to the decay function given. Effects are a function of the remaining insecticide content:
  * relative availability = exp( log(insecticideFactor) × [1-exp(-insecticideContent × insecticideScalingFactor)] )
  * killing effect = baseFactor + insecticideFactor × [1-exp(-insecticideContent × insecticideScalingFactor)]
  * survival factor = (1 - killing effect) / (1 - baseFactor)

Deployment has the same options as with the other model.

## Uninfect vectors ##

A special intervention which instantly uninfects all mosquitoes of malarial parasites (though doesn't prevent reinfection). If combining with mass drug administration to eliminate malaria from a scenario, note that humans can harbour gametocytes up to 20 days following "parasite clearance", hence vectors will need to be uninfected multiple times.

Example, clearing vector infections multiple times in line with above note about eliminating malaria:
```
<interventions name="uninfect population">
  <human>
    <component id="clearInfections">
      <MDA>
        <effects>
          <option pSelection="1">
            <clearInfections timesteps="5" stage="both"/>
          </option>
        </effects>
      </MDA>
    </component>
    <deployment name="clear human infections">
      <component id="clearInfections"/>
      <timed>
        <deploy coverage="1" time="73"/>
      </timed>
    </deployment>
  </human>
  <uninfectVectors name="clear mosquito infections">
    <timedDeployment time="73"/>
    <timedDeployment time="74"/>
    <timedDeployment time="75"/>
    <timedDeployment time="76"/>
    <timedDeployment time="77"/>
    <timedDeployment time="78"/>
  </uninfectVectors>
</interventions>
```


## Vector population interventions ##

The following interventions are all applied at the vector population level (not to specific human or non-human hosts). [Their description in the XML and deployment is described here.](ModelInterventions#Host-independent_vector_interventions.md)

### Seeking death rate increase (or decrease) ###

The [vector model](ModelTransmission#Vector_transmission.md) specifies a rate of mosquito death while host seeking; this intervention gives a temporary increase in that rate. The rate becomes `"old rate" × (1 + increase)`, where `increase` starts at `initial` and decays according to the [decay function given](ModelDecayFunctions.md) (thus with any decay function except `constant` the increase will decay towards 0). Example:

```
<anopheles mosquito="...">
  <seekingDeathRateIncrease initial="0.5">
    <decay function="exponential" L="0.1"/>
  </seekingDeathRateIncrease>
</anophelese>
```

Initial must be greater than or equal to -1; a (post-decay) value of -1 implies no death while host seeking, 0 implies no effect, 1 implies double the usual death rate, etc.

### Probability of death while ovipositing ###

The [vector model](ModelTransmission#Vector_transmission.md) describes a base probability of a mosquito surviving ovipositing to return to the host seeking population; this intervention describes an additional killing factor. The probability of surviving ovipositing becomes `"old probability" × (1 − factor)`, where `factor` starts at `initial` and decays according to the [decay function given](ModelDecayFunctions.md) (thus with any decay function except `constant` the factor will decay towards 0). Example:

```
<anopheles mosquito="...">
  <probDeathOvipositing initial="0.6">
    <decay function="exponential" L="1"/>
  </probDeathOvipositing>
</anophelese>
```

Initial must be between 0 and 1; a (post-decay) value of 0 implies no additional risk of death while a value of 1 implies no mosquitoes survive ovipositing.

### Emergence reduction (or increase) ###

Each day, some population of new vectors emerge from water bodies to join the population of flying vectors; this increases the number of flying vectors if it exceeds the number which died since the previous day. There are multiple models to describe the rate at which new vectors emerge. This intervention acts on top of whichever emergence model is in use by specifying a killing factor; the emergence rate becomes `"old rate" × (1 − factor)`, where `factor` starts at `initial` and decays according to the [decay function given](ModelDecayFunctions.md) (thus with any decay function except `constant` the factor will decay towards 0). Example:

```
<anopheles mosquito="...">
  <emergenceReduction initial="-1">
    <decay function="constant"/>
  </emergenceReduction>
</anophelese>
```

Initial must be at most 1; a (post-decay) value of 1 implies no emergence, 0 implies no effect, -1 implies double the original emergence rate, etc.