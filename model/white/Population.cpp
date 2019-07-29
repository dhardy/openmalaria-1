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

#include "white/Population.hpp"
#include "util/random.h"

#include <iostream>
#include <cmath>
#include "Eigen/Dense"

namespace OM { namespace white {

using std::cout;
using std::endl;
using Eigen::Array;
using Eigen::VectorXd;
using Eigen::MatrixXd;
using Eigen::Matrix;


Population::Population(size_t N_pop):
    N_pop(N_pop),
    pi_n(N_pop, N_spec),
    lam_n(N_pop, N_spec)
{
}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  2.4. Update the vector of human classes                                 //
//                                                                          // 
//       THINK CAREFULLY ABOUT THE ORDERING OF EVENTS                       //
//////////////////////////////////////////////////////////////////////////////

void Population::human_step(Params& theta)
{

    ///////////////////////////////////////////////
    // 2.4.2. Apply ageing

    for (int n = 0; n<N_pop; n++)
    {
        people[n].ager(theta);
    }


    ///////////////////////////////////////////////
    // 2.4.3. Deaths
    //
    // Look again at how things are erased from vectors.

    int N_dead = 0;

    for (size_t n = 0; n<people.size(); n++)
    {
        /////////////////////////////////////////////
        // Everyone has an equal probability of dying

        if (util::random::bernoulli(theta.P_dead))
        {
            people.erase(people.begin() + n);

            N_dead += 1;
            n = n - 1;      // If we erase something, the next one moves into it's place so we don't want to step forward.
        }
        else {

            ///////////////////////////////////////////
            // People die once they reach the maximum age

            if (people[n].age > theta.age_max)
            {
                people.erase(people.begin() + n);

                N_dead += 1;
                n = n - 1;       // If we erase something, the next one moves into it's place so we don't want to step forward.
            }
        }
    }

    /////////////////////////////////////////////////////////////
    // 2.4.4. Births - set up to ensure balanced population.
    //        Can be adjusted to account for changing demography.

    for (int n = 0; n<N_dead; n++)
    {
        double zeta_start;
        do
        {
            zeta_start = util::random::log_normal(-0.5*theta.sig_het*theta.sig_het, theta.sig_het);
        }
        while (zeta_start > theta.het_max);

        Individual HH(theta, 0.0, zeta_start);

        HH.S = true;


        /////////////////////////////////
        // Assign levels of maternally-acquired immunity
        // by finding women of child-bearing age with the
        // closest level of heterogeneity

        double het_dif_track = 1e10;

        for (size_t j = 0; j<people.size(); j++)
        {
            if (people[j].preg_age)
            {
                if (abs(HH.zeta_het - people[j].zeta_het) < het_dif_track)
                {
                    HH.A_par_mat = theta.P_mat*people[j].A_par_mat;
                    HH.A_clin_mat = theta.P_mat*people[j].A_clin_mat;

                    het_dif_track = (HH.zeta_het - people[j].zeta_het)*(HH.zeta_het - people[j].zeta_het);
                }
            }
        }


        /////////////////////////////////////////////////////////////////
        // 2.4.5. Push the created individual onto the vector of people

        people.push_back(std::move(HH));
    }



    ///////////////////////////////////////////////////
    // 2.4.6. Update individual-level vector control

    for (int n = 0; n<N_pop; n++)
    {
        people[n].intervention_updater(theta);
    }


    ///////////////////////////////////////////////////
    // 2.4.7. Update proportion of bites
    //
    //        Note the ordering of n and g loops. Need to 
    //        check if this makes a difference for speed.
    //
    //        Should be able to make this quicker


    for (int n = 0; n<N_pop; n++)
    {
        double pi = people[n].zeta_het*(1.0 - theta.rho_age*exp(-people[n].age*theta.age_0_inv));
        pi_n.row(n) = pi;
    }

    auto sigma_pi = pi_n.colwise().sum().inverse();
    pi_n.rowwise() *= sigma_pi;


    ///////////////////////////////////////////////////
    // 2.4.8 Update population-level vector control quantities

    SUM_pi_w.setZero();
    for (int n = 0; n < N_pop; n++) {
        SUM_pi_w += pi_n.row(n).transpose() * people[n].w_VC;
    }


    W_VC = 1.0 + theta.Q_0 * (SUM_pi_w - 1.0);
    Z_VC = theta.Q_0 * SUM_pi_z;
    
    delta_1_VC = theta.delta_1 / (1.0 - Z_VC);
    delta_VC = delta_1_VC + theta.delta_2;

    p_1_VC = theta.p_1 * W_VC / (1.0 - Z_VC * theta.p_1);

    mu_M_VC = -log(p_1_VC * theta.p_2) / delta_VC;

    Q_VC = 1.0 - (1.0 - theta.Q_0) / W_VC;

    aa_VC = Q_VC / delta_VC;

    exp_muM_tauM_VC = exp(-mu_M_VC * theta.tau_M);
    beta_VC = theta.eps_max * mu_M_VC / (exp(delta_VC * mu_M_VC) - 1.0);


    ///////////////////////////////////////////////////
    // 2.4.9. Update individual-level force of infection on humans

    for (int n = 0; n < N_pop; n++) {
        lam_n.row(n).transpose() = aa_VC * pi_n.row(n).transpose() * people[n].w_VC / SUM_pi_w;
    }


    ///////////////////////////////////////////////////
    // 2.4.10. Implement moves between compartments
    //
    // TO DO: Can take some multiplications out of the loop.

    Array<double, N_spec, 1> lam_bite_base = N_pop * theta.bb * yM.col(5);

    for (int n = 0; n<N_pop; n++)
    {
        double lam_bite = (lam_n.row(n).transpose() * lam_bite_base).sum();
        
        people[n].state_mover(theta, lam_bite);
    }

}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  2.5. Summarise the output from the population                           //
//                                                                          // 
//////////////////////////////////////////////////////////////////////////////

void Population::summary()
{
    yH.setZero();

    prev_all.setZero();
    prev_U5.setZero();
    prev_U10.setZero();


    for (int n = 0; n<N_pop; n++)
    {
        ////////////////////////////////////////
        // Numbers in each compartment

        yH[0] += people[n].S;
        yH[1] += people[n].I_PCR;
        yH[2] += people[n].I_LM;
        yH[3] += people[n].I_D;
        yH[4] += people[n].T;
        yH[5] += people[n].P;


        //////////////////////////////////////////////
        //////////////////////////////////////////////
        // Summary - full population

        ////////////////////////////////////////
        // Prevalence

        prev_all[0] += 1;                                                                        // Numbers - denominator
        prev_all[1] += people[n].I_PCR + people[n].I_LM +
                                            + people[n].I_D + people[n].T;                                      // PCR detectable infections
        prev_all[2] += people[n].I_LM + people[n].I_D + people[n].T;                // LM detectable infections
        prev_all[3] += people[n].I_D + people[n].T;                                      // Clinical episodes

        if (people[n].Hyp > 0)
        {
            prev_all[4] += 1;                     // Hypnozoite positive

            prev_all[5] += people[n].Hyp;    // Number of batches of hypnozoites
        }


        ////////////////////////////////////////
        // Incidence

        prev_all[6]  += people[n].I_PCR_new;
        prev_all[7]  += people[n].I_LM_new;
        prev_all[8]  += people[n].I_D_new;
        prev_all[9]  += people[n].ACT_treat;
        prev_all[10] += people[n].PQ_treat;


        //////////////////////////////////////////////
        //////////////////////////////////////////////
        // Summary - under 5's

        if (people[n].age < 1825.0)
        {
            ////////////////////////////////////////
            // Prevalence

            prev_U5[0] += 1;                                                                // Numbers - denominator
            prev_U5[1] += people[n].I_PCR + people[n].I_LM
                                              + people[n].I_D + people[n].T;                              // PCR detectable infections
            prev_U5[2] += people[n].I_LM + people[n].I_D + people[n].T;        // LM detectable infections
            prev_U5[3] += people[n].I_D + people[n].T;                              // Clinical episodes

            if (people[n].Hyp > 0)
            {
                prev_U5[4] += 1;                     // Hypnozoite positive

                prev_U5[5] += people[n].Hyp;    // Number of batches of hypnozoites
            }


            ////////////////////////////////////////
            // Incidence

            prev_U5[6] += people[n].I_PCR_new;
            prev_U5[7] += people[n].I_LM_new;
            prev_U5[8] += people[n].I_D_new;
            prev_U5[9] += people[n].ACT_treat;
            prev_U5[10] += people[n].PQ_treat;
        }

        //////////////////////////////////////////////
        //////////////////////////////////////////////
        // Summary - under 10's

        if (people[n].age < 3650.0)
        {
            ////////////////////////////////////////
            // Prevalence

            prev_U10[0] += 1;                                                            // Numbers - denominator
            prev_U10[1] += people[n].I_PCR + people[n].I_LM
                                                + people[n].I_D + people[n].T;                          // PCR detectable infections
            prev_U10[2] += people[n].I_LM + people[n].I_D + people[n].T;    // LM detectable infections
            prev_U10[3] += people[n].I_D + people[n].T;                          // Clinical episodes

            if (people[n].Hyp > 0)
            {
                prev_U10[4] += 1;                     // Hypnozoite positive

                prev_U10[5] += people[n].Hyp;    // Number of batches of hypnozoites
            }


            ////////////////////////////////////////
            // Incidence

            prev_U10[6]  += people[n].I_PCR_new;
            prev_U10[7]  += people[n].I_LM_new;
            prev_U10[8]  += people[n].I_D_new;
            prev_U10[9]  += people[n].ACT_treat;
            prev_U10[10] += people[n].PQ_treat;
        }
    }


    //////////////////////////////
    // Intervention coverage

    LLIN_cov_t = 0;
    IRS_cov_t = 0;
    ACT_treat_t = 0;
    PQ_treat_t = 0;
    pregnant_t = 0;

    PQ_overtreat_t = 0;
    PQ_overtreat_9m_t = 0;


    for (int n = 0; n<N_pop; n++)
    {
        LLIN_cov_t  += people[n].LLIN;
        IRS_cov_t   += people[n].IRS;
        ACT_treat_t += people[n].ACT_treat;
        PQ_treat_t  += people[n].PQ_treat;
        pregnant_t  += (people[n].pregnant ? 1 : 0);

        PQ_overtreat_t    += people[n].PQ_overtreat;
        PQ_overtreat_9m_t += people[n].PQ_overtreat_9m;
    }


    //////////////////////////////
    // Immunity

    double A_par_mean = 0.0, A_clin_mean = 0.0;

    for (int n = 0; n<N_pop; n++)
    {
        A_par_mean += people[n].A_par;
        A_clin_mean += people[n].A_clin;
    }

    A_par_mean_t = A_par_mean / ((double)N_pop);
    A_clin_mean_t = A_clin_mean / ((double)N_pop);
}


/////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////// 
//          //                                                         // 
//   ####   //  #####  ####  ##  ## #### ##      #####   ####  #####   //
//  ##  ##  //  ##    ##  ## ##  ##  ##  ##      ##  ## ##  ## ##  ##  //
//     ##   //  ####  ##  ## ##  ##  ##  ##      #####  ##  ## #####   //
//  ##  ##  //  ##     ####  ##  ##  ##  ##      ##     ##  ## ##      //
//   ####   //  #####     ##  ####  #### #####   ##      ####  ##      //
//          //                                                         //
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////
///////////////////////////////////////////////////
//       //                                      // 
//  3.5  //  Equilibrium matrix                  //
//       //                                      //
///////////////////////////////////////////////////
///////////////////////////////////////////////////

void MM_ij(int i, int j, Params& theta, Array<double, N_age, 1>& r_age,
           MatrixXd& MM, MatrixXd lam_eq,
           Array<Array<double, K_max + 1, 1>, Dynamic, N_het>& phi_LM_eq,
           Array<Array<double, K_max + 1, 1>, Dynamic, N_het>& phi_D_eq,
           Array<Array<double, K_max + 1, 1>, Dynamic, N_het>& r_PCR_eq)
{
    size_t K_dim = K_max + 1;
    
    //////////////////////////////////////////////
    // 4.5.1. Initialise matrix with zeros

    MM.setZero();


    //////////////////////////////////////////////
    // 4.5.2. Fill out non-zero elements

    for (size_t k1 = 0; k1<K_dim; k1++)
    {
        for (size_t k2 = 0; k2<K_dim; k2++)
        {
            MM(0 * K_dim + k1, 0 * K_dim + k2) = - lam_eq(i, j)*theta.D_MAT(k1, k2) - theta.ff*theta.K_MAT(k1, k2)
                                                             + theta.gamma_L*theta.L_MAT(k1, k2) - theta.mu_H*theta.D_MAT(k1, k2) - r_age[i] * theta.D_MAT(k1, k2);
            MM(0 * K_dim + k1, 1 * K_dim + k2) = + r_PCR_eq(i, j)[k2]*theta.D_MAT(k1, k2);
            MM(0 * K_dim + k1, 5 * K_dim + k2) = +theta.r_P*theta.D_MAT(k1, k2);

            MM(1 * K_dim + k1, 0 * K_dim + k2) = + lam_eq(i, j)*(1.0 - phi_LM_eq(i, j)[k2])*theta.OD_MAT(k1, k2) + theta.ff*(1.0 - phi_LM_eq(i, j)[k2])*theta.K_MAT(k1, k2);
            MM(1 * K_dim + k1, 1 * K_dim + k2) = - lam_eq(i, j)*theta.D_MAT(k1, k2) - theta.ff*theta.K_MAT(k1, k2) - r_PCR_eq(i, j)[k2]*theta.D_MAT(k1, k2)
                                                             + lam_eq(i, j)*(1.0 - phi_LM_eq(i, j)[k2])*theta.OD_MAT(k1, k2) + theta.ff*(1.0 - phi_LM_eq(i, j)[k2])*theta.K_MAT(k1, k2)
                                                             + theta.gamma_L*theta.L_MAT(k1, k2) - theta.mu_H*theta.D_MAT(k1, k2) - r_age[i] * theta.D_MAT(k1, k2);
            MM(1 * K_dim + k1, 2 * K_dim + k2) = + theta.r_LM*theta.D_MAT(k1, k2);

            MM(2 * K_dim + k1, 0 * K_dim + k2) = + lam_eq(i, j)*phi_LM_eq(i, j)[k2]*(1.0 - phi_D_eq(i, j)[k2])*theta.OD_MAT(k1, k2) + theta.ff*phi_LM_eq(i, j)[k2]*(1.0 - phi_D_eq(i, j)[k2])*theta.K_MAT(k1, k2);
            MM(2 * K_dim + k1, 1 * K_dim + k2) = + lam_eq(i, j)*phi_LM_eq(i, j)[k2]*(1.0 - phi_D_eq(i, j)[k2])*theta.OD_MAT(k1, k2) + theta.ff*phi_LM_eq(i, j)[k2] * (1.0 - phi_D_eq(i, j)[k2])*theta.K_MAT(k1, k2);
            MM(2 * K_dim + k1, 2 * K_dim + k2) = - lam_eq(i, j)*theta.D_MAT(k1, k2) - theta.ff*theta.K_MAT(k1, k2) - theta.r_LM*theta.D_MAT(k1, k2)
                                                             + lam_eq(i, j)*(1.0 - phi_D_eq(i, j)[k2])*theta.OD_MAT(k1, k2) + theta.ff*(1.0 - phi_D_eq(i, j)[k2])*theta.K_MAT(k1, k2)
                                                             + theta.gamma_L*theta.L_MAT(k1, k2) - theta.mu_H*theta.D_MAT(k1, k2) - r_age[i] * theta.D_MAT(k1, k2);
            MM(2 * K_dim + k1, 3 * K_dim + k2) = + theta.r_D*theta.D_MAT(k1, k2);

            MM(3 * K_dim + k1, 0 * K_dim + k2) = + lam_eq(i, j)*phi_LM_eq(i, j)[k2]*phi_D_eq(i, j)[k2]*(1.0 - theta.treat_BScover*theta.treat_BSeff)*theta.OD_MAT(k1, k2) 
                                                             + theta.ff*phi_LM_eq(i, j)[k2]*phi_D_eq(i, j)[k2]*(1.0 - theta.treat_BScover*theta.treat_BSeff)*theta.K_MAT(k1, k2);
            MM(3 * K_dim + k1, 1 * K_dim + k2) = + lam_eq(i, j)*phi_LM_eq(i, j)[k2]*phi_D_eq(i, j)[k2] * (1.0 - theta.treat_BScover*theta.treat_BSeff)*theta.OD_MAT(k1, k2) 
                                                             + theta.ff*phi_LM_eq(i, j)[k2]*phi_D_eq(i, j)[k2]*(1.0 - theta.treat_BScover*theta.treat_BSeff)*theta.K_MAT(k1, k2);
            MM(3 * K_dim + k1, 2 * K_dim + k2) = + lam_eq(i, j)*phi_D_eq(i, j)[k2]*(1.0 - theta.treat_BScover*theta.treat_BSeff)*theta.OD_MAT(k1, k2) + theta.ff*phi_D_eq(i, j)[k2]*(1.0 - theta.treat_BScover*theta.treat_BSeff)*theta.K_MAT(k1, k2);
            MM(3 * K_dim + k1, 3 * K_dim + k2) = - lam_eq(i, j)*theta.D_MAT(k1, k2) - theta.r_D*theta.D_MAT(k1, k2) + lam_eq(i, j)*theta.OD_MAT(k1, k2)
                                                             + theta.gamma_L*theta.L_MAT(k1, k2) - theta.mu_H*theta.D_MAT(k1, k2) - r_age[i]*theta.D_MAT(k1, k2);

            MM(4 * K_dim + k1, 0 * K_dim + k2) = + lam_eq(i, j)*phi_LM_eq(i, j)[k2]*phi_D_eq(i, j)[k2]*theta.treat_BScover*theta.treat_BSeff*theta.OD_MAT(k1, k2) 
                                                             + theta.ff*phi_LM_eq(i, j)[k2]*phi_D_eq(i, j)[k2]*theta.treat_BScover*theta.treat_BSeff*theta.K_MAT(k1, k2);
            MM(4 * K_dim + k1, 1 * K_dim + k2) = + lam_eq(i, j)*phi_LM_eq(i, j)[k2]*phi_D_eq(i, j)[k2]*theta.treat_BScover*theta.treat_BSeff*theta.OD_MAT(k1, k2) 
                                                             + theta.ff*phi_LM_eq(i, j)[k2]*phi_D_eq(i, j)[k2]*theta.treat_BScover*theta.treat_BSeff*theta.K_MAT(k1, k2);
            MM(4 * K_dim + k1, 2 * K_dim + k2) = + lam_eq(i, j)*phi_D_eq(i, j)[k2]*theta.treat_BScover*theta.treat_BSeff*theta.OD_MAT(k1, k2) + theta.ff*phi_D_eq(i, j)[k2]*theta.treat_BScover*theta.treat_BSeff*theta.K_MAT(k1, k2);
            MM(4 * K_dim + k1, 4 * K_dim + k2) = - lam_eq(i, j)*theta.D_MAT(k1, k2) - theta.r_T*theta.D_MAT(k1, k2) + lam_eq(i, j) * theta.OD_MAT(k1, k2)
                                                             + theta.gamma_L*theta.L_MAT(k1, k2) - theta.mu_H*theta.D_MAT(k1, k2) - r_age[i] * theta.D_MAT(k1, k2);

            MM(5 * K_dim + k1, 4 * K_dim + k2) = + theta.r_T*theta.D_MAT(k1, k2);
            MM(5 * K_dim + k1, 5 * K_dim + k2) = - lam_eq(i, j)*theta.D_MAT(k1, k2) - theta.r_P*theta.D_MAT(k1, k2) + lam_eq(i, j) * theta.OD_MAT(k1, k2)
                                                             + theta.gamma_L*theta.L_MAT(k1, k2) - theta.mu_H*theta.D_MAT(k1, k2) - r_age[i] * theta.D_MAT(k1, k2);
        }
    }

}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//       //                                                 //
//  3.6  //  Gauss-Hermite weights for Gaussian quadrature  //
//       //  integration with Normal distribution.          //
//       //  Code is adapted from gauher.cpp from NR3       // 
//       //                                                 //
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void Population::gauher(Params& theta)
{
    Array<double, N_het, 1> x;
    Array<double, N_het, 1> w;

    /////////////////////////////
    // PART 1

    const double EPS = 1.0e-14, PIM4 = 0.7511255444649425;
    const int MAXIT = 10;
    int i, its, j, m;
    double p1, p2, p3, pp, z, z1;

    m = (N_het + 1) / 2;
    for (i = 0; i<m; i++) {
        if (i == 0) {
            z = sqrt(double(2 * N_het + 1)) - 1.85575*pow(double(2 * N_het + 1), -0.16667);
        }
        else if (i == 1) {
            z -= 1.14*pow(double(N_het), 0.426) / z;
        }
        else if (i == 2) {
            z = 1.86*z - 0.86*x[0];
        }
        else if (i == 3) {
            z = 1.91*z - 0.91*x[1];
        }
        else {
            z = 2.0*z - x[i - 2];
        }
        for (its = 0; its<MAXIT; its++) {
            p1 = PIM4;
            p2 = 0.0;
            for (j = 0; j<N_het; j++) {
                p3 = p2;
                p2 = p1;
                p1 = z*sqrt(2.0 / (j + 1))*p2 - sqrt(double(j) / (j + 1))*p3;
            }
            pp = sqrt(double(2 * N_het))*p2;
            z1 = z;
            z = z1 - p1 / pp;
            if (fabs(z - z1) <= EPS) break;
        }
        if (its >= MAXIT) throw("too many iterations in gauher");
        x[i] = z;
        x[N_het - 1 - i] = -z;
        w[i] = 2.0 / (pp*pp);
        w[N_het - 1 - i] = w[i];
    }


    /////////////////////////////
    // PART 2

    double w_sum = w.sum();

    ////////////////////////////////
    // Note that N_het-1-j here instead of j to ensure x_het increasing

    for (int j = 0; j<N_het; j++) {
        x_het[j] = exp(theta.sig_het*x[N_het - 1 - j] * sqrt(2.0)
                - 0.5*theta.sig_het*theta.sig_het);
    }
    w_het = w / w_sum;


    ////////////////////////////
    // temporary for N_het = 1

    if (N_het == 1) {
        x_het[0] = 1.0;
        w_het[0] = 1.0;
    }

    x_age_het = age_bite.matrix() * x_het.transpose().matrix();
    w_age_het = age_demog.matrix() * w_het.transpose().matrix();

    ////////////////////////////////
    // Boundaries of heterogeneity compartments

    x_het_bounds[0] = 0.0;

    for (int j = 1; j<N_het; j++) {
        x_het_bounds[j] = exp(0.5*(log(x_het[j - 1]) + log(x_het[j])));
    }

    x_het_bounds[N_het] = theta.het_max;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//       //                                                                   // 
//  3.7  //  Initialise a population of individuals at equilibrium            //
//       //  This calculates the non-seasonal equilibrium solution from a     //
//       //  comparable deterministic model. This will give an approximately  // 
//       //  correct solution for a seasonal setting.                         //           
//       //                                                                   //
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Population::equi_pop_setup(Params& theta)
{
    size_t K_dim = K_max + 1;
    
    //////////////////////////////////////////////////////
    // 3.7.1. Set up age and heterogeneity compartments

    ////////////////////////////////////////
    // 3.7.1.1. Bounds of age bins

    // double age_bounds[N_age+1] = {0.0*365.0, 20.0*365.0, 40.0*365.0, 60.0*365.0, 80.0*365.0};

    double age_bounds[N_age + 1] = { 0.0*365.0, 0.2*365.0, 0.4*365.0, 0.6*365.0, 0.8*365.0, 1.0*365.0,
                                     1.2*365.0, 1.4*365.0, 1.6*365.0, 1.8*365.0, 2.0*365.0,
                                     2.2*365.0, 2.4*365.0, 2.6*365.0, 2.8*365.0, 3.0*365.0,
                                     3.4*365.0, 3.8*365.0, 4.2*365.0, 4.6*365.0, 5.0*365.0,
                                     5.5*365.0, 6.0*365.0, 6.5*365.0, 7.0*365.0, 7.5*365.0, 8.0*365.0, 8.5*365.0, 9.0*365.0, 9.5*365.0, 10.0*365.0,
                                     11.0*365.0, 12.0*365.0, 13.0*365.0, 14.0*365.0, 15.0*365.0, 16.0*365.0, 17.0*365.0, 18.0*365.0, 19.0*365.0, 20.0*365.0,
                                     22.0*365.0, 24.0*365.0, 26.0*365.0, 28.0*365.0, 30.0*365.0, 32.0*365.0, 34.0*365.0, 36.0*365.0, 38.0*365.0, 40.0*365.0,
                                     45.0*365.0, 50.0*365.0, 55.0*365.0, 60.0*365.0, 65.0*365.0, 70.0*365.0, 75.0*365.0, 80.0*365.0 };


    ////////////////////////////////////////////
    // 3.7.1.2 Proportion in each age bin

    for (int i = 0; i < N_age - 1; i++)
    {
        age_demog[i] = exp(-theta.mu_H*age_bounds[i]) - exp(-theta.mu_H*age_bounds[i + 1]);
    }

    age_demog[N_age - 1] = 1.0 - age_demog.head(N_age - 1).sum();


    ////////////////////////////////////////
    // 3.7.1.3. Ageing rates - formula below ensures
    //          balanced demography

    r_age[0] = theta.mu_H*(1.0 - age_demog[0]) / age_demog[0];

    for (int i = 1; i<(N_age - 1); i++)
    {
        r_age[i] = (r_age[i - 1] * age_demog[i - 1] - theta.mu_H*age_demog[i]) / age_demog[i];
    }

    r_age[N_age - 1] = 0.0;


    ////////////////////////////////////////
    // 3.7.1.4. Age-dependent mosquito biting rates

    for (int i = 0; i<N_age; i++)
    {
        age_mids[i] = 0.5*(age_bounds[i] + age_bounds[i + 1]);
    }

    age_bite = 1.0 - theta.rho_age*exp(-age_mids / theta.age_0);

    P_age_bite = exp(-t_step / theta.age_0);


    ///////////////////////////////////////
    // Ensure total bites are normalised

    double omega_age = 1.0 / (age_demog * age_bite).sum();
    age_bite *= omega_age;


    //////////////////////////////////////////////////////
    // 3.7.1.5. Find age category closest to 20 yr old woman

    index_age_20 = 0;

    double age_diff = numeric_limits<double>::infinity();

    for (int i = 0; i<N_age; i++)
    {
        auto diff = age_mids[i] - 20.0*365.0;
        diff = diff * diff;
        if (diff < age_diff) {
            age_diff = diff;
            index_age_20 = i;
        }
    }


    ////////////////////////////////////////
    // 3.7.1.6. Heterogeneity in expsoure and age demographics.
    //          Weights for heterogeneity calculate using Gaussian-Hemite quadrature.
    //          The function also fills out the N_age*N_het matrix

    gauher(theta);


    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // 3.7.2. Calculate equilibrium of model in humans

    //////////////////////////////////////////////////////////////
    // 3.7.2.1. Object for storing equilibrium solution
    // Note: we use Dynamic only because these arrays are too large for the stack!
    
    Array<Array<double, K_max + 1, N_H_comp>, Dynamic, N_het> yH_eq;
    yH_eq.resize(N_age, N_het);
    
    Array<Array<double, K_max + 1, 1>, Dynamic, N_het> A_par_eq;
    A_par_eq.resize(N_age, N_het);
    
    Array<Array<double, K_max + 1, 1>, Dynamic, N_het> A_clin_eq;
    A_clin_eq.resize(N_age, N_het);
    Array<double, N_age, N_het> A_clin_eq_mean;

    Array<Array<double, K_max + 1, 1>, Dynamic, N_het> phi_LM_eq;
    phi_LM_eq.resize(N_age, N_het);
    Array<Array<double, K_max + 1, 1>, Dynamic, N_het> phi_D_eq;
    phi_D_eq.resize(N_age, N_het);
    Array<Array<double, K_max + 1, 1>, Dynamic, N_het> r_PCR_eq;
    r_PCR_eq.resize(N_age, N_het);

    size_t dim = N_H_comp * K_dim;
    MatrixXd MM(dim, dim);
    VectorXd bb(dim);
    VectorXd xx(dim);


    //////////////////////////////////////////////////////////////
    // 3.7.2.2. Equilibrium force of infection
    //
    //   Only the contribution from mosquito bites

    Array<double, N_age, N_het> lam_eq = theta.EIR_equil * theta.bb * x_age_het;


    //////////////////////////////////////////////////////////////
    // 3.7.2.5. Equilibrium number of batches of relapses

    Array<Array<double, K_max + 1, 1>, Dynamic, N_het> HH_eq;
    HH_eq.resize(N_age, N_het);

    Matrix<double, K_max + 1, K_max + 1> HH_mat;
    Matrix<double, K_max + 1, 1> HH_xx;


    for (int j = 0; j < N_het; j++)
    {
        for (int i = 0; i < N_age; i++)
        {
            Matrix<double, K_max + 1, 1> HH_bb;
            if (i == 0) {
                HH_bb.setZero();
                HH_bb[0] = w_het[j] * theta.mu_H;
            } else {
                double nr = -r_age[i - 1];
                HH_bb = nr * HH_xx;
            }

            HH_mat = lam_eq(i, j) * theta.H_MAT
                    + theta.gamma_L * theta.L_MAT
                    - theta.mu_H * theta.D_MAT
                    - r_age[i] * theta.D_MAT;

            HH_xx = HH_mat.inverse() * HH_bb;

            HH_eq(i, j) = -HH_xx;
        }
    }


    //////////////////////////////////////////////////////////////
    // 3.7.2.7. Equilibrium levels of immunity

    Matrix<double, K_max + 1, 1> ODE_eq_vec;
    Matrix<double, K_max+1, K_max+1> ODE_eq_MAT;

    Matrix<double, K_max+1, K_max+1> LAM_MAT(K_dim, K_dim);
    Matrix<double, K_max+1, K_max+1> GAM_MAT(K_dim, K_dim);


    //////////////////////////////////////////////////////////////
    // 3.7.2.7.1. ANTI-PARASITE IMMUNITY

    for (int j = 0; j<N_het; j++)
    {
        for (int i = 0; i < N_age; i++)
        {
            /////////////////////////////
            // Vector part of ODE

            Array<double, K_max + 1, 1> G_VEC;
            G_VEC[0] = 0.0;
            for (size_t k = 1; k < K_dim; k++) {
                G_VEC[k] = lam_eq(i, j) * HH_eq(i, j)[k - 1] / HH_eq(i, j)[k] + theta.ff*((double)k);
            }
            G_VEC[K_max] += lam_eq(i, j);

            G_VEC = G_VEC / (G_VEC * theta.u_par + 1.0);

            if (i > 0) {
                G_VEC += r_age[i - 1] * A_par_eq(i - 1, j) * HH_eq(i - 1, j) / HH_eq(i, j);
            }
            ODE_eq_vec = G_VEC;


            /////////////////////////////
            // Matrix part of ODE

            LAM_MAT.setZero();
            GAM_MAT.setZero();

            for (int k = 0; k<K_max; k++) {
                LAM_MAT(k, k) = -1.0;
                LAM_MAT(k + 1, k) = HH_eq(i, j)[k] / HH_eq(i, j)[k + 1];
            }

            for (size_t k = 1; k < K_dim; k++) {
                GAM_MAT(k, k) = -((double)k);
                GAM_MAT(k - 1, k) = ((double)k)*HH_eq(i, j)[k] / HH_eq(i, j)[k - 1];
            }

            ODE_eq_MAT = -(lam_eq(i, j) * LAM_MAT
                    + theta.gamma_L * GAM_MAT
                    - theta.r_par * theta.D_MAT
                    - theta.mu_H * theta.D_MAT
                    - r_age[i] * theta.D_MAT);


            /////////////////////////////
            // Solve matrix equation

            A_par_eq(i, j) = (ODE_eq_MAT.inverse() * ODE_eq_vec);
        }
    }


    //////////////////////////////////////////////////////////////
    // 3.7.2.7.2. MEAN ANTI-PARASITE IMMUNITY

    Array<double, N_age, N_het> A_par_eq_mean;
    
    for (int i = 0; i < N_age; i++)
    {
        for (int j = 0; j < N_het; j++)
        {
            /////////////////////////////
            // Set up weights and rates

            Array<double, K_max + 1, 1> w_HH = HH_eq(i, j);
            double HH_denom = w_HH.sum();
            w_HH /= HH_denom;

            /////////////////////////////
            // Average level of immunity

            A_par_eq_mean(i, j) = (A_par_eq(i, j) * w_HH).sum();
        }
    }


    //////////////////////////////////////////////////////////////
    // 3.7.2.7.3. ANTI-CLINICAL IMMUNITY

    for (int j = 0; j<N_het; j++)
    {
        for (int i = 0; i < N_age; i++)
        {
            /////////////////////////////
            // Vector part of ODE

            Array<double, K_max + 1, 1> G_VEC;
            G_VEC[0] = 0.0;
            for (size_t k = 1; k < K_dim; k++) {
                G_VEC[k] = lam_eq(i, j) * HH_eq(i, j)[k - 1] / HH_eq(i, j)[k] + theta.ff*((double)k);
            }
            G_VEC[K_max] += lam_eq(i, j);

            G_VEC = G_VEC / (G_VEC * theta.u_clin + 1.0);

            if (i > 0) {
                G_VEC += r_age[i - 1] * A_clin_eq(i - 1, j) * HH_eq(i - 1, j) / HH_eq(i, j);
            }
            ODE_eq_vec = G_VEC;


            /////////////////////////////
            // Matrix part of ODE

            LAM_MAT.setZero();
            GAM_MAT.setZero();

            for (int k = 0; k<K_max; k++) {
                LAM_MAT(k, k) = -1.0;
                LAM_MAT(k + 1, k) = HH_eq(i, j)[k] / HH_eq(i, j)[k + 1];
            }

            for (size_t k = 1; k < K_dim; k++) {
                GAM_MAT(k, k) = -((double)k);
                GAM_MAT(k - 1, k) = ((double)k)*HH_eq(i, j)[k] / HH_eq(i, j)[k - 1];
            }

            ODE_eq_MAT = -(lam_eq(i, j) * LAM_MAT
                    + theta.gamma_L * GAM_MAT
                    - theta.r_clin * theta.D_MAT
                    - theta.mu_H * theta.D_MAT
                    - r_age[i] * theta.D_MAT);


            /////////////////////////////
            // Solve matrix equation

            A_clin_eq(i, j) = ODE_eq_MAT.inverse() * ODE_eq_vec;
        }
    }


    //////////////////////////////////////////////////////////////
    // 3.7.2.7.4. MEAN ANTI-CLINICAL IMMUNITY

    for (int i = 0; i < N_age; i++)
    {
        for (int j = 0; j < N_het; j++)
        {
            /////////////////////////////
            // Set up weights and rates

            Array<double, K_max + 1, 1> w_HH = HH_eq(i, j);
            double HH_denom = w_HH.sum();
            w_HH /= HH_denom;


            /////////////////////////////
            // Average level of immunity

            A_clin_eq_mean(i, j) = 0.0;

            for (size_t k = 0; k < K_dim; k++)
            {
                A_clin_eq_mean(i, j) += A_clin_eq(i, j)[k] * w_HH[k];
            }
        }
    }


    //////////////////////////////////////////////////////////////
    // 3.7.2.7.5. ADD IN MATERNAL IMMUNITY

    for (int j = 0; j < N_het; j++)
    {
        for (int i = 0; i < N_age; i++)
        {
            for (size_t k = 0; k < K_dim; k++)
            {
                A_par_eq(i, j)[k] += A_par_eq_mean(index_age_20, j) * theta.P_mat*exp(-age_mids[i] / theta.d_mat);
                A_clin_eq(i, j)[k] += A_clin_eq_mean(index_age_20, j) * theta.P_mat*exp(-age_mids[i] / theta.d_mat);
            }
        }
    }


    //////////////////////////////////////////////////////////////
    // 3.7.2.7.6. EFFECTS OF IMMUNITY

    for (int j = 0; j < N_het; j++)
    {
        for (int i = 0; i < N_age; i++)
        {
            for (size_t k = 0; k < K_dim; k++)
            {
                phi_LM_eq(i, j)[k] = theta.phi_LM_min + (theta.phi_LM_max - theta.phi_LM_min) / (1.0 + pow(A_par_eq(i, j)[k] / theta.A_LM_50pc, theta.K_LM));
                phi_D_eq(i, j)[k] = theta.phi_D_min + (theta.phi_D_max - theta.phi_D_min) / (1.0 + pow(A_clin_eq(i, j)[k] / theta.A_D_50pc, theta.K_D));
                r_PCR_eq(i, j)[k] = 1.0 / (theta.d_PCR_min + (theta.d_PCR_max - theta.d_PCR_min) / (1.0 + pow(A_par_eq(i, j)[k] / theta.A_PCR_50pc, theta.K_PCR)));
            }
        }
    }


    ///////////////////////////////////////////////
    // 3.7.2.8.. Equilibrium states

    for (int j = 0; j<N_het; j++)
    {
        ///////////////////////////////////////////////
        // 3.7.2.8.1. Youngest age category

        MM_ij(0, j, theta, r_age, MM,
            lam_eq, phi_LM_eq, phi_D_eq, r_PCR_eq);

        for (size_t k = 0; k < N_H_comp * K_dim; k++)
        {
            bb[k] = 0.0;
        }

        bb[0] = -w_het[j] * theta.mu_H;

        xx = MM.inverse() * bb;


        /////////////////////////////////////
        // Fill out equilibrium levels

        for (int c = 0; c < N_H_comp; c++)
        {
            for (size_t k = 0; k < K_dim; k++)
            {
                yH_eq(0, j)(k, c) = xx[c*K_dim + k];
            }
        }


        ///////////////////////////////////////////////
        // 3.7.2.8.2. Older age categories

        for (int i = 1; i<N_age; i++)
        {
            MM_ij(i, j, theta, r_age, MM,
                lam_eq, phi_LM_eq, phi_D_eq, r_PCR_eq);

            for (size_t c = 0; c < N_H_comp * K_dim; c++)
            {
                bb[c] = -r_age[i - 1] * xx[c];
            }

            xx = MM.inverse() * bb;


            /////////////////////////////////////
            // Fill out equilibrium levels

            for (int c = 0; c < N_H_comp; c++)
            {
                for (size_t k = 0; k < K_dim; k++)
                {
                    yH_eq(i, j)(k, c) = xx[c*K_dim + k];
                }
            }

        }
    }

    /*

    cout << "Testing yH sum......" << endl;

    double yH_sum = 0.0;

    for (int i = 0; i < N_age; i++)
    {
    for (int j = 0; j < N_het; j++)
    {
    for (size_t k = 0; k < K_dim; k++)
    {
    for (int c = 0; c < N_H_comp; c++)
    {
    yH_sum += yH_eq(i, j)(k, c);
    }
    }
    }
    }


    cout << "yH_sum = " << yH_sum << endl;
    system("PAUSE");

    */

    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // 3.7.3. Calculate equilibrium of model in mosquitoes

    for (int g = 0; g < N_spec; g++)
    {
        theta.lam_M[g] = 0.0;

        for (int i = 0; i<N_age; i++)
        {
            for (int j = 0; j<N_het; j++)
            {
                for (size_t k = 0; k < K_dim; k++)
                {
                    theta.lam_M[g] += x_age_het(i, j) * theta.aa[g] * (
                            theta.c_PCR*yH_eq(i, j)(k, 1) +
                            theta.c_LM*yH_eq(i, j)(k, 2) +
                            theta.c_D*yH_eq(i, j)(k, 3) +
                            theta.c_T*yH_eq(i, j)(k, 4)
                        );
                }
            }
        }
    }


    Array<double, N_spec, 1> I_M_star = theta.lam_M
            * exp(-theta.mu_M * theta.tau_M)
            / (theta.lam_M + theta.mu_M);

    double a_I_M_sum;

    if (theta.Prop_mosq[0] > 0.0)
    {
        a_I_M_sum = theta.aa[0] * I_M_star[0];

        for (int g = 1; g < N_spec; g++)
        {
            a_I_M_sum += (theta.Prop_mosq[g] / theta.Prop_mosq[0])*theta.aa[g] * I_M_star[g];
        }

        theta.mm_0[0] = theta.EIR_equil / a_I_M_sum;

        for (int g = 1; g < N_spec; g++)
        {
            theta.mm_0[g] = (theta.Prop_mosq[g] / theta.Prop_mosq[0])*theta.mm_0[0];
        }
    }
    else if (theta.Prop_mosq[1] > 0.0)
    {
        theta.mm_0[0] = 0.0;

        a_I_M_sum = theta.aa[1] * I_M_star[1];

        for (int g = 2; g < N_spec; g++)
        {
            a_I_M_sum += (theta.Prop_mosq[g] / theta.Prop_mosq[1])*theta.aa[g] * I_M_star[g];
        }

        theta.mm_0[1] = theta.EIR_equil / a_I_M_sum;

        for (int g = 2; g < N_spec; g++)
        {
            theta.mm_0[g] = (theta.Prop_mosq[g] / theta.Prop_mosq[1])*theta.mm_0[1];
        }
    }
    else
    {
        theta.mm_0[0] = 0.0;
        theta.mm_0[1] = 0.0;
        theta.mm_0[2] = theta.EIR_equil / (theta.aa[2] * I_M_star[2]);

        //theta.mm_0[2] = theta.EIR_equil / (theta.aa[2]*(theta.lam_M[2] / (theta.lam_M[2] + theta.mu_M[2]))*exp(-theta.mu_M[2]*theta.tau_M[2]));
    }


    yM.col(0) = 2.0 * theta.omega_larvae * theta.mu_M * theta.d_L_larvae
            * (1.0 + theta.d_pupae * theta.mu_P) * theta.mm_0;
    yM.col(1) = 2.0 * theta.mu_M * theta.d_L_larvae
            * (1.0 + theta.d_pupae * theta.mu_P) * theta.mm_0;
    yM.col(2) = 2.0 * theta.d_pupae * theta.mu_M * theta.mm_0;
    yM.col(3) = theta.mm_0 * (theta.mu_M / (theta.lam_M + theta.mu_M));
    yM.col(4) = theta.mm_0 * (theta.lam_M / (theta.lam_M + theta.mu_M))
            * (1.0 - exp(-theta.mu_M * theta.tau_M));
    yM.col(5) = theta.mm_0 * (theta.lam_M / (theta.lam_M + theta.mu_M))
            * exp(-theta.mu_M * theta.tau_M);

    // Larval carry capacity
    theta.Karry = theta.mm_0 * 2.0 * theta.d_L_larvae * theta.mu_M
            * (1.0 + theta.d_pupae * theta.mu_P) * theta.gamma_larvae * (theta.omega_larvae + 1.0)
            / (theta.omega_larvae / (theta.mu_L0 * theta.d_E_larvae) - 1.0 / (theta.mu_L0 * theta.d_L_larvae) - 1.0);

    theta.Karry = theta.Karry.max(1.0e-10);


    for (int g = 0; g < N_spec; g++)
    {
        if (g == 0) { cout << "An. farauti:  " << 100.0 * theta.Prop_mosq[0] << "%" << endl; }
        if (g == 1) { cout << "An. punctulatus:  " << 100.0 * theta.Prop_mosq[1] << "%" << endl; }
        if (g == 2) { cout << "An. koliensis:  " << 100.0 * theta.Prop_mosq[2] << "%" << endl; }

        cout << "EL_M  " << yM(g, 0) << endl;
        cout << "LL_M  " << yM(g, 1) << endl;
        cout << "P_M  " << yM(g, 2) << endl;
        cout << "S_M  " << yM(g, 3) << endl;
        cout << "E_M  " << yM(g, 4) << endl;
        cout << "I_M  " << yM(g, 5) << endl;

        cout << "lam_M = " << theta.lam_M[g] << endl;

        cout << "I_M = " << yM(g, 5) << endl;

        cout << "mm = " << theta.mm_0[g] << endl;

        cout << endl;
    }
    cout << endl;

    cout << "lam_H = " << theta.bb*theta.EIR_equil << endl;

    double EIR_out = 365.0 * (theta.aa * yM.col(5)).sum();

    cout << "EIR = " << EIR_out << endl;


    //////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////
    // 3.7.4. Proportion in each age and heterogeneity stratified category

    //////////////////////////////////////////
    // Fill out vector of lagged lam_M*S_M

    theta.lam_S_M_track.resize(N_spec, theta.M_track);
    theta.lam_S_M_track.colwise() = theta.lam_M * yM.col(3);


    //////////////////////////////////////////////////////////////
    // 3.7.3. Calculate probability for each compartment 

    vector<vector<vector<double>>> yH_eq_cumsum;
    yH_eq_cumsum.resize(N_age);
    for (int i = 0; i<N_age; i++)
    {
        yH_eq_cumsum[i].resize(N_het);
        for (int j = 0; j<N_het; j++)
        {
            yH_eq_cumsum[i][j].resize(N_H_comp*K_dim);
        }
    }


    for (int i = 0; i<N_age; i++)
    {
        for (int j = 0; j<N_het; j++)
        {
            yH_eq_cumsum[i][j][0] = yH_eq(i, j)(0, 0);

            for (size_t k = 1; k < K_dim; k++)
            {
                yH_eq_cumsum[i][j][k] = yH_eq_cumsum[i][j][k - 1] + yH_eq(i, j)(k, 0);
            }

            for (int c = 1; c < N_H_comp; c++)
            {
                yH_eq_cumsum[i][j][c*K_dim] = yH_eq_cumsum[i][j][c*K_dim - 1] + yH_eq(i, j)(0, c);

                for (size_t k = 1; k < K_dim; k++)
                {
                    yH_eq_cumsum[i][j][c*K_dim + k] = yH_eq_cumsum[i][j][c*K_dim + k - 1] + yH_eq(i, j)(k, c);
                }
            }
        }
    }


    for (int i = 0; i < N_age; i++)
    {
        for (int j = 0; j < N_het; j++)
        {
            for (size_t k = 0; k < N_H_comp * K_dim; k++)
            {
                yH_eq_cumsum[i][j][k] /= yH_eq_cumsum[i][j][N_H_comp*K_dim - 1];
            }
        }
    }


    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    // 3.7.4. Initialise Population of individuals                           

    ///////////////////////////////////////////////////////////////////////////
    // 3.7.4.1. Temporary objects for setting up individuals            

    double age_start, zeta_start;

    int i_index, j_index;


    ///////////////////////////////////////////////////////////////////////////
    // 3.7.4.2 Loop through and create N_pop individuals            

    for (int n = 0; n<N_pop; n++)
    {
        //////////////////////////////////////////////////////////////////
        // 3.7.4.2.1. Assign age and heterogeneity 

        do
        {
            age_start = util::random::exponential(theta.age_mean);
        }
        while (age_start > theta.age_max);


        do
        {
            zeta_start = util::random::log_normal(-0.5*theta.sig_het*theta.sig_het, theta.sig_het);
        }
        while (zeta_start > theta.het_max);


        //////////////////////////////////////////////////////////////////
        // 3.7.4.2.2. Find appropriate age and het compartment 

        i_index = 0;

        for (int i = 0; i<N_age; i++)
        {
            if ((age_start > age_bounds[i]) && (age_start <= age_bounds[i + 1]))
            {
                i_index = i;
            }
        }


        j_index = 0;

        for (int j = 0; j<N_het; j++)
        {
            if ((zeta_start > x_het_bounds[j]) && (zeta_start < x_het_bounds[j + 1]))
            {
                j_index = j;
            }
        }

        //////////////////////////////////////////////////////////////////
        // 3.7.4.2.3. Construct a new individual

        Individual HH(theta, age_start, zeta_start);

        ///////////////////////////////////////////
        //  3.7.4.2.4. An indicator for pregnancy appropriate age
        //             Only women between ages 18 and 40 can be pregnant.

        if (HH.gender == Gender::Female)
        {
            if ((HH.age > 6570.0) && (HH.age < 14600.0))
            {
                HH.preg_age = true;
            }
        }


        ///////////////////////////////////////////////////////////////////
        // Randomly assign a state according to equilibrium probabilities

        double rand_comp = util::random::uniform_01();

        if (rand_comp <= yH_eq_cumsum[i_index][j_index][0])
        {
            HH.S = 1;
            HH.Hyp = 0;
        }

        for (size_t k = 1; k < K_dim; k++)
        {
            if ((rand_comp > yH_eq_cumsum[i_index][j_index][k - 1]) && (rand_comp <= yH_eq_cumsum[i_index][j_index][k]))
            {
                HH.S = 1;
                HH.Hyp = k;
            }
        }

        for (int c = 1; c < N_H_comp; c++)
        {
            for (size_t k = 0; k < K_dim; k++)
            {
                if ((rand_comp > yH_eq_cumsum[i_index][j_index][c*K_dim + k - 1]) && (rand_comp <= yH_eq_cumsum[i_index][j_index][c*K_dim + k]))
                {
                    if (c == 1) { HH.I_PCR = 1; }
                    if (c == 2) { HH.I_LM = 1; }
                    if (c == 3) { HH.I_D = 1; }
                    if (c == 4) { HH.T = 1; }
                    if (c == 5) { HH.P = 1; }

                    HH.Hyp = k;
                }
            }
        }

        if (rand_comp > yH_eq_cumsum[i_index][j_index][N_H_comp*K_dim - 1])
        {
            HH.P = 1;
            HH.Hyp = K_max;
        }


        ////////////////////////////////////////////
        //  3.7.4.2.5. Initialise immunity

        HH.A_par_mat = A_par_eq_mean(index_age_20, j_index) * theta.P_mat*exp(-age_mids[i_index] / theta.d_mat);
        HH.A_clin_mat = A_clin_eq_mean(index_age_20, j_index) * theta.P_mat*exp(-age_mids[i_index] / theta.d_mat);

        HH.A_par = A_par_eq(i_index, j_index)[HH.Hyp] - HH.A_par_mat;
        HH.A_clin = A_clin_eq(i_index, j_index)[HH.Hyp] - HH.A_clin_mat;


        HH.A_par_boost = 1;
        HH.A_clin_boost = 1;


        //////////////////////////////////////////////////
        // TO DO: set this up in equilibrium

        if ((HH.age > 6570.0) && (HH.age < 14600.0))
        {
            if (util::random::bernoulli(0.05))
            {
                HH.pregnant = true;
                HH.preg_timer = 0;
            }
        }


        ////////////////////////////////////////////
        //  3.7.4.2.6. A vector for storing lagged force of infection

        for (int k = 0; k<theta.H_track; k++)
        {
            HH.lam_bite_track[k] = lam_eq(i_index, j_index);
            HH.lam_rel_track[k] = HH.Hyp*theta.ff;
        }


        ///////////////////////////////////////////////////
        ////////////////////////////////////////////////////
        // 3.7.4.3.. Add this person to the vector of people

        people.push_back(std::move(HH));
    }

    ///////////////////////////////////////////////////////////
    // 3.7.5.1. Proportion of bites received by each person

    for (int n = 0; n<N_pop; n++)
    {
        for (int g = 0; g < N_spec; g++)
        {
            pi_n(n, g) = people[n].zeta_het*(1.0 - theta.rho_age*exp(-people[n].age*theta.age_0_inv));
        }
    }



    double SIGMA_PI[N_spec];

    for (int g = 0; g < N_spec; g++)
    {
        SIGMA_PI[g] = 0.0;
        for (int n = 0; n<N_pop; n++)
        {
            SIGMA_PI[g] += pi_n(n, g);
        }

        for (int n = 0; n<N_pop; n++)
        {
            pi_n(n, g) /= SIGMA_PI[g];
        }
    }

    ///////////////////////////////////////////////////////////
    // 3.7.5. Initialise population level quantitites

    //////////////////////////////////////////////
    // 3.7.5.1. Vector control quantities

    for (int g = 0; g < N_spec; g++)
    {
        SUM_pi_w[g] = 0;
        SUM_pi_z[g] = 0;


        for (int n = 0; n < N_pop; n++)
        {
            SUM_pi_w[g] += pi_n(n, g) * people[n].w_VC[g];
            SUM_pi_z[g] += pi_n(n, g) * people[n].z_VC[g];
        }
    }


    for (int g = 0; g < N_spec; g++)
    {
        W_VC[g] = 1.0 - theta.Q_0[g] + theta.Q_0[g] * SUM_pi_w[g];
        Z_VC[g] = theta.Q_0[g] * SUM_pi_z[g];

        delta_1_VC[g] = theta.delta_1 / (1.0 - Z_VC[g]);
        delta_VC[g] = delta_1_VC[g] + theta.delta_2;

        p_1_VC[g] = exp(-theta.mu_M[g] * delta_1_VC[g]);
        mu_M_VC[g] = -log(p_1_VC[g] * theta.p_2[g]) / delta_VC[g];

        Q_VC[g] = 1.0 - (1.0 - theta.Q_0[g]) / W_VC[g];

        aa_VC[g] = Q_VC[g] / delta_VC[g];

        exp_muM_tauM_VC[g] = exp(-mu_M_VC[g] * theta.tau_M[g]);
        beta_VC[g] = theta.eps_max[g] * mu_M_VC[g] / (exp(delta_VC[g] * mu_M_VC[g]) - 1.0);
    }


    /////////////////////////////////////////////////////////////////////////
    // 3.7.5.2. The rate at which person n is bitten by a single mosquito


    for (int n = 0; n < N_pop; n++)
    {
        for (int g = 0; g < N_spec; g++)
        {
            lam_n(n, g) = theta.aa[g] * pi_n(n, g);
        }
    }


    /////////////////////////////////////////////////////////////////////////
    // 3.7.5.3. Output an overview of the initial set up

    cout << "Equilibrium set up......." << endl;

    double S_ind = 0.0, I_PCR_ind = 0.0, I_LM_ind = 0.0, I_D_ind = 0.0, T_ind = 0.0, P_ind = 0.0;
    double S_eqq = 0.0, I_PCR_eqq = 0.0, I_LM_eqq = 0.0, I_D_eqq = 0.0, T_eqq = 0.0, P_eqq = 0.0;

    for (int n = 0; n<N_pop; n++)
    {
        S_ind += people[n].S;
        I_PCR_ind += people[n].I_PCR;
        I_LM_ind += people[n].I_LM;
        I_D_ind += people[n].I_D;
        T_ind += people[n].T;
        P_ind += people[n].P;
    }


    for (int i = 0; i<N_age; i++)
    {
        for (int j = 0; j<N_het; j++)
        {
            for (size_t k = 0; k < K_dim; k++)
            {
                S_eqq += yH_eq(i, j)(k, 0);
                I_PCR_eqq += yH_eq(i, j)(k, 1);
                I_LM_eqq += yH_eq(i, j)(k, 2);
                I_D_eqq += yH_eq(i, j)(k, 3);
                T_eqq += yH_eq(i, j)(k, 4);
                P_eqq += yH_eq(i, j)(k, 5);
            }
        }
    }


    cout << "S = " << ((double)S_ind) / N_pop << "\t" << S_eqq << endl;
    cout << "I_PCR = " << ((double)I_PCR_ind) / N_pop << "\t" << I_PCR_eqq << endl;
    cout << "I_LM = " << ((double)I_LM_ind) / N_pop << "\t" << I_LM_eqq << endl;
    cout << "I_D = " << ((double)I_D_ind) / N_pop << "\t" << I_D_eqq << endl;
    cout << "T = " << ((double)T_ind) / N_pop << "\t" << T_eqq << endl;
    cout << "P = " << ((double)P_ind) / N_pop << "\t" << P_eqq << endl;

}

} }
