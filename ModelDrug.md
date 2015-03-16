# Drug models #



&lt;placeholder&gt;




# Weight model #

Simulation of PK and PD dynamics requires knowledge of an individual's body mass. For that, we use a simple model: a function mapping age to mass as in a piece-wise linear manner ([code](http://code.google.com/p/openmalaria/source/browse/trunk/model/AgeGroupData.cpp)). The model has no hetereogeneity (variation between hosts or between samples).

Note that this model is used only for PK & PD modelling; drug doses are determined by the case management model based on age, not weight.