# Decay functions #

Several interventions, such as bed nets and vaccines, have an effectiveness which
decays over time (at the population and possibly the individual level). To allow
use of different decay functions, a generalized method of describing decay was
added into schema version 25. The following set of functions are available:

| function    | formula | L is time to | |
|:------------|:--------|:-------------|:|
| constant    | `1` |  | no decay, ever |
| step        | `1` | full decay | no decay for `t < L`, full decay (0) at time `L` |
| linear      | `1 - t/L` | full decay | for `t < L`, otherwise 0 |
| exponential | `exp( - t/L * log(2) )` | half decay |  |
| weibull     | `exp( -(t/L)^k * log(2) )` | half decay | equivalent to exponential when `k = 1` |
| hill        | `1 / (1 + (t/L)^k)` | half decay |  |
| smooth-compact | `exp( k - k / (1 - (t/L)^2) )` | full decay | for `t < L`, otherwise 0 |

where `L` is a description of the rate of decay, either the time until half decay or the time until full decay (see above) in years, and `k` is a shape
parameter (no dimension).

The smooth-compact, step, and linear decay functions reach zero at time L, whereas the
Weibull, Hill and exponential functions never reach zero and have half their
original efficacies at time L.

![http://openmalaria.googlecode.com/svn/wiki/graphs/decay-functions.png](http://openmalaria.googlecode.com/svn/wiki/graphs/decay-functions.png)

## Configuration ##

For certain interventions, the decay of efficacy is described by an XML element of type `DecayFunction`.  A few examples follow:

```
<eltName L="10.0" function="exponential"/>
<eltName L="3" function="weibull" k="1.5"/>
<eltName L="3" function="weibull" k="1.5" mu="1" sigma="0.1"/>
```

The `function`, `L` and `k` attributes act as described above, with `k` optional (defaults to `1` if not specified). Note: `L` must be specified even when `function="constant"`, but is ignored in this case (so stick in any value to make OpenMalaria happy).

## Heterogeneity ##

Starting with version 26 of **OpenMalaria**, variations can be introduced into decay rate: a random variate x is sampled from the log-normal distribution, and t in the above formulas is replaced by t/x.

The attributes `mu` and `sigma` can be specified as part of the decay-function description element. Both take value 0 if not specified. If sigma this is non-zero, x is sampled from the log-normal: `x ~ lnN(mu,sigma²)`.