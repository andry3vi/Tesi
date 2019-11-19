/*****************************************************************************
 * Copyright (C) 2009-2014    this file is part of the NPTool Project        *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: Adrien MATTA  contact address: a.matta@surrey.ac.uk      *
 *                                                                           *
 * Creation Date  : march 2025                                               *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 * Class describing the property of an Analysis object                       *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
#include<iostream>
using namespace std;

#include "Analysis.h"
#include "NPFunction.h"
#include "NPAnalysisFactory.h"
#include "NPDetectorManager.h"
#include "NPOptionManager.h"
////////////////////////////////////////////////////////////////////////////////
Analysis::Analysis(){
}
////////////////////////////////////////////////////////////////////////////////
Analysis::~Analysis(){
}

////////////////////////////////////////////////////////////////////////////////
void Analysis::Init() {
	// initialize input and output branches
	InitOutputBranch();
	InitInputBranch();
	myInit = new TInitialConditions();
	// get Mugast objects
	MG = (TMugastPhysics*) m_DetectorManager -> GetDetector("Mugast");

	// get reaction information
	myReaction.ReadConfigurationFile(NPOptionManager::getInstance()->GetReactionFile());
	OriginalBeamEnergy = myReaction.GetBeamEnergy();

	// target thickness
	TargetThickness = m_DetectorManager->GetTargetThickness();
	string TargetMaterial = m_DetectorManager->GetTargetMaterial();

	// Cryo target case
	WindowsThickness = 0;//m_DetectorManager->GetWindowsThickness();
	string WindowsMaterial = "";//m_DetectorManager->GetWindowsMaterial();

	// energy losses
	string light=NPL::ChangeNameToG4Standard(myReaction.GetNucleus3()->GetName());
	string beam=NPL::ChangeNameToG4Standard(myReaction.GetNucleus1()->GetName());
	LightTarget = NPL::EnergyLoss(light+"_"+TargetMaterial+".G4table","G4Table",100 );
	LightAl = NPL::EnergyLoss(light+"_Al.G4table","G4Table",100);
	LightSi = NPL::EnergyLoss(light+"_Si.G4table","G4Table",100);
	BeamCD2 = NPL::EnergyLoss(beam+"_"+TargetMaterial+".G4table","G4Table",100);

	if(WindowsThickness){
		BeamWindow= new NPL::EnergyLoss(beam+"_"+WindowsMaterial+".G4table","G4Table",100);
		LightWindow=  new NPL::EnergyLoss(light+"_"+WindowsMaterial+".G4table","G4Table",100);
	}

	else{
		BeamWindow= NULL;
		LightWindow=NULL;
	}

	//set beam energy at mid target
	FinalBeamEnergy = BeamCD2.Slow(OriginalBeamEnergy,TargetThickness*0.5,0);
	myReaction.SetBeamEnergy(FinalBeamEnergy);

	// initialize various parameters
	Rand = TRandom3();
	DetectorNumber = 0;
	ThetaNormalTarget = 0;
	ThetaMGSurface = 0;
	BeamDirection = TVector3(0,0,1);


	//target thickness scan definition
	for (size_t i = -1.0; i <= 1.0 ; i+=0.1) {
		Target_dThickness.push_back(i);
	}
}

////////////////////////////////////////////////////////////////////////////////
void Analysis::TreatEvent() {

	// Reinitiate calculated variable
	ReInitValue();

	double zImpact = 0 ;
	BeamImpact = TVector3(0,0,zImpact);

	// determine beam energy for a randomized interaction point in target
	// 1% FWHM randominastion (E/100)/2.35
	//myReaction.SetBeamEnergy(Rand.Gaus(myInit->GetIncidentFinalKineticEnergy(),myInit->GetIncidentFinalKineticEnergy()/235));

	//Particle multiplicity
	nbParticleMG =  MG->DSSD_E.size();
	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	//////////////////////////// LOOP on MUGAST //////////////////


	for(unsigned int countMugast = 0 ; countMugast < MG->DSSD_E.size() ; countMugast++){

		std::vector<double> Ex_event;
		Ex_event.reserve(Target_dThickness.size());

		TVector3 HitDirection = MG -> GetPositionOfInteraction(countMugast) - BeamImpact ;

		ThetaLab.push_back(HitDirection.Theta());
    PhiLab.push_back(HitDirection.Phi());

		ThetaMGSurface = HitDirection.Angle( TVector3(0,0,1) ) ;
		ThetaNormalTarget = HitDirection.Angle( TVector3(0,0,1) ) ;

		// Part 2 : Impact Energy
		Energy = 0;
		Energy = MG->GetEnergyDeposit(countMugast);

		for (auto& dthick : Target_dThickness) {

					EffectiveThickness = TargetThickness + dthick;

					FinalBeamEnergy = BeamCD2.Slow(OriginalBeamEnergy,EffectiveThickness*0.5,0);
					myReaction.SetBeamEnergy(FinalBeamEnergy);

					// Target Correction
					double ELab = LightTarget.EvaluateInitialEnergy( Energy ,EffectiveThickness*0.5-zImpact, ThetaNormalTarget));

					// Excitation Energy Calculation
					Ex_event.push_back( myReaction.ReconstructRelativistic( ELab , ThetaLab.back()));

		}

		Ex.push_back(Ex_event);

	}//end loop Mugast
}

////////////////////////////////////////////////////////////////////////////////
void Analysis::End(){
}
////////////////////////////////////////////////////////////////////////////////
void Analysis::InitOutputBranch() {
	RootOutput::getInstance()->GetTree()->Branch("Ex",&Ex);
	RootOutput::getInstance()->GetTree()->Branch("Target_dThickness",&Target_dThickness);
  RootOutput::getInstance()->GetTree()->Branch("nbParticleMG",&nbParticleMG,"nbParticleMG/I");
}


////////////////////////////////////////////////////////////////////////////////
void Analysis::InitInputBranch(){
}
////////////////////////////////////////////////////////////////////////////////
void Analysis::ReInitValue(){
	Ex.clear() ;
	ThetaLab.clear();
	PhiLab.clear();

}


////////////////////////////////////////////////////////////////////////////////
//            Construct Method to be pass to the AnalysisFactory              //
////////////////////////////////////////////////////////////////////////////////
NPL::VAnalysis* Analysis::Construct(){
	return (NPL::VAnalysis*) new Analysis();
}

////////////////////////////////////////////////////////////////////////////////
//            Registering the construct method to the factory                 //
////////////////////////////////////////////////////////////////////////////////
extern "C"{
	class proxy_analysis{
		public:
			proxy_analysis(){
				NPL::AnalysisFactory::getInstance()->SetConstructor(Analysis::Construct);
			}
	};

	proxy_analysis p_analysis;
}
