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

// Include guard
#ifndef PVIVAX_MODEL_RNG
#define PVIVAX_MODEL_RNG

#include <random>
#include <Eigen/Core>


extern std::mt19937 random_engine;


/// Sample a double uniformly between 0 and 1
inline double gen_u01() {
    static std::uniform_real_distribution<> unif(0.0, 1.0);
    return unif(random_engine);
}

/// Sample a boolean with given probability
inline bool gen_bool(double p) {
    return gen_u01() < p;
}

/// Generate an exponential sample with λ=1 and given mean
inline double gen_exp(double mean) {
    static std::exponential_distribution<> exp(1.0);
    return mean * exp(random_engine);
}

/// Generate a normal sample
inline double gen_normal(double mean, double sd) {
    std::normal_distribution<> norm(mean, sd);
    return norm(random_engine);
}

/// A sampler for the multivariate normal distribution.
/// 
/// This implementation requires that the covariance matrix is positive
/// definite, and assumes a mean of zero (for non-zero mean, simply add the
/// mean to the generated sample).
/// 
/// Adapted from https://stackoverflow.com/a/40245513/314345
struct MultivariateNormal
{
    /// Construct with an empty matrix.
    MultivariateNormal() {}
    
    /// Construct with the given covariance matrix.
    /// This matrix must be positive definite.
    /// 
    /// Only values from the lower triangular part of the martix are read.
    MultivariateNormal(Eigen::MatrixXd const& covar) {
        set(covar);
    }
    
    /// Initialise with the given covariance matrix.
    /// This matrix must be positive definite.
    /// 
    /// Only values from the lower triangular part of the martix are read.
    void set(Eigen::MatrixXd const& covar);

    Eigen::MatrixXd transform;

    Eigen::VectorXd operator()() const {
        auto n = transform.rows();
        std::normal_distribution<> std_norm(0.0, 1.0);
        auto norm_vec = Eigen::VectorXd(n);
        for (auto i = 0; i < n; ++i) {
            norm_vec(i) = std_norm(random_engine);
        }
        return transform * std::move(norm_vec);
    }
};

#endif
