/* This file is part of OpenMalaria.
 * 
 * Copyright (C) 2005-2014 Swiss Tropical and Public Health Institute
 * Copyright (C) 2005-2014 Liverpool School Of Tropical Medicine
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

#include "mon/info.h"
#include "mon/reporting.h"
#include "mon/management.h"
#define H_OM_mon_cpp
#include "mon/OutputMeasures.hpp"
#include "Monitoring/Survey.h"
#include "WithinHost/Genotypes.h"
#include "Clinical/CaseManagementCommon.h"
#include "Host/Human.h"
#include "util/errors.h"
#include "schema/monitoring.h"

#include <FastDelegate.h>
#include <iostream>
#include <boost/format.hpp>

namespace OM {
namespace mon {
    using namespace fastdelegate;

namespace impl {
    // Accumulators, variables:
    size_t currentSurvey = NOT_USED;
    SimTime nextSurveyTime = sim::future();
}

#ifndef NDEBUG
const size_t NOT_ACCEPTED = boost::integer_traits<size_t>::const_max - 1;
#endif

typedef FastDelegate4<ostream&,size_t,int,size_t> WriteDelegate;

// Store by human age and cohort
template<typename T, bool BY_AGE, bool BY_COHORT, bool BY_SPECIES,
    bool BY_GENOTYPE>
class Store{
    // This maps from an index in reports to an output measure
    vector<int> outMeasures;
    
    // mIndices and deployIndices both map from measures to indices.
    // The former should be faster but is insufficient for deployments.
    // Usage should not overlap (i.e. only one should match any measure).
    
    // This maps from measures (MHR_HOSTS, etc.) to an index in reports or NOT_USED
    vector<size_t> mIndices;
    // This maps from measures (MHD_VACCINATIONS, etc.) to a Deploy::Method enum
    // and an index in reports. Measures may have any number of matches here.
    typedef multimap<int,pair<uint8_t,size_t> > DeployInd_t;
    DeployInd_t deployIndices;
    
    size_t nAgeGroups, nCohortSets, nSpecies, nGenotypes;
    // These are the stored reports (multidimensional; use size() and index())
    vector<T> reports;
    
    // get size of reports
    inline size_t size(){ return outMeasures.size() * impl::nSurveys *
        nAgeGroups * nCohortSets * nSpecies * nGenotypes; }
    // get an index in reports
    inline size_t index( size_t m, size_t s, size_t a, size_t c, size_t sp, size_t g ){
        return g + nGenotypes * (sp + nSpecies * (c + nCohortSets *
            (a + nAgeGroups * (s + impl::nSurveys * m))));
    }
    
    inline void add(T val, size_t mIndex, size_t survey, size_t ageIndex,
                    uint32_t cohortSet, size_t species, size_t genotype)
    {
#ifndef NDEBUG
        if( mIndex >= outMeasures.size() ||
            survey >= impl::nSurveys ||
            ageIndex >= nAgeGroups ||
            cohortSet >= nCohortSets ||
            species >= nSpecies ||
            genotype >= nGenotypes
        ){
            cout << "Index out of bounds for survey:\t" << survey << " of " << impl::nSurveys
                << "\nmeasure number\t" << mIndex << " of " << outMeasures.size()
                << "\nage group\t" << ageIndex << " of " << nAgeGroups
                << "\ncohort set\t" << cohortSet << " of " << nCohortSets
                << "\nspecies\t" << species << " of " << nSpecies
                << "\ngenotype\t" << genotype << " of " << nGenotypes
                << endl;
        }
#endif
        reports[index(mIndex,survey,ageIndex,cohortSet,species,genotype)] += val;
    }
    
    void writeM( ostream& stream, size_t survey, int outMeasure, size_t inMeasure ){
        if( BY_SPECIES ){
            assert( !BY_AGE && !BY_COHORT );  // output col2 conflicts
            for( size_t species = 0; species < nSpecies; ++species ){
            for( size_t genotype = 0; genotype < nGenotypes; ++genotype ){
                const int col2 = species + 1 +
                    1000000 * genotype;
                T value = reports[index(inMeasure,survey,0,0,species,genotype)];
                stream << (survey+1) << '\t' << col2 << '\t' << outMeasure
                    << '\t' << value << lineEnd;
            } }
        }else{
            // Backwards compatibility: first age group starts at 1, unless
            // there isn't an age group:
            const int ageGroupAdd = BY_AGE ? 1 : 0;
            for( size_t cohortSet = 0; cohortSet < nCohortSets; ++cohortSet ){
            for( size_t ageGroup = 0; ageGroup < nAgeGroups; ++ageGroup ){
            for( size_t genotype = 0; genotype < nGenotypes; ++genotype ){
                // Yeah, >999 age groups clashes with cohort sets, but unlikely a real issue
                const int col2 = ageGroup + ageGroupAdd +
                    1000 * internal::cohortSetOutputId( cohortSet ) +
                    1000000 * genotype;
                T value = reports[index(inMeasure,survey,ageGroup,cohortSet,0,genotype)];
                stream << (survey+1) << '\t' << col2 << '\t' << outMeasure
                    << '\t' << value << lineEnd;
            } } }
        }
    }
    
public:
    // Set up ready to accept reports.
    void init( const list<OutMeasure>& required, size_t nSp ){
        // Set these internally to 1 if we don't segregate.
        // Age groups -1 because last isn't reported.
        nAgeGroups = BY_AGE ? (AgeGroup::numGroups() - 1) : 1;
        nCohortSets = BY_COHORT ? impl::nCohortSets : 1;
        nSpecies = (BY_SPECIES && nSp > 0) ? nSp : 1;
        nGenotypes = BY_GENOTYPE ? WithinHost::Genotypes::N() : 1;
        mIndices.assign( M_NUM, NOT_USED );
        // outMeasures.size() is the number of measures we store here
        outMeasures.assign( 0, 0 );
        
        for( list<OutMeasure>::const_iterator it = required.begin();
            it != required.end(); ++it )
        {
            if( it->m >= M_NUM ) continue;      // skip: obsolete/special
            if( it->isDouble != (typeid(T) == typeid(double) ) ){
#ifndef NDEBUG
                // Debug mode: this should prevent silly errors where the type
                // reported does not match the type defined for some output:
                mIndices[it->m] = NOT_ACCEPTED;
#endif
                continue;
            }
            if( it->byAge != BY_AGE ||
                it->byCohort != BY_COHORT ||
                it->bySpecies != BY_SPECIES ||
                it->byGenotype != BY_GENOTYPE ) continue;
            
            if( mIndices[it->m] != NOT_USED ||
                (it->method == Deploy::NA && deployIndices.count(it->m) > 0) )
            {
                //FIXME: use a name not a number to describe the measure
                throw util::xml_scenario_error( (boost::format("multiple use "
                    "of output measure %1% by age and cohort") %it->m).str() );
            }
            
            size_t newInd = outMeasures.size();     // length becomes our index
            if( it->method == Deploy::NA ){
                mIndices[it->m] = newInd;
            }else{
                deployIndices.insert( make_pair(it->m, make_pair(it->method, newInd)) );
            }
            outMeasures.push_back( it->outId ); // increment length
        }
        
        reports.assign(size(), 0);
    }
    
    // Take a reported value and either store it or forget it.
    // If some of ageIndex, cohortSet, species are not applicable, use 0.
    void report( T val, Measure measure, size_t survey, size_t ageIndex = 0,
                 uint32_t cohortSet = 0, size_t species = 0, size_t genotype = 0 )
    {
        if( survey == NOT_USED ) return; // pre-main-sim & unit tests we ignore all reports
        // last category is for humans too old for reporting groups:
        if( ageIndex == nAgeGroups ) return;
        assert( measure < mIndices.size() );
        assert( mIndices[measure] != NOT_ACCEPTED );
        assert( ageIndex < nAgeGroups && (BY_AGE || nAgeGroups == 1) );
        assert( cohortSet < nCohortSets && (BY_COHORT || nCohortSets == 1) );
        assert( species < nSpecies && (BY_SPECIES || nSpecies == 1) );
        assert( genotype < nGenotypes && (BY_GENOTYPE || nGenotypes == 1) );
        if( mIndices[measure] == NOT_USED ){    // measure not used by this store
            assert( deployIndices.count(measure) == 0 );
            return;
        }
        add( val, mIndices[measure], survey, ageIndex, cohortSet, species, genotype );
    }
    
    // Take a deployment report and potentially store it in one or more places
    // If some ageIndex or cohortSet are not applicable, use 0.
    void deploy( T val, Measure measure, size_t survey, size_t ageIndex,
                 uint32_t cohortSet, Deploy::Method method )
    {
        if( survey == NOT_USED ) return; // pre-main-sim & unit tests we ignore all reports
        if( ageIndex == nAgeGroups ) return;    // last category is for humans too old for reporting groups
        assert( method == Deploy::NA || method == Deploy::TIMED ||
            method == Deploy::CTS || method == Deploy::TREAT );
        assert( measure < mIndices.size() );
        assert( mIndices[measure] == NOT_USED );
        assert( nSpecies == 1 && nGenotypes == 1 );     // never used for deployments
        
        typedef DeployInd_t::const_iterator const_it_t;
        pair<const_it_t,const_it_t> range = deployIndices.equal_range( measure );
        for( const_it_t it = range.first; it != range.second; ++it ){
            uint8_t mask = it->second.first;
            if( (mask & method) == 0 ) continue;
            add( val, it->second.second, survey, ageIndex, cohortSet, 0, 0 );
        }
    }
    
    // Order self in a list of outputs
    void addMeasures( map<int,pair<WriteDelegate,size_t> >& mOrdered ){
        for( size_t m = 0; m < outMeasures.size(); ++m ){
            mOrdered[outMeasures[m]] = make_pair( MakeDelegate( this,
                    &Store<T,BY_AGE,BY_COHORT,BY_SPECIES,BY_GENOTYPE>::writeM), m );
        }
    }
    
    // Write stored values to stream
    void write( ostream& stream, size_t survey ){
        // use a (tree) map to sort by external measure
        map<int,size_t> measuresOrdered;
        for( size_t m = 0; m < outMeasures.size(); ++m ){
            measuresOrdered[outMeasures[m]] = m;
        }
        typedef pair<int,size_t> P;
        foreach( P mp, measuresOrdered ){
            writeM( stream, survey, mp.first, mp.second );
        }
    }
    
    // Checkpointing
    void checkpoint( ostream& stream ){
        reports.size() & stream;
        BOOST_FOREACH (T& y, reports) {
            y & stream;
        }
        // mIndices and outMeasures are constant after initialisation
    }
    void checkpoint( istream& stream ){
        size_t l;
        l & stream;
        if( l != size() ){
            throw util::checkpoint_error( "mon::reports: invalid list size" );
        }
        reports.resize (l);
        BOOST_FOREACH (T& y, reports) {
            y & stream;
        }
        // mIndices and outMeasures are constant after initialisation
    }
};

Store<int, false, false, false, false> storeI;
Store<double, false, false, false, false> storeF;
Store<int, true, true, false, false> storeHACI;
Store<double, true, true, false, false> storeHACF;
Store<double, true, true, false, true> storeHACGF;
Store<double, false, false, true, false> storeSF;
Store<double, false, false, true, true> storeSGF;
int reportIMR = -1; // special output for fitting

void initReporting( size_t nSpecies, const scnXml::Monitoring& monElt )
{
    defineOutMeasures();
    const scnXml::OptionSet& optsElt = monElt.getSurveyOptions();
    list<OutMeasure> enabledOutMeasures;
    foreach( const scnXml::Option& optElt, optsElt.getOption() ){
        if( optElt.getValue() == false ) continue;      // option is disabled
        NamedMeasureMapT::const_iterator it =
            namedOutMeasures.find( optElt.getName() );
        if( it == namedOutMeasures.end() ){
            throw util::xml_scenario_error( (boost::format("unrecognised "
                "survey option: %1%") %optElt.getName()).str() );
        }
        if( it->second.m == M_NUM ){
            throw util::xml_scenario_error( (boost::format("obsolete "
                "survey option: %1%") %optElt.getName()).str() );
        }
        if( it->second.m == M_ALL_CAUSE_IMR && it->second.isDouble &&
            !it->second.byAge && !it->second.byCohort && !it->second.bySpecies ){
            reportIMR = it->second.outId;
        }
        enabledOutMeasures.push_back( it->second );
    }
    storeI.init( enabledOutMeasures, nSpecies );
    storeF.init( enabledOutMeasures, nSpecies );
    storeHACI.init( enabledOutMeasures, nSpecies );
    storeHACF.init( enabledOutMeasures, nSpecies );
    storeHACGF.init( enabledOutMeasures, nSpecies );
    storeSF.init( enabledOutMeasures, nSpecies );
    storeSGF.init( enabledOutMeasures, nSpecies );
    
    internal::initCohorts( monElt );
}

void internal::write( ostream& stream ){
    // use a (tree) map to sort by external measure
    typedef pair<WriteDelegate,size_t> MPair;
    map<int,MPair> measuresOrdered;
    storeI.addMeasures( measuresOrdered );
    storeF.addMeasures( measuresOrdered );
    storeHACI.addMeasures( measuresOrdered );
    storeHACF.addMeasures( measuresOrdered );
    storeHACGF.addMeasures( measuresOrdered );
    storeSF.addMeasures( measuresOrdered );
    storeSGF.addMeasures( measuresOrdered );
    
    typedef pair<int,MPair> PP;
    for( size_t survey = 0; survey < impl::nSurveys; ++survey ){
        foreach( PP pp, measuresOrdered ){
            pp.second.first( stream, survey, pp.first, pp.second.second );
        }
    }
    if( reportIMR >= 0 ){
        // Infant mortality rate is a single number, therefore treated specially.
        // It is calculated across the entire intervention period and used in
        // model fitting.
        stream << 1 << "\t" << 1 << "\t" << reportIMR
            << "\t" << Clinical::infantAllCauseMort() << lineEnd;
    }
}

// Report functions: each reports to all usable stores (i.e. correct data type
// and where parameters don't have to be fabricated).
void reportMI( Measure measure, int val ){
    storeI.report( val, measure, impl::currentSurvey );
}
void reportMF( Measure measure, double val ){
    storeF.report( val, measure, impl::currentSurvey );
}
void reportMHI( Measure measure, const Host::Human& human, int val ){
    storeI.report( val, measure, impl::currentSurvey );
    size_t ageIndex = human.monAgeGroup().i();
    storeHACI.report( val, measure, impl::currentSurvey, ageIndex,
                      human.cohortSet() );
}
void reportMHF( Measure measure, const Host::Human& human, double val ){
    storeF.report( val, measure, impl::currentSurvey );
    size_t ageIndex = human.monAgeGroup().i();
    storeHACF.report( val, measure, impl::currentSurvey, ageIndex,
                      human.cohortSet() );
}
void reportMSACI( Measure measure, size_t survey,
                  AgeGroup ageGroup, uint32_t cohortSet, int val )
{
    storeI.report( val, measure, impl::currentSurvey );
    storeHACI.report( val, measure, survey, ageGroup.i(), cohortSet );
}
void reportMACGF( Measure measure, size_t ageIndex, uint32_t cohortSet,
                  size_t genotype, double val )
{
    storeF.report( val, measure, impl::currentSurvey );
    storeHACF.report( val, measure, impl::currentSurvey, ageIndex, cohortSet );
    storeHACGF.report( val, measure, impl::currentSurvey, ageIndex, cohortSet,
                       0, genotype );
}
void reportMSF( Measure measure, size_t species, double val ){
    storeF.report( val, measure, impl::currentSurvey );
    storeSF.report( val, measure, impl::currentSurvey, 0, 0, species );
}
void reportMSGF( Measure measure, size_t species, size_t genotype, double val ){
    storeF.report( val, measure, impl::currentSurvey );
    storeSF.report( val, measure, impl::currentSurvey, 0, 0, species );
    storeSGF.report( val, measure, impl::currentSurvey, 0, 0, species, genotype );
}
// Deployment reporting uses a different function to handle the method
// (mostly to make other types of report faster).
void reportMHD( Measure measure, const Host::Human& human,
                Deploy::Method method )
{
    const int val = 1;  // always report 1 deployment
    storeI.deploy( val, measure, impl::currentSurvey, 0, 0, method );
    size_t ageIndex = human.monAgeGroup().i();
    storeHACI.deploy( val, measure, impl::currentSurvey, ageIndex,
                      human.cohortSet(), method );
    // This is for nTreatDeployments:
    storeHACI.deploy( val, MHD_ALL_DEPLOYS, impl::currentSurvey, ageIndex,
                      human.cohortSet(), method );
}

void checkpoint( ostream& stream ){
    impl::currentSurvey & stream;
    impl::nextSurveyTime & stream;
    storeI.checkpoint(stream);
    storeF.checkpoint(stream);
    storeHACI.checkpoint(stream);
    storeHACF.checkpoint(stream);
    storeHACGF.checkpoint(stream);
    storeSF.checkpoint(stream);
    storeSGF.checkpoint(stream);
}
void checkpoint( istream& stream ){
    impl::currentSurvey & stream;
    impl::nextSurveyTime & stream;
    storeI.checkpoint(stream);
    storeF.checkpoint(stream);
    storeHACI.checkpoint(stream);
    storeHACF.checkpoint(stream);
    storeHACGF.checkpoint(stream);
    storeSF.checkpoint(stream);
    storeSGF.checkpoint(stream);
}

}
}
