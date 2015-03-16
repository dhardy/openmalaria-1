# Miscellaneous properties of humans #

## host-weight and availability to mosquitoes ##

From the perspective of the mosquito, human hosts can be thought of as a resource, different amounts of which are offered by different humans.
The simulations allow the availability of individual humans to mosquitoes to depend on the host-age, with the intention of capturing observations that larger humans are bitten more by mosquitoes.
Before schema 24, this function was hard coded; since then, the data for them have been included in the scenario file. Standard values (based on a dataset describing ponderal growth in rural Tanzania) are as follows:
```
<human>
  <availabilityToMosquitoes>
    <group lowerbound="0.0" value="0.225940909648"/>
    <group lowerbound="1.0" value="0.286173633441"/>
    <group lowerbound="2.0" value="0.336898395722"/>
    <group lowerbound="3.0" value="0.370989854675"/>
    <group lowerbound="4.0" value="0.403114915112"/>
    <group lowerbound="5.0" value="0.442585112522"/>
    <group lowerbound="6.0" value="0.473839351511"/>
    <group lowerbound="7.0" value="0.512630464378"/>
    <group lowerbound="8.0" value="0.54487872702"/>
    <group lowerbound="9.0" value="0.581527755812"/>
    <group lowerbound="10.0" value="0.630257580698"/>
    <group lowerbound="11.0" value="0.663063362714"/>
    <group lowerbound="12.0" value="0.702417432755"/>
    <group lowerbound="13.0" value="0.734605377277"/>
    <group lowerbound="14.0" value="0.788908765653"/>
    <group lowerbound="15.0" value="0.839587932303"/>
    <group lowerbound="20.0" value="1.0"/>
    <group lowerbound="20.0" value="1.0"/>
  </availabilityToMosquitoes>
  <weight multStdDev="0.14">
    <group lowerbound="0.0" value="13.9856718"/>
    <group lowerbound="1.0" value="18.30372108"/>
    <group lowerbound="2.0" value="21.745749"/>
    <group lowerbound="3.0" value="24.25753512"/>
    <group lowerbound="4.0" value="26.06595444"/>
    <group lowerbound="5.0" value="28.48868784"/>
    <group lowerbound="6.0" value="30.84202788"/>
    <group lowerbound="7.0" value="33.48638244"/>
    <group lowerbound="8.0" value="35.20335432"/>
    <group lowerbound="9.0" value="37.19394024"/>
    <group lowerbound="10.0" value="40.1368962"/>
    <group lowerbound="11.0" value="42.00539916"/>
    <group lowerbound="12.0" value="44.53731348"/>
    <group lowerbound="13.0" value="46.77769728"/>
    <group lowerbound="14.0" value="49.48396092"/>
    <group lowerbound="15.0" value="54.36"/>
    <group lowerbound="20.0" value="60.0"/>
    <group lowerbound="20.0" value="60.0"/>
  </weight>
</human>
```
The `weight` data is only required for 1-day timestep models (it is used by the drug model); the `availabilityToMosquitoes` data is needed for all models. Graphs of the availability and weight data with interpolations (linear interpolations, shown by the blue lines, are enabled by default):

<a href='Hidden comment: 
Graphs generated with the trunk/unittest/AgeGroupInterpolationSuite.py script.
'></a>
![http://openmalaria.googlecode.com/svn/wiki/graphs/human_avail_and_mass_age.png](http://openmalaria.googlecode.com/svn/wiki/graphs/human_avail_and_mass_age.png)