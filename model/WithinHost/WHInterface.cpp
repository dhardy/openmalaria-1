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

#include "WithinHost/WHInterface.h"
#include "WithinHost/WHVivax.h"
#include "WithinHost/DescriptiveWithinHost.h"
#include "WithinHost/CommonWithinHost.h"
#include "WithinHost/Infection/DummyInfection.h"
#include "WithinHost/Infection/EmpiricalInfection.h"
#include "WithinHost/Infection/MolineauxInfection.h"
#include "WithinHost/Infection/PennyInfection.h"
#include "WithinHost/Treatments.h"
#include "white/driver.hpp"
#include "util/random.h"
#include "util/ModelOptions.h"
#include "util/errors.h"
#include "schema/scenario.h"
//using namespace std;

#include <cmath>
#include <boost/format.hpp>


namespace OM {
namespace WithinHost {

using namespace OM::util;

bool opt_vivax_simple = false, opt_vivax_white = false,
        opt_dummy_whm = false, opt_empirical_whm = false,
        opt_molineaux_whm = false, opt_penny_whm = false,
        opt_common_whm = false;

// -----  static functions  -----

void WHInterface::init( const OM::Parameters& parameters, const scnXml::Scenario& scenario ) {
    opt_vivax_simple = util::ModelOptions::option( util::VIVAX_SIMPLE_MODEL );
    opt_vivax_white = util::ModelOptions::option( util::VIVAX_WHITE_MODEL );
    opt_dummy_whm = util::ModelOptions::option (util::DUMMY_WITHIN_HOST_MODEL);
    opt_empirical_whm = util::ModelOptions::option (util::EMPIRICAL_WITHIN_HOST_MODEL);
    opt_molineaux_whm = util::ModelOptions::option (util::MOLINEAUX_WITHIN_HOST_MODEL);
    opt_penny_whm = util::ModelOptions::option (util::PENNY_WITHIN_HOST_MODEL);
    
    if( opt_vivax_white ){
        white::init_model( scenario );
    } else if( opt_vivax_simple ){
        WHVivax::init( parameters, scenario.getModel() );
    }else{
        WHFalciparum::init( parameters, scenario.getModel() );
        
        if (opt_dummy_whm) {
            DummyInfection::init ();
        } else if (opt_empirical_whm) {
            EmpiricalInfection::init();    // 1-day time step check
        } else if (opt_molineaux_whm) {
            MolineauxInfection::init( parameters );
        } else if (opt_penny_whm) {
            PennyInfection::init();
        } else {
            DescriptiveInfection::init( parameters );      // 5-day time step check
        }
        opt_common_whm = opt_dummy_whm || opt_empirical_whm
                || opt_molineaux_whm || opt_penny_whm;
        
        if( opt_common_whm ){
            CommonWithinHost::init( scenario );
        }else{
            DescriptiveWithinHostModel::initDescriptive();
        }
    }
}

TreatmentId WHInterface::addTreatment(const scnXml::TreatmentOption& desc){
    return Treatments::addTreatment( desc );
}

unique_ptr<WHInterface> WHInterface::createWithinHostModel ( double comorbidityFactor ) {
    if( opt_vivax_simple ) {
        return unique_ptr<WHInterface>(new WHVivax( comorbidityFactor ));
    } else if( opt_common_whm ) {
        return unique_ptr<WHInterface>(new CommonWithinHost( comorbidityFactor ));
    } else {
        return unique_ptr<WHInterface>(new DescriptiveWithinHostModel( comorbidityFactor ));
    }
}


// -----  Non-static  -----


void WHInterface::checkpoint (istream& stream) {
    numInfs & stream;

    if (numInfs > MAX_INFECTIONS)
        throw util::checkpoint_error( (boost::format("numInfs: %1%") %numInfs).str() );
}
void WHInterface::checkpoint (ostream& stream) {
    numInfs & stream;
}

}
}
