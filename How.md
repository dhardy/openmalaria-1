# Introduction #

Individuals interested in simulating malaria in a particular field site may find this guide to the process setting up OpenMalaria, creating scenario files, and analyzing results a useful starting point.

# Details #

A glossary of useful terms related to the OpenMalaria wiki can be found: [here](http://code.google.com/p/openmalaria/wiki/Terminology)

**1.	Download OpenMalaria**

The model is written in C++ with Fortran roots. In order to run the model you need to download the simulator that contains all the necessary code. As of April 2011 the current version of the OpenMalaria schema is version 26.  Check back periodically to ensure you have the most up to date version. If you are in doubt contact the OpenMalaria administrator. To run the OpenMalaria simulator you need to create an XML file that contains all the data on malaria in your study area (described in later steps).

A description of how to download OpenMalaria for a PC can be found
[here](http://code.google.com/p/openmalaria/wiki/openmalariaTools). Note: OpenMalaria is not available yet as an executable for Mac users –contact the administrator if you would like to run OpenMalaria on a Mac.

**2.	Decide on what you will be simulating**

Before doing anything else, collaborate with the research team and/or control program managers in the study area (if possible) to ensure field data is available for what you want to simulate. For example, if the field study is a cohort where people are tested for malaria using RDTs, you will be simulating patent infections. If the field study is an entomological study where mosquitoes are trapped, dissected and analyzed for sporozoites, you will be simulating number of infectious mosquitoes. Try to keep the number of outputs limited to 3 or 4 max for ease of analysis. EIR is considered the main measure of transmission so this should ideally be one of the outputs.

It is important for this step to happen early on because you don’t want to get to the end of running simulations and find out your analysis is not relevant in the field. It will help immensely if everyone is on the same page at the beginning with what you are simulating and why.

The full list of outputs that OpenMalaria is able to simulate can be found
[here](http://code.google.com/p/openmalaria/wiki/XmlMonitoring)

**3.	Choose model(s)**

What you want to simulate (Step #2) will determine which model(s) to use from the following options:
-Transmission (vector or non-vector)
-Within-host (drug action PKPD and/or asexual infection)
-Clinical/sickness (morbidity/pathogenesis or neonatal mortality)
-Interventions (human stage, mosquito stage, and/or “special” interventions)

Full descriptions of each model and how they work can be found [here](http://code.google.com/p/openmalaria/wiki/ModelsOverview)

**4.	Design your scenario**

Depending on what model(s) you choose, the next step is to make a list of the inputs necessary for each of those models.

In general, the types of information include:
-Demography
-Monitoring system
-Interventions
-Health system
-Entomological data
-Drug description
-Model options
-Human availability
-Parameters

A full list of these parameters as well as their definitions can be found [here](http://code.google.com/p/openmalaria/wiki/ScenarioDesign)

A description of the XML syntax required for the scenario files can be found
[here](http://code.google.com/p/openmalaria/wiki/XmlScenario)


**5.	Gather data**

The next step is to gather the necessary data to include in the scenario file to describe the situation in your study area as closely as possible to reality. There are more or less 90 separate pieces of data that need to go into the model. As many inputs as possible should come from MTC field studies in the particular study area, but realistically the overall proportion of inputs that come from site-specific data will be rather low (about 1/3rd). A sizeable number of inputs will come from previously published literature at the district, national or regional level through a literature review. There will be a number of inputs that will remain the same as the initial model parameterization ((Smith, Maire et al. 2006), (Maire, Smith et al. 2006), (Chitnis, Smith et al. 2008), (Ross, Penny et al. 2008),(Penny, Maire et al. 2008),(Chitnis, Schapira et al. 2010)) depending on which model(s) you choose, particularly data on the vector life cycle and drug action. It is helpful to document the source of the data you choose for each parameter and if there are multiple sources why you chose the value that will go into the model.

**6.	Create XML scenario file**

The easiest way to do this is to use an example file and replace the values with those you compiled in Step #5. Keep in mind that while you can replace the individual values, the overall structure of the file (i.e. the order the parameters are listed) cannot change or the simulator will not work. Examples of the XML syntax that can be adapted and edited to fit your particular scenario file can be found
[here](http://code.google.com/p/openmalaria/source/browse/#svn%2Ftrunk%2Ftest)

These files are updated on a continual basis so be sure you download a file with a recent date. For troubleshooting contact the administrator.

**7.	Test scenario with one seed value and a small number of individuals**

The run time of the model is dependent on the population size, so to make sure your scenario file works before sending it to BOINC (Step #10) test run your scenario with one seed value and a population of 100. If there is a problem with the syntax an error message will alert you to the problem. Once the file successfully completes, you can increase your population size and number of seed values.

**8.	Analyze outputs (statistical, visual, etc)**

The simulation results of the outputs you have chosen from Step #2 are produced in the form of two separate .txt files (ctsout.txt and output.txt) that can be imported into your program of choice for analysis (i.e. Excel, R, STATA, etc).  A guide to interpreting the output files can be found [here](http://code.google.com/p/openmalaria/wiki/OutputFiles)

Creating graphs that overlay observed field data onto the simulations will give you an idea of how much stochasticity exists in the model. Because there are many runs, consider showing the minimum, maximum and median simulation values as well as the inter-quartile range and compare this to the observed field data with its 95% CIs. There is no consensus in the modeling community on how to quantify how well the model “fits” the data so in practice this will evolve over time. For example, there is currently a debate over whether you can average the simulation runs for analysis. Picking a method and making sure to acknowledge its limitations is the best we can do at this point. Visually comparing model outputs to observed field data is a first step, then determining what proportion of model runs fall within the 95% CIs of the field data, and finally the sum of least squares if relevant.

**9.	Conduct experiments – evaluate uncertainty with a sensitivity analysis and examining a mix of intervention options**

By running simulations with multiple seed values you are addressing stochasticity of the model. However, once you have your fitted model, conducting a sensitivity analysis of parameters can address parameter uncertainty. Choose parameters that you believe are important to the model but have input values you are not completely certain about. For example, measures of EIR can vary in the same location when different methods for measuring sporozoites and biting rate are applied (Kelly-Hope and McKenzie 2009), so this parameter is a good candidate for a sensitivity analysis.  Choose a range of values for the parameter that makes sense given your study area context – usually a minimum, maximum and median value.  Using the EIR example, if your estimated EIR for the study area is 4, for your range you may want to choose a lower value (0.4), a slightly higher value (10) and an extremely high value (40). Using the scenario file of the “best fit” model from your simulations above, only change the value you are conducting the sensitivity analysis with and create as many new scenario files as you have values (i.e. one file for the minimum EIR, one for the maximum EIR, etc).

To explore the effects of different interventions, take your “best fit” model and update the scenario file to simulate different coverage levels, timing, and target age groups. Set the scenario to run for an extended period time over the whole population of the study area to see the impact.

**10.	Send to BOINC**

Once you are satisfied that the syntax in your scenario file is working and you are ready to run the simulations with all seed values and parameterizations in the chosen population, send the scenario files in .xml format to the Dynamical Modelling Group at Swiss TPH. They will ensure the scenario file is in order and then send the scenarios to the volunteer computer network. Because you have gone through the setup and analysis exercises on a smaller population this part should run more smoothly.

**References
Chitnis, N., A. Schapira, et al. (2010). "Comparing the effectiveness of malaria vector-control interventions through a mathematical model." The American journal of tropical medicine and hygiene 83(2): 230-240.
Chitnis, N., T. Smith, et al. (2008). "A mathematical model for the dynamics of malaria in mosquitoes feeding on a heterogeneous host population." Journal of Biological Dynamics 2(3): 259 - 285.
Kelly-Hope, L. A. and F. E. McKenzie (2009). "The multiplicity of malaria transmission: a review of entomological inoculation rate measurements and methods across sub-Saharan Africa." Malaria journal 8: 19.
Maire, N., T. Smith, et al. (2006). "A model for natural immunity to asexual blood stages of Plasmodium falciparum malaria in endemic areas." Am J Trop Med Hyg 75(2 Suppl): 19-31.
Penny, M. A., N. Maire, et al. (2008). "What should vaccine developers ask? Simulation of the effectiveness of malaria vaccines." PLoS One 3(9): e3193.
Ross, A., M. Penny, et al. (2008). "Modelling the epidemiological impact of intermittent preventive treatment against malaria in infants." PLoS One 3(7): e2661.
Smith, T., N. Maire, et al. (2006). "Relationship between the entomologic inoculation rate and the force of infection for Plasmodium falciparum malaria." Am J Trop Med Hyg 75(2 Suppl): 11-18.**

