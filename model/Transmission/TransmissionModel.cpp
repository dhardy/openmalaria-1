/*

 This file is part of OpenMalaria.

 Copyright (C) 2005,2006,2007,2008 Swiss Tropical Institute and Liverpool School Of Tropical Medicine

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
#include "Transmission/TransmissionModel.h"
#include "Transmission/NonVector.h"
#include "Transmission/Vector/VectorTransmission.h"
#include "Transmission/PerHostTransmission.h"

#include "inputData.h"
#include "Monitoring/Continuous.h"
#include "util/BoincWrapper.h"
#include "util/StreamValidator.h"
#include "util/CommandLine.h"
#include "util/vectors.h"

#include <cmath>
#include <cfloat>
#include <gsl/gsl_vector.h>

namespace OM { namespace Transmission {
namespace vectors = util::vectors;

TransmissionModel* TransmissionModel::createTransmissionModel (int populationSize) {
  // EntoData contains either a list of at least one anopheles or a list of at
  // least one EIRDaily.
  const scnXml::EntoData& entoData = InputData().getEntomology();
  const scnXml::EntoData::VectorOptional& vectorData = entoData.getVector();

  TransmissionModel *model;
  if (vectorData.present())
    model = new VectorTransmission(vectorData.get(), populationSize);
  else {
      const scnXml::EntoData::NonVectorOptional& nonVectorData = entoData.getNonVector();
    if (!nonVectorData.present())       // should be a validation error, but anyway...
      throw util::xml_scenario_error ("Neither vector nor non-vector data present in the XML!");
    model = new NonVectorTransmission(nonVectorData.get());
  }

  if( entoData.getAnnualEIR().present() ){
      model->scaleEIR( entoData.getAnnualEIR().get() / model->annualEIR );
      assert( vectors::approxEqual( model->annualEIR, entoData.getAnnualEIR().get() ) );
  }

  if( util::CommandLine::option( util::CommandLine::PRINT_ANNUAL_EIR ) ){
      //Note: after internal scaling (which doesn't imply exit)
      //but before external scaling.
      cout << "Total annual EIR: "<<model->annualEIR<<endl;
  }
  if( util::CommandLine::option( util::CommandLine::SET_ANNUAL_EIR ) ){
      model->scaleXML_EIR(
        InputData.getMutableScenario().getEntomology(),
        util::CommandLine::getNewEIR() / model->annualEIR
      );
      InputData.documentChanged = true;
  }

  return model;
}


// The times here should be for the last updated index of arrays:
void TransmissionModel::ctsCbInputEIR (ostream& stream){
    stream<<'\t'<<initialisationEIR[TimeStep::simulation % TimeStep::stepsPerYear];
}
void TransmissionModel::ctsCbSimulatedEIR (ostream& stream){
    stream<<'\t'<<lastTsAdultEIR;
}
void TransmissionModel::ctsCbKappa (ostream& stream){
    // updateKappa() has been called for this TimeStep::simulation, so this is the latest value in laggedKappa:
    stream<<'\t'<<laggedKappa[TimeStep::simulation % laggedKappa.size()];
}
void TransmissionModel::ctsCbNumTransmittingHumans (ostream& stream){
    stream<<'\t'<<numTransmittingHumans;
}

TransmissionModel::TransmissionModel() :
    simulationMode(equilibriumMode),
    interventionMode(InputData().getEntomology().getMode()),
    laggedKappa(1, 0.0),        // if using non-vector model, it will resize this
    annualEIR(0.0),
    _annualAverageKappa(numeric_limits<double>::signaling_NaN()),
    _sumAnnualKappa(0.0),
    adultAge(PerHostTransmission::adultAge()),
    tsAdultEntoInocs(0.0),
    lastTsAdultEIR(0.0),
    surveyInputEIR(0.0),
    surveySimulatedEIR(0.0),
    numTransmittingHumans(0),
    tsNumAdults(0),
    timeStepNumEntoInocs (0)
{
    if (interventionMode != equilibriumMode && interventionMode != dynamicEIR){
        // Note: previously 3 was allowed -- but mode is set to 3 anyway when
        // "intervention" EIR data is loaded, so 2 or 4 should be used here.
        throw util::xml_scenario_error("mode attribute has invalid value (expected: 2 or 4)");
    }

  initialisationEIR.assign (TimeStep::stepsPerYear, 0.0);
  inoculationsPerAgeGroup.assign (Monitoring::AgeGroup::getNumGroups(), 0.0);
  timeStepEntoInocs.assign (Monitoring::AgeGroup::getNumGroups(), 0.0);

  // noOfAgeGroupsSharedMem must be at least as large as both of these to avoid
  // memory corruption or extra tests when setting/copying values
  noOfAgeGroupsSharedMem = std::max(Monitoring::AgeGroup::getNumGroups(), util::SharedGraphics::KappaArraySize);

  using Monitoring::Continuous;
  Continuous::registerCallback( "input EIR", "\tinput EIR", MakeDelegate( this, &TransmissionModel::ctsCbInputEIR ) );
  Continuous::registerCallback( "simulated EIR", "\tsimulated EIR", MakeDelegate( this, &TransmissionModel::ctsCbSimulatedEIR ) );
  Continuous::registerCallback( "human infectiousness", "\thuman infectiousness", MakeDelegate( this, &TransmissionModel::ctsCbKappa ) );
  Continuous::registerCallback( "num transmitting humans", "\tnum transmitting humans", MakeDelegate( this, &TransmissionModel::ctsCbNumTransmittingHumans ) );
}

TransmissionModel::~TransmissionModel () {
}


void TransmissionModel::updateKappa (const std::list<Host::Human>& population) {
    // We calculate kappa for output and non-vector model, and kappaByAge for
    // the shared graphics.

    double sumWt_kappa= 0.0;
    double sumWeight  = 0.0;
    kappaByAge.assign (noOfAgeGroupsSharedMem, 0.0);
    nByAge.assign (noOfAgeGroupsSharedMem, 0);
    numTransmittingHumans = 0;

    for (std::list<Host::Human>::const_iterator h = population.begin(); h != population.end(); ++h) {
        double t = h->perHostTransmission.relativeAvailabilityHetAge(h->getAgeInYears());
        sumWeight += t;
        t *= h->probTransmissionToMosquito();
        sumWt_kappa += t;
        if( t > 0.0 )
            ++numTransmittingHumans;

        // kappaByAge and nByAge are used in the screensaver only
        Monitoring::AgeGroup ag = h->ageGroup();
        kappaByAge[ag.i()] += t;
        ++nByAge[ag.i()];
    }


    int tmod = TimeStep::simulation % TimeStep::stepsPerYear;
    int t1mod = (TimeStep::simulation-TimeStep(1)) % TimeStep::stepsPerYear;
    size_t lKMod = TimeStep::simulation % laggedKappa.size();
    if( population.empty() ){     // this is valid
        laggedKappa[lKMod] = 0.0;        // no humans: no infectiousness
    } else {
        if ( !(sumWeight > DBL_MIN * 10.0) ){       // if approx. eq. 0, negative or an NaN
            ostringstream msg;
            msg<<"sumWeight is invalid: "<<sumWeight<<", "<<sumWt_kappa<<", "<<population.size();
            throw util::traced_exception(msg.str(),util::Error::SumWeight);
        }
        laggedKappa[lKMod] = sumWt_kappa / sumWeight;
    }

    //Calculate time-weighted average of kappa
    _sumAnnualKappa += laggedKappa[lKMod] * initialisationEIR[t1mod];
    if (tmod == 0) {
        // if annualEIR == 0.0 (or an NaN), we just get some nonsense output like inf or nan.
        // This is a better solution than printing a warning no-one will see and outputting 0.
        _annualAverageKappa = _sumAnnualKappa / annualEIR;
        _sumAnnualKappa = 0.0;
    }

    // Shared graphics: report infectiousness
    if (TimeStep::simulation % 6 ==  0) {
        for (size_t i = 0; i < noOfAgeGroupsSharedMem; i++)
            kappaByAge[i] /= nByAge[i];
        util::SharedGraphics::copyKappa(&kappaByAge[0]);
    }

    // Sum up inoculations this timestep
    for (size_t group = 0; group < timeStepEntoInocs.size(); ++group) {
        inoculationsPerAgeGroup[group] += timeStepEntoInocs[group];
        // Reset to zero:
        timeStepEntoInocs[group] = 0.0;
    }
    timeStepNumEntoInocs = 0;

    lastTsAdultEIR = tsAdultEntoInocs / tsNumAdults;
    tsAdultEntoInocs = 0.0;
    tsNumAdults = 0;

    surveyInputEIR += initialisationEIR[tmod];
    surveySimulatedEIR += lastTsAdultEIR;
    modelUpdateKappa(laggedKappa[lKMod]);
}

double TransmissionModel::getEIR (OM::Transmission::PerHostTransmission& host, double ageYears, OM::Monitoring::AgeGroup ageGroup) {
  /* For the NonVector model, the EIR should just be multiplied by the
   * availability. For the Vector model, the availability is also required
   * for internal calculations, but again the EIR should be multiplied by the
   * availability. */
  double EIR = calculateEIR (host, ageYears);

  //NOTE: timeStep*EntoInocs will rarely be used despite frequent updates here
  timeStepEntoInocs[ageGroup.i()] += EIR;
  timeStepNumEntoInocs ++;
  if( ageYears >= adultAge ){
     tsAdultEntoInocs += EIR;
     tsNumAdults += 1;
  }
  util::streamValidate( EIR );
  return EIR;
}

void TransmissionModel::summarize (Monitoring::Survey& survey) {
  survey.setInfectiousnessToMosq(laggedKappa[TimeStep::simulation % laggedKappa.size()]);
  survey.setAnnualAverageKappa(_annualAverageKappa);

  survey.setInoculationsPerAgeGroup (inoculationsPerAgeGroup);        // Array contents must be copied.
  inoculationsPerAgeGroup.assign (inoculationsPerAgeGroup.size(), 0.0);

  double duration = (TimeStep::simulation-lastSurveyTime).asInt();
  if( duration == 0.0 ){
      if( !( surveyInputEIR == 0.0 && surveySimulatedEIR == 0.0 ) ){
          throw util::traced_exception( "non-zero EIR over zero duration??" );
      }
      duration = 1.0;   // avoid outputting NaNs. 0 isn't quite correct, but should do.
  }
  survey.set_Vector_EIR_Input (surveyInputEIR / duration);
  survey.set_Vector_EIR_Simulated (surveySimulatedEIR / duration);

  surveyInputEIR = 0.0;
  surveySimulatedEIR = 0.0;
  lastSurveyTime = TimeStep::simulation;
}

const char* viError = "vector interventions require vector model";
void TransmissionModel::setITNDescription (const scnXml::ITNDescription&) {
  throw util::xml_scenario_error (viError);
}
void TransmissionModel::setIRSDescription (const scnXml::IRS&) {
  throw util::xml_scenario_error (viError);
}
void TransmissionModel::setVADescription (const scnXml::VectorDeterrent&) {
  throw util::xml_scenario_error (viError);
}
void TransmissionModel::intervLarviciding (const scnXml::Larviciding&) {
  throw util::xml_scenario_error (viError);
}


// -----  checkpointing  -----

void TransmissionModel::checkpoint (istream& stream) {
    simulationMode & stream;
    interventionMode & stream;
    initialisationEIR & stream;
    laggedKappa & stream;
    annualEIR & stream;
    _annualAverageKappa & stream;
    _sumAnnualKappa & stream;
    adultAge & stream;
    tsAdultEntoInocs & stream;
    lastTsAdultEIR & stream;
    surveyInputEIR & stream;
    surveySimulatedEIR & stream;
    lastSurveyTime & stream;
    numTransmittingHumans & stream;
    tsNumAdults & stream;
    inoculationsPerAgeGroup & stream;
    timeStepEntoInocs & stream;
    timeStepNumEntoInocs & stream;
    noOfAgeGroupsSharedMem & stream;
    kappaByAge & stream;
    nByAge & stream;
}
void TransmissionModel::checkpoint (ostream& stream) {
    simulationMode & stream;
    interventionMode & stream;
    initialisationEIR & stream;
    laggedKappa & stream;
    annualEIR & stream;
    _annualAverageKappa & stream;
    _sumAnnualKappa & stream;
    adultAge & stream;
    tsAdultEntoInocs & stream;
    lastTsAdultEIR & stream;
    surveyInputEIR & stream;
    surveySimulatedEIR & stream;
    lastSurveyTime & stream;
    numTransmittingHumans & stream;
    tsNumAdults & stream;
    inoculationsPerAgeGroup & stream;
    timeStepEntoInocs & stream;
    timeStepNumEntoInocs & stream;
    noOfAgeGroupsSharedMem & stream;
    kappaByAge & stream;
    nByAge & stream;
}

} }

