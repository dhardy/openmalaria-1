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

#include "Simulation.hpp"

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;

std::mt19937 random_engine;


////////////////////////////////////////////
////////////////////////////////////////////
//        //                              //
//   ##   //  #     #  ####  #### #   ##  //
//  ###   //  ##   ## ##  ##  ##  ##  ##  //
//   ##   //  ####### ######  ##  ### ##  //
//   ##   //  ## # ## ##  ##  ##  ## ###  //
//  ####  //  ##   ## ##  ## #### ##  ##  //
//        //                              //
////////////////////////////////////////////
////////////////////////////////////////////

int main(int argc, char** argv)
{
    clock_t clock_time;
    clock_time = clock();


    ////////////////////////////////////////////
    //                                        //  
    //  1.1 Read in file names                //
    //                                        //
    ////////////////////////////////////////////

    // do we have the correct command line?
    if (argc < 4 + N_spec_max || argc > 5 + N_spec_max)
    {
        cout << "Incorrect command line. Expected:" << endl;
        cout << argv[0] << " PARAMS MOSQ MOSQ MOSQ COVERAGE OUTPUT SEED" << endl;
        cout << "where SEED is optional; all other args are file names." << endl;
        return 0;
    }

    const char* parameter_File = argv[1];

    const char* mosquito_File[N_spec_max];
    for (int g = 0; g < N_spec_max; g++)
    {
        mosquito_File[g] = argv[2 + g];
    }

    const char* coverage_File = argv[2 + N_spec_max];
    const char* output_File = argv[3 + N_spec_max];
    
    if (argc == 5 + N_spec_max) {
        std::string arg = argv[4 + N_spec_max];
        long seed = stol(arg);
        cout << "Using fixed seed " << seed << endl;
        random_engine.seed(seed);
    } else {
        std::random_device r;
        long seed = r();
        cout << "Using random seed " << seed << endl;
        random_engine.seed(seed);
    }


    try {
        ////////////////////////////////////////////
        //                                        //
        // 1.2. Initialise objects                //
        //                                        //
        ////////////////////////////////////////////

        Population PNG_pop;
        Params Pv_mod_par;


        ////////////////////////////////////////////
        //                                        //
        // 1.3. Read in model parameters          //
        //                                        //
        ////////////////////////////////////////////

        SimTimes times = Pv_mod_par.read(parameter_File, mosquito_File);
        PNG_pop.N_pop = Pv_mod_par.N_pop;

        Intervention PNG_intven(coverage_File);

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

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << endl;
    } catch (const char *msg) {
        std::cerr << "Exception: " << msg << endl;
        return 1;
    }

    cout << "Time taken: " << ((double)clock() - clock_time) / ((double)CLOCKS_PER_SEC) << " seconds" << endl;

    return 0;
}
