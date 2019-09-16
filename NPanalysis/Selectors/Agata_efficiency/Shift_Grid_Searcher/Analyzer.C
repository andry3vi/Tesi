#define Analyzer_cxx
// The class definition in Analyzer.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.


// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// root> T->Process("Analyzer.C")
// root> T->Process("Analyzer.C","some options")
// root> T->Process("Analyzer.C+")
//


#include "Analyzer.h"
#include <TH2.h>
#include <TStyle.h>
#include "colormod.h"

void Analyzer::Begin(TTree * /*tree*/)
{
	// The Begin() function is called at the start of the query.
	// When running with PROOF Begin() is only called on the client.
	// The tree argument is deprecated (on PROOF 0 is passed).

	TString option = GetOption();
}

void Analyzer::SlaveBegin(TTree * /*tree*/)
{
	// The SlaveBegin() function is called after the Begin() function.
	// When running with PROOF SlaveBegin() is called on each slave server.
	// The tree argument is deprecated (on PROOF 0 is passed).

	TString option = GetOption();

	outfile = new TFile("Out_tmp/BETAopt.root","recreate");

	optree = new TTree ("BETAopt","BETAopt");

	dBetaStep = 0.5;
	dBetaRange[0] = 10;  //dbetarange for 1
	dBetaRange[1] = 80;
//	dBetaRange[0] = -0.03;
//	dBetaRange[1] = 0.08;

  optree->Branch("dBeta",&dBeta);
	optree->Branch("Edopp",&Edopp);

	Mass = 15829.5; //17O MeV
	SoL  = 0.299792458; // c [mm/ps]
	LifeTime = 178.2; // ps

}

Bool_t Analyzer::Process(Long64_t entry)
{
	// The Process() function is called for each entry in the tree (or possibly
	// keyed object in the case of PROOF) to be processed. The entry argument
	// specifies which entry in the currently loaded tree is to be processed.
	// When processing keyed objects with PROOF, the object is already loaded
	// and is available via the fObject pointer.
	//
	// This function should contain the \"body\" of the analysis. It can contain
	// simple or elaborate selection criteria, run algorithms on the data
	// of the event and typically fill histograms.
	//
	// The processing can be stopped by calling Abort().
	//
	// Use fStatus to set the return value of TTree::Process().
	//
	// The return value is currently not used.
	fReader.SetLocalEntry(entry);

	int nbEntries = fReader.GetEntries(true);
	if(entry%100 == 0) printProgBar(entry*100.0/nbEntries);

	//------------Doppler Correction---------------//
	if(*nbParticleM2 == 0 && *nbParticleMG == 1 && *nbTrack == 1){

		dBeta.clear();
		Edopp.clear();

		for(double i = dBetaRange[0]; i<= dBetaRange[1]; i+=dBetaStep){

			dBeta.push_back(i);

			double Beta = TMath::Sqrt(EheavyAfterTg[0]*EheavyAfterTg[0]+2*EheavyAfterTg[0]*Mass)/(EheavyAfterTg[0]+Mass); // Beta reconstructed with kinematics
		  //double Beta = i+0.106;	//beta averaged fixed

			TVector3 BetaVector(-1.0*Beta*sin(ThetaHeavy[0]*M_PI/180.0)*cos(PhiLab[0]*M_PI/180.0),-1.0*Beta*sin(ThetaHeavy[0]*M_PI/180.0)*sin(PhiLab[0]*M_PI/180.0),Beta*cos(ThetaHeavy[0]*M_PI/180.0)); //correction for beta direction from kinematic
			//TVector3 BetaVector(0,0,Beta);//heavy direction z axis

			double Egamma = trackE[0]/1000; //MeV converted

			TVector3 HitPosition(trackX1[0],trackY1[0],trackZ1[0]+i);

			//TVector3 EmissionPosition(BetaVector.X()*SoL*LifeTime,BetaVector.Y()*SoL*LifeTime,BetaVector.Z()*SoL*LifeTime);//correction for decay position
			TVector3 EmissionPosition(0,0,0);//decay postion at target center

			TVector3 GammaDirection = HitPosition - EmissionPosition;
			TVector3 GammaVersor = GammaDirection.Unit();

			TLorentzVector Gamma;
			Gamma.SetPx(Egamma*GammaVersor.X());
			Gamma.SetPy(Egamma*GammaVersor.Y());
			Gamma.SetPz(Egamma*GammaVersor.Z());
			Gamma.SetE(Egamma);

			Gamma.Boost(-1.0*BetaVector);

			Edopp.push_back(Gamma.Energy()*1000);
		}

		optree->Fill();
	        //optree->Write();
	}
	//------------------------------------------------------------------//
	return kTRUE;
}

void Analyzer::SlaveTerminate()
{
	// The SlaveTerminate() function is called after all entries or objects
	// have been processed. When running with PROOF SlaveTerminate() is called
	// on each slave server.

	optree->Write();
	outfile->Close();

}

void Analyzer::Terminate()
{
	// The Terminate() function is the last function to be called during
	// a query. It always runs on the client, it can be used to present
	// the results graphically or save the results to file.

}

void Analyzer::printProgBar( float percent){
	std::string bar;

	for(int i = 0; i < 50; i++){
		if( i < (int(percent)/2)){
			bar.replace(i,1,"=");
		}else if( i == int(percent/2)){
			bar.replace(i,1,">");
		}else{
			bar.replace(i,1," ");
		}
	}

	Color::Modifier red(Color::FG_RED);
	Color::Modifier def(Color::FG_DEFAULT);
	Color::Modifier green(Color::FG_GREEN);

	std::cout<<"\r [" << red << bar << def <<"]";
	std::cout.width( 3 );
	std::cout<< std::fixed;
	std::cout<< std::setprecision(2);

	std::cout<< green << percent << def << "%"<< std::flush;

}
