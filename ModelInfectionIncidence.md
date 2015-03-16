# Introduction #

The [transmission model](ModelTransmission.md) is able to simulate each human's exposure (Entomological Infection Rate, or EIR, as bites per person per day), and the [within-host model](ModelWithinHost.md) simulates infection development, but between these we need a model to simulate the start of infections based on EIR.

This model is what we call, in the code, the `InfectionIncidenceModel`. This component of the simulator has four implementations, differing primarily in the expected number of infections calculated. In all cases, the expected number of infections is reduced by any preerythrocytic vaccines' effectiveness, and the number of infections introduced is sampled from a poisson distribution with mean the (vaccine-affected) expected number of infections.

Implementing models are:
  * InfectionIncidenceModel (the default): [Smith et al, 2006](http://www.ajtmh.org/cgi/content/abstract/75/2_suppl/11)
  * HeterogeneityWorkaroundII: emulates old, presumably unintended, behaviour
  * NegBinomMAII: Infections introduced by mass action. Baseline availability of humans is sampled from a gamma distribution.
  * LogNormalMAII: Infections introduced by mass action with log normal variation in human availability.

Susceptibility of humans (proportion of bites from sporozoite positive mosquitoes resulting in infection) also has two models:
  * Equation (7) of [Smith et al, 2006](http://www.ajtmh.org/cgi/content/abstract/75/2_suppl/11).
  * Fixed at 0.702; this is computed as 0.19 (the value S from a neg bin mass action model fitted to Saradidi data, divided by 0.302 (the ratio of body surface area in a 0.5-6 year old child (as per Saradidi) to adult).