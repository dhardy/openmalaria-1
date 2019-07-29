/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///                                                                       ///
///  Individual-based Plasmodium vivax transmission model.                ///
///                                                                       ///
///  Dr Michael White                                                     ///
///  Institut Pasteur                                                     ///
///  michael.white@pasteur.fr                                             ///
///                                                                       ///
///  This file and accompanying .cpp contain:                             ///
///                                                                       ///
///     A structure called Population stores all individuals.             ///
///                                                                       ///
///  3. EQUILIBRIUM SETUP                                                 ///
///     This set of functions calculates the equilibrium set up of the    ///
///     population. It is only called once while the population is        ///
///     being initialised.                                                ///
///                                                                       ///
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// Include guard
#ifndef PVIVAX_MODEL_POPULATION
#define PVIVAX_MODEL_POPULATION

#include "white/driver.hpp"
#include "white/Individual.hpp"

#define N_PREV 11

using Eigen::Array;
using Eigen::Dynamic;

namespace OM { namespace white {

/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
// 0.3. Define a structure for a population of individuals                             //
//                                                                                     //
//      Note that this object only stores information at a fixed point in time         //
//      and as such is memoryless                                                      //
//                                                                                     //
/////////////////////////////////////////////////////////////////////////////////////////

class Population
{
public:
    /// Constructor
    Population(size_t N_pop);
    
    //////////////////////////////////////////////////////////////////////////
    //  Class member functions
    //////////////////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////
    // Initialise a population of individuals at equilibrium
    void equi_pop_setup(Params& theta);

    ////////////////////////////////////////////////////
    // Update human individuals
    void human_step(Params& theta);
    
    ////////////////////////////////////////////////////
    // Summarise population outputs
    void summary();


private:
    void gauher(Params& theta);

public:
    //////////////////////////////////////////////////////////////////////////
    //  Data
    //////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////
    // 0.3.1. Human population

    size_t N_pop;                      // Population size - we have balanced demography at the moment so this will be effectively constant 

    vector<Individual> people;      // A vector of individuals

    Array<double, Dynamic, N_spec> pi_n;    // Proportion of bites on humans that person n receives
    Array<double, Dynamic, N_spec> lam_n;   // Biting rate of a single mosquito on person n


    ///////////////////////////////////////////////
    // 0.3.2. Mosquito population
    //
    //        Depends dynamically on vector control

    Array<double, N_spec, N_M_comp> yM;      // mosquito state

    Array<double, N_spec, 1> SUM_pi_w;
    Array<double, N_spec, 1> SUM_pi_z;


    Array<double, N_spec, 1> delta_1_VC;        // duration spent foraging
    Array<double, N_spec, 1> delta_VC;          // duration of gonotrophic cycle = duration between blood meals

    Array<double, N_spec, 1> Z_VC;              // average probability of mosquito repeating during a single attempt
    Array<double, N_spec, 1> W_VC;              // average probability of successfully feeding on a human during a single attempt
    Array<double, N_spec, 1> Q_VC;              // human blood index
    Array<double, N_spec, 1> p_1_VC;            // probability of surviving foraging
    Array<double, N_spec, 1> mu_M_VC;           // mosquito death rate
    Array<double, N_spec, 1> aa_VC;             // mosquito biting rate
    Array<double, N_spec, 1> exp_muM_tauM_VC;   // probability of surviving sporogony
    Array<double, N_spec, 1> beta_VC;           // egg oviposition rate


    ////////////////////////////////////////////////////////////////
    // 0.3.3. Objects for storing summary output of the population

    Array<int, N_H_comp, 1> yH;   // Human compartmental states

    Array<int, N_PREV, 1> prev_all;   // Contains {N_pop, PvPR_PCR, PvPR_LM, Pv_clin, PvHR, PvHR_batches, new_PCR, new_LM, new_D, new_ACT, new_PQ} 
    Array<int, N_PREV, 1> prev_U5;    // Contains {N_pop, PvPR_PCR, PvPR_LM, Pv_clin, PvHR, PvHR_batches, new_PCR, new_LM, new_D, new_ACT, new_PQ} 
    Array<int, N_PREV, 1> prev_U10;   // Contains {N_pop, PvPR_PCR, PvPR_LM, Pv_clin, PvHR, PvHR_batches, new_PCR, new_LM, new_D, new_ACT, new_PQ} 

    double EIR_t;       // EIR
    int LLIN_cov_t;     // LLIN coverage
    int IRS_cov_t;      // IRS coverage
    int ACT_treat_t;    // Coverage with front-line treatment (ACT) 
    int PQ_treat_t;     // Coverage with front-line treatment (primaquine or tafenoquine)
    int pregnant_t;     // Coverage with front-line treatment (primaquine or tafenoquine)

    int PQ_overtreat_t;
    int PQ_overtreat_9m_t;

    double A_par_mean_t;     // Average anti-parasite immunity 
    double A_clin_mean_t;    // Average anti-clinical immunity


    ///////////////////////////////////////////////////////////////
    // 0.3.4. Equilibrium settings of population
    //        These are only needed for initialising the simulation

    ////////////////////////////////////////
    // Age and heterogeneity demographics 

    Array<double, N_age + 1, 1> age_bounds;

    Array<double, N_age, 1> age_demog;
    Array<double, N_age, 1> age_bite;
    Array<double, N_age, 1> age_mids;

    Array<double, N_het + 1, 1> x_het_bounds;

    Array<double, N_het, 1> x_het;
    Array<double, N_het, 1> w_het;

    Array<double, N_age, N_het> x_age_het;
    Array<double, N_age, N_het> w_age_het;


    ////////////////////////////////////////
    // Ageing rates 

    Array<double, N_age, 1> r_age;

    double P_age_bite;     // Proportional change for age-dependent biting


    ////////////////////////////////////////
    // Indicator for age group of 20 year old woman

    int index_age_20;
};

} }

#endif
