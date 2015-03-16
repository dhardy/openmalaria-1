# Mosquito Population Dynamics #

In the default vector model, emergence of nulliparous vectors from breeding sites follows a periodic orbit (seasonal pattern) that is calculated from that of the EIR used to drive the simulations.  This seasonal pattern may be varied by simulating [larviciding](ModelIntervVector#Larviciding.md), (which acts by explicitly reducing the emergence rate, proportionately to the coverage).

The Mosquito Population Dynamics Models described here are alternatives, that allow for feedback from the densities of ovipositing female mosquitoes on subsequent emergence.  This allows the user to specify varying degrees of density-dependent regulation of populations immature mosquitoes, so that adulticiding interventions may reduce emergence rates.
The Mosquito Population Dynamics Models must be explicitly activated.


## Simple Mosquito Population Dynamics Model ##

This model relates emergence to the number of ovipositing mosquitoes, limiting the population via the use of a Beverton-Holt model. For a full description of this model, see [A Simple Periodically-Forced Difference Equation Model for Mosquito Population Dynamics, Nakul Chitnis, 2012](http://openmalaria.googlecode.com/svn/download/articles/MosquitoLifeCycleSimplePer.pdf).

This model is activated with the `VECTOR_SIMPLE_MPD_MODEL` model switch:
```
    <ModelOptions>
      ...
      <option name="VECTOR_SIMPLE_MPD_MODEL" value="true"/>
      ...
    </ModelOptions>
```

The following parameters must be specified for each vector species:
```
      <anopheles mosquito=...>
        <seasonality ... >
        <mosq ... >
        <simpleMPD>
          <!-- These are made-up values! Parameterisations needed! -->
          <developmentDuration value="11"/>
          <developmentSurvival value="0.6"/>
          <femaleEggsLaidByOviposit value="50"/>
        </simpleMPD>
        <nonHumanHosts ... >
      </anopheles>
```

  * `developmentDuration` is the duration from egg-laying to emergence as an adult, and must be constant. It is _θj_ in the model description.
  * `developmentSurvival`, or _ρ_, is the probability of an egg surviving to emerge as an adult in the absense of density/resource restrictions.
  * `femaleEggsLaidByOviposit`, or _b_, is the mean number of female eggs laid by an adult per feeding-ovipositing-cycle.