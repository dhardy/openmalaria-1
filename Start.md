# OpenMalaria #

Welcome to the **OpenMalaria** wiki on google-code.

**OpenMalaria** is an open source C++ program for simulating malaria epidemiology and the impacts on that epidemiology of interventions against malaria.  It is based on microsimulations of _Plasmodium falciparum_ malaria in humans, originally developed for simulating malaria vaccines and described
[here](http://www.ajtmh.org/content/75/2_suppl). These models simulate the dynamics of malaria parasitaemia in the course of an infection, of transmission, of immunity, and of the processes leading to illness and death. The system is set up to simulate malaria in village, or district size human populations. The original models have been extended to include simulation of the dynamics of malaria in the mosquito vector, and by including alternative model structures for many of the components of the human models. The models allow for variations among humans in their exposure to mosquitoes, and in their responses to the parasite.

**OpenMalaria** can be used to simulate interventions against the mosquito vector and interventions that act against malaria in humans (for example, chemotherapy, or vaccines).

**OpenMalaria** is a general platform for comparing, fitting, and evaluating different malaria models, to ensure that the models reproduce patterns of infection and disease observed in the field in malaria-endemic areas.

For public discussion, please use the [OpenMalaria](http://groups.google.com/group/openmalaria) google group.   A glossary of useful terms related to OpenMalaria can be found [here](http://code.google.com/p/openmalaria/wiki/Terminology)

# Software #

## Code ##

The core program is written in C++ with a [GNU GPL 2 licence](http://www.gnu.org/licenses/old-licenses/gpl-2.0.html).

The status of **OpenMalaria** software development is summarized on [ohlo.net](https://www.ohloh.net/p/openmalaria).  The source code is stored as part of the google code repository [repository](http://code.google.com/p/openmalaria/source/browse/). Instructions for compiling the code from source can be found on this wiki [site](BuildSystem.md).

## Models ##

Each simulation requires implementation of various submodels, or model components, from transmission to within-host parasite dynamics to drug action. These model components are detailed in our publications, and on this website [here](ModelsOverview.md).

## Running **OpenMalaria** ##

There are several methods for running **OpenMalaria** simulations:

  * as a [stand-alone program](SetupOverview.md), capable of simulating a few hand-crafted scenarios and producing results analyzable with spread-sheets or simple scripts.

Users with login credentials can also run **OpenMalaria** via:

  * the [OpenMalaria Basic User Interface](https://ci.vecnet.org/ts_om/) (on the [Vector-Borne Disease Network (VecNet)](https://www.vecnet.org/) website).

  * the [OpenMalaria Portal](https://vecnet1.jcu.edu.au/)

These are GUI-driven experiment designer and job submission systems, ( capable of deploying simulation experiments on different computer resources (at varying stages of development).

  * via the [BOINC](http://boinc.berkeley.edu/) volunteer computing platform http://www.malariacontrol.net/ and a semi-automated experiment design and analysis platform, capable of handling entire experiments of 10000-100000 scenarios

# Applications #

The main applications of **OpenMalaria** are:

## Simulation of current malariological situations ##

Simulations of _status quo_ malaria situations can be used to estimate parameters that have not been measured from field data.  For instance, models can be used to predict mortality rates from entomological data. Such simulations are also useful for validating models.

Guidance on how to apply **OpenMalaria** to simulate malaria at a field site can be found [here](http://code.google.com/p/openmalaria/wiki/Instructions).

## Prediction of epidemiological impacts of interventions ##

Prediction of epidemiological impacts of interventions is used to

  * analyze novel interventions to inform the development of target product profiles

  * analyze the effects of different deployment strategies for existing interventions

  * analysis the potential effects for new interventions or planned malaria control programs.

## Economic (cost-effectiveness) analyses ##

The outputs from predictive simulations can be linked to cost data from endemic countries to analyze the economics of malaria interventions.

## Uncertainty analysis ##

The value of a point estimate is greatly enhanced when the variability associated with that point estimate is also available. The project analyzes different components of uncertainty associated with sets of model predictions, considering stochastic uncertainty, parameter uncertainty, and model uncertainty.

# Team #

The **OpenMalaria** development team includes staff of the [Swiss Tropical and Public Health Institute (Swiss TPH)](http://www.swisstph.ch/en/about-us/departments/epidemiology-and-public-health-eph/health-systems-research-and-dynamical-modelling/dynamical-modelling.html), the [Network Dynamics and Simulation Science Laboratory at Virginia Bioinformatics Institute](http://ndssl.vbi.vt.edu/index.php)
and the [Liverpool School of Tropical Medicine](http://www.lstmliverpool.ac.uk).  Financial support is from the [Bill & Melinda Gates Foundation](http://www.gatesfoundation.org/Pages/home.aspx)

# References #

The project publishes descriptions of the models and their use in the scientific literature (mostly open access).  Selected publications can be found [here](References.md).

# Offline Documentation #

In case you wish to have access to the documentation on this wiki without an internet connection, you can download and extract the following:

  * [OM\_wiki\_dump\_2011\_02\_17.zip](http://openmalaria.googlecode.com/svn/download/wiki-dumps/OM_wiki_dump_2011_02_17.zip)