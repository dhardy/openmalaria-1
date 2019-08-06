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

#include <cmath>


namespace OM { namespace white {

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//          //                                                              //
//   ####   //  ##### ##  ## #   ##  ####  ###### ####  ####  #   ##  ###   //
//  ##  ##  //  ##    ##  ## ##  ## ##  ##   ##    ##  ##  ## ##  ## ##     // 
//     ##   //  ####  ##  ## ### ## ##       ##    ##  ##  ## ### ##  ###   //
//    ##    //  ##    ##  ## ## ### ##  ##   ##    ##  ##  ## ## ###    ##  //
//   #####  //  ##     ####  ##  ##  ####    ##   ####  ####  ##  ##  ###   //
//          //                                                              //
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  2.1. Derivatives of mosquito ODE model                                  //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

void mosq_derivs(const double t,
        Array<double, N_spec, N_M_comp>& yM,
        Array<double, N_spec, N_M_comp>& dyMdt,
        size_t track_index, Params& theta, Population& POP)
{
    Array<double, N_spec, 1> Karry_seas_inv;
    
    Karry_seas_inv = 1.0
        / (theta.Karry
            * (theta.dry_seas + (1.0 - theta.dry_seas)
                * pow(0.5 + 0.5 * cos((2.0 * M_PI / 365.0) * (t - theta.t_peak_seas)), theta.kappa_seas)
                / theta.denom_seas));

    //Karry_seas_inv = 1.0 / theta.Karry;

    dyMdt.col(0) = POP.beta_VC * (yM.col(3) + yM.col(4) + yM.col(5)) - yM.col(0) / theta.d_E_larvae - yM.col(0) * theta.mu_E0*(1.0 + (yM.col(0) + yM.col(1))*Karry_seas_inv);
    dyMdt.col(1) = yM.col(0) / theta.d_E_larvae - yM.col(1) / theta.d_L_larvae - yM.col(1) * theta.mu_L0*(1.0 + theta.gamma_larvae*(yM.col(0) + yM.col(1))*Karry_seas_inv);
    dyMdt.col(2) = yM.col(1) / theta.d_L_larvae - yM.col(2) / theta.d_pupae - yM.col(2) * theta.mu_P;
    dyMdt.col(3) = 0.5*yM.col(2) / theta.d_pupae - theta.lam_M * yM.col(3) - POP.mu_M_VC * yM.col(3);
    dyMdt.col(4) = theta.lam_M * yM.col(3) - theta.lam_S_M_track.col(track_index) * POP.exp_muM_tauM_VC - POP.mu_M_VC * yM.col(4);
    dyMdt.col(5) = theta.lam_S_M_track.col(track_index) * POP.exp_muM_tauM_VC - POP.mu_M_VC * yM.col(5);
}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//  2.2. Runge-Kutta 4 step updater for mosquito model                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

void mosq_rk4(const double t, const double t_step_mosq,
        Array<double, N_spec, N_M_comp>& yM,
        size_t track_index, Params& theta, Population& POP)
{
    Array<double, N_spec, N_M_comp> k1_yM, k2_yM, k3_yM, k4_yM, yM_temp;

    //////////////////////////
    // step 1

    mosq_derivs(t, yM, k1_yM, track_index, theta, POP);


    //////////////////////////
    // step 2

    yM_temp = yM + 0.5 * t_step_mosq * k1_yM;

    mosq_derivs(t + 0.5*t_step_mosq, yM_temp, k2_yM, track_index, theta, POP);


    //////////////////////////
    // step 3

    yM_temp = yM + 0.5 * t_step_mosq * k2_yM;

    mosq_derivs(t + 0.5*t_step_mosq, yM_temp, k3_yM, track_index, theta, POP);


    //////////////////////////
    // step 4

    yM_temp = yM + t_step_mosq * k3_yM;

    mosq_derivs(t + t_step_mosq, yM_temp, k4_yM, track_index, theta, POP);


    //////////////////////////
    // output

    yM = yM
            + t_step_mosq * k1_yM / 6.0
            + t_step_mosq * k2_yM / 3.0
            + t_step_mosq * k3_yM / 3.0
            + t_step_mosq * k4_yM / 6.0;
}

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  2.3. Update step for mosquitoes                                          //
//                                                                           // 
//       For every human step we take mosq_steps (=10) steps for mosquitoes  //
//       The smaller step size ensures that the ODE solver works smoothly.   //
//       Especially an issue for the larval stages                           //   
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

void mosquito_step(Params& theta, Population& POP)
{
    //////////////////////////////////
    // Force of infection on mosquitoes

    theta.lam_M.setZero();

    size_t n = 0;
    for (const Individual& person: POP.m_people) {
        theta.lam_M += POP.lam_n.row(n) * (
                  theta.c_PCR * person.I_PCR
                + theta.c_LM * person.I_LM
                + theta.c_D * person.I_D
                + theta.c_T*person.T);
        n++;
    }


    //////////////////////////////////////
    // Carry out the mosq_steps
    
    double t = sim::ts0().inDays();
    double t_step_mosq = (double(t_step)) / (double(mosq_steps));

    for (size_t j = 0; j<mosq_steps; j++)
    {
        size_t step = sim::ts0().inSteps() * mosq_steps + j;
        size_t track_index = step % theta.lam_S_M_track.cols();
        
        mosq_rk4(t, t_step_mosq, POP.yM, track_index, theta, POP);

        theta.lam_S_M_track.col(track_index) = theta.lam_M * POP.yM.col(3);
    }
}

} }
