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
	// get MUST2 and Gaspard objects
	M2 = (TMust2Physics*)  m_DetectorManager -> GetDetector("M2Telescope");
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
	string heavy=NPL::ChangeNameToG4Standard(myReaction.GetNucleus4()->GetName());
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
	ThetaM2Surface = 0;
	ThetaMGSurface = 0;
	Si_E_M2 = 0;
	CsI_E_M2 = 0;
	BeamDirection = TVector3(0,0,1);
}

////////////////////////////////////////////////////////////////////////////////
void Analysis::TreatEvent() {
	// Reinitiate calculated variable
	ReInitValue();
	//double zImpact = Rand.Uniform(-TargetThickness*0.5,TargetThickness*0.5);
	//trackE_tmp=trackE;

	double zImpact = 0 ;
	BeamImpact = TVector3(0,0,zImpact);
	// determine beam energy for a randomized interaction point in target
	// 1% FWHM randominastion (E/100)/2.35
	//myReaction.SetBeamEnergy(Rand.Gaus(myInit->GetIncidentFinalKineticEnergy(),myInit->GetIncidentFinalKineticEnergy()/235));

	//Particle multiplicity
	nbParticleM2 =  M2->Si_E.size();
	nbParticleMG =  MG->DSSD_E.size();

	//////////////////////////// LOOP on MUST2 //////////////////
	for(unsigned int countMust2 = 0 ; countMust2 < M2->Si_E.size() ; countMust2++){
		/************************************************/
		//Part 0 : Get the usefull Data
		// MUST2
		int TelescopeNumber = M2->TelescopeNumber[countMust2];

		/************************************************/
		// Part 1 : Impact Angle
		ThetaM2Surface = 0;
		ThetaNormalTarget = 0;
		TVector3 HitDirection = M2 -> GetPositionOfInteraction(countMust2) - BeamImpact ;
		//ThetaLab.push_back( HitDirection.Angle( BeamDirection ));
		ThetaLab.push_back(HitDirection.Theta());
    PhiLab.push_back(HitDirection.Phi());

		X.push_back( M2 -> GetPositionOfInteraction(countMust2).X());
		Y.push_back( M2 -> GetPositionOfInteraction(countMust2).Y());
		Z.push_back( M2 -> GetPositionOfInteraction(countMust2).Z());

		ThetaM2Surface = HitDirection.Angle(- M2 -> GetTelescopeNormal(countMust2) );
		ThetaNormalTarget = HitDirection.Angle( TVector3(0,0,1) ) ;

		/************************************************/

		/************************************************/
		// Part 2 : Impact Energy
		Energy = 0;
		Si_E_M2 = M2->Si_E[countMust2];
		CsI_E_M2= M2->CsI_E[countMust2];

		//E-dE must retrieving
		dE.push_back(Si_E_M2);
		E.push_back(CsI_E_M2);

		// if CsI
		if(CsI_E_M2>0 ){
			// The energy in CsI is calculate form dE/dx Table because
			Energy = CsI_E_M2;
			Energy = LightAl.EvaluateInitialEnergy( Energy ,0.4*micrometer , ThetaM2Surface);
			Energy+=Si_E_M2;
		}

		else
			Energy= Si_E_M2;


		// Evaluate energy using the thickness //// REPEATED???
		Energy = LightAl.EvaluateInitialEnergy( Energy ,0.4*micrometer , ThetaM2Surface);
		// Target Correction
		ELab.push_back( LightTarget.EvaluateInitialEnergy( Energy,TargetThickness/2.-zImpact, ThetaNormalTarget));


		/************************************************/
		// Part 3 : Excitation Energy Calculation
		Ex.push_back( myReaction.ReconstructRelativistic( ELab.back() , ThetaLab.back()));

		/************************************************/

		/************************************************/
		// Part 4 : Theta CM Calculation
		ThetaCM.push_back( myReaction.EnergyLabToThetaCM( ELab.back() , ThetaLab.back()));
		/************************************************/

		ThetaLab.back()=ThetaLab.back()/deg;
		PhiLab.back()=PhiLab.back()/deg;
		ThetaCM.back()=ThetaCM.back()/deg;
	}//end loop MUST2

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	//////////////////////////// LOOP on MUGAST //////////////////


	for(unsigned int countMugast = 0 ; countMugast < MG->DSSD_E.size() ; countMugast++){

		// Part 1 : Impact Angle
		ThetaMGSurface = 0;
		ThetaNormalTarget = 0;

		TVector3 Inter(MG->PosX[countMugast],MG->PosY[countMugast],MG->PosZ[countMugast]);

		//TVector3 HitDirection = MG -> GetPositionOfInteraction(countMugast) - BeamImpact ;
		TVector3 HitDirection = Inter - BeamImpact ;


		//ThetaLab.push_back( HitDirection.Angle( BeamDirection ));
		ThetaLab.push_back(HitDirection.Theta());
      PhiLab.push_back(HitDirection.Phi());

		X.push_back(  MG -> GetPositionOfInteraction(countMugast).X());
		Y.push_back(  MG -> GetPositionOfInteraction(countMugast).Y());
		Z.push_back(  MG -> GetPositionOfInteraction(countMugast).Z());
    //if(MG->TelescopeNumber[countMugast]==11) cout<<"X-Y-Z --->>> "<<X.back()<<" "<<Y.back()<<" "<<Z.back()<<" "<<endl<<endl;
		ThetaMGSurface = HitDirection.Angle( TVector3(0,0,1) ) ;
		ThetaNormalTarget = HitDirection.Angle( TVector3(0,0,1) ) ;
		ThetaMGSurface = HitDirection.Angle(- MG -> GetTelescopeNormal(countMugast) );

		// Part 2 : Impact Energy
		Energy = 0;
		Energy = MG->GetEnergyDeposit(countMugast);
		Energy = LightAl.EvaluateInitialEnergy( Energy ,0.4*micrometer , ThetaMGSurface);


		// Target Correction
		ELab.push_back( LightTarget.EvaluateInitialEnergy( Energy ,TargetThickness*0.5-zImpact, ThetaNormalTarget));

		/************************************************/
		// Part 3 : Excitation Energy Calculation
		Ex.push_back( myReaction.ReconstructRelativistic( ELab.back() , ThetaLab.back()));

		/************************************************/

		/************************************************/
		// Part 4 : Theta CM Calculation
		ThetaCM.push_back( myReaction.EnergyLabToThetaCM( ELab.back() , ThetaLab.back()));
		/************************************************/

		ThetaLab.back()=ThetaLab.back()/deg;
		PhiLab.back()=PhiLab.back()/deg;
		ThetaCM.back()=ThetaCM.back()/deg;

	}//end loop Mugast


}

////////////////////////////////////////////////////////////////////////////////
void Analysis::End(){
}
////////////////////////////////////////////////////////////////////////////////
void Analysis::InitOutputBranch() {
	RootOutput::getInstance()->GetTree()->Branch("Ex",&Ex);
	RootOutput::getInstance()->GetTree()->Branch("ELab",&ELab);
	RootOutput::getInstance()->GetTree()->Branch("ThetaLab",&ThetaLab);
	RootOutput::getInstance()->GetTree()->Branch("PhiLab",&PhiLab);
	RootOutput::getInstance()->GetTree()->Branch("ThetaCM",&ThetaCM);
	RootOutput::getInstance()->GetTree()->Branch("X",&X);
	RootOutput::getInstance()->GetTree()->Branch("Y",&Y);
	RootOutput::getInstance()->GetTree()->Branch("Z",&Z);
	RootOutput::getInstance()->GetTree()->Branch("E",&E);
	RootOutput::getInstance()->GetTree()->Branch("dE",&dE);
	RootOutput::getInstance()->GetTree()->Branch("nbParticleM2",&nbParticleM2,"nbParticleM2/I");
	RootOutput::getInstance()->GetTree()->Branch("nbParticleMG",&nbParticleMG,"nbParticleMG/I");

	RootOutput::getInstance()->GetTree()->Branch("Run",&Run,"Run/I");
}


////////////////////////////////////////////////////////////////////////////////
void Analysis::InitInputBranch(){

}
////////////////////////////////////////////////////////////////////////////////
void Analysis::ReInitValue(){
	Ex.clear() ;
	ELab.clear();
	ThetaLab.clear();
	PhiLab.clear();
	ThetaCM.clear();
	X.clear();
	Y.clear();
	Z.clear();
	E.clear();
	dE.clear();
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
