/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///                                                                       ///
///  Individual-based Plasmodium vivax transmission model.                ///
///                                                                       ///
///  Dr Michael White                                                     ///
///  Institut Pasteur                                                     ///
///  michael.white@pasteur.fr                                             ///
///                                                                       ///
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

#include "white/Intervention.hpp"
#include "util/random.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <limits>

namespace OM { namespace white {

using std::cerr;
using std::cout;
using std::endl;


////////////////////////////////////////////////////////////
//                                                        //
//  Function declarations                                 //
//                                                        //
////////////////////////////////////////////////////////////

double phi_inv(double pp, double mu, double sigma);


/////////////////////////////////////
// Read intervention data from input files
Intervention::Intervention(const std::string& coverage_File)
{
    /////////////////////////////////////////////////////////////////////////
    //                                                                     //
    // 1.7. Read in intervention coverage                                  //
    //                                                                     //
    /////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////
    // 1.7.1. Read in matrix of years and coverages

    cout << "Read in intervention coverage file............" << endl;

    std::ifstream coverage_Stream(coverage_File);

    if (coverage_Stream.fail())
    {
        cout << "Failure reading in data." << endl;
    }


    /////////////////////////////////////////////////////////////////////////
    // 1.7.2. Note that the matrix we read in may have variable size.
    //        We first read in the first line of the file, and then matching
    //        the spaces between columns to get the number of interventions 
    //        rounds.
    //
    //        There's very likely a much more effective way to do this.

    std::string line_read;
    int N_cov_rounds = 0;

    getline(coverage_Stream, line_read);


    std::string str(line_read);
    std::string str2(" ");

    size_t space_find = str.find(str2);


    // Go through the string finding spaces (" ")

    while (space_find < 10000)
    {
        space_find = line_read.find(str2);

        line_read.erase(0, space_find + 1);

        N_cov_rounds += 1;
    }

    N_cov_rounds = N_cov_rounds - 2;

    cout << "Number of intervention rounds: " << N_cov_rounds << endl;

    coverage_Stream.close();


    /////////////////////////////////////////////////////////////////////////
    // 1.7.3. Note that the matrix we read in may have variable size.

    std::ifstream coverage_Stream2(coverage_File);

    std::string discard;

    vector<vector<double>> coverage;
    coverage.resize(0);

    vector<double> cov_read(N_cov_rounds + 1);

    for (int i = 0; i < 55; i++)
    {
        coverage_Stream2 >> discard;

        for (int j = 1; j<(N_cov_rounds + 2); j++)
        {
            coverage_Stream2 >> cov_read[j-1];
        }

        coverage.push_back(cov_read);
    }


    /////////////////////////////////////////////////////////////////////////
    // 1.7.2. Fill out Intervention structure

    for (int j = 0; j<N_cov_rounds; j++)
    {
        // skip rows without year
        if (!(coverage[0][j] > -0.5)) continue;
        
        SimDate date_on = SimDate::origin() + SimTime::fromYearsN(coverage[0][j]);
        
        //////////////////////////////////////////////////////////////
        // LLINs

        if (coverage[1][j] > -0.5)
        {
            LLIN_date.push_back(  date_on );
            LLIN_cover.push_back( coverage[1][j] );
        }


        //////////////////////////////////////////////////////////////
        // IRS

        if (coverage[2][j] > -0.5)
        {
            IRS_date.push_back(  date_on );
            IRS_cover.push_back( coverage[2][j] );
        }


        //////////////////////////////////////////////////////////////
        // Front-line treatment - blood-stage drugs

        if (coverage[4][j] > -0.5)
        {
            BS_treat_date_on.push_back(  date_on );
            SimDate date_off = SimDate::origin() + SimTime::fromYearsN(coverage[3][j]);
            BS_treat_date_off.push_back( date_off );
            BS_treat_BScover.push_back(  coverage[4][j] );
            BS_treat_BSeff.push_back(    coverage[5][j] );
            BS_treat_BSproph.push_back(  SimTime::fromDays(coverage[6][j]) );
        }


        //////////////////////////////////////////////////////////////
        // Front-line treatment - primaquine

        if (coverage[8][j] > -0.5)
        {
            PQ_treat_date_on.push_back(     date_on );
            SimDate date_off = SimDate::origin() + SimTime::fromYearsN(coverage[7][j]);
            PQ_treat_date_off.push_back(    date_off );
            PQ_treat_BScover.push_back(     coverage[8][j] );
            PQ_treat_BSeff.push_back(       coverage[9][j] );
            PQ_treat_BSproph.push_back(     SimTime::fromDays(coverage[10][j]) );
            PQ_treat_PQavail.push_back(     coverage[11][j] );
            PQ_treat_PQeff.push_back(       coverage[12][j] );
            PQ_treat_PQproph.push_back(     SimTime::fromDays(coverage[13][j]) );
            PQ_treat_G6PD_risk.push_back(   (int)(coverage[14][j]) );
            PQ_treat_CYP2D6_risk.push_back( (int)(coverage[15][j]) );
            PQ_treat_preg_risk.push_back(   (int)(coverage[16][j]) );
            PQ_treat_low_age.push_back(     coverage[17][j] );
        }


        //////////////////////////////////////////////////////////////
        // MDA - blood-stage drugs

        if (coverage[18][j] > -0.5)
        {
            MDA_BS_date.push_back(    date_on );
            MDA_BS_BScover.push_back( coverage[18][j] );
            MDA_BS_BSeff.push_back(   coverage[19][j] );
            MDA_BS_BSproph.push_back( SimTime::fromDays(coverage[20][j]) );
        }


        //////////////////////////////////////////////////////////////
        // MDA - blood-stage drugs plus primaquine

        if (coverage[21][j] > -0.5)
        {
            MDA_PQ_date.push_back(        date_on );
            MDA_PQ_BScover.push_back(     coverage[21][j] );
            MDA_PQ_BSeff.push_back(       coverage[22][j] );
            MDA_PQ_BSproph.push_back(     SimTime::fromDays(coverage[23][j]) );
            MDA_PQ_PQavail.push_back(     coverage[24][j] );
            MDA_PQ_PQeff.push_back(       coverage[25][j] );
            MDA_PQ_PQproph.push_back(SimTime::fromDays(coverage[26][j]));
            MDA_PQ_G6PD_risk.push_back(   (int)(coverage[27][j]) );
            MDA_PQ_CYP2D6_risk.push_back( (int)(coverage[28][j]) );
            MDA_PQ_preg_risk.push_back(   (int)(coverage[29][j]) );
            MDA_PQ_low_age.push_back(     coverage[30][j] );
        }


        //////////////////////////////////////////////////////////////
        // MSAT - blood-stage drugs plus primaquine

        if (coverage[31][j] > -0.5)
        {
            MSAT_PQ_date.push_back(        date_on );
            MSAT_PQ_BScover.push_back(     coverage[31][j]);
            MSAT_PQ_RDT_PCR.push_back(     coverage[32][j] );
            MSAT_PQ_sens.push_back(        coverage[33][j] );
            MSAT_PQ_BSeff.push_back(       coverage[34][j] );
            MSAT_PQ_BSproph.push_back(     SimTime::fromDays(coverage[35][j]) );
            MSAT_PQ_PQavail.push_back(     coverage[36][j] );
            MSAT_PQ_PQeff.push_back(       coverage[37][j] );
            MSAT_PQ_PQproph.push_back(SimTime::fromDays(coverage[38][j]));
            MSAT_PQ_G6PD_risk.push_back(   (int)(coverage[39][j]) );
            MSAT_PQ_CYP2D6_risk.push_back( (int)(coverage[40][j]) );
            MSAT_PQ_preg_risk.push_back(   (int)(coverage[41][j]) );
            MSAT_PQ_low_age.push_back(     coverage[42][j] );
        }

        //////////////////////////////////////////////////////////////
        // SSAT - blood-stage drugs plus primaquine

        if (coverage[43][j] > -0.5)
        {
            SSAT_PQ_date.push_back(        date_on );
            SSAT_PQ_BScover.push_back(     coverage[43][j] );
            SSAT_PQ_sens.push_back(        coverage[44][j] );
            SSAT_PQ_spec.push_back(        coverage[45][j] );
            SSAT_PQ_BSeff.push_back(       coverage[46][j] );
            SSAT_PQ_BSproph.push_back(     SimTime::fromDays(coverage[47][j]) );
            SSAT_PQ_PQavail.push_back(     coverage[48][j] );
            SSAT_PQ_PQeff.push_back(       coverage[49][j] );
            SSAT_PQ_PQproph.push_back(SimTime::fromDays(coverage[50][j]));
            SSAT_PQ_G6PD_risk.push_back(   (int)(coverage[51][j]) );
            SSAT_PQ_CYP2D6_risk.push_back( (int)(coverage[52][j]) );
            SSAT_PQ_preg_risk.push_back(   (int)(coverage[53][j]) );
            SSAT_PQ_low_age.push_back(     coverage[54][j] );
        }
    }
    
    cout << endl;
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  2.7. Vector control distribution                                        //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


void Intervention::distribute(Params& theta, Population& POP)
{
    SimDate date = sim::intervDate();
    
    double QQ;

    bool BS_effective;
    bool PQ_treat;
    bool PQ_effective;

    bool MSAT_pos;
    bool SSAT_pos;

    //////////////////////////////////////////////////////////
    // Intervention 1: LLINS

    for (size_t m = 0; m<LLIN_date.size(); m++)
    {
        if (date == LLIN_date[m]) {
            cout << "LLIN distribution" << endl;

            QQ = phi_inv(LLIN_cover[m], 0.0, sqrt(1.0 + theta.sig_round_LLIN*theta.sig_round_LLIN));

            for (Individual& person: POP.m_people) {
                if (util::random::normal(person.zz_int[0], theta.sig_round_LLIN) < QQ)
                {
                    person.LLIN = 1;
                    person.LLIN_age = 0.0;

                    person.d_LLIN = theta.d_LLIN_0;
                    person.r_LLIN = theta.r_LLIN_0;
                    person.s_LLIN = 1.0 - person.d_LLIN - person.r_LLIN;
                }
            }
        }
    }


    //////////////////////////////////////////////////////////
    // Intervention 2: IRS

    for (size_t m = 0; m<IRS_date.size(); m++)
    {
        if (date == IRS_date[m]) {
            cout << "IRS distribution" << endl;

            QQ = phi_inv(IRS_cover[m], 0.0, sqrt(1.0 + theta.sig_round_IRS*theta.sig_round_IRS));

            for (Individual& person: POP.m_people) {
                if (util::random::normal(person.zz_int[1], theta.sig_round_IRS) < QQ)
                {
                    person.IRS = 1;
                    person.IRS_age = 0.0;

                    person.d_IRS = theta.d_IRS_0;
                    person.r_IRS = theta.r_IRS_0;
                    person.s_IRS = 1.0 - person.d_IRS - person.r_IRS;
                }
            }
        }
    }


    //////////////////////////////////////////////////////////
    // Intervention 3: first-line treatment (blood-stage)

    for (size_t m = 0; m<BS_treat_date_on.size(); m++)
    {
        if (date == BS_treat_date_on[m]) {
            cout << "New front-line BS treatment" << endl;

            theta.BS_treat_BScover = BS_treat_BScover[m];
            theta.BS_treat_BSeff   = BS_treat_BSeff[m];
            theta.BS_treat_BSproph = BS_treat_BSproph[m];

            theta.treat_BScover = theta.BS_treat_BScover;
            theta.treat_BSeff   = theta.BS_treat_BSeff;
            theta.treat_PQavail = 0.0;
            theta.r_P           = 1.0 / theta.BS_treat_BSproph.inDays();
        }
    }


    //////////////////////////////////////////////////////////
    // Switching back to baseline.

    for (size_t m = 0; m<BS_treat_date_on.size(); m++)
    {
        if (date == BS_treat_date_off[m]) {
            cout << "End of changing front-line BS treatment" << endl;

            theta.treat_BScover = theta.BS_treat_BScover_base;
            theta.treat_BSeff   = theta.BS_treat_BSeff_base;
            theta.treat_PQavail = 0.0;
            theta.r_P           = 1.0 / theta.BS_treat_BSproph_base;
        }
    }


    //////////////////////////////////////////////////////////
    // Intervention 4: first-line treatment (primaquine)

    for (size_t m = 0; m<PQ_treat_date_on.size(); m++)
    {
        if (date == PQ_treat_date_on[m]) {
            cout << "New front-line PQ treatment" << endl;

            theta.PQ_treat_BScover     = PQ_treat_BScover[m];
            theta.PQ_treat_BSeff       = PQ_treat_BSeff[m];
            theta.PQ_treat_BSproph     = PQ_treat_BSproph[m];
            theta.PQ_treat_PQavail     = PQ_treat_PQavail[m];
            theta.PQ_treat_PQeff       = PQ_treat_PQeff[m];
            theta.PQ_treat_PQproph     = PQ_treat_PQproph[m];
            theta.PQ_treat_G6PD_risk   = PQ_treat_G6PD_risk[m];
            theta.PQ_treat_CYP2D6_risk = PQ_treat_CYP2D6_risk[m];
            theta.PQ_treat_preg_risk   = PQ_treat_preg_risk[m];
            theta.PQ_treat_low_age     = PQ_treat_low_age[m];

            theta.treat_BScover = theta.PQ_treat_BScover;
            theta.treat_BSeff   = theta.PQ_treat_BSeff;
            theta.treat_PQavail = theta.PQ_treat_PQavail;
            theta.r_P           = 1.0 / theta.PQ_treat_BSproph.inDays();
        }
    }


    //////////////////////////////////////////////////////////
    // Switching back to baseline.

    for (size_t m = 0; m<PQ_treat_date_on.size(); m++)
    {
        if (date == PQ_treat_date_off[m]) {
            cout << "End of changing front-line PQ treatment" << endl;

            theta.PQ_treat_BScover     = 0.0;
            theta.PQ_treat_BSeff       = 0.0;
            theta.PQ_treat_BSproph     = SimTime::fromDays(10);
            theta.PQ_treat_PQavail     = 0.0;
            theta.PQ_treat_PQeff       = 0.0;
            theta.PQ_treat_PQproph     = SimTime::fromDays(10);
            theta.PQ_treat_G6PD_risk   = 1;
            theta.PQ_treat_CYP2D6_risk = 1;
            theta.PQ_treat_preg_risk   = 1;
            theta.PQ_treat_low_age     = 180.0;

            theta.treat_BScover = theta.BS_treat_BScover_base;
            theta.treat_BSeff   = theta.BS_treat_BSeff_base;
            theta.treat_PQavail = 0.0;
            theta.r_P           = 1.0 / theta.BS_treat_BSproph_base;
        }
    }


    //////////////////////////////////////////////////////////
    // Intervention 5: MDA (blood-stage)

    for (size_t m = 0; m<MDA_BS_date.size(); m++)
    {
        if (date == MDA_BS_date[m]) {
            cout << "MDA (BS) distribution" << endl;

            theta.MDA_BS_BScover = MDA_BS_BScover[m];
            theta.MDA_BS_BSeff   = MDA_BS_BSeff[m];
            theta.MDA_BS_BSproph = MDA_BS_BSproph[m];

            QQ = phi_inv(theta.MDA_BS_BScover, 0.0, sqrt(1.0 + theta.sig_round_MDA*theta.sig_round_MDA));

            for (Individual& person: POP.m_people) {
                if (util::random::normal(person.zz_int[2], theta.sig_round_MDA) < QQ)
                {
                    person.ACT_treat = 1;

                    if (util::random::bernoulli(theta.MDA_BS_BSeff))
                    {
                        if (person.S == 1    ) { person.S = 0;     person.P = 1; }
                        if (person.I_PCR == 1) { person.I_PCR = 0; person.P = 1; }
                        if (person.I_LM == 1 ) { person.I_LM = 0;  person.P = 1; }
                        if (person.I_D == 1  ) { person.I_D = 0;   person.T = 1; }
                    }
                }
            }
        }
    }


    //////////////////////////////////////////////////////////
    // Intervention 6: MDA (blood-stage and liver-stage)

    for (size_t m = 0; m<MDA_PQ_date.size(); m++)
    {
        if (date == MDA_PQ_date[m]) {
            cout << "MDA (BS+PQ) distribution" << endl;

            theta.MDA_PQ_BScover     = MDA_PQ_BScover[m];
            theta.MDA_PQ_BSeff       = MDA_PQ_BSeff[m];
            theta.MDA_PQ_BSproph     = MDA_PQ_BSproph[m];
            theta.MDA_PQ_PQavail     = MDA_PQ_PQavail[m];
            theta.MDA_PQ_PQeff       = MDA_PQ_PQeff[m];
            theta.MDA_PQ_PQproph     = MDA_PQ_PQproph[m];
            theta.MDA_PQ_G6PD_risk   = MDA_PQ_G6PD_risk[m];
            theta.MDA_PQ_CYP2D6_risk = MDA_PQ_CYP2D6_risk[m];
            theta.MDA_PQ_preg_risk   = MDA_PQ_preg_risk[m];
            theta.MDA_PQ_low_age     = MDA_PQ_low_age[m];

            QQ = phi_inv(theta.MDA_PQ_BScover, 0.0, sqrt(1.0 + theta.sig_round_MDA*theta.sig_round_MDA));

            for (Individual& person: POP.m_people) {
                if (util::random::normal(person.zz_int[3], theta.sig_round_MDA) < QQ)
                {
                    /////////////////////////////////////////////////////
                    // Blood-stage treatment is always administered
                    // Is blood-stage treatment effective

                    BS_effective = 0;

                    if (util::random::bernoulli(theta.MDA_PQ_BSeff))
                    {
                        BS_effective = 1;
                    }


                    /////////////////////////////////////////////////////////////////////
                    // Is PQ administered?

                    PQ_treat = 0;

                    if( util::random::bernoulli(theta.MDA_PQ_PQavail) )
                    {
                        PQ_treat = 1;
                    }
                    

                    /////////////////////////////////////////////////////////////////////
                    // Exclude PQ because of G6PD deficiency

                    if( (theta.MDA_PQ_G6PD_risk == 1) && (person.is_G6PD_deficient()) )
                    {
                        PQ_treat = 0;
                    }


                    /////////////////////////////////////////////////////////////////////
                    // Exclude PQ because of pregancy

                    if( (theta.MDA_PQ_preg_risk == 1) && person.pregnant )
                    {
                        PQ_treat = 0;
                    }


                    /////////////////////////////////////////////////////////////////////
                    // Exclude PQ because of young age

                    if (person.age < theta.MDA_PQ_low_age)
                    {
                        PQ_treat = 0;
                    }

                    if (PQ_treat == 1)
                    {
                        person.PQ_treat = 1;
                    }

                    /////////////////////////////////////////////////////////////////////
                    // Is PQ effective?

                    PQ_effective = 1;

                    if( !util::random::bernoulli(theta.MDA_PQ_PQeff) )
                    {
                        PQ_effective = 0;
                    }

                    if( (theta.MDA_PQ_CYP2D6_risk == 1) && person.has_low_cyp2d6_action() )
                    {
                        PQ_effective = 0;
                    }


                    /////////////////////////////////////////////////////////////////////
                    // Was there PQ overtreatment?

                    if( (PQ_treat == 1) && (person.Hyp == 0) )
                    {
                        person.PQ_overtreat = 1;
                    }

                    if( (PQ_treat == 1) && (person.T_last_BS > 270.0) )
                    {
                        person.PQ_overtreat_9m = 1;
                    }


                    /////////////////////////////////////////////////////////////////////
                    // ACTION: administer blood-stage drug

                    person.ACT_treat = 1;

                    if (BS_effective == 1)
                    {
                        if (person.S == 1) {     person.S = 0;     person.P = 1; }
                        if (person.I_PCR == 1) { person.I_PCR = 0; person.P = 1; }
                        if (person.I_LM == 1) {  person.I_LM = 0;  person.P = 1; }
                        if (person.I_D == 1) {   person.I_D = 0;   person.T = 1; }
                    }


                    /////////////////////////////////////////////////////////////////////
                    // ACTION: administer primaquine

                    if ((PQ_treat == 1) && (PQ_effective == 1))
                    {
                        person.Hyp = 0;

                        person.PQ_proph = sim::now() + theta.MDA_PQ_PQproph;
                    }
                }
            }
        }
    }


    //////////////////////////////////////////////////////////
    // Intervention 7: MSAT (blood-stage and liver-stage)

    for (size_t m = 0; m < MSAT_PQ_date.size(); m++)
    {
        if (date == MSAT_PQ_date[m]) {
            cout << "MSAT (BS+PQ) distribution" << endl;

            theta.MSAT_PQ_BScover     = MSAT_PQ_BScover[m];
            theta.MSAT_PQ_RDT_PCR     = MSAT_PQ_RDT_PCR[m];
            theta.MSAT_PQ_sens        = MSAT_PQ_sens[m];
            theta.MSAT_PQ_BSeff       = MSAT_PQ_BSeff[m];
            theta.MSAT_PQ_BSproph     = MSAT_PQ_BSproph[m];
            theta.MSAT_PQ_PQavail     = MSAT_PQ_PQavail[m];
            theta.MSAT_PQ_PQeff       = MSAT_PQ_PQeff[m];
            theta.MSAT_PQ_PQproph     = MSAT_PQ_PQproph[m];
            theta.MSAT_PQ_G6PD_risk   = MSAT_PQ_G6PD_risk[m];
            theta.MSAT_PQ_CYP2D6_risk = MSAT_PQ_CYP2D6_risk[m];
            theta.MSAT_PQ_preg_risk   = MSAT_PQ_preg_risk[m];
            theta.MSAT_PQ_low_age     = MSAT_PQ_low_age[m];

            QQ = phi_inv(theta.MSAT_PQ_BScover, 0.0, sqrt(1.0 + theta.sig_round_MDA*theta.sig_round_MDA));

            for (Individual& person: POP.m_people) {
                if (util::random::normal(person.zz_int[4], theta.sig_round_MDA) < QQ)
                {
                    /////////////////////////////////////////////////////
                    // Blood-stage treatment is always administered

                    MSAT_pos = 0;

                    ////////////////////////////////////////////////
                    // Diagnosis by RDT, assumed the same as LM 

                    if (theta.MSAT_PQ_RDT_PCR == 1)
                    {
                        if ((person.I_LM == 1) || (person.I_D == 1) || (person.T == 1))
                        {
                            if (util::random::bernoulli(theta.MSAT_PQ_sens))
                            {
                                MSAT_pos = 1;
                            }
                        }
                    }


                    ////////////////////////////////////////////////
                    // Diagnosis by PCR 

                    if (theta.MSAT_PQ_RDT_PCR == 2)
                    {
                        if ((person.I_PCR == 1) || (person.I_LM == 1) || (person.I_D == 1) || (person.T == 1))
                        {
                            if (util::random::bernoulli(theta.MSAT_PQ_sens))
                            {
                                MSAT_pos = 1;
                            }
                        }
                    }


                    /////////////////////////////////////////////////////
                    // Is blood-stage treatment effective

                    BS_effective = 0;

                    if (util::random::bernoulli(theta.MSAT_PQ_BSeff))
                    {
                        BS_effective = 1;
                    }


                    /////////////////////////////////////////////////////////////////////
                    // Is PQ administered?

                    PQ_treat = 0;

                    if (MSAT_pos == 1)
                    {
                        if (util::random::bernoulli(theta.MSAT_PQ_PQavail))
                        {
                            PQ_treat = 1;
                        }
                    }


                    /////////////////////////////////////////////////////////////////////
                    // Exclude PQ because of G6PD deficiency

                    if ( (theta.MSAT_PQ_G6PD_risk == 1) && (person.is_G6PD_deficient()) )
                    {
                        PQ_treat = 0;
                    }

                    /////////////////////////////////////////////////////////////////////
                    // Exclude PQ because of pregancy

                    if ( (theta.MSAT_PQ_preg_risk == 1) && person.pregnant )
                    {
                        PQ_treat = 0;
                    }

                    /////////////////////////////////////////////////////////////////////
                    // Exclude PQ because of young age

                    if (person.age < theta.MSAT_PQ_low_age)
                    {
                        PQ_treat = 0;
                    }

                    if (PQ_treat == 1)
                    {
                        person.PQ_treat = 1;
                    }

                    /////////////////////////////////////////////////////////////////////
                    // Is PQ effective?

                    PQ_effective = 1;

                    if (!util::random::bernoulli(theta.MSAT_PQ_PQeff))
                    {
                        PQ_effective = 0;
                    }

                    if ((theta.MSAT_PQ_CYP2D6_risk == 1) && person.has_low_cyp2d6_action())
                    {
                        PQ_effective = 0;
                    }


                    /////////////////////////////////////////////////////////////////////
                    // Was there PQ overtreatment?

                    if ((PQ_treat == 1) && (person.Hyp == 0))
                    {
                        person.PQ_overtreat = 1;
                    }

                    if ((PQ_treat == 1) && (person.T_last_BS > 270.0))
                    {
                        person.PQ_overtreat_9m = 1;
                    }


                    /////////////////////////////////////////////////////////////////////
                    // ACTION: administer blood-stage drug

                    if (MSAT_pos == 1)
                    {
                        person.ACT_treat = 1;

                        if (BS_effective == 1)
                        {
                            if (person.S == 1) {     person.S = 0;     person.P = 1; }
                            if (person.I_PCR == 1) { person.I_PCR = 0; person.P = 1; }
                            if (person.I_LM == 1) {  person.I_LM = 0;  person.P = 1; }
                            if (person.I_D == 1) {   person.I_D = 0;   person.T = 1; }
                        }
                    }

                    
                    /////////////////////////////////////////////////////////////////////
                    // ACTION: administer primaquine

                    if ((PQ_treat == 1) && (PQ_effective == 1))
                    {
                        person.Hyp = 0;

                        person.PQ_proph = sim::now() + theta.MSAT_PQ_PQproph;
                    }
                }
            }
        }
    }


    //////////////////////////////////////////////////////////
    // Intervention 8: SSAT (blood-stage and liver-stage)

    for (size_t m = 0; m<SSAT_PQ_date.size(); m++)
    {
        if (date == SSAT_PQ_date[m]) {
            cout << "SSAT (BS+PQ) distribution" << endl;

            theta.SSAT_PQ_BScover     = SSAT_PQ_BScover[m];
            theta.SSAT_PQ_sens        = SSAT_PQ_sens[m];
            theta.SSAT_PQ_spec        = SSAT_PQ_spec[m];
            theta.SSAT_PQ_BSeff       = SSAT_PQ_BSeff[m];
            theta.SSAT_PQ_BSproph     = SSAT_PQ_BSproph[m];
            theta.SSAT_PQ_PQavail     = SSAT_PQ_PQavail[m];
            theta.SSAT_PQ_PQeff       = SSAT_PQ_PQeff[m];
            theta.SSAT_PQ_PQproph     = SSAT_PQ_PQproph[m];
            theta.SSAT_PQ_G6PD_risk   = SSAT_PQ_G6PD_risk[m];
            theta.SSAT_PQ_CYP2D6_risk = SSAT_PQ_CYP2D6_risk[m];
            theta.SSAT_PQ_preg_risk   = SSAT_PQ_preg_risk[m];
            theta.SSAT_PQ_low_age     = SSAT_PQ_low_age[m];

            QQ = phi_inv(theta.SSAT_PQ_BScover, 0.0, sqrt(1.0 + theta.sig_round_MDA*theta.sig_round_MDA));

            for (Individual& person: POP.m_people) {
                if (util::random::normal(person.zz_int[5], theta.sig_round_MDA) < QQ)
                {
                    /////////////////////////////////////////////////////
                    // Blood-stage treatment is always administered

                    person.ACT_treat = 1;


                    /////////////////////////////////////////////////////
                    // Is blood-stage treatment effective

                    BS_effective = 0;

                    if( util::random::bernoulli(theta.SSAT_PQ_BSeff) )
                    {
                        BS_effective = 1;
                    }

                    /////////////////////////////////////////////////////////////////////
                    // SSAT screening for blood-stage infection in the last 9 months
                    //
                    // There are two options here. Option 1 define over-treatment on the 
                    // basis of blood-stage infection with the last 9 month. Option 2
                    // defines over-treatment on the basis of presence of hypnozoites.

                    SSAT_pos = 0;

                    // OPTION 1
/*
                    if( (person.T_last_BS <= 270.0) && (util::random::bernoulli(theta.SSAT_PQ_sens)) )
                    {
                        SSAT_pos = 1;
                    }

                    if( (person.T_last_BS > 270.0) && (!util::random::bernoulli(theta.SSAT_PQ_spec)) )
                    {
                        SSAT_pos = 1;
                    }
*/

                    // OPTION 2

                    if ((person.Hyp > 0) && (util::random::bernoulli(theta.SSAT_PQ_sens)))
                    {
                        SSAT_pos = 1;
                    }

                    if ((person.Hyp == 0) && (!util::random::bernoulli(theta.SSAT_PQ_spec)))
                    {
                        SSAT_pos = 1;
                    }


                    /////////////////////////////////////////////////////////////////////
                    // Is PQ administered?

                    PQ_treat = 0;

                    if( SSAT_pos == 1 )
                    {
                        if( util::random::bernoulli(theta.SSAT_PQ_PQavail) )
                        {
                            PQ_treat = 1;
                        }
                    }


                    /////////////////////////////////////////////////////////////////////
                    // Exclude PQ because of G6PD deficiency

                    if( (theta.SSAT_PQ_G6PD_risk == 1) && (person.is_G6PD_deficient()) )
                    {
                        PQ_treat = 0;
                    }

                    /////////////////////////////////////////////////////////////////////
                    // Exclude PQ because of pregancy

                    if( (theta.SSAT_PQ_preg_risk == 1) && person.pregnant )
                    {
                        PQ_treat = 0;
                    }

                    /////////////////////////////////////////////////////////////////////
                    // Exclude PQ because of young age

                    if( person.age < theta.SSAT_PQ_low_age )
                    {
                        PQ_treat = 0;
                    }

                    if( PQ_treat == 1 )
                    {
                        person.PQ_treat = 1;
                    }

                    /////////////////////////////////////////////////////////////////////
                    // Is PQ effective?

                    PQ_effective = 0;

                    if( util::random::bernoulli(theta.SSAT_PQ_PQeff) )
                    {
                        PQ_effective = 1;
                    }

                    if( (theta.SSAT_PQ_CYP2D6_risk == 1) && person.has_low_cyp2d6_action() )
                    {
                        PQ_effective = 0;
                    }


                    /////////////////////////////////////////////////////////////////////
                    // Was there PQ overtreatment?

                    if ((PQ_treat == 1) && (person.Hyp == 0))
                    {
                        person.PQ_overtreat = 1;
                    }

                    if ((PQ_treat == 1) && (person.T_last_BS > 270.0))
                    {
                        person.PQ_overtreat_9m = 1;
                    }


                    /////////////////////////////////////////////////////////////////////
                    // ACTION: administer blood-stage drug

                    if( BS_effective == 1 )
                    {
                        if (person.S     == 1) { person.S = 0;     person.P = 1; }
                        if (person.I_PCR == 1) { person.I_PCR = 0; person.P = 1; }
                        if (person.I_LM  == 1) { person.I_LM = 0;  person.P = 1; }
                        if (person.I_D   == 1) { person.I_D = 0;   person.T = 1; }
                    }

                    /////////////////////////////////////////////////////////////////////
                    // ACTION: administer primaquine

                    if( (PQ_treat == 1) && (PQ_effective == 1) )
                    {
                        person.Hyp = 0;

                        person.PQ_proph = sim::now() + theta.SSAT_PQ_PQproph;
                    }
                }
            }


        }
    }

}


//////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////// 
//        //                                                                            // 
//  2.9.  //  Inverse of the cumulative normal distribution function required           //
//        //  for implementing correlated intervention coverage.                        //
////////////                                                                            //
////////////  The code is based on the following website                                //
////////////  http://www.johndcook.com/blog/cpp_phi_inverse/                            //
////////////  which is based the algorithm in Abramowitz and Stegun formula 26.2.23.    // 
////////////                                                                            //
////////////  The absolute value of the error should be less than 4.5 e-4 and it tests  //
////////////  out nicely in R.                                                          //
////////////                                                                            //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

double phi_inv(double pp, double mu, double sigma)
{
    if (pp < 0.0 || pp > 1.0)
    {
        throw("bad vlaue of pp (coverage) in phi_inv");
    }
    else if (pp == 0.0) 
    {
        return -std::numeric_limits<double>::infinity();
    }
    else if (pp == 1.0)
    {
        return std::numeric_limits<double>::infinity();
    }

    double cc[] = { 2.515517, 0.802853, 0.010328 };
    double dd[] = { 1.432788, 0.189269, 0.001308 };

    double tt, temp;

    if (pp < 0.5)
    {
        tt = sqrt(-2.0*log(pp));

        temp = -(tt - ((cc[2] * tt + cc[1])*tt + cc[0]) / (((dd[2] * tt + dd[1])*tt + dd[0])*tt + 1.0));

    }
    else {

        tt = sqrt(-2.0*log(1.0 - pp));

        temp = (tt - ((cc[2] * tt + cc[1])*tt + cc[0]) / (((dd[2] * tt + dd[1])*tt + dd[0])*tt + 1.0));
    }

    return mu + sigma*temp;
}

} }
