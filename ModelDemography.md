# Human population demographics #

The human population is regulated by the demography model. After individuals who die from malaria (direct or indirect) are removed from the population, the demography model uses out-migrations to enforce the specified age-structure and births to keep population size constant.

![http://openmalaria.googlecode.com/svn/wiki/graphs/human-demography.png](http://openmalaria.googlecode.com/svn/wiki/graphs/human-demography.png)

## Configuration ##

Our standard set of demography data comes from Ifakara, Tanzania:

```
  <demography maximumAgeYrs="90" name="Ifakara" popSize="1000">
    <ageGroup lowerbound="0.0">
      <group poppercent="3.474714994" upperbound="1"/>
      <group poppercent="12.76004028" upperbound="5"/>
      <group poppercent="14.52151394" upperbound="10"/>
      <group poppercent="12.75565434" upperbound="15"/>
      <group poppercent="10.83632374" upperbound="20"/>
      <group poppercent="8.393312454" upperbound="25"/>
      <group poppercent="7.001421452" upperbound="30"/>
      <group poppercent="5.800587654" upperbound="35"/>
      <group poppercent="5.102136612" upperbound="40"/>
      <group poppercent="4.182561874" upperbound="45"/>
      <group poppercent="3.339409351" upperbound="50"/>
      <group poppercent="2.986112356" upperbound="55"/>
      <group poppercent="2.555766582" upperbound="60"/>
      <group poppercent="2.332763433" upperbound="65"/>
      <group poppercent="1.77400255" upperbound="70"/>
      <group poppercent="1.008525491" upperbound="75"/>
      <group poppercent="0.74167341" upperbound="80"/>
      <group poppercent="0.271863401" upperbound="85"/>
      <group poppercent="0.161614642" upperbound="90"/>
    </ageGroup>
  </demography>
```

The only aspect of this we commonly change is the population size. Population sizes below around 1000 humans tend to have large amounts of stochastic noise (although this is still clearly noticeable with 1000 humans, particularly in low-frequency outputs such as deaths resulting from malaria). Large numbers increase computation time and RAM usage; as a rough guide 100,000 individuals when using a 5-day time-step or 10,000 individuals when using a 1-day time-step should require somewhere (very roughly, depending on machine performance) in the region of two hours computation.