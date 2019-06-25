/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///                                                                       ///
///  Individual-based Plasmodium vivax transmission model.                ///
///                                                                       ///
///  Dr Michael White                                                     ///
///  Institut Pasteur                                                     ///
///  michael.white@pasteur.fr                                             ///
///                                                                       ///
///  With contributions from Dr Thomas Obadia                             /// 
///                                                                       ///
///  Please feel free to use and modify if you wish. However,             ///
///  please provide appropriate acknowledgement and get in touch          ///
///  if you have any questions. This is not necessarily the               ///
///  final, canonical version of the code - contact me to get that.       ///
///                                                                       ///
///  There is a prize of a pint for reporting any serious bugs or         ///
///  finding something new that results in >20% speed up.                 ///
///                                                                       ///
///                                                                       ///
///  Model code is split up into multiple files as follows:               ///
///                                                                       ///
///  -  Params.hpp, Params.cpp                                            ///
///     The Params structure stores input parameters and has              ///
///     associated code for reading parameters from input files.          ///
///                                                                       ///
///  -  Individual.hpp, Individual.cpp                                    ///
///     A class is created which stores all the information of a          ///
///     single individual.                                                ///
///     Details of the stochastic individual-based model for each         ///
///     person. Transitions occur with a fixed time step according to     ///
///     compting hazards                                                  ///
///                                                                       ///
///  -  Population.hpp, Population.cpp                                    ///
///     A structure called Population stores all individuals.             ///
///     This set of functions calculates the equilibrium set up of the    ///
///     population. It is only called once while the population is        ///
///     being initialised.                                                ///
///                                                                       ///
///  -  Intervention.hpp, Intervention.cpp                                ///
///     The Intervention struct stores intervention parameters.           ///
///                                                                       ///
///  -  Mosquito.cpp                                                      ///
///     Mosquitoes are simulated using a deterministic ODE solver.        ///
///                                                                       ///
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

#include "white/Simulation.hpp"
#include "util/errors.h"
#include "util/CommandLine.h"

#include <iostream>
#include <cmath>
#include <schema/scenario.h>

namespace OM { namespace white {

using std::cout;
using std::endl;

std::mt19937 random_engine;

Population PNG_pop;
Params Pv_mod_par;
SimTimes times;
Intervention PNG_intven;
const char* output_File;


void init_model( const scnXml::Scenario& scenario ) {
    ////////////////////////////////////////////
    //                                        //  
    //  1.1 Read in file names                //
    //                                        //
    ////////////////////////////////////////////
    
    const scnXml::Model& model = scenario.getModel();
    if( !model.getWhiteVivax().present() ){
        throw util::xml_scenario_error( "Require scenario/model/whiteVivax" );
    }
    const scnXml::WhiteVivax& wv = model.getWhiteVivax().get();
    
    auto coverage_File = util::CommandLine::lookupResource(wv.getCoverage().getPath());
    output_File = wv.getOutput().getPath().c_str();

    auto seed = model.getParameters().getIseed();
    cout << "Using fixed seed " << seed << endl;
    random_engine.seed(seed);
    
    
    ////////////////////////////////////////////
    //                                        //
    // 1.3. Read in model parameters          //
    //                                        //
    ////////////////////////////////////////////

    auto parameter_File = util::CommandLine::lookupResource (wv.getParametersW().getPath());
    std::string mosquito_File[N_spec_max];
    if (wv.getMosquito().size() != N_spec_max) {
        throw util::xml_scenario_error( "Wrong number of mosquito files" );
    }
    for (int i = 0; i < N_spec_max; ++i) {
        mosquito_File[i] = util::CommandLine::lookupResource (wv.getMosquito()[i].getPath()).c_str();
    }
    
    times = Pv_mod_par.read(parameter_File, mosquito_File);
    PNG_pop.N_pop = Pv_mod_par.N_pop;

    PNG_intven.read(coverage_File);

    ///////////////////////////////////////////////////////////////////////////
    //                                                                       //
    // 1.8. Initialise Population of individuals                             //
    //      Note that they begin with exponential age distribution           //
    //      and susceptible without immunity                                 //
    //                                                                       // 
    ///////////////////////////////////////////////////////////////////////////

    cout << "Initialise population of individuals for simulation at equilbirium EIR of " << 365.0*Pv_mod_par.EIR_equil << endl;
    cout << endl;

    PNG_pop.equi_pop_setup(Pv_mod_par);

    cout << "Population of size " << PNG_pop.N_pop << " initialised!" << endl;
    cout << endl;
}

void run_model() {
    /////////////////////////////////////////////////////////////////////////
    //                                                                     //
    // 1.9. Create Simulation object                                       //
    //                                                                     //
    /////////////////////////////////////////////////////////////////////////

    Simulation PNG_sim(times);


    //////////////////////////////////////////////////////
    //                                                  //
    // 1.10. Begin stochastic simulations               //
    //                                                  //
    ////////////////////////////////////////////////////// 

    cout << "Starting model simulations......." << endl;

    PNG_sim.run(Pv_mod_par, PNG_pop, PNG_intven);

    cout << "Model simulations completed....." << endl;
    cout << endl;


    //////////////////////////////////////////////////////
    //                                                  //
    // 1.11. Output to file                             //
    //                                                  //
    ////////////////////////////////////////////////////// 

    PNG_sim.write_output(output_File);
}

} }
