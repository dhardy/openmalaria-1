

# Vector parameters #

The vector model allows simulation of any number of different vectors, each of which can have different interactions with hosts, where hosts may be humans or one of a few specified non-human populations (e.g. sprayed & unsprayed animals).

The vector element consists of a list of one `anopheles` sub-element for each vector taxon, followed by a list of zero or more `nonHumanHosts` sub-elements. Each has a name, which serves as an identifier for the Anopheles species or non-human-host category (for example, to match per-species vector intervention parameters specified elsewhere with the correct species).

Located within the `anopheles` sub-element, the initial conditions to start the simulation (actual values do not impact the results) include the `propInfected` and `propInfectious` per-species mosquito parameters.

| name | type | unit | description |
|:-----|:-----|:-----|:------------|
| propInfected | int | proportion | Proportion of vectors infected (oocysts identified in midgut or would develop oocysts if kept alive - usually called the delayed oocyst rate in entomological literature) in the month the simulation starts|
| propInfectious | int | proportion | Proportion of vectors infectious (sporozoites identified in salivary glands) in the month the simulation starts |

## Mosquito parameters ##

Each anopheles elements has the following sub-elements: either `EIR` or `monthlyEIR`, `mosq`, and a list of zero or more `nonHumanHosts`.

### Entomological Inoculation Rate (EIR) ###

The level and seasonality of transmission are input via a description of the approximate seasonal pattern of the EIR.  EIR can be specified either via 5 Fourier coefficients and a rotation factor (`EIR` element) or via 12 monthly values plus an annual level (`monthlyEIR` element). Exactly one of these elements must appear. Details are given [here](EIRSeasonDynamic.md)

### Mosquito lifecycle and human-interaction parameters ###

Other per-species mosquito parameters (except non-human-host interaction) are described in the `mosq` element. This has the following attributes:

| name | type | unit | description |
|:-----|:-----|:-----|:------------|
| mosqRestDuration | int | days | Duration of the resting period of the vector |
| extrinsicIncubationPeriod | int | days | Extrinsic incubation period (time required for sporozoite development) |
| minInfectedThreshold | int | number | Number of infected mosquitos below which the values for proportion of infected mosquitoes and proportion of infectious mosquitoes are set to zero. Used in order to simulate interruption of transmission within the deterministic model. |
| mosqLaidEggsSameDayProportion | double | proportion | Proportion of mosquitoes host seeking on same day as ovipositing |
| mosqSeekingDuration | double | days | Maximum proportion of day spent host-seeking by the vector |
| mosqSurvivalFeedingCycleProbability | double | probability | Probability that the mosquito survives the feeding cycle|
| mosqProbBiting | double | probability | Probability that the mosquito succesfully bites chosen host |
| mosqProbFindRestSite | double | probability | Probability that the mosquito escapes host and finds a resting place after biting |
| mosqProbResting | double | probability | Probability of mosquito successfully resting after finding a resting site |
| mosqProbOvipositing | double | probability | Probability of a mosquito successfully laying eggs given that it has rested |
| mosqHumanBloodIndex | double | proportion | Human blood index: the proportion of resting mosquitoes which fed on human blood during the last feed  |

## Non-human host parameters ##

Non-human parameters are contianed within two sets of elements (all called `nonHumanHosts`): for vector transmission as a whole, and per species.

The first is a list of sub-elements of the `vector` element (under `entomology`), each containing just the category `name` and the `number` of hosts of that type.

Mosquito-host interaction parameters for these categories are then described within the `anopheles` elements, with the attributes:

| name | type | unit | description |
|:-----|:-----|:-----|:------------|
| name | string |  | Identifier for this category of non-human hosts |
| mosqRelativeEntoAvailability | double | proportion | Relative availability of nonhuman hosts of type i (to other nonhuman hosts) |
| mosqProbBiting | double | probability | Probability of mosquito successfully biting host |
| mosqProbFindRestSite | double | probability | Probability that the mosquito escapes host and finds a resting place after biting |
| mosqProbResting | double | probability | Probability of mosquito successfully resting after finding a resting site |