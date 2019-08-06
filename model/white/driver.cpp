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

unique_ptr<Params> Pv_mod_par;
SimTimes times;
unique_ptr<Intervention> PNG_intven;
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
    
    Pv_mod_par = unique_ptr<Params>(new Params{});
    times = Pv_mod_par->read(parameter_File, mosquito_File);

    PNG_intven = unique_ptr<Intervention>(new Intervention{ coverage_File });
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

    PNG_sim.run(*Pv_mod_par, *PNG_intven);


    //////////////////////////////////////////////////////
    //                                                  //
    // 1.11. Output to file                             //
    //                                                  //
    ////////////////////////////////////////////////////// 

    PNG_sim.write_output(output_File);
}

} }
