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

#include "white/Individual.hpp"
#include "util/random.h"

#include <cmath>
#include <limits>

namespace OM { namespace white {

using std::numeric_limits;


////////////////////////////////////////////////////////////
//                                                        //
//  Function declarations                                 //
//                                                        //
////////////////////////////////////////////////////////////

size_t CH_sample(double weights[], size_t num);


////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////// 
//          //                                                                    // 
//  ##      //  #### #    ## ####   #### ##   ## #### ####   ##  ##  ####  ##     //
//  ## ##   //   ##  ##   ## ## ##   ##  ##   ##  ##  ## ##  ##  ## ##  ## ##     //
//  ######  //   ##  ###  ## ##  ##  ##   ## ##   ##  ##  ## ##  ## ###### ##     //
//     ##   //   ##  ## #### ## ##   ##    ###    ##  ## ##  ##  ## ##  ## ##     //
//     ##   //  #### ##   ## ####   ####    #    #### ####    ####  ##  ## #####  //
//          //                                                                    //
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////
// 0.2.1. Class constructor
// 
// We initialise all class fields to reasonable values
Individual::Individual(Params& theta, double a, double zeta) :
    age(a), zeta_het(zeta),
    preg_age(false), pregnant(false), preg_timer(0.0),
    
    // Lagged exposure equals zero - they're not born yet!
    lam_bite_track(theta.H_track.inSteps(), 0.0),
    lam_rel_track(theta.H_track.inSteps(), 0.0),
    
    S(false), I_PCR(false), I_LM(false), I_D(false), T(false), P(false),
    Hyp(0),
    I_new(0),
    ACT_treat(false), PQ_treat(false),
    PQ_effective(false), PQ_overtreat(false), PQ_overtreat_9m(false),
    A_par(0.0), A_clin(0.0),
    A_par_mat(0.0), A_clin_mat(0.0),
    A_par_boost(false), A_clin_boost(false),

    LLIN(false), LLIN_age(numeric_limits<double>::quiet_NaN()),
    IRS(false), IRS_age(numeric_limits<double>::quiet_NaN()),
    T_last_BS(numeric_limits<double>::infinity())
{
    gender = util::random::bernoulli(0.5) ? Gender::Male : Gender::Female;

    if (gender == Gender::Male) {
        G6PD_deficient = util::random::bernoulli(theta.G6PD_prev);
    } else /* female */ {
        // p1 = P(homozygous deficiency)
        double p1 = theta.G6PD_prev * theta.G6PD_prev;
        // p2 = P(heterozygous deficiency)
        double p2 = 2 * theta.G6PD_prev * (1.0 - theta.G6PD_prev);
        // The model doesn't currently differentiate between these.
        G6PD_deficient = util::random::bernoulli(p1 + p2);
    }

    CYP2D6_low = util::random::bernoulli(theta.CYP2D6_prev);
    
    
    ///////////////////////////////////////////////////
    // Assign intervention access scores

    auto mvn_samples = theta.V_sampler();

    for (int k = 0; k<N_int; k++)
    {
        zz_int[k] = mvn_samples(k);
    }


    ///////////////////////////////////////////////////
    // Born with no interventions
    
    r_LLIN = numeric_limits<double>::quiet_NaN();
    d_LLIN = numeric_limits<double>::quiet_NaN();
    s_LLIN = numeric_limits<double>::quiet_NaN();
    
    r_IRS = numeric_limits<double>::quiet_NaN();
    d_IRS = numeric_limits<double>::quiet_NaN();
    s_IRS = numeric_limits<double>::quiet_NaN();
    
    w_VC = 1.0;
    y_VC = 1.0;
    z_VC = 0.0;

}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//       //                                                                               //
// 4.1.  //  THE MODEL (within humans at least!)                                          //
//       //  Stochastic moves between compartments for each individual                    //
///////////  for a fixed time step                                                        //
///////////                                                                               //
///////////  TO DO: (i) lots of small stuff to speed things up                            //
///////////         (ii) might be able to save on some multiplications by not normalising //
///////////             vector of probabilities and doing it instead inside CH_sample     //
///////////         (iii) add new state for MDA prophylaxis                               //
///////////                                                                               //
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void Individual::state_mover(Params& theta, double lam_bite)
{
    lam_bite_track.push_back(lam_bite);
    lam_bite_track.erase(lam_bite_track.begin());

    // Lagged force of infection due to moquito bites
    double lam_bite_lag = lam_bite_track[0];


    lam_rel_track.push_back(((double)Hyp)*theta.ff);
    lam_rel_track.erase(lam_rel_track.begin());

    // Lagged force of infection due to relapses
    double lam_rel_lag = lam_rel_track[0];

    // Lagged total force of infection
    double lam_H_lag = lam_bite_lag + lam_rel_lag;
    
    auto do_new_inf = [this, &theta](double lam_lag) {
        if (util::random::bernoulli(lam_lag))
        {
            if (PQ_proph < sim::ts0())
            {
                Hyp += 1;
            }
        }

        if (A_par_boost == 1)
        {
            A_par += 1.0;
            A_par_expiry = sim::ts0() + SimTime::fromDays(theta.u_par);
            A_par_boost = 0;
        }

        if (A_clin_boost == 1)
        {
            A_clin += 1.0;
            A_clin_expiry = sim::ts0() + SimTime::fromDays(theta.u_clin);
            A_clin_boost = 0;
        }
    };


    ///////////////////////////////////
    // Indicators for new events

    I_new = 0;
    ACT_treat = 0;
    PQ_treat = 0;

    PQ_effective    = 0;
    PQ_overtreat    = 0;
    PQ_overtreat_9m = 0;
    
    auto do_pq_admin = [this, &theta](bool is_PQ_effective) {
        /////////////////////////////////////////////////////////////////////
        // Is PQ administered?
        // For efficiency, the PQ related commands are only implemented if
        // PQ is available

        if( util::random::bernoulli(theta.PQ_treat_PQavail) ) {
            PQ_treat = 1;

            /////////////////////////////////////////////////////////////////////
            // Exclude PQ because of G6PD deficiency

            if ((theta.PQ_treat_G6PD_risk == 1) && is_G6PD_deficient())
            {
                PQ_treat = 0;
            }

            /////////////////////////////////////////////////////////////////////
            // Exclude PQ because of pregancy

            if ((theta.PQ_treat_preg_risk == 1) && pregnant)
            {
                PQ_treat = 0;
            }

            /////////////////////////////////////////////////////////////////////
            // Exclude PQ because of young age

            if (age < theta.PQ_treat_low_age)
            {
                PQ_treat = 0;
            }


            /////////////////////////////////////////////////////////////////////
            // Is PQ effective?

            PQ_effective = 0;

            if (util::random::bernoulli(theta.PQ_treat_PQeff))
            {
                PQ_effective = is_PQ_effective;
            }

            if ((theta.PQ_treat_CYP2D6_risk == 1) && has_low_cyp2d6_action())
            {
                PQ_effective = 0;
            }

            if ((PQ_treat == 1) && (PQ_effective == 1))
            {
                Hyp = 0;
                PQ_proph = max(PQ_proph, sim::ts0() + SimTime::oneTS());
            }
        }
    };


    //////////////////////////////////////////////////////////////////////
    //     //                                                           // 
    //  0  //  S: Susceptible                                           //
    //     //                                                           //
    //////////////////////////////////////////////////////////////////////

    if (S == 1)
    {
        double S_out = lam_H_lag;       // Probability of exiting state

        if (!util::random::bernoulli(exp(-t_step*S_out)))
        {
            //theta.r_PCR   = 1.0/( theta.d_PCR_min + (theta.d_PCR_max-theta.d_PCR_min)/( 1.0 + pow((A_par+A_par_mat)*theta.A_PCR_50pc_inv,theta.K_PCR) )); 
            theta.phi_LM = theta.phi_LM_min + (theta.phi_LM_max - theta.phi_LM_min) / (1.0 + pow((A_par + A_par_mat)*theta.A_LM_50pc_inv, theta.K_LM));
            theta.phi_D = theta.phi_D_min + (theta.phi_D_max - theta.phi_D_min) / (1.0 + pow((A_clin + A_clin_mat)*theta.A_D_50pc_inv, theta.K_D));

            // Sample outcome from probabilities of competing hazards
            double p = theta.phi_LM*theta.phi_D;
            double CH_prob[] = {
                    (1.0 - theta.phi_LM),               // Move to I_PCR
                    theta.phi_LM*(1.0 - theta.phi_D),   // Move to I_LM
                    p*(1.0 - theta.treat_BScover),      // Move to I_D
                    p*theta.treat_BScover,              // Move to T
            };
            size_t CH_move = CH_sample(CH_prob, 4);

            ////////////////////////////////
            // Behaviour common to all transitions
            
            do_new_inf(lam_bite_lag / lam_H_lag);
            
            T_last_BS = 0.0;
            
            if (CH_move == 0) {
                ////////////////////////////////
                // S -> I_PCR

                I_new = I_DETECT_PCR;

                S = 0;
                I_PCR = 1;
                
            } else if (CH_move == 1) {
                ////////////////////////////////
                // S -> I_LM

                I_new = I_DETECT_PCR | I_DETECT_LM;

                S = 0;
                I_LM = 1;
                
            } else if (CH_move == 2) {
                ////////////////////////////////
                // S -> I_D

                I_new = I_DETECT_PCR | I_DETECT_LM | I_DETECT_D;

                S = 0;
                I_D = 1;
                
            } else if (CH_move == 3) {
                ////////////////////////////////
                // S -> T

                /////////////////////////////////////////////////////////////////////
                // Blood-stage drug administered

                ACT_treat = 1;

                if( util::random::bernoulli(theta.treat_BSeff) )
                {
                    S = 0;
                    T = 1;
                }

                I_new = I_DETECT_PCR | I_DETECT_LM | I_DETECT_D;

                // FIXME: why is PQ treatment not effective in this case? Was this a bug?
                do_pq_admin(false);
            }
            
            return;
        }
    }


    //////////////////////////////////////////////////////////////////////
    //     //                                                           // 
    //  1  //  I_PCR: PCR detectable BS infections                      //
    //     //                                                           //
    //////////////////////////////////////////////////////////////////////

    if (I_PCR == 1)
    {
        theta.r_PCR = 1.0 / (theta.d_PCR_min + (theta.d_PCR_max - theta.d_PCR_min) / (1.0 + pow((A_par + A_par_mat)*theta.A_PCR_50pc_inv, theta.K_PCR)));

        double I_PCR_out = lam_H_lag + theta.r_PCR;        // Probability of exiting state

        if (!util::random::bernoulli(exp(-t_step*I_PCR_out)))
        {
            theta.phi_LM = theta.phi_LM_min + (theta.phi_LM_max - theta.phi_LM_min) / (1.0 + pow((A_par + A_par_mat)*theta.A_LM_50pc_inv, theta.K_LM));
            theta.phi_D = theta.phi_D_min + (theta.phi_D_max - theta.phi_D_min) / (1.0 + pow((A_clin + A_clin_mat)*theta.A_D_50pc_inv, theta.K_D));

            // Sample outcome from probabilities of competing hazards
            double r = 1.0 / I_PCR_out;
            double p = lam_H_lag*theta.phi_LM;
            double q = p*theta.phi_D;
            double CH_prob[] = {
                    theta.r_PCR * r,                    // Move to S 
                    lam_H_lag*(1 - theta.phi_LM) * r,   // Move to I_PCR
                    p*(1.0 - theta.phi_D) * r,          // Move to I_LM
                    q*(1.0 - theta.treat_BScover) * r,  // Move to I_D
                    q*theta.treat_BScover * r           // Move to T
            };
            size_t CH_move = CH_sample(CH_prob, 5);


            if (CH_move == 0) {
                ////////////////////////////////
                // I_PCR -> S
                
                I_PCR = 0;
                S = 1;
                
            } else {
                ////////////////////////////////
                // Behaviour common to all infected transitions

                do_new_inf(lam_bite_lag / lam_H_lag);
                
                
                if (CH_move == 1) {
                    ////////////////////////////////
                    // I_PCR -> I_PCR
                    // 
                    // This is a super-infection event and we assumes boosting of immunity
                    // 
                    // No additional actions
                    
                    // FIXME: should we set I_new = I_DETECT_PCR ?
                    
                } else if (CH_move == 2) {
                    ////////////////////////////////
                    // I_PCR -> I_LM

                    I_new = I_DETECT_PCR | I_DETECT_LM;

                    I_PCR = 0;
                    I_LM = 1;
                    
                } else if (CH_move == 3) {
                    ////////////////////////////////
                    // I_PCR -> I_D

                    I_new = I_DETECT_PCR | I_DETECT_LM | I_DETECT_D;

                    I_PCR = 0;
                    I_D = 1;
                    
                } else if (CH_move == 4) {
                    ////////////////////////////////
                    // I_PCR -> T

                    /////////////////////////////////////////////////////////////////////
                    // Blood-stage drug administered

                    ACT_treat = 1;

                    if (util::random::bernoulli(theta.treat_BSeff))
                    {
                        I_PCR = 0;
                        T = 1;
                    }


                    I_new = I_DETECT_PCR | I_DETECT_LM | I_DETECT_D;

                    T_last_BS = 0.0;

                    do_pq_admin(true);
                }
            }

            return;
        }
    }


    //////////////////////////////////////////////////////////////////////
    //     //                                                           // 
    //  2  //  I_LM: LM detectable BS infection                         //
    //     //                                                           //
    //////////////////////////////////////////////////////////////////////

    if (I_LM == 1)
    {
        double I_LM_out = lam_H_lag + theta.r_LM;       // Probability of exiting state

        if (!util::random::bernoulli(exp(-t_step*I_LM_out)))
        {
            //theta.r_PCR = 1.0/( theta.d_PCR_min + (theta.d_PCR_max-theta.d_PCR_min)/( 1.0 + pow((A_par+A_par_mat)*theta.A_PCR_50pc_inv,theta.K_PCR) ) ); 
            theta.phi_LM = theta.phi_LM_min + (theta.phi_LM_max - theta.phi_LM_min) / (1.0 + pow((A_par + A_par_mat)*theta.A_LM_50pc_inv, theta.K_LM));
            theta.phi_D = theta.phi_D_min + (theta.phi_D_max - theta.phi_D_min) / (1.0 + pow((A_clin + A_clin_mat)*theta.A_D_50pc_inv, theta.K_D));

            // Sample outcome from probabilities of competing hazards
            double r = 1.0 / I_LM_out;
            double p = lam_H_lag*theta.phi_D;
            double CH_prob[] = {
                    theta.r_LM * r,                     // Move to I_PCR
                    lam_H_lag*(1.0 - theta.phi_D) * r,  // Move to I_LM
                    p*(1.0 - theta.treat_BScover) * r,  // Move to I_D
                    p*theta.treat_BScover * r           // Move to T
            };
            size_t CH_move = CH_sample(CH_prob, 4);

            if (CH_move == 0) {
                ////////////////////////////////
                // I_LM -> I_PCR

                I_LM = 0;
                I_PCR = 1;

            } else {
                ////////////////////////////////
                // Behaviour common to all infected transitions

                do_new_inf(lam_bite_lag / lam_H_lag);


                if (CH_move == 1) {
                    ////////////////////////////////
                    // I_LM -> I_LM
                    //
                    // Super-infection boosts immunity
                    // 
                    // No additional actions

                    // FIXME: should we set I_new = I_DETECT_LM ?
                    
                } else if (CH_move == 2) {
                    ////////////////////////////////
                    // I_LM -> I_D

                    I_new = I_DETECT_PCR | I_DETECT_LM | I_DETECT_D;

                    I_LM = 0;
                    I_D = 1;

                } else if (CH_move == 3) {
                    ////////////////////////////////
                    // I_LM -> T

                    /////////////////////////////////////////////////////////////////////
                    // Blood-stage drug administered

                    ACT_treat = 1;

                    if (util::random::bernoulli(theta.treat_BSeff))
                    {
                        I_LM = 0;
                        T = 1;
                    }


                    I_new = I_DETECT_PCR | I_DETECT_LM | I_DETECT_D;

                    T_last_BS = 0.0;

                    do_pq_admin(true);
                }
            }

            return;
        }
    }


    //////////////////////////////////////////////////////////////////////
    //     //                                                           // 
    //  3  //  I_D: Clinical disease                                    //
    //     //                                                           //
    //////////////////////////////////////////////////////////////////////

    if (I_D == 1)
    {
        double I_D_out = lam_H_lag + theta.r_D;     // Probability of exiting state

        if (!util::random::bernoulli(exp(-t_step*I_D_out)))
        {
            // Sample outcome from probabilities of competing hazards
            double r = 1.0 / I_D_out;
            double CH_prob[] = {
                    theta.r_D * r,                      // Move to I_LM
                    lam_H_lag * r                       // Move to D
            };
            size_t CH_move = CH_sample(CH_prob, 2);

            if (CH_move == 0) {
                ////////////////////////////////
                // I_D -> I_LM

                I_D = 0;
                I_LM = 1;

            } else if (CH_move == 1) {
                ////////////////////////////////
                // I_D -> I_D

                do_new_inf(lam_bite_lag / lam_H_lag);

            }

            return;
        }
    }


    //////////////////////////////////////////////////////////////////////
    //     //                                                           // 
    //  4  //  T : Clinical episode under treatment                     //
    //     //                                                           //
    //////////////////////////////////////////////////////////////////////

    if (T == 1)
    {
        double T_out = lam_H_lag + theta.r_T;       // Probability of exiting state

        if (!util::random::bernoulli(exp(-t_step*T_out)))
        {
            // Sample outcome from probabilities of competing hazards
            double r = 1.0 / T_out;
            double CH_prob[] = {
                    theta.r_T * r,                      // Move to P
                    lam_H_lag * r                       // Move to T
            };
            size_t CH_move = CH_sample(CH_prob, 2);

            if (CH_move == 0) {
                ////////////////////////////////
                // T -> P

                T = 0;
                P = 1;

            } else if (CH_move == 1) {
                ////////////////////////////////
                // T -> T
                //
                // Note that we don't allow immune boosting but we do
                // allow for the accumulation of new hypnozoites

                do_new_inf(lam_bite_lag / lam_H_lag);

            }

            return;
        }
    }




    //////////////////////////////////////////////////////////////////////
    //     //                                                           // 
    //  5  //  P: Treatment prophylaxis                                 //
    //     //                                                           //
    //////////////////////////////////////////////////////////////////////

    if (P == 1)
    {
        double P_out = lam_H_lag + theta.r_P;       // Probability of exiting state

        if (!util::random::bernoulli(exp(-t_step*P_out)))
        {
            // Sample outcome from probabilities of competing hazards
            double r = 1.0 / P_out;
            double CH_prob[] = {
                    theta.r_P * r,                      // Move to S
                    lam_H_lag * r                       // Move to P
            };
            size_t CH_move = CH_sample(CH_prob, 2);

            if (CH_move == 0) {
                ////////////////////////////////
                // P -> S

                P = 0;
                S = 1;

            } else if (CH_move == 1) {
                ////////////////////////////////
                // P -> P
                //
                // Note that we don't allow immune boosting but we do
                // allow for the accumulation of new hypnozoites

                do_new_inf(lam_bite_lag / lam_H_lag);
            }
        }
    }
}



////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//       //                                                                               //
// 4.2.  //  Ageing and immune boosting                                                   //
//       //                                                                               //  
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


void Individual::ager(Params& theta)
{
    /////////////////////////
    // Ageing

    age += t_step;


    /////////////////////////
    // Loss of hypnozoites

    if (Hyp > K_max)
    {
        Hyp = K_max;
    }

    if (util::random::bernoulli(1.0 - exp(-t_step*theta.gamma_L*Hyp)))
    {
        Hyp = Hyp - 1;
    }


    /////////////////////////
    // Immune decay

    A_par *= theta.A_par_decay;
    A_clin *= theta.A_clin_decay;


    ///////////////////////////////////////////////////
    // Maternal immunity decays exponentially for the first year
    // and is then set to zero afterwards

    if (age < 365.0)
    {
        A_par_mat *= theta.mat_decay;
        A_clin_mat *= theta.mat_decay;
    }
    else {
        A_par_mat = 0.0;
        A_clin_mat = 0.0;
    }


    /////////////////////////
    // Of child-bearing age? ~ 20 years
    // age in (18, 22) years

    if (gender == Gender::Female)
    {
        if ((age > 18. * 365.) && (age < 40. * 365.)) {
            preg_age = true;
        } else {
            preg_age = false;
        }

        if (pregnant) {
            preg_timer += 1.0;
        }

        if (!pregnant) {
            if (preg_age) {
                if (util::random::bernoulli(theta.P_preg)) {
                    pregnant = true;
                    preg_timer = 0.0;
                }
            }
        }

        if (pregnant)
        {
            if (preg_timer > 270.0)
            {
                pregnant = false;
                preg_timer = 0.0;
            }
        }
    }


    //////////////////////////////////////////////////////
    // Switches for refractory period of immune boosting

    if (A_par_boost == 0)
    {
        if (A_par_expiry < sim::ts0())
        {
            A_par_boost = 1;
        }
    }

    if (A_clin_boost == 0)
    {
        if (A_clin_expiry < sim::ts0())
        {
            A_clin_boost = 1;
        }
    }


    //////////////////////////////////////////////////////
    // Time since last blood-stage infection

    if (S == 1)
    {
        T_last_BS += 1.0;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
//       //                                                                               //
// 4.3.  //  Individual-level vector control updater                                      //
//       //                                                                               //
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void Individual::intervention_updater(Params& theta)
{

    ///////////////////////////////////////////
    // Is LLIN lost

    if (LLIN == 1)
    {
        if (util::random::bernoulli(theta.P_LLIN_loss))
        {
            LLIN = 0;
        }
    }

    ///////////////////////////////////////////
    // IRS turned off if sufficiently long ago (3 years???)

    if (IRS_age > 1095.0)
    {
        IRS = 0;
    }


    if (LLIN == 1 && IRS == 0)
    {
        LLIN_age += t_step;

        r_LLIN = theta.r_LLIN_net + (r_LLIN - theta.r_LLIN_net)*theta.P_PYR_decay;
        d_LLIN *= theta.P_PYR_decay;
        s_LLIN = 1.0 - r_LLIN - d_LLIN;

        w_VC = 1.0 - theta.PSI_bed + theta.PSI_bed * s_LLIN;
        y_VC = w_VC;
        z_VC = theta.PSI_bed * r_LLIN;
    }

    if (LLIN == 0 && IRS == 1)
    {
        IRS_age += t_step;

        r_IRS *= theta.P_IRS_decay;
        d_IRS *= theta.P_IRS_decay;
        s_IRS = 1.0 - r_IRS - d_IRS;

        w_VC = 1 - theta.PSI_indoors + theta.PSI_indoors * (1.0 - r_IRS)*s_IRS;
        y_VC = 1 - theta.PSI_indoors + theta.PSI_indoors * (1.0 - r_IRS);
        z_VC = theta.PSI_indoors * r_IRS;
    }

    if (LLIN == 1 && IRS == 1)
    {
        LLIN_age += t_step;
        IRS_age += t_step;

        r_LLIN = theta.r_LLIN_net + (r_LLIN - theta.r_LLIN_net)*theta.P_PYR_decay;
        d_LLIN *= theta.P_PYR_decay;
        s_LLIN = 1.0 - r_LLIN - d_LLIN;

        r_IRS *= theta.P_IRS_decay;
        d_IRS *= theta.P_IRS_decay;
        s_IRS = 1.0 - r_IRS - d_IRS;

        w_VC = 1.0 - theta.PSI_indoors + theta.PSI_bed * (1.0 - r_IRS)*s_LLIN * s_IRS + (theta.PSI_indoors - theta.PSI_bed)*(1.0 - r_IRS)*s_IRS;
        y_VC = 1.0 - theta.PSI_indoors + theta.PSI_bed * (1.0 - r_IRS)*s_LLIN + (theta.PSI_indoors - theta.PSI_bed)*(1.0 - r_IRS);
        z_VC = theta.PSI_bed * (1.0 - r_IRS)*r_LLIN + theta.PSI_indoors * r_IRS;
    }

    if (LLIN == 0 && IRS == 0)
    {
        w_VC = 1.0;
        y_VC = 1.0;
        z_VC = 0.0;

    }

}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  2.8. Competing hazards sampler                                          //
//                                                                          //
// Generates a weighted sample, assuming weights add to 1.                  //
//////////////////////////////////////////////////////////////////////////////

size_t CH_sample(double weights[], size_t num)
{
    // Accumulate weights
    for (size_t k = 1; k < num; k++) {
        weights[k] += weights[k - 1];
    }

    double unif = util::random::uniform_01();
    
    for (size_t k = 0; k < num; k++) {
        if (unif < weights[k]) {
            return k;
        }
    }

    return num - 1; // just in case
}

} }
