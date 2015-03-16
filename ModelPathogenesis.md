# Pathogenesis model #

OpenMalaria's pathogenesis models determines the onset of sickness bouts (the start is termed an "event") based on parasite density. Different models are implemented including:

  * **Base pathogenesis model** This is a stochastic model that relates the probability of a clinical attack of malaria to the peripheral parasite densities via a pyrogenic threshold that itself responds dynamically to the parasite load. [Smith et al, 2006](http://www.ncbi.nlm.nih.gov/pubmed/16931816) (this is implemented in the `PyrogenPathogenesis` class): The parameters of the model have been estimated by fitting it to the relationship between incidence of clinical episodes and the entomologic inoculation rate, using age-specific incidence data from two villages in Senegal and one village in Tanzania.
  * **Predetermined bouts pathogenesis model** This is a variant of the base pathogenesis model in which bouts occur deterministically when the parasite density exceeds the pyrogenic threshold (`PredetPathogenesis` class).
  * **Mueller pathogenesis model** This is a stochastic model that uses a log-linear function of parasite densities to determine the probability of a clinical bout, based on equations fitted by [Mueller et al, 2009](http://www.ncbi.nlm.nih.gov/pubmed/19602275) to data from Papua New Guinea.  There is no explicit pyrogenic threshold in this model. (This is implemented in the `MuellerPathogenesis` class).

### Non-malarial fever incidence and severity ###

| **Age-group**| **All fever pppy**| | **RR**| **Estimated non-malarial fevers pppy**|
|:-------------|:------------------|:|:------|:--------------------------------------|

| 0-11 mo.| 9.1|9.66| 1| 6.08|
|:--------|:---|:---|:-|:----|
| 12-59 mos|9.8| 9.66| 1| 6.08|
| 5-9 vrs.| 6.06| 6.06| 0.627329| 3.81|
| 10-14 yrs.| 4.16| 4.16| 0.430642| 2.62|
| 15-59 yrs.| 6.43| 6.43| 0.665631| 4.05|
| 60 yrs +| 8.6| 8.6| 0.890269| 5.41|

_Table: Estimated incidence of non-malarial fevers. Source: Incidence of non-malarial fever in various age-groups and epidemiological settings, Allan Schapira, 8/12/2009._

To model age-based incidence of non-malarial fever, we currently use the data shown in the above table. In order to transform this into a continuous function for frequency given age, we interpolate linearly on the data-points generated as follows:
1.	For each age group, add a data point centred on the age axis with frequency as given in the age group.
2.	Add data points at ages 0 and infinity, taking frequency from that of the youngest and oldest age groups.
We model the incidence of non-malarial fever as an interpolated linear function. Non-malarial fevers can only start at a given time point if it has been determined that a malarial fever is not starting at that time point. If both a non-malarial fever and a malarial fever occur within the health system memory, only the malaria episode is reported.

Each non-malarial fever has a certain age-based probability of needing antibiotic treatment, which we assume to be independent of parasitological status.