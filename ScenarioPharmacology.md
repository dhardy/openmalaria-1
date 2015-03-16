# Antimalarial modeling #

We consider three options for modeling antimalarials in OpenMalaria:

  1. A simple success/failure model which clears all infections (`treatSimple` option in health system / intervention component) or does nothing. This has been extended to include a prophylactic effect which simply removes all new infections for a configurable number of steps in the future.
  1. Clearing infections via above method then using [Pre-erythrocytic vaccines](ModelIntervHuman#Vaccines.md) to probabilistically prevent reinfection (this allows a slightly more realistic prophylactic effect than the above).
  1. Pharmacokinetic and pharmacodynamic modeling of drugs (`treatPKPD` option in health system / intervention component)

This page is about the last option.

## Drug administration (`treatPKPD`) ##

Treatment via the PK/PD model involves using a `treatPKPD` element something like the following:
```
<treatPKPD schedule="CQ_3_dose" dosage="CQ"/>
```

This selects a schedule and dosage table for treatment. The schedule determines which drugs are administered and when; for example "CQ\_3\_dose" may specify three daily administrations of CQ. Poor drug quality and poor adherence may be modeled indirectly by using a dosage table with lower than expected quantities of active ingredient and using a schedule which misses certain doses.

It is also possible to delay all doses (by some number of hours) without creating a new schedule:
```
<treatPKPD schedule="AL_treat" dosage="AL_dosages" delay_h="84"/>
```

## Pharmacology data ##

The `<pharmacology>` element specifies drug parameters as well as schedules and dosage data. It has the following structure:
```
  <pharmacology>
    <treatments>
      <schedule name="..."> ... </schedule>
      <schedule name="..."> ... </schedule>
      ...
      <dosages name="..."> ... </dosages>
      <dosages name="..."> ... </dosages>
      ...
    <treatments/>
    <drugs>
      <drug abbrev="..."> ... </drug>
      <drug abbrev="..."> ... </drug>
      ...
    </drugs>
  </pharmacology>
```

### Drug descriptions ###

Starting with the most pharmacology-related part, PK and PD data for one drug can be specified as below. [Also see the reference.](https://github.com/vecnet/om_schema_docs/wiki/GeneratedSchema33Doc#elt-drug)
```
      <drug abbrev="MQ">
        <PD>
          <phenotype>
            <max_killing_rate>3.45</max_killing_rate>
            <IC50>0.5</IC50>
            <slope>5</slope>
          </phenotype>
        </PD>
        <PK>
          <negligible_concentration>0.005</negligible_concentration>
          <half_life>13.078</half_life>
          <vol_dist>20.8</vol_dist>
        </PK>
      </drug>
```

#### Sampling parameters (v34+) ####

In versions 34 and later of OpenMalaria it should be possible to specify `k` instead of a half-life, and to enable sampling per-human of this parameter (when sigma>0):
```
          <k mean="0.053" sigma="0"/>
```
Here, `mean` is the real mean and `sigma` is the standard deviation passed to the log-normal sampler. To get an equivalent sample in R:
```
mean <- 0.053
sigma <- 0.1
mu <- log(mean) - sigma*sigma / 2
hist(rlnorm(10000, mu, sigma), breaks=300)
```

#### Multiple genotypes ####

PD parameters are mapped to infection genotypes. Genotypes are specified in the [parasiteGenetics element](https://github.com/vecnet/om_schema_docs/wiki/GeneratedSchema33Doc#elt-parasiteGenetics).

Each set of PD parameters is labelled a ["phenotype"](https://github.com/vecnet/om_schema_docs/wiki/GeneratedSchema33Doc#elt-phenotype). When there are multiple phenotypes, each phenotype must specify how genotypes are mapped to the phenotype via ["restriction"](https://github.com/vecnet/om_schema_docs/wiki/GeneratedSchema33Doc#elt-restriction) elements.

An example:
```
      <drug abbrev="CQ">
        <PD>
          <!-- three alleles match those described above -->
          <phenotype name="sensitive">
            <restriction onLocus="mdr" toAllele="sensitive"/>
            <max_killing_rate>3.45</max_killing_rate>
            <IC50>0.02</IC50>
            <slope>1.6</slope>
          </phenotype>
          <phenotype name="resistant">
            <restriction onLocus="mdr" toAllele="resistant"/>
            <max_killing_rate>3.45</max_killing_rate>
            <IC50>0.04</IC50>
            <slope>1.6</slope>
          </phenotype>
          <phenotype name="highly resistant">
            <restriction onLocus="mdr" toAllele="highly resistant"/>
            <max_killing_rate>3.45</max_killing_rate>
            <IC50>0.08</IC50>
            <slope>1.6</slope>
          </phenotype>
        </PD>
        <PK>
          <negligible_concentration>0.00036</negligible_concentration>
          <half_life>30.006</half_life>
          <vol_dist>300</vol_dist>
        </PK>
      </drug>
```


### Treatment schedules ###

Treatment schedules and dosage information is also stored in the `pharmacology` section. When a patient is treated (via a `treatPKPD` element), a schedule and a dosage table (`dosages`) must be specified by name. Drugs are prescribe according to the schedule, except that the dosage table modifies the quantity of drug given.

Doses can be calculated in one of three ways:

1.  According to age. In this case the `mg` specified in the schedule is a base amount (e.g. one pill or 1/4 pill), and multipliers are specified according to age group in the table: `<age lowerbound="5" dose_mult="2"/>` (`lowerbound` is an age in years; multiple `age` elements should be used).
2.  According to weight (mass) in kg. This is the same as for age, except that `lowerbound` is now a mass in kg and `age` is replaced with `bodymass`.
3.  In terms of mg active ingredient per kg body mass (versions 34 and later). In this case a single `multiply` element is used: `<multiply by="kg"/>` and the `mg` specified in the schedule is mg/kg.

For example, the code below configures one schedule of three doses over three days (using a base amount of active ingredient) and a dosage table multiplying the base quantities in the schedule to get a final quantity (based on body mass in this case, but optionally based on age):
```
  <pharmacology>
    <treatments>
      <schedule name="CQ_3_dose">
        <!-- This is recommend dose per kg (KK) -->
        <medicate drug="CQ" hour="0" mg="10"/>
        <medicate drug="CQ" hour="24" mg="10"/>
        <medicate drug="CQ" hour="48" mg="5"/>
      </schedule>
      <dosages name="CQ">
        <!-- contrived multipliers by DH to give roughly a 1-per-kg multiplier,
        rounding up -->
        <bodymass lowerbound="0" dose_mult="5"/>
        <bodymass lowerbound="5" dose_mult="10"/>
        <bodymass lowerbound="10" dose_mult="20"/>
        <bodymass lowerbound="20" dose_mult="40"/>
        <bodymass lowerbound="40" dose_mult="70"/>
      </dosages>
    </treatments>
    <drugs>
    </drugs>
  </pharmacology>
```

Dosage tables can use age instead of body mass:
```
      <dosages name="standard_dosages">
        <age lowerbound="0" dose_mult="5"/>
        <age lowerbound="2" dose_mult="20"/>
        <age lowerbound="6" dose_mult="60"/>
      </dosages>
```

Alternatively, it is possible (v34+) to multiply directly by body-mass (thus making the dose specified in "mg" to be mg/kg):
```
      <dosages name="perkilo">
        <multiply by="kg"/>
      </dosages>
```