# Specification of level and seasonality of transmission #

The models require as input, data on the overall average transmission level in the absence of interventions (measured by the entomological inoculation rate, EIR).

Data on seasonality of malaria transmission for driving models might be available in the form of seasonality in any of a number of malariological indices (see Table below).  The models, however, expect the seasonality in the EIR as the input.  If the data are available in the form of a different measure of seasonality, they need to be transformed before being used to drive the models.  The easiest approximation is to introduce a fixed lag period, depending on which measure is used.  While this is a considerable simplification, because it assumes proportionality between different measures, this may be reasonable, especially if the data relate to mosquito densities or emergence rates.

The table contains suggestions for what might be the approximate lag periods between different measures of seasonality.  A positive value (Lx) for the lag for measure x implies that the EIR seasonality reflects the value of x, Lx days previously.  This table provides only a very approximate guide with values rounded to multiples of 5 days.  The actual average lag periods in the simulations are model dependent and will vary somewhat from these.  The lag periods in the field also vary and, in the case of quantities measured in the vector population, are dependent on the environmental temperature.

**Table: Approximate lag periods between different seasonality measures**
| **Transmission measure** | **Lag period (Lx) (days)** |
|:-------------------------|:---------------------------|
| Rainfall |+30 |
| Emergence rate of vector | +20 |
| Density of host-seeking vectors | +10 |
| _Entomological inoculation rate_ | 0 |
| Incidence of infection | -5 |
| Incidence of patent infection | -10 |
| Incidence of clinical malaria | -15 |
| Incidence of severe malaria | -20 |

## Ways of specifying level and seasonality in EIR ##

EIR can be specified either via 5 Fourier coefficients and a rotation factor (`EIR` element) or via 12 monthly values plus an annual level (`monthlyEIR` element). Exactly one of these elements must appear.

#### EIR via Fourier coefficients ####

This is the older method (the only available method before schema version 22).

The `EIR` element describes the Entomological Infection Rate for this mosquito species, which is used as a target when fitting the emergence rate of adult mosquitoes. The EIR is given via a [Fourier series](http://en.wikipedia.org/wiki/Fourier_series) and a rotation offset; more accurately, the exposed EIR, in units of bites per day, is:

![http://openmalaria.googlecode.com/svn/wiki/eqns/EIR_Fourier.png](http://openmalaria.googlecode.com/svn/wiki/eqns/EIR_Fourier.png)

<a href='Hidden comment: 
Above image rendered as latex from:
 \Xi_t = \exp \left( a_0 + \sum_{n=1}^2 \left[ a_n \cos \left(n (w t - \theta) \right) + b_n \sin \left(n (w t - \theta) \right) \right] \right) 
(Use, for example, [http://sciencesoft.at/latex/?lang=en] with the AMS maths formula template.)

MathML version (not so nice and less compatible):
<wiki:gadget url="http://mathml-gadget.googlecode.com/svn/trunk/mathml-gadget.xml" border="0" up_content="eir_t = exp ( a_0 + sum_(n=1)^2 { a_n cos(n (w t - theta)) + b_n sin(n (w t - theta)) } )"/>

(Formula uses MathML. For IE, a [http://www.dessci.com/en/products/mathplayer/ plugin] is needed.)'></a>

Here, Ξ<sub>t</sub> is the number of innoculations per person per day, where `t` is the day of year (from 0 to 364), `w = 2π / 365`, `θ` is the `EIRRotateAngle` attribute and a<sub>0</sub> to a<sub>2</sub>, b<sub>1</sub> and b<sub>2</sub> are the corresponding attributes of the `EIR` element.

If we introduce the function `f(t)` dependent on a<sub>n</sub> and b<sub>n</sub> for n≥1, we can reformulate Ξ<sub>t</sub> as

![http://openmalaria.googlecode.com/svn/wiki/eqns/EIR_Fourier_f.png](http://openmalaria.googlecode.com/svn/wiki/eqns/EIR_Fourier_f.png)
<a href='Hidden comment: 
\begin{align*}
\Xi_t &= e^{a_0 + f(t)} \\
&= e{a_0} \cdot e{f(t)}
\end{align*}
'></a>

and thus show that the annual EIR is scaled by ![http://openmalaria.googlecode.com/svn/wiki/eqns/exp_a0.png](http://openmalaria.googlecode.com/svn/wiki/eqns/exp_a0.png):

![http://openmalaria.googlecode.com/svn/wiki/eqns/EIR_Fourier_annual.png](http://openmalaria.googlecode.com/svn/wiki/eqns/EIR_Fourier_annual.png)
<a href='Hidden comment: 
\begin{align*}
\Xi &= \int_{t=0}{365} e{a_0} e^{f(t)} dt \\
&= e{a_0} \int_{t=0}{365} e^{f(t)} dt
\end{align*}
'></a>


This is configured using a section in the XML, per species, similar to the following:
```
<seasonality annualEIR="178.60558666831946" input="EIR">
          <fourierSeries EIRRotateAngle="0">
            <coeffic a="-0.692164" b="0.002098"/>
            <coeffic a="0.401189" b="-0.375356"/>
          </fourierSeries>
        </seasonality>
```

#### EIR via monthly values ####

As of schema version 22 this method of entering EIR was added to simplify entry of field data. Fourier coefficients are still used within the code (for smoothing), but are calculated internally.

The `monthlyEIR` element requires one attribute: `annualEIR`, specifying the total annual EIR as infectious bites per person per year. It should have a sequence of 12 child elements, named `item`, specifying the relative in terms of infectious bites per person per month (which might be approximated by densities of  mosquito densities, with an appropriate lag), in the months of January through December. The overall EIR is scaled to the annualEIR value indicated at the top of the element. An (unrealistic) example:

```
        <seasonality annualEIR="20" input="EIR">
          <monthlyValues smoothing="fourier">
            <value>10</value>
            <value>1</value>
            <value>1</value>
            <value>1</value>
            <value>1</value>
            <value>1</value>
            <value>1</value>
            <value>1</value>
            <value>1</value>
            <value>1</value>
            <value>1</value>
            <value>1</value>
          </monthlyValues>
        </seasonality>
```

## Seasonality in the static transmission mode (mode="2") ##

With static transmission the seasonality pattern during the intervention phase of the simulation, reproduces the input pattern of seasonality.

## Seasonality in the dynamic transmission mode (mode="4") ##

If the simulation is run using the dynamic transmission mode (mode="4") then the simulated EIR is not constrained to repeat the same seasonal pattern as the input EIR after the warm-up period.

The simulated EIR will vary because of the effects of interventions.  In addition, there may be stochastic variations or fluctuations even in the absence of interventions.  This is particularly the case with small simulated populations or when the simulation is close to the limit of sustainable transmission (e.g. with high case-management coverage) the simulated EIR will usually be somewhat below the level input.

During the warm-up period, the entomological parameters are adjusted to try to ensure that the output EIR corresponds as closely as possible to that input.  An exact match, consistent with the emergence rate (N\_v0) being positive, is in general impossible. The seasonality of the simulated EIR (which is close to that of S\_v plus the delay due to the lag) may differ from the seasonality of the input EIR substantially

In versions before 30 of OpenMalaria the change from the static to dynamic generation of the EIR is at the end of the warm-up period.  In these simulations, even if the annualEIR is equal before and after the end of the warm-up period, the switch in seasonality may alter the annual number of new infections. Running scenarios without interventions shows the dynamics of EIR and human infectiousness (the continuous output mode with duringInit="true" is convenient for viewing this).