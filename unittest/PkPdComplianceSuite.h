/*
 This file is part of OpenMalaria.
 
 Copyright (C) 2005-2015 Swiss Tropical and Public Health Institute
 Copyright (C) 2005-2014 Liverpool School Of Tropical Medicine
 
 OpenMalaria is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or (at
 your option) any later version.
 
 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/
// Unittest for the LSTM drug model

#ifndef Hmod_PkPdComplianceSuite
#define Hmod_PkPdComplianceSuite

#include <cxxtest/TestSuite.h>
#include "PkPd/LSTMModel.h"
#include "UnittestUtil.h"
#include "ExtraAsserts.h"
#include <limits>

using std::pair;
using std::multimap;
using namespace OM;
using namespace OM::PkPd;

/** Test outcomes from the PK/PD code in OpenMalaria with LSTM's external
 * model. Numbers should agree (up to rounding errors of around 5e-3). */
class PkPdComplianceSuite : public CxxTest::TestSuite
{
public:
    PkPdComplianceSuite() :
            proxy(0)
    {
        genotype = 0;                // 0 should work; we definitely don't want random allocation
        bodymass = 50;
       
        cout << "\n[ Unittest Output Legend: \033[35mDrug Factor\033[0m, \033[36mDrug Concentration\033[0m ]" << endl;
    }
    
    void setUp () {
        UnittestUtil::initTime(1);
        UnittestUtil::PkPdSuiteSetup();
        proxy = new LSTMModel ();
        schedule.clear();
    }
    
    void tearDown () {
        delete proxy;
        LSTMDrugType::clear();
    }
    
    void assembleTripleDosageSchedule ( double dosage ){
        schedule.clear();
        schedule.insert(make_pair(0, make_pair(0, dosage)));
        schedule.insert(make_pair(1, make_pair(0, dosage)));
        schedule.insert(make_pair(2, make_pair(0, dosage)));
    }
    void assembleHexDosageSchedule ( double dosage ){
        schedule.clear();
        schedule.insert(make_pair(0, make_pair(0, dosage)));
        schedule.insert(make_pair(0, make_pair(0.5, dosage)));
        schedule.insert(make_pair(1, make_pair(0, dosage)));
        schedule.insert(make_pair(1, make_pair(0.5, dosage)));
        schedule.insert(make_pair(2, make_pair(0, dosage)));
        schedule.insert(make_pair(2, make_pair(0.5, dosage)));
    }
    
    void assembleVariableDosageSchedule ( ){
        //FIXME: 
        // the only case for CQ, which needs 10, 10, 5 dosages instead of constant dosages
        schedule.clear();
        schedule.insert(make_pair(0, make_pair(0, 10)));
        schedule.insert(make_pair(1, make_pair(0, 10)));
        schedule.insert(make_pair(2, make_pair(0, 5)));
    }
    
    void testDrugResults (string drugName, const double drug_conc[], const double drug_factors[]) {
        size_t drugIndex = LSTMDrugType::findDrug( drugName );
        cout << "\n\033[32mTesting \033[1m" << drugName << ": " << endl;
        double totalFac = 1;
        for( size_t i = 0; i < 6; i++){
            // before update (after last step):
            double fac = proxy->getDrugFactor(genotype);
            totalFac *= fac;
            cout << "\033[35m";
            TS_ASSERT_APPROX_TOL (totalFac, drug_factors[i], 5e-3, 1e-9);
            cout << "\033[0m";
            double errorF = totalFac - drug_factors[i];
            
            // update (two parts):
            UnittestUtil::incrTime(sim::oneDay());
            proxy->decayDrugs();
            
            // after update:
            double conc = proxy->getDrugConc(drugIndex);
            cout << "\033[36m";
            TS_ASSERT_APPROX_TOL (conc, drug_conc[i], 5e-3, 1e-9);
            cout << "\033[0m";
            double errorC = conc - drug_conc[i];
            cout << "\t Day " << i << ": " << \
            "\033[33mfactor (day 0 to " << i << "): " << totalFac << " [ " << errorF << " ]" << "\033[0m" << \
            ", \033[31mtotal / factor: " << totalFac / drug_factors[i] << "\033[0m" <<
            ", \033[32mconc: " << conc << " [ " << errorC << ", " << conc/drug_conc[i] << " ]" << "\033[0m" <<  endl;
            
            // medicate (take effect on next update):
            medicate( drugIndex, i );
        }
    }
    
    void medicate ( size_t drugIndex,  size_t i) {
            typedef multimap<size_t,pair<double, double> >::const_iterator iter;
            pair<iter, iter> doses_tmp = schedule.equal_range(i);
            for( iter it = doses_tmp.first; it != doses_tmp.second; it++){
                UnittestUtil::medicate( *proxy, drugIndex, it->second.second, it->second.first,
                        numeric_limits< double >::quiet_NaN(), bodymass );
            }
    }
    
    void testAR () {
        const double dose = 1.7 * bodymass;   // 1.7 mg/kg * 50 kg
        assembleHexDosageSchedule(dose);
        const double drug_conc[] = { 0.0, 0.0153520114,
            0.0156446685, 0.01560247467, 0.000298342, 5.68734e-6 };
        const double drug_factors[] = { 1,1.0339328333924E-012, 1.06887289270302E-024,
            1.10329635519261E-036, 4.73064069783747E-042, 4.73064069783747E-042 };
        testDrugResults("AR", drug_conc, drug_factors);
    }
    
    void testAS () {
        const double dose = 4 * bodymass;   // 4 mg/kg * 50 kg
        assembleTripleDosageSchedule(dose);
        const double drug_conc[] = { 0, 8.98E-008, 8.98E-008, 8.98E-008, 5.55E-015, 3.43E-022 };
        const double drug_factors[] = { 1, 0.000012, 1.45E-010, 1.75E-015,  1.75E-015, 1.75E-015 };
        testDrugResults("AS", drug_conc, drug_factors);
    }
    
    void _testCQ () { 
        //FIXME needs dosage 10, 10, 5. so third dose must be 5mg
        assembleVariableDosageSchedule();
        const double drug_conc[] = { 0, 0.0786272312, 0.1554589687, 0.2305362134, 0.2252717988, 0.2201276 };
        const double drug_factors[] = { 1, 0.0580181371, 0.0021554332, 7.39575036687833E-005, 2.51535706689484E-006, 8.57663220896078E-008 };
        testDrugResults("CQ", drug_conc, drug_factors);
    }
    
    void testDHA () {
        const double dose = 4 * bodymass;   // 4 mg/kg * 50 kg
        assembleTripleDosageSchedule(dose);
        const double drug_conc[] = { 0, 6.76E-009, 6.76E-009, 6.76E-009, 1.7E-017, 4.28E-026 };
        const double drug_factors[] = { 1, 0.000355234, 0.000000126, 4.48E-011, 4.48E-011, 4.48E-011 };
        testDrugResults("DHA", drug_conc, drug_factors);
    }
    
    void testLF () {
        const double dose = 12 * bodymass;   // 12 mg/kg * 50 kg
        assembleHexDosageSchedule(dose);
        const double drug_conc[] = { 0, 1.014434363, 1.878878305, 2.615508841, 2.228789614, 1.899249226 };
        const double drug_factors[] = { 1, 0.031746317, 0.001007809, 0.000032, 0.00000102, 3.22E-008 };
        testDrugResults("LF", drug_conc, drug_factors);
    }
    
    void testMQ () {
        const double dose = 8.3 * bodymass;   // 8.3 mg/kg * 50 kg
        assembleTripleDosageSchedule( dose) ;
        const double drug_conc[] = { 0, 0.378440101, 0.737345129, 1.077723484,
                1.022091411, 0.969331065 };
        const double drug_factors[] = { 1, 0.031745814, 0.001007791, 0.000032,
                1.02e-6, 3.22E-008 };
        testDrugResults("MQ", drug_conc, drug_factors);
    }
    
    void testPPQ (){
        const double dose = 18 * bodymass;   // 18 mg/kg * 50 kg
        assembleTripleDosageSchedule( dose );
        const double drug_conc[] = { 0, 0.116453464, 0.2294652081, 0.339137, 0.3291139387, 0.3193871518 };
        //TODO: these numbers are wrong
        const double drug_factors[] = { 1, 0.9999797, 0.9987926, 0.9865442, 0.9764225, 0.9680462 };
        testDrugResults("PPQ", drug_conc, drug_factors);
    }
    
private:
    LSTMModel *proxy;
    uint32_t genotype;
    double bodymass;
    std::multimap<size_t, pair<double, double> > schedule; // < day, pair < part of day, dosage > >
};

#endif