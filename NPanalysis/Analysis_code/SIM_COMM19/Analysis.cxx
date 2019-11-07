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
#include <fstream>
#include <iostream>

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
	GDtracker = (GaspardTracker*) m_DetectorManager -> GetDetector("GaspardTracker");
	GD = (TGaspardTrackerPhysics*) GDtracker->GetEventPhysics();
        AN = (TAnnularS1Physics*) m_DetectorManager -> GetDetector("AnnularS1");

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

	// initialize various parameters
	Rand = TRandom3();
	DetectorNumber = 0;
	ThetaNormalTarget = 0;
	ThetaM2Surface = 0;
	ThetaMGSurface = 0;
	ThetaANSurface = 0;
	Si_E_M2 = 0;
	CsI_E_M2 = 0;
	ThetaGDSurface = 0;
	BeamDirection = TVector3(0,0,1);

	//read badstrip file
	ifstream BStripFile("badstrip.txt",std::ifstream::in);

	while(!BStripFile.eof()){
		if(!BStripFile.eof()){
			std::string strip;
			BStripFile>>strip;
			BadStripSet.insert(strip);
		}
	}
        
	
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
	nbParticleMG = 0;
	nbParticleAN = 0;
	if(GDtracker->GetEnergyDeposit()>0) nbParticleMG = 1; 
	if(AN->GetEventMultiplicity()>0) nbParticleAN = 1; 
         

	//////////////////////////// LOOP on MUST2 //////////////////
	for(unsigned int countMust2 = 0 ; countMust2 < M2->Si_E.size() ; countMust2++){
		/************************************************/
		//Part 0 : Get the usefull Data
		// MUST2
		int TelescopeNumber = M2->TelescopeNumber[countMust2];
		int Xstripnumber = M2->Si_X[countMust2];
		int Ystripnumber = M2->Si_Y[countMust2];
//
		std::string stripXID = "MM"+std::to_string(TelescopeNumber)+"X"+std::to_string(Xstripnumber); 
		std::string stripYID = "MM"+std::to_string(TelescopeNumber)+"Y"+std::to_string(Ystripnumber); 
		if( (BadStripSet.find(stripXID) == BadStripSet.end()) && (BadStripSet.find(stripYID) == BadStripSet.end()) ) {

			/************************************************/
			// Part 1 : Impact Angle
			ThetaM2Surface = 0;
			ThetaNormalTarget = 0;
			TVector3 HitDirection = M2 -> GetPositionOfInteraction(countMust2) - BeamImpact ;
			ThetaLab.push_back( HitDirection.Angle( BeamDirection ));

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
			ThetaCM.back()=ThetaCM.back()/deg;


			ThetaLab.back()=ThetaLab.back()/deg;
		}    
		/************************************************/
	}//end loop MUST2

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	//////////////////////////// LOOP on MUGAST //////////////////


	for(unsigned int countMugast = 0 ; countMugast < nbParticleMG ; countMugast++){
                std::string stripXID;
		std::string stripYID;
	        	
		int TelescopeNumber = GD->fModuleNumber[countMugast];
		if (TelescopeNumber == 201) { 
                     stripXID = "MG11X"+std::to_string(anularX[GD->fFirstStage_FrontPosition[countMugast]-1]);
                     stripYID = "MG11Y"+std::to_string(anularY[GD->fFirstStage_BackPosition[countMugast]-1]);
		}
                else{
			stripXID = "MG"+std::to_string(GaspardMap[TelescopeNumber-101])+"X"+std::to_string(trapeX[GD->fFirstStage_FrontPosition[countMugast]-1]); 
			stripYID = "MG"+std::to_string(GaspardMap[TelescopeNumber-101])+"Y"+std::to_string(trapeY[GD->fFirstStage_BackPosition[countMugast]-1]); 
		}
	        
		//cout<<"IDx -> "<<stripXID<<"IDy ->"<<stripYID<<endl;	
		
		if( (BadStripSet.find(stripXID) == BadStripSet.end()) && (BadStripSet.find(stripYID) == BadStripSet.end()) ) {
		// Part 1 : Impact Angle
		ThetaMGSurface = 0;
		ThetaNormalTarget = 0;
		TVector3 HitDirection = GDtracker -> GetPositionOfInteraction() - BeamImpact ;

		ThetaLab.push_back( HitDirection.Angle( BeamDirection ));
		X.push_back(  GDtracker -> GetPositionOfInteraction().X());
		Y.push_back(  GDtracker -> GetPositionOfInteraction().Y());
		Z.push_back(  GDtracker -> GetPositionOfInteraction().Z());

		ThetaMGSurface = HitDirection.Angle( TVector3(0,0,1) ) ;
		ThetaNormalTarget = HitDirection.Angle( TVector3(0,0,1) ) ;


		// Part 2 : Impact Energy
		Energy = 0;
		Energy = GDtracker->GetEnergyDeposit();
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
		ThetaCM.back()=ThetaCM.back()/deg;
		}
	}//end loop Mugast

	////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////
	//////////////////////////// LOOP on Annular ///////////////////////////////


	for(unsigned int countAN = 0 ; countAN < nbParticleAN ; countAN++){
                /*
		std::string stripXID;
		std::string stripYID;
		
		int TelescopeNumber = GD->fModuleNumber[countMugast];
		
		if (TelescopeNumber == 201) { 
                     stripXID = "MG11X"+std::to_string(anularX[GD->fFirstStage_FrontPosition[countMugast]-1]);
                     stripYID = "MG11Y"+std::to_string(anularY[GD->fFirstStage_BackPosition[countMugast]-1]);
		}
                else{
			stripXID = "MG"+std::to_string(GaspardMap[TelescopeNumber-101])+"X"+std::to_string(trapeX[GD->fFirstStage_FrontPosition[countMugast]-1]); 
			stripYID = "MG"+std::to_string(GaspardMap[TelescopeNumber-101])+"Y"+std::to_string(trapeY[GD->fFirstStage_BackPosition[countMugast]-1]); 
	//	}
	        
		//cout<<"IDx -> "<<stripXID<<"IDy ->"<<stripYID<<endl;	
		
		if( (BadStripSet.find(stripXID) == BadStripSet.end()) && (BadStripSet.find(stripYID) == BadStripSet.end()) ) {
		*/
		// Part 1 : Impact Angle
		ThetaANSurface = 0;
		ThetaNormalTarget = 0;
		TVector3 HitDirection = AN -> GetPositionOfInteraction(0) - BeamImpact ;

		ThetaLab.push_back( HitDirection.Angle( BeamDirection ));
		X.push_back(  AN -> GetPositionOfInteraction(0).X());
		Y.push_back(  AN -> GetPositionOfInteraction(0).Y());
		Z.push_back(  AN -> GetPositionOfInteraction(0).Z());

		ThetaANSurface = HitDirection.Angle( TVector3(0,0,1) ) ;
		ThetaNormalTarget = HitDirection.Angle( TVector3(0,0,1) ) ;


		// Part 2 : Impact Energy
		Energy = 0;
		Energy = AN->Strip_E[countAN];
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
		ThetaCM.back()=ThetaCM.back()/deg;

	}//end loop Annular
}

////////////////////////////////////////////////////////////////////////////////
void Analysis::End(){
}
////////////////////////////////////////////////////////////////////////////////
void Analysis::InitOutputBranch() {
	RootOutput::getInstance()->GetTree()->Branch("Ex",&Ex);
	RootOutput::getInstance()->GetTree()->Branch("ELab",&ELab);
	RootOutput::getInstance()->GetTree()->Branch("ThetaLab",&ThetaLab);
	RootOutput::getInstance()->GetTree()->Branch("ThetaCM",&ThetaCM);
	RootOutput::getInstance()->GetTree()->Branch("X",&X);
	RootOutput::getInstance()->GetTree()->Branch("Y",&Y);
	RootOutput::getInstance()->GetTree()->Branch("Z",&Z);
	RootOutput::getInstance()->GetTree()->Branch("E",&E);
	RootOutput::getInstance()->GetTree()->Branch("dE",&dE);
	RootOutput::getInstance()->GetTree()->Branch("nbParticleM2",&nbParticleM2,"nbParticleM2/I");
	RootOutput::getInstance()->GetTree()->Branch("nbParticleMG",&nbParticleMG,"nbParticleMG/I");

	RootOutput::getInstance()->GetTree()->Branch("Run",&Run,"Run/I");
	//  //Agata
	//  RootOutput::getInstance()->GetTree()->Branch("nbTrack",&nbTrack,"nbTrack/I");
	//  RootOutput::getInstance()->GetTree()->Branch("trackE",trackE,"trackE[nbTrack]/F");
	//  RootOutput::getInstance()->GetTree()->Branch("trackX1",trackX1,"trackX1[nbTrack]/F");
	//  RootOutput::getInstance()->GetTree()->Branch("trackY1",trackY1,"trackY1[nbTrack]/F");
	//  RootOutput::getInstance()->GetTree()->Branch("trackZ1",trackZ1,"trackZ1[nbTrack]/F");
	//  RootOutput::getInstance()->GetTree()->Branch("trackT",trackT,"trackT[nbTrack]/F");
	//  RootOutput::getInstance()->GetTree()->Branch("trackCrystalID",trackCrystalID,"trackCrystalID[nbTrack]/I");
	//  RootOutput::getInstance()->GetTree()->Branch("nbCores",&nbCores,"nbCores/I");
	//  RootOutput::getInstance()->GetTree()->Branch("coreId",coreId,"coreId[nbCores]/I");
	//  RootOutput::getInstance()->GetTree()->Branch("coreTS",coreTS,"coreTS[nbCores]/l");
	//  RootOutput::getInstance()->GetTree()->Branch("coreE0",coreE0,"coreE0[nbCores]/F");
	//  RootOutput::getInstance()->GetTree()->Branch("VTS",&VTS,"VTS/l");

} 


////////////////////////////////////////////////////////////////////////////////
void Analysis::InitInputBranch(){
	//Agata
	RootInput::getInstance()->GetChain()->SetBranchAddress("VTS",&VTS);
	RootInput::getInstance()->GetChain()->SetBranchAddress("nbTrack",&nbTrack);
	RootInput::getInstance()->GetChain()->SetBranchAddress("trackE",trackE);
	RootInput::getInstance()->GetChain()->SetBranchAddress("trackX1",trackX1);
	RootInput::getInstance()->GetChain()->SetBranchAddress("trackY1",trackY1);
	RootInput::getInstance()->GetChain()->SetBranchAddress("trackZ1",trackZ1);
	RootInput::getInstance()->GetChain()->SetBranchAddress("trackT",trackT);
	RootInput::getInstance()->GetChain()->SetBranchAddress("trackCrystalID",trackCrystalID);
	RootInput::getInstance()->GetChain()->SetBranchAddress("nbCores",&nbCores);
	RootInput::getInstance()->GetChain()->SetBranchAddress("coreId",coreId);
	RootInput::getInstance()->GetChain()->SetBranchAddress("coreTS",coreTS);
	RootInput::getInstance()->GetChain()->SetBranchAddress("coreE0",coreE0);
}
////////////////////////////////////////////////////////////////////////////////
void Analysis::ReInitValue(){
	Ex.clear() ;
	ELab.clear();
	ThetaLab.clear();
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

