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
#ifndef PVIVAX_MODEL_INTERVENTION
#define PVIVAX_MODEL_INTERVENTION

#include "white/Population.hpp"


namespace OM { namespace white {

/////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     //
// 0.4. Define a structure for details of interventions                                //
//                                                                                     //
/////////////////////////////////////////////////////////////////////////////////////////

class Intervention
{
public:
    //////////////////////////////////////////////////////////////////////////
    //  Class constructors and destructors
    //////////////////////////////////////////////////////////////////////////
    
    /// read intervention data from input files
    Intervention(const std::string& coverage_File);
    
    
    ////////////////////////////////////////////////////
    // Copy and move constructors

    // Delete unwanted copy constructors
    Intervention(Intervention&) =delete;
    void operator= (Intervention&) =delete;
    // Allow default move constructors
    Intervention(Intervention&&) = default;
    Intervention& operator= (Intervention&&) = default;


    //////////////////////////////////////////////////////////////////////////
    //  Class member functions
    //////////////////////////////////////////////////////////////////////////
    
    /////////////////////////////////////
    // Distribute interventions
    void distribute(Params& theta, Population& POP);
    

private:
    //////////////////////////////////////////////////////////////////////////
    //  Data
    //////////////////////////////////////////////////////////////////////////


    ////////////////////////////////
    // LLINs

    vector<SimDate> LLIN_date;
    vector<double> LLIN_cover;


    ////////////////////////////////
    // IRS

    vector<SimDate> IRS_date;
    vector<double> IRS_cover;

    ////////////////////////////////
    // First-line treatment - blood-stage drugs

    vector<SimDate> BS_treat_date_on;
    vector<SimDate> BS_treat_date_off;
    vector<double> BS_treat_BScover;
    vector<double> BS_treat_BSeff;
    vector<SimTime> BS_treat_BSproph;


    ////////////////////////////////
    // First-line treatment - blood-stage drugs
    // plus primaquine

    vector<SimDate> PQ_treat_date_on;
    vector<SimDate> PQ_treat_date_off;
    vector<double> PQ_treat_BScover;
    vector<double> PQ_treat_BSeff;
    vector<SimTime> PQ_treat_BSproph;
    vector<double> PQ_treat_PQavail;
    vector<double> PQ_treat_PQeff;
    vector<SimTime> PQ_treat_PQproph;
    vector<int>    PQ_treat_G6PD_risk;
    vector<int>    PQ_treat_CYP2D6_risk;
    vector<int>    PQ_treat_preg_risk;
    vector<double> PQ_treat_low_age;


    ////////////////////////////////
    // MDA - blood-stage drugs

    vector<SimDate> MDA_BS_date;
    vector<double> MDA_BS_BScover;
    vector<double> MDA_BS_BSeff;
    vector<SimTime> MDA_BS_BSproph;


    ////////////////////////////////
    // MDA - blood-stage drugs + primaquine

    vector<SimDate> MDA_PQ_date;
    vector<double> MDA_PQ_BScover;
    vector<double> MDA_PQ_BSeff;
    vector<SimTime> MDA_PQ_BSproph;
    vector<double> MDA_PQ_PQavail;
    vector<double> MDA_PQ_PQeff;
    vector<SimTime> MDA_PQ_PQproph;
    vector<int>    MDA_PQ_G6PD_risk;
    vector<int>    MDA_PQ_CYP2D6_risk;
    vector<int>    MDA_PQ_preg_risk;
    vector<double> MDA_PQ_low_age;


    ////////////////////////////////
    // MSAT - RDT or PCR with blood-stage drugs + primaquine

    vector<SimDate> MSAT_PQ_date;
    vector<int>    MSAT_PQ_RDT_PCR;
    vector<double> MSAT_PQ_sens;
    vector<double> MSAT_PQ_spec;
    vector<double> MSAT_PQ_BScover;
    vector<double> MSAT_PQ_BSeff;
    vector<SimTime> MSAT_PQ_BSproph;
    vector<double> MSAT_PQ_PQavail;
    vector<double> MSAT_PQ_PQeff;
    vector<SimTime> MSAT_PQ_PQproph;
    vector<int>    MSAT_PQ_G6PD_risk;
    vector<int>    MSAT_PQ_CYP2D6_risk;
    vector<int>    MSAT_PQ_preg_risk;
    vector<double> MSAT_PQ_low_age;


    ////////////////////////////////
    // SSAT - blood-stage drugs + primaquine

    vector<SimDate> SSAT_PQ_date;
    vector<double> SSAT_PQ_sens;
    vector<double> SSAT_PQ_spec;
    vector<double> SSAT_PQ_BScover;
    vector<double> SSAT_PQ_BSeff;
    vector<SimTime> SSAT_PQ_BSproph;
    vector<double> SSAT_PQ_PQavail;
    vector<double> SSAT_PQ_PQeff;
    vector<SimTime> SSAT_PQ_PQproph;
    vector<int>    SSAT_PQ_G6PD_risk;
    vector<int>    SSAT_PQ_CYP2D6_risk;
    vector<int>    SSAT_PQ_preg_risk;
    vector<double> SSAT_PQ_low_age;
};

} }

#endif
