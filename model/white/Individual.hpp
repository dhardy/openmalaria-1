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
///  4. INDIVIDUAL-BASED MODEL                                            ///
///     Details of the stochastic individual-based model for each         ///
///     person. Transitions occur with a fixed time step according to     ///
///     compting hazards                                                  ///
///                                                                       ///
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// Include guard
#ifndef PVIVAX_MODEL_INDIVIDUAL
#define PVIVAX_MODEL_INDIVIDUAL

#include "white/Params.hpp"

using Eigen::Array;

namespace OM { namespace white {

enum class Gender { Male, Female };

/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
// 0.2. Define a  for humans                                                      //
//                                                                                     //
/////////////////////////////////////////////////////////////////////////////////////////

class Individual
{
public:
    //////////////////////////////////////////////////////////////////////////
    //  Class constructors and destructors
    //////////////////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////
    // 0.2.1. Class constructor initialises all Individual state
    Individual(Params& theta, double a, double zeta);
    
    
    ////////////////////////////////////////////////////
    // Copy and move constructors

    // Delete unwanted copy constructors
    Individual(Individual&) =delete;
    void operator= (Individual&) =delete;
    // Allow default move constructors
    Individual(Individual&&) = default;
    Individual& operator= (Individual&&) = default;


    //////////////////////////////////////////////////////////////////////////
    //  Class member functions
    //////////////////////////////////////////////////////////////////////////
    
    ////////////////////////////////////////////////////
    // 0.2.2. Function declarations within the human class

    void state_mover(Params& theta, double lam_bite);
    void ager(Params& theta);
    void intervention_updater(Params& theta);

    ////////////////////////////////////////////////////
    // Query functions. These are useful to isolate internal state.

    inline bool is_G6PD_deficient() const {
        return G6PD_deficient;
    }

    inline bool has_low_cyp2d6_action() const {
        return CYP2D6_low;
    }

    static const uint32_t I_DETECT_PCR = 1 << 0;    // detectable by PCR
    static const uint32_t I_DETECT_LM  = 1 << 1;    // detectable by Microscopy
    static const uint32_t I_DETECT_D   = 1 << 2;    // detectable by Disease
    
    /// Query whether individual is newly infected.
    /// 
    /// @param detection One or more of the I_DETECT_* bit-flags
    inline bool infected_new(uint32_t detection) const {
        return I_new & detection;
    }

private:
public: // TODO: access control
    //////////////////////////////////////////////////////////////////////////
    //  Data
    //////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////
    // 0.2.3. Person-specific age and exposure classifiers

    double age;                      // Person's age
    double zeta_het;                 // Heterogeneity in exposure to mosquitoes

    Gender gender;
    bool G6PD_deficient;             // true if any deficiency (hemi-, homo- or heterozygous)
    bool CYP2D6_low;                 // true if individual has low CYP2D6 action (slow metabolism)


    ////////////////////////////////////////////////////
    //  0.2.4. Child-bearing age.
    //         Indicator for people of 18-22 years of age. New-born
    //         children acquire a proportion of the immunity of a
    //         women aged 20 years.

    bool preg_age;
    bool pregnant;
    double preg_timer;


    vector<double> lam_bite_track;   // Tracking lagged force of infection due to moquito bites
    vector<double> lam_rel_track;    // Tracking lagged force of infection due to relapses


    ////////////////////////////////////////////////////
    // 0.2.5. Indicators for compartments

    bool S;
    bool I_PCR;
    bool I_LM;
    bool I_D;
    bool T;
    bool P;

    /////////////////////////////////////////////////////////////////
    //  0.2.7. Number of batches of hypnozoites. Must be an integer. 

    int Hyp;


    ////////////////////////////////////////////////////
    //  0.2.6. Indicators for new events

private:
    uint32_t I_new;  // indicators for new infections (see I_DIAG_* flags)
    
public: // TODO: access control
    bool ACT_treat;    // 
    bool PQ_treat;     // 

    bool PQ_effective;
    bool PQ_overtreat;
    bool PQ_overtreat_9m;


    ////////////////////////////////////////////////////
    //  0.2.8. Person-specific levels of immunity and indicators 
    //         for whether immunity is suppressed

    double A_par;
    double A_clin;

    double A_par_mat;
    double A_clin_mat;

    bool A_par_boost;
    bool A_clin_boost;

    double A_par_timer;
    double A_clin_timer;

    SimTime PQ_proph;         // protected when PQ_proph >= sim::ts0()


    //////////////////////////////////////////////////////////
    // 0.2.9. Person-specific intervention access parameter
    //
    // Note that while there are 9 interventions in total, 2 of these are
    // are related to first-line treatment. Therefore there are N_int = 6
    // 'pulsed' interventions, i.e. those distributed by campaigns where 
    // access will be an issue. 
    //
    // Of course there will be differential access to first-line treatment,
    // but that's a story for another day.

    double zz_int[N_int];


    ///////////////////////////////
    // LLINs

    bool LLIN;                 // Does the person have an LLIN?
    double LLIN_age;           // How old is the LLIN

    Array<double, N_spec, 1> r_LLIN;     // repellency
    Array<double, N_spec, 1> d_LLIN;     // killing effect
    Array<double, N_spec, 1> s_LLIN;     // survival


    ///////////////////////////////
    // IRS

    bool IRS;                 // Is the person protected by IRS
    double IRS_age;           // How long ago was their house sprayed?

    Array<double, N_spec, 1> r_IRS;     // repellency
    Array<double, N_spec, 1> d_IRS;     // killing effect
    Array<double, N_spec, 1> s_IRS;     // survival


    ///////////////////////////////
    // SSAT

    double T_last_BS;         // Tracking of time since last PCR-detectable blood-stage infection

    ///////////////////////////////////////////////////
    // Individual-level effect of vector control

    Array<double, N_spec, 1> z_VC;      // probability of mosquito being repelled from this individual during a single feeding attempt
    Array<double, N_spec, 1> y_VC;      // probability of mosquito feeding on this individual during a single attempt
    Array<double, N_spec, 1> w_VC;      // probability of mosquito feeding and surviving on this individual during a single feeding attempt
};

} }

#endif
