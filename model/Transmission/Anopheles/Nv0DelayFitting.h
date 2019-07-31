/* This file is part of OpenMalaria.
 * 
 * Copyright (C) 2005-2015 Swiss Tropical and Public Health Institute
 * Copyright (C) 2005-2015 Liverpool School Of Tropical Medicine
 * 
 * OpenMalaria is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef Hmod_VectorNv0DelayFitting
#define Hmod_VectorNv0DelayFitting

#include <boost/version.hpp>
#include <boost/foreach.hpp>
#include <boost/math/tools/roots.hpp>

// boost::math::tuple doesn't work prior to Boost 1.42
#if BOOST_VERSION > 104100
#define OM_tuple boost::math::tuple
#define OM_make_tuple boost::math::make_tuple
#else
// we don't ever actually need this:
//#include <boost/tr1/tuple.hpp>  // import for compatibility with both boost 1.42 and 1.45
#define OM_tuple std::tr1::tuple
#define OM_make_tuple std::tr1::make_tuple
#endif

#include "util/errors.h"
#include "Global.h"

namespace OM {
namespace Transmission {
namespace Anopheles {

namespace Nv0DelayFitting {
template <class T>
struct eDFunctor
{
  eDFunctor(double r, const vector<T>& fc_, const vector<T>& samples_) : p(samples_.size()), fcR(r), fc(fc_), logSamples(samples_) {
    if (mod_nn(fc.size(), 2u) == 0)
      throw TRACED_EXCEPTION_DEFAULT("The number of Fourier coefficents should be odd.");
    w = 2*M_PI / T(p);
    fn = (fc.size()-1)/2;
    foreach (T& sample, logSamples) {
      sample = log (sample);	// compare logarithms of EIR to make differentiation easier
    }
  }
  
  OM_tuple<T, T, T> operator()(T const& d)
  {
    T f = 0.0, df = 0.0, ddf = 0.0;
    
    // Calculate inverse discrete Fourier transform
    // TODO(vec lifecycle): This may not interpolate sensibly. See for example
    // https://en.wikipedia.org/wiki/Discrete_Fourier_transform#Trigonometric_interpolation_polynomial
    for(size_t t=0; t<p; ++t) {
      T wt = w*t+fcR+d;
      T val = fc[0], dval = 0.0, ddval = 0.0;
      for(size_t n=1;n<=fn; ++n){
	T temp = fc[2*n-1]*cos(n*wt) + fc[2*n]*sin(n*wt);	// value
	val  += temp;
	dval += (n*fc[2*n]*cos(n*wt)) - (n*fc[2*n-1]*sin(n*wt));	// first derivative wrt d
	ddval = -(n*n*temp);					// 2nd derivative wrt d
      }
      
      // The difference of logarithms of sample and fourier value
      T diff = val - logSamples[t];		// deriv. wrt. d is just dval
      // Differences of first and second derivatives of samples
      // TODO(vec lifecycle): derivatives of samples shouldn't be zero. Presumably, however, fitting still works.
      T ddiff = dval - 0.0;
      T dddiff = ddval - 0.0;
      
      f += diff*diff;				// add diff²
      df += 2.0*diff * ddiff;			// add 1st deriv. diff²
      ddf += 2.0*ddiff*ddiff + 2.0*diff*dddiff;	// add 2nd deriv. diff²
//       cout << t << "\t"<< val << "\t" << logSamples[t] << "\t" << diff*diff << "\t" <<2*diff * (dval - 0.0) << "\t" << 2*dval*dval + 2*diff*ddval << endl;
    }
    
    return OM_make_tuple(f, df, ddf);
  }
  private:
    size_t p, fn;
    T w,fcR;
    const vector<T>& fc;
    vector<T> logSamples;
};


/** Calculate rotation angle needed to match up fourier series defined by fc with samples.
 *
 * The series given by fc and fcR is rotated to match samples, and negative
 * this angle is returned (i.e. amount samples should be rotated by).
 *
 * @param fcR Angle (in radians) to rotate by, before matching.
 * @param fc Fourier coefficients for S_v series (a0, a1, b1, ...)
 * @param samples The calculated S_v values we want to match
 * @returns Angle to rotate by (including existingR).
 */
template <class T>
T fit (double fcR, const vector<T>& fc, const vector<T>& samples)
{
  T min = 0.0;
  T max = 2*M_PI;	// can we provide better bounds?
  T guess = 0.0;
  //NOTE: fitting can be tweaked. Can alter expected digits, and test whether
  // halley_iterate or schroeder_iterate converges faster. Current setup works fine in any case.
  int digits = std::numeric_limits<T>::digits / 2;
  return -boost::math::tools::halley_iterate (eDFunctor<T>(fcR, fc, samples), guess, min, max, digits);
}

}
}
}
}
#endif
