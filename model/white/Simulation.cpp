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

#include "white/Simulation.hpp"
#include "Population.h"
#include "util/errors.h"

#include <iostream>
#include <fstream>
#include <cmath>

namespace OM { namespace white {

using std::cout;
using std::endl;

////////////////////////////////////////////////////////////
//                                                        //
//  Function declarations                                 //
//                                                        //
////////////////////////////////////////////////////////////

// Defined in Mosquito.cpp
void mosquito_step(Params& theta, Population& POP);


Simulation::Simulation(SimTimes times)
{
    sim::s_start = SimDate::origin() + times.start;
    sim::s_end = SimDate::origin() + times.end;
    N_time = (times.burnin + times.end - times.start).inSteps();
    m_burnin = times.burnin;
    

    /////////////////////////////////////////////////////////////////////////
    // 1.9.2. Create storage for output

    yH_t.resize(N_time, N_H_comp);
    yM_t.resize(N_time);

    prev_all.resize(N_time, N_PREV);
    prev_U5.resize(N_time, N_PREV);
    prev_U10.resize(N_time, N_PREV);

    EIR_t.resize(N_time);

    LLIN_cov_t.resize(N_time);
    IRS_cov_t.resize(N_time);
    ACT_treat_t.resize(N_time);
    PQ_treat_t.resize(N_time);
    pregnant_t.resize(N_time);

    PQ_overtreat_t.resize(N_time);
    PQ_overtreat_9m_t.resize(N_time);

    A_par_mean_t.resize(N_time);
    A_clin_mean_t.resize(N_time);
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  2.6. Simulate the model and store the output in SIM                     //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

void Simulation::run(Params& theta, Intervention& INTVEN)
{
    sim::s_t0 = SimTime::zero();
    sim::s_t1 = SimTime::never();
    sim::s_interv = SimTime::zero() - m_burnin;
    
    // Initialise the population
    Population POP{ OM::Population::size(), theta };

    cout << "Starting model simulations......." << endl;

    for (int i = 0; i<N_time; i++)
    {
        if (sim::now().inDays() % 365 == 0) {
            cout
                << sim::intervDate()
                << "\t"
                << 100.0*i / (double(N_time))
                << "% complete"
                << endl;
        }
        
        POP.human_step(theta);

        mosquito_step(theta, POP);
        
        INTVEN.distribute(theta, POP);

        sim::s_t0 += SimTime::oneTS();
        sim::s_interv += SimTime::oneTS();

        POP.summary();

        //////////////////////////////////////
        // Fill out Simulation object

        yH_t.row(i) = POP.yH;
        yM_t[i] = POP.yM;

        prev_all.row(i) = POP.prev_all;
        prev_U5.row(i) = POP.prev_U5;
        prev_U10.row(i) = POP.prev_U10;

        LLIN_cov_t[i] = POP.LLIN_cov_t;
        IRS_cov_t[i] = POP.IRS_cov_t;
        ACT_treat_t[i] = POP.ACT_treat_t;
        PQ_treat_t[i] = POP.PQ_treat_t;
        pregnant_t[i] = POP.pregnant_t;

        PQ_overtreat_t[i] = POP.PQ_overtreat_t;
        PQ_overtreat_9m_t[i] = POP.PQ_overtreat_9m_t;


        EIR_t[i] = (POP.aa_VC * POP.yM.col(5)).sum();

        A_par_mean_t[i] = POP.A_par_mean_t;
        A_clin_mean_t[i] = POP.A_clin_mean_t;
    }
    
    cout << "Model simulations completed....." << endl;
    cout << endl;
}


void Simulation::write_output(const char *output_File)
{
    cout << "Start writing output to file......" << endl;
    cout << endl;

    std::ofstream out(output_File);
    
    // Write header
    out << "\
time (days)\t\
S\tI_PCR\tI_LM\tI_D\tT\tP\t";
    for (int g = 0; g < N_spec; g++)
    {
        // Write only compartments S, E and I in mosquitoes
        for (int k = 3; k < N_M_comp; k++)
        // Write all compartments in mosquitoes
        // for (int k = 0; k < N_M_comp; k++)
        {
            out << "yM_" << g << "_" << k << "\t";
        }
    }
    out << "\
N_pop\t\
PvPR_PCR\t\
PvPR_LM\t\
Pv_clin\t\
PvHR\t\
PvHR_batches\t\
new_PCR\t\
new_LM\t\
new_D\t\
new_T\t\
";
    out << "\
EIR\t\
LLIN_cov\t\
IRS_cov\t\
ACT_treat\t\
PQ_treat\t\
PQ_overtreat\t\
PQ_overtreat_9m\t\
" << endl;

    SimTime time_offset = sim::startDate() - SimDate::origin() - m_burnin;
    
    for (int i = m_burnin.inSteps(); i<N_time; i++)
    {
        double t = (time_offset + SimTime::oneTS() * i).inDays();
        out << t << "\t";

        for (int k = 0; k<N_H_comp; k++)
        {
            out << yH_t(i, k) << "\t";
        }

        for (int g = 0; g < N_spec; g++)
        {
            // Write only compartments S, E and I in mosquitoes
            for (int k = 3; k < N_M_comp; k++)
            // Write all compartments in mosquitoes
            // for (int k = 0; k < N_M_comp; k++)
            {
                out << yM_t[i](g, k) << "\t";
            }
        }

        for (int k = 0; k<10; k++)
        {
            out << prev_all(i, k) << "\t";
        }

        // Write output for age categories U5 and U10
        /*for (int k = 0; k<10; k++)
        {
            out << prev_U5(i, k) << "\t";
        }

        for (int k = 0; k<10; k++)
        {
            out << prev_U10(i, k) << "\t";
        }*/

        out << EIR_t[i] << "\t";
        out << LLIN_cov_t[i] << "\t";
        out << IRS_cov_t[i] << "\t";
        out << ACT_treat_t[i] << "\t";
        out << PQ_treat_t[i] << "\t";
        // Write number of pregnant women
        // out << pregnant_t[i] << "\t";

        out << PQ_overtreat_t[i] << "\t";
        out << PQ_overtreat_9m_t[i] << "\t";

        // Write A_par_mean_t and A_clin_mean_t
        /*out << A_par_mean_t[i] << "\t";
        out << A_clin_mean_t[i] << "\t";*/

        out << endl;
    }

    out.close();


    cout << "Output successfully written to file......" << endl;
    cout << endl;
}

} }
