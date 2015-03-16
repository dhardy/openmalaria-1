# Overview #

## Simulation time and phases ##

The OpenMalaria simulator uses discrete time step updates of one and/or five days, resulting in 365 or 73 steps per year.

Prior to simulating the outcomes of an intervention or steady-state scenario in what we term the **intervention phase**, the code runs through a **warm-up phase**.

[More on time...](ModelTimeUpdates.md)

## Model diagrams ##

The diagram below gives a high-level view of the processes abstracted into a model. All model-specific behaviour is abstracted into one block, or model component. More detailed graphs restricted to
the components of interest are available on some of the sub-model pages.

Color key: green arrows increase a quantity, red arrows decrease a quantity. Shape key: oval
nodes are processes affecting something, boxes surround models, and hexagonal nodes are
quantities without a process in view of the diagram.

### Population-oriented views ###

Some perspectives for looking at model processes are those of [human population](ModelDemography.md) processes or of [mosquito population](ModelTransmission#Vector_transmission.md) processes.

### Infection cycle ###

A common perspective is that of the malarial infection cycle. The following graph shows the cycle assumed by the model. Blood-stage parasite densities are
one of the most important aspects here. The effects of interventions are not shown.

![http://openmalaria.googlecode.com/svn/wiki/graphs/infection-cycle.png](http://openmalaria.googlecode.com/svn/wiki/graphs/infection-cycle.png)