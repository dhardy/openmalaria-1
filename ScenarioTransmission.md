# Transmission models #

While the malaria parasite cycle within humans is modelled by using an agent-based [model](ModelWithinHost.md), the entomological part of malaria transmission must be modelled in a less detailed manner. We currently have two [models available](ModelTransmission.md).

## Common entomological data ##

Entomological data are described by the `entomology` element, containing either a `vector` or a `nonVector` sub-element. For example:
```
<entomology mode="4" name="a name">
  <nonVector eipDuration="10">
    <EIRDaily origin="monthly">0.0738</EIRDaily>
    <EIRDaily origin="monthly">0.0738</EIRDaily>
    <EIRDaily origin="monthly">0.0738</EIRDaily>
    <EIRDaily origin="monthly">0.0738</EIRDaily>
    <EIRDaily origin="monthly">0.0738</EIRDaily>
    <EIRDaily origin="monthly">0.0738</EIRDaily>
    ...
  </nonVector>
</entomology>
```

Attributes of the `entomology` element:
| name | type | description |
|:-----|:-----|:------------|
| name | text | A user-friendly name for the transmission settings |
| mode | integer | Can be 2 (static — EIR humans are exposed to is fixed to the annual cycle given) or 4 (dynamic — transmission is affected by human infectiousness). From schema 30 onwards this element will be of type 'text' with "forced" for static transmission (previously "2") and "dynamic" for dynamic transmission (previously "4")  |
| annualEIR | number, optional | If provided, the EIR description provided is scaled such that the total number of infectious bites per adult is this number. (Added in schema version 22.) |

Note that prior to schema 24, the vector model used EIR in units of infectious bites per person per time-period, averaged across the population, while the non-vector model used units of infectious bites per adult per time-period. From schema version 24 both will use units of infectious bites per adult per day/timestep/month/year. (The difference being that children receive fewer bites than adults.)

### Vector parameters: `vector` ###

[Vector-model parameter description.](XmlEntoVector.md)

### Non-vector parameters: `nonVector` ###

The `nonVector` element primarily consists of a list of daily EIR (_Entomological Inoculation Rate_) parameters (`EIRDaily` elements) specifying the annual EIR (thus 365 values are expected). Assuming the first value is the EIR for January 1st, time 0 corresponds to the beginning of the year (since this is the only input affecting seasonality it can be rotated as desired).

Values in this list are averaged per timestep to calculate the EIR per timestep of the year for the pre-intervention equilibrium state. Where data for more than one year are provided, the data is assumed to wrap into the next year and all values for the same timestep of the year are averaged.

When mode=2 this same data is used as the transmission during the intervention period; when mode=4 it is scaled by the human infectiousness relative to infectiousness near the end of the warmup phase.

`nonVector` also has an `eipDuration` attribute: the _extrinsic incubation period_ (sporozoite development time, in days), which determines the delay before changes in human infectiousness affect the EIR (in dynamic mode only).