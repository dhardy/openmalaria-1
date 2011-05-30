/* This file is part of OpenMalaria.
 * 
 * Copyright (C) 2005-2011 Swiss Tropical Institute and Liverpool School Of Tropical Medicine
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

#ifndef UTIL_SAMPLER
#define UTIL_SAMPLER

#include "Global.h"
#include "schema/util.h"
#include "util/random.h"
#include <cmath>
#include <limits>

namespace OM { namespace util {
    
    /** Sampler for log-normal values */
    class LognormalSampler {
    public:
        LognormalSampler() :
            mu( numeric_limits<double>::signaling_NaN() ),
            sigma( numeric_limits<double>::signaling_NaN() )
        {}
        
        /** Set parameters such that samples taken are:
         * X ~ log N( log(mean)-s²/2, s² )
         * 
         * @param mean Mean of sampled variates
         * @param s Square-root of variance of logarithm of sampled variates
         */
        void setParams( double mean, double s ){
            mu = log(mean) - 0.5*s*s;
            sigma = s;
        }
        /** As above, using an XML element. */
        void setParams( const scnXml::LognormalSample& elt ){
            setParams( elt.getMean(), elt.getSigma() );
        }
        
        /** Sample a value. */
        inline double sample() const{
            return random::log_normal( mu, sigma );
        }
        
    private:
        double mu, sigma;
    };
    
} }
#endif

