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
	HeavyCD2 = NPL::EnergyLoss(heavy+"_"+TargetMaterial+".G4table","G4Table",100);

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
	ThetaGDSurface = 0;
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
			Energy = Si_E_M2;


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
		TVector3 HitDirection = MG -> GetPositionOfInteraction(countMugast) - BeamImpact ;
                
		//ThetaLab.push_back( HitDirection.Angle( BeamDirection ));
		ThetaLab.push_back( HitDirection.Theta());
                PhiLab.push_back(HitDirection.Phi());
		
		X.push_back(  MG -> GetPositionOfInteraction(countMugast).X());
		Y.push_back(  MG -> GetPositionOfInteraction(countMugast).Y());
		Z.push_back(  MG -> GetPositionOfInteraction(countMugast).Z());

		ThetaMGSurface = HitDirection.Angle( TVector3(0,0,1) ) ;
		ThetaNormalTarget = HitDirection.Angle( TVector3(0,0,1) ) ;


		// Part 2 : Impact Energy
		Energy = 0;
		Energy = MG->GetEnergyDeposit(countMugast);
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
                
                double thetah_tmp;
                double Eh_tmp;

                double thetal_tmp;
                double El_tmp;
                
                myReaction.SetNuclei3(ELab.back(),ThetaLab.back()*deg); 
                myReaction.KineRelativistic(thetal_tmp, El_tmp,thetah_tmp, Eh_tmp);
                
                ThetaHeavy.push_back(thetah_tmp/deg);
		EheavyAfterTg.push_back(HeavyCD2.Slow(Eh_tmp,TargetThickness*0.5,thetah_tmp));
       

	}//end loop Mugast

	////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// LOOP on AGATA ////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	// Agata by track
	
	//position evaluated with the lifetime of first 17O Ex state and its velocity
        /*
       	TVector3 GammaEmission(0,0,0.557); 
	
	double agata_zShift=5.1;
	
	for(int j=0; j<nbTrack; j++){ // all multiplicity
		
		TLorentzVector GammaLV;
		
		// Measured E
		double Egamma=trackE[j]/1000.; // From keV to MeV 
		
		// Gamma detection position
		// TrackZ1 to be corrected there is a shift of +51mm
		TVector3 GammaHit(trackX1[j],trackY1[j],trackZ1[j]+agata_zShift); 
		//TVector3 GammaHit(trackX1[0],trackY1[0],trackZ1[0]); 
		
		// Gamma Direction 
		TVector3 GammaDirection = GammaHit-(BeamImpact+GammaEmission);
		GammaDirection = GammaDirection.Unit();
		
		// Beta from Two body kinematic
		TVector3 beta = myReaction.GetEnergyImpulsionLab_4().BoostVector();
		
		// Beta from the Beam mid target 
		//reaction.GetKinematicLine4();
		//TVector3 beta(0,0,-reaction.GetNucleus4()->GetBeta());

		double ThetaGamma = GammaDirection.Angle(BeamDirection)/deg;
		// Construct LV in lab frame
		GammaLV.SetPx(Egamma*GammaDirection.X());
		GammaLV.SetPy(Egamma*GammaDirection.Y());
		GammaLV.SetPz(Egamma*GammaDirection.Z());
		GammaLV.SetE(Egamma);
		// Boost back in CM
		GammaLV.Boost(beta);
		// Get EDC
		EDC.push_back(GammaLV.Energy());
	}


	// Agata add back is not always multiplicity 1 ?? NO, not necessarily!
	if(nbAdd==1){
		TLorentzVector GammaLV;
		// Measured E
		double Egamma=AddE[0]/1000.; // From keV to MeV 
		// Gamma detection position
		// TrackZ1 to be corrected there is a shift of +51mm
		TVector3 GammaHit(AddX[0],AddY[0],AddZ[0]+agata_zShift); 
		// TVector3 GammaHit(trackX1[0],trackY1[0],trackZ1[0]); 
		// Gamma Direction 
		TVector3 GammaDirection = GammaHit-BeamImpact;
		GammaDirection = GammaDirection.Unit();
		// Beta from Two body kinematic
		//TVector3 beta = reaction.GetEnergyImpulsionLab_4().BoostVector();
		// Beta from the Beam mid target 
		reaction.GetKinematicLine4();
		// TVector3 beta(0,0,-reaction.GetNucleus4()->GetBeta());
		TVector3 beta(0,0,-0.132);

		double ThetaGamma = GammaDirection.Angle(BeamDirection)/deg;
		// Construct LV in lab frame
		GammaLV.SetPx(Egamma*GammaDirection.X());
		GammaLV.SetPy(Egamma*GammaDirection.Y());
		GammaLV.SetPz(Egamma*GammaDirection.Z());
		GammaLV.SetE(Egamma);
		// Boost back in CM
		GammaLV.Boost(beta);
		// Get EDC
		AddBack_EDC = GammaLV.Energy();
	}
	*/

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
	RootOutput::getInstance()->GetTree()->Branch("ThetaHeavy",&ThetaHeavy);
	RootOutput::getInstance()->GetTree()->Branch("EheavyAfterTg",&EheavyAfterTg);

	RootOutput::getInstance()->GetTree()->Branch("Run",&Run,"Run/I");
	RootOutput::getInstance()->GetTree()->Branch("LTS",&LTS,"LTS/l");

	// Agata
	// Time stamp of the agata trigger

	RootOutput::getInstance()->GetTree()->Branch("TStrack",&TStrack,"TStrack/l");

	// Array of reconstructed tracks
	RootOutput::getInstance()->GetTree()->Branch("nbTrack",&nbTrack,"nbTrack/I");
	RootOutput::getInstance()->GetTree()->Branch("trackE",trackE,"trackE[nbTrack]/F");
	RootOutput::getInstance()->GetTree()->Branch("trackX1",trackX1,"trackX1[nbTrack]/F");
	RootOutput::getInstance()->GetTree()->Branch("trackY1",trackY1,"trackY1[nbTrack]/F");
	RootOutput::getInstance()->GetTree()->Branch("trackZ1",trackZ1,"trackZ1[nbTrack]/F");
	RootOutput::getInstance()->GetTree()->Branch("trackT",trackT,"trackT[nbTrack]/F");
	RootOutput::getInstance()->GetTree()->Branch("trackCrystalID",trackCrystalID,"trackCrystalID[nbTrack]/I");

	//Agata Addback
	RootOutput::getInstance()->GetTree()->Branch("nbAdd",&nbAdd,"nbAdd/I");
	RootOutput::getInstance()->GetTree()->Branch("TSHit",&TSHit,"TSHit/l");
	RootOutput::getInstance()->GetTree()->Branch("AddE",AddE,"AddE[nbAdd]/F");
	RootOutput::getInstance()->GetTree()->Branch("AddX",AddX,"AddX[nbAdd]/F");
	RootOutput::getInstance()->GetTree()->Branch("AddY",AddY,"AddY[nbAdd]/F");
	RootOutput::getInstance()->GetTree()->Branch("AddZ",AddZ,"AddZ[nbAdd]/F");
} 


////////////////////////////////////////////////////////////////////////////////
void Analysis::InitInputBranch(){
	//Agata
	RootInput::getInstance()->GetChain()->SetBranchAddress("LTS",&LTS);
	RootInput::getInstance()->GetChain()->SetBranchAddress("TStrack",&TStrack);
	RootInput::getInstance()->GetChain()->SetBranchAddress("nbTrack",&nbTrack);
	RootInput::getInstance()->GetChain()->SetBranchAddress("trackE",trackE);
	RootInput::getInstance()->GetChain()->SetBranchAddress("trackX1",trackX1);
	RootInput::getInstance()->GetChain()->SetBranchAddress("trackY1",trackY1);
	RootInput::getInstance()->GetChain()->SetBranchAddress("trackZ1",trackZ1);
	RootInput::getInstance()->GetChain()->SetBranchAddress("trackT",trackT);
	RootInput::getInstance()->GetChain()->SetBranchAddress("trackCrystalID",trackCrystalID);
	RootInput::getInstance()->GetChain()->SetBranchAddress("TSHit",&TSHit);
	RootInput::getInstance()->GetChain()->SetBranchAddress("nbAdd",&nbAdd);
	RootInput::getInstance()->GetChain()->SetBranchAddress("AddE",AddE);
	RootInput::getInstance()->GetChain()->SetBranchAddress("AddX",AddX);
	RootInput::getInstance()->GetChain()->SetBranchAddress("AddY",AddY);
	RootInput::getInstance()->GetChain()->SetBranchAddress("AddZ",AddZ);
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
        ThetaHeavy.clear();
        EheavyAfterTg.clear();
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

