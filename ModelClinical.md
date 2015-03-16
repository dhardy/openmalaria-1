# Clinical models #

In **OpenMalaria**, the [pathogenesis models](ModelPathogenesis.md) determine whether an individual becomes sick, and whether the sickness is uncomplicated or severe. The clinical models determine whether, when and how the sickness is treated and how long the sickness lasts. There are two implementations of the clinical models. The first model uses a five-day time step, the second a one-day time step. In the former, clinical and parasitological outcomes are predicted within the model, whereas the latter works together with the models for [drug Action](ModelDrug.md), severe outcomes, and [within-host dynamics](ModelWithinHost.md) to predict these outcomes.

### Notation ###
Due to confusion of terms while adapting the clinical model for a one-day timestep, we have developed the following notation:

  * event -- an occurrence without measurable duration. This can be a bout in the five-day time step model since the whole bout occurs on a single time step, but in the one-day time step model an event is strictly a state-change.
  * bout -- a "bout of sickness" describes a fever, malarial or otherwise, or period of severe illness, usually lasting no more than five days.
  * episode -- the health-system's reporting unit; each bout is either considered the start of a new episode, or, if occurring within the health system memory (currently 30 days) of the start of the last episode, considered part of that episode. Thus, at least in low transmission settings, an episode should roughly correspond to one malaria infection (which can cause multiple bouts). An episode's severity (non-malarial, malarial or severe) is considered to be that of its worst bout.

## Shared components ##

### Parasite diagnostics ###

This was designed for the 1-day time-step model, but should be equally applicable to other models and may in the future be used for monitoring outputs like the number of patent hosts (`nPatent`).

As part of the case management model, we need to simulate outcomes of parasitological tests, both rapid diagnostic tests (RDTs) and microscopy. These outcomes are known to depend on parasite density of the individual being tested(http://www.wpro.who.int/sites/rdt/whatis/action.htm, [Kilian et al, 2000](http://www.ncbi.nlm.nih.gov/pubmed/10672199)). To model this, we looked for a function to relate parasite densities to the probability of a positive outcome.

**Source data**  We used the data in Table 4 in [Murray et al, 2008](http://www.ncbi.nlm.nih.gov/pubmed/18202438) to model the outcome of RDTs for <i>Plasmodium falciparum</i>. This table shows the percent sensitivity (and the 95% confidence interval) found for the RDT BinaxNOW Malaria in a rigorous field trial, at different levels of parasitaemia. This assay is based on detection of the antigens HRP-2 for <i>P. falciparum</i> and aldolase for generic <i>Plasmodium</i>.  We associate the sensitivities listed for <i>P. falciparum</i> with the mid-points of the parasite density ranges listed, and one minus the specificity for a density of 0 parasites.

To model microscopy, we assume the data shown in Table 1. This is sourced from expert opinion (personal communication, A. Schapira), backed by the observation that microscopy diagnostic errors are noted more commonly for low-density parasitemias of 10 to 100 parasites/µL([Kilian et al, 2000](http://www.ncbi.nlm.nih.gov/pubmed/10672199)).

Table 1:

| **parasites / µL**| **sensitivity (**<i>P. falciparum</i>)|
|:-------------------|:--------------------------------------|
| >100|90%|
| 0-100|75%|
| Specificity|75%|

**Model** We found a sigmoidal function a good match for this data. Note that since the probability of obtaining a positive outcome when testing a sample with no parasites is non-zero, we need to scale the sigmoidal. With a parasite density of Y, probability of a positive outcome with zero parasites α, and parameter β, this gives us the probability of a positive test outcome as a function of parasite density:

![http://openmalaria.googlecode.com/svn/wiki/eqns/P_of_positive_test.png](http://openmalaria.googlecode.com/svn/wiki/eqns/P_of_positive_test.png)

and thus the diagnostic test’s output can be modeled as Bernoulli (f(Y)).


**Parameters** We use the parameters in Table 2 to model RDT and microscopy diagnostics.

Table 2:

| **Parameter**| **Description**| **RDT**| **Microscopy**|
|:-------------|:---------------|:-------|:--------------|

| α|specificity|0.942| 0.75|
|:--|:----------|:----|:----|
| β|density at which sensitivity is half given α=1| 50|


**Evaluation** For the <i>Plasmodium falciparum data</i>, plotting our predictions for the probability of a positive outcome by RDT against the source data shows that our model predictions are in good agreement with the source on which they are based (Figure 1).

Figure 1:

http://openmalaria.googlecode.com/svn/wiki/graphs/RDT_model_correlation.PNG

## Five-day timestep ##

[Tediosi et al, 2006](http://www.ncbi.nlm.nih.gov/pubmed/16931820) (implemented by the ClinicalImmediateOutcomes class) presented a clinical model for the case management of malaria. In this model there are three entry points to the decision trees: uncomplicated clinical malaria with no recent history of treatment, uncomplicated clinical malaria with recent history of treatment, and severe malaria. “Recent history” is defined as within the last 30 days.

This model uses a five-day time step, and bouts of sickness are assumed to have concluded within this time period (either the individual recovers or dies). Following recovery, the individual can experience another bout at the next time step, but bouts of illness occurring within 30 days of a previous bout are considered to be a recurrence of the same episode. The severity assigned to the episode (for reporting purposes) is that of the most severe bout within the 30-day period.

An initial bout of uncomplicated clinical malaria may either be treated at home or in peripheral health facilities, or go untreated, with the probability of a parasitological cure dependent on drug efficacy and patient compliance. In this model, treatment either completely clears parasites or has no effect. An uncomplicated malaria case that was treated in the last 30 days is assumed to either seek care or not. If care is sought, it is assumed to take place in the formal health system, with treatment being based on the second-line drug. Severe malaria cases can either be treated as an inpatient, or not be treated at all, each with a corresponding age-specific case fatality rate. Parasites are cleared in all hospitalized cases who survive. The epidemiologic effect depends stochastically on the values of the joint probabilities of the clinical and parasitologic outcomes, conditional on the clinical event.

Predictions are made across a range of different transmission intensities, by varying the entomological inoculation rate, and across case management scenarios with different coverage rates, by varying the decision tree probabilities.

## One-day timestep ##

The Crowell _et al._ model (paper not yet published; implemented by the ClinicalEventScheduler class) is an adaptation of the above [Tediosi et al model](http://www.ncbi.nlm.nih.gov/pubmed/16931820) to a one-day time step. We break the model down into two components: clinical scheduler and clinical decisions.

Moving to a one-day time step implied the need for a clinical scheduler to mediate between the [Pathogenesis](ModelPathogenesis.md), clinical decisions and severe outcomes models. The clinical scheduler decides and tracks the clinical state of the individual, based on the state as determined by the pathogenesis model and recent treatment history, allocates individuals to the clinical decisions and severe outcomes models as necessary, and determines final clinical outcomes.

The clinical decisions component of the model (implemented in the ESCaseManagement class) consists of stochastic decision trees concerned with determining the treatments given (including the actual schedule followed and dose sizes taken), along with whether the patient was hospitalized and whether treatment or hospitalization delays occurred. The decision trees also provide information necessary for calculating costs, derived from treatment-seeking behaviour, drugs administered, and diagnostic tests used.

### Clinical scheduler ###

In this model, each individual is assigned a clinical state which lasts until the next event. There is the possibility to enable the NON\_MALARIA\_FEVERS option. If this is not enabled, possible states are healthy, uncomplicated malarial fever, severe malaria, and dead. The corresponding events are start of an uncomplicated malarial fever from a healthy state, start of severe malaria (from healthy or uncomplicated malaria states), direct death (from a severe malaria state) and recovery (from uncomplicated malaria or severe malaria states). When the NON\_MALARIA\_FEVERS option is enabled, an additional state of uncomplicated non-malarial fever exists. Then, the corresponding events are start of an uncomplicated fever (malarial or non-malarial) from a healthy state, start of severe malaria (from healthy or uncomplicated fever states), direct death from a severe malaria state, death from a non-malarial illness, and recovery (from uncomplicated fever or severe malaria states). Transitions from severe malaria to uncomplicated fever, or from uncomplicated malaria to direct death, are not allowed.

A _bout_ commences with a morbidity _event_ and concludes with a recovery or mortality _event_ (stressing terminology used). When the bout first occurs, the clinical scheduler calls the clinical decisions model. At the start of an uncomplicated fever, we model a potential treatment-seeking delay. In this model, the bout duration of uncomplicated malarial fevers and non-malarial fevers is fixed at three days from the time of seeking treatment, or from the start of illness (if treatment is not sought). If an individual delays seeking treatment for one or two days, bout duration becomes four or five days, respectively. During this time, the clinical scheduler maintains the individual in the same state, unless the [Pathogenesis models](ModelPathogenesis.md) indicate that the illness has progressed to severe or the individual has died from a non-malarial illness. After this time period, it is assumed that parasite densities have been reduced by treatment or immunity, or that the individual has recovered from the non-malarial illness, and the individual is returned to the healthy state.

While healthy or during an uncomplicated bout, a severe bout may commence. In this case, the clinical scheduler calls the clinical decisions model and cancels any recent drug treatment. The severe bout is fixed at six days long when the patient enters hospital immediately or not at all; a one-day delay to hospital entry is also considered possible which increases the bout length to seven days.

Mortality is possible during severe malaria bouts, and considered as a stochastic function of the proportional reduction in parasite density each day of the case (TODO: link article). Each day during the severe illness, the clinical scheduler calls the severe outcomes model, which applies the appropriate case fatality rate to determine whether or not the individual died that day. For delayed hospital entry, the community death rate takes effect on the first day. Mortality events are always modeled as happening at the end of the associated severe episode.

### Clinical decisions ###

Case management in bouts of uncomplicated and severe sickness is determined using separate decision trees, with the following three entry points: uncomplicated fever with no recent history of treatment, uncomplicated fever with recent history of treatment, and severe malaria. In both cases, arbitrary decisions based on age, the outcome of a parasitological test, and decisions based on previous decisions and random sampling are allowed.

The decision trees in the [Tediosi et al model](http://www.ncbi.nlm.nih.gov/pubmed/16931820) were revised to describe more realistically the patient and provider decisions affecting case management coverage and to be more generally applicable for case management in areas of any level of P.falciparum risk. Also, the finer-grained simulation, when linked with models of pharmacokinetics and pharmacodynamics, allows modelling of parasite response to treatment and thus partial drug failure due to parasite resistance or inadequate dosing.

#### Uncomplicated fever ####

At the start of an uncomplicated fever, we model a potential treatment-seeking delay. The simulated individual has a fixed chance of seeking treatment immediately, or, if yet to do so, on the following two days. If treatment is not sought on all three occasions, the individual reverts to the healthy state. When the individual does seek treatment, the uncomplicated case management tree determines any treatments taken, and the individual is deemed to recover three days from the time of treatment-seeking.

The decision tree provides a more detailed description of treatment-seeking than the previous model, including from the informal sector. It accounts for sub-optimal provider compliance to treatment guidelines and varying quality of drugs and describes patient adherence more precisely by modeling vomiting, stopping treatment early, and selective adherence.

The uncomplicated case management decision tree allows for simulation of parasitological testing. Uncomplicated fevers may or may not be caused by malaria and, if they are non-malarial, may or may not be accompanied by parasitaemia. However, initially they appear identical to patients and health care providers and are thus included in the same entry point. Non-malarial fevers are generated by an age-dependent function in the pathogenesis model; parasitological testing helps health care providers to distinguish between non-malarial and malarial fevers and target antimalarial drugs more effectively.

If a bout of uncomplicated fever (whether malarial or not) occurs in an individual who has been treated with antimalarials within the last 14 days, it is considered a result of treatment failure which should be treated with the second-line drug. The decision tree is identical but the values of treatment-seeking, quality of care and compliance parameters are modified accordingly.

#### Severe malaria ####

In the case of severe malaria, the path through the case management decision tree is determined at the start of the episode. The tree may determine that a one-day travel-time delay occurs, in which case in-hospital decisions are not determined until the next day.

Individuals can initially either seek treatment from a lower-level source, in hospital, or not at all. There is a possibility of pre-referral treatment with referral to hospital on the same or next day; pre-referral treatment reduces parasite densities on the first day of illness. Diagnostic testing is considered for costing purposes but is assumed not to affect treatment received. Treatment schedule is found and applied in the same way as with an uncomplicated case. Entry to hospital temporarily removes the individual from the transmission cycle.

#### Medications ####

In both severe and uncomplicated cases, the relevant case management decision tree determines treatments given. For each drug and age group, the standard regimen is specified– dose size (in milligrams), and times given (in hours), and then modified, based on the decision tree outputs, by reduced drug quality, adherence and delays to taking treatment.

The treatment schedule is applied to the individual as a list of pending medications that are given over the next time points. Any previously pending medications are cancelled when new treatments are prescribed, and drugs are only costed when taken, so that if an individual progresses to severe after receiving treatments for an uncomplicated case, remained unconsumed medications are not costed.


#### Antibiotic administration ####

We assume that each individual has a probability of antibiotic administration, Pr(AB), according to whether s(he) has a NMF or a malarial fever and according to treatment-seeking location:

##### For NMF: #####

- No treatment-seeking: Pr(AB) = 0

- Health facility: Pr(AB) is a function of the average probability of being prescribed an antibiotic in the absence of a malaria test, P0; whether the individual had a negative or a positive malaria diagnostic test result, and whether the individual needs or does not need an antibiotic (this latter captures signs and symptoms that would indicate to a health worker need for antibiotics, in the absence of knowledge of malaria parasitological status). Thus:

β0= logit (P0) - β3 x Pr(NMF needs antibiotics)<br>
and <br>
Logit(Pr(AB)= β0 + β1 x I (MD-) + β2 x I(MD+) + β3 (need)<br>
<br>
where β1 and β2 are the effect of a negative and positive test, respectively, on the log odds of receiving an antibiotic and β3 is the effect of needing an antibiotic on the log odds of receiving it. I(X) is 1 when event X is true and 0 otherwise, event "need" is the event that death may occur without treatment, events "MD-" and "MD+" are the events that a malaria parasite diagnostic was used and indicated no parasites and parasites respectively.<br>
<br>
- In the informal sector:<br>
<br>
Logit(Pr(AB)= β0 + β4<br>
<br>
where exp(β4) is the effect of seeking treatment in the informal sector on the odds of getting antibiotics. In general, we do not know how the chance of getting an antibiotic depends on health facility attendance. Therefore, for the purposes of this model, we set β4 to 0, assuming that the overall probability of getting an antibiotic is not affected by seeking in the informal sector but targeting of antibiotics to those in need is improved in health facilities both by malaria diagnosis and additional diagnostic procedures (e.g. measuring respiratory rates, examining for symptoms of viral infection, etc).<br>
<br>
- Community health workers:<br>
If they are trained to administer only presumptive treatment for malaria, then we assume Pr(AB)= 0. If they are trained in malaria diagnostic testing and management of non-malarial fevers, then Pr(AB) is assumed to be as in the formal sector.<br>
<br>
<h5>For malarial fever:</h5>

β0= logit (P0) - β3 x Pr(malarial fever needs antibiotics)<br>
and<br>
Logit(Pr(AB)= β0 + β1 x I (MD-) + β2 x I(MD+) + β3 (need)<br>
<br>
<br>
<h5>Case fatality rates</h5>

The CFR of fevers that do not need antibiotics is assumed to be 0.<br>
<br>
CFR(a)¦ need= γ0(a) (1-εAB x I(AB))<br>
Where γ0(a) is the CFR(a) in absence of antibiotic, given need, and εAB is the efficacy of the antibiotic on CFR. I(AB) takes the value 1 if the individual receives an antibiotic and 0 if s(he) does not.<br>
<br>
<h3>Costing</h3>

The following costs are considered:<br>
<br>
<ul><li>drugs medicated, including vomited doses but not doses skipped due to poor adherence<br>
</li><li>parasite (RDT and microscopy) diagnostics used<br>
</li><li>outpatient visits<br>
</li><li>patient hospital days, using fixed average lengths for stays resulting in recovery and stays ending with mortality<br>
</li><li>patient direct costs, including travel, medical and non-medical supplies</li></ul>

A rough diagram of the model processes is below (states and state choices in blue). The diagram ignores the time component (most notably: change in parasite density over a severe bout is what input the parasite density has on chance of death).<br>
<br>
<img src='http://openmalaria.googlecode.com/svn/wiki/graphs/EV-PKPD-model.png' />