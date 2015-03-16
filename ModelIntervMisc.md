# Pseudo interventions #

The simulation description can include a number of actions that, while not technically corresponding to malaria interventions, act on the simulated population in an analogous way and are hence specified in the intervention section of the scenaria description.
The following table lists these pseudo-interventions and the deployment methods for them:

| pseudo-intervention | mass-deployment | continuous-deployment |
|:--------------------|:----------------|:----------------------|
| [Import infections](ModelIntervMisc#Import_Infections.md) | yes | no |
| [InsertR\_0Case](ModelIntervMisc#Insert_R_0_case.md) | yes | no |
| [immuneSuppression](ModelIntervMisc#Immune_suppression.md) | yes | no |
| [recruitmentOnly (sub-populations)](ModelIntervMisc#Subpopulations.md) | yes | yes |

## Import Infections ##

This pseudo-intervention provides a way to model infections being imported into the simulated population. It is based on an importation rate, x(t), and forcibly importing cases according to the Bernoulli distribution (that is, assuming an importation rate of r cases per person per time-step, for each simulated human and for each time-step the probability of that person receiving an imported infection is r).

The importation rate is actually specified with units of imported cases per 1000 people per year. In the following example, 5 cases per 1000 people per year are imported for the first 10 years, after which the importation rate doubles:

```
<interventions>
  <importedInfections name="example">
    <timed>
      <rate time="0" value="5"/>
      <rate time="3650" value="10"/>
    </timed>
  </importedInfections>
</interventions>
```

If you only need a constant case-importation rate, only include one `<rate>` element with time 0. Periodically varying importation rates can also be specified, for example using three different rates depending on the time of year (this example using the one day timestep):
```
    <importedInfections>
      <timed name="annually varying rates" period="365">
        <rate time="0" value="1"/>
        <rate time="100" value="5"/>
        <rate time="200" value="2"/>
      </timed>
    </importedInfections>
```

## Insert R\_0 case ##

_As of schema 32, this intervention has been removed._ If necessary it can be re-introduced.

The `InsertR_0Case` pseudo-intervention is used in estimating R<sub>0</sub>. It is assumed before deployment that all infections have been removed from the population, and that all humans are completely protected with a transmission-blocking vaccine.

This intervention then picks one human, removes his transmission-blocking vaccine but gives him a pre-erythrocytic vaccine to prevent re-infection (bypassing the usual vaccine configuration to allow this person to have a PEV vaccine without the rest of the population), and finally infects this human.

Its purpose is to allow estimates of R<sub>0</sub>, along the following lines: if only one person is infected and is able to transmit infections, then the total number of infections received after this (reported by "new infections" output) is the number of secondary cases one primary infection has caused (within the simulator).

XML description accepts no configuration other than time:
```
    <insertR_0Case>
      <timed time="146"/>
    </insertR_0Case>
```

## Immune suppression ##

The `immuneSuppression` intervention removes all natural immunity (though not vaccine effects) from humans. It does not prevent development of new immunity.

It is described with standard [timed deployment](ModelInterventions#Timed_descriptions.md):
```
    <immuneSuppression>
      <timed coverage="1" time="78"/>
    </immuneSuppression>
```

## Recruitment only ##

To allow recruitment into a sub-population without an associated intervention deployment, the psuedo-intervention `recruitmentOnly` can be used:

```
<interventions>
  <component id="my sub-pop">
    <recruitmentOnly/>
  </component>
  ...
  <deployment>
    <component id="my sub-pop"/>
    <!-- continuous and/or timed deployments here to recruit members -->
  </deployment>
</interventions>
```

Note: if deploying an intervention to a sub-population on the same timestep as that sub-population was recruited, read [order of deployments](ModelInterventions#Order_of_deployments.md).