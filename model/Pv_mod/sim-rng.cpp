/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
///                                                                       ///
///  Individual-based Plasmodium vivax transmission model.                ///
///                                                                       ///
///  Dr Michael White                                                     ///
///  Institut Pasteur                                                     ///
///  michael.white@pasteur.fr                                             ///
///                                                                       ///
///                                                                       ///
///  Random number generator support code                                 ///
///                                                                       ///
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


#include "sim-rng.hpp"
#include <Eigen/Cholesky>


void MultivariateNormal::set(Eigen::MatrixXd const& covar) {
    Eigen::LLT<Eigen::MatrixXd> llt(covar);
    if (llt.info() != Eigen::ComputationInfo::Success) {
        throw("Multivariate normal: Cholesky decomposition failed: covariance matrix must be positive definite");
    }
    transform = llt.matrixL();
}
