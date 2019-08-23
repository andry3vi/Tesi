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

#include <TLorentzVector.h>
	
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
	//
	//	nbPmultM2 = new TH1F("multM2","multM2",10,-0.5,9.5);
	//	nbPmultMG = new TH1F("multMG","multMG",10,-0.5,9.5);
	//	nbPmultHv = new TH1F("multHv","multHv",10,-0.5,9.5);
	//	nbPmultAg = new TH1F("multAg","multAg",10,-0.5,9.5);
	//
}

void Analyzer::SlaveBegin(TTree * /*tree*/)
{
	// The SlaveBegin() function is called after the Begin() function.
	// When running with PROOF SlaveBegin() is called on each slave server.
	// The tree argument is deprecated (on PROOF 0 is passed).

	TString option = GetOption();

	enable_cut = true; //apply graph. cut on EdE MUST2 deutons;

	EdE_M2 = new TH2D("EdE","EdE",1000,0,40,1000,0,40);
	EdE_M2->GetXaxis()->SetTitle("CsI Energy [MeV]");
	EdE_M2->GetYaxis()->SetTitle("Si Energy [MeV]");

	Eraw = new TH1D("Eraw","Eraw",2000,0,2000);
	Eraw->GetXaxis()->SetTitle("Gamma Energy [keV]");
	Eraw->GetYaxis()->SetTitle("Counts");

	Eraw_gated = new TH1D("Eraw_gated","Eraw_gated",2000,0,2000);
	Eraw_gated->GetXaxis()->SetTitle("Gamma Energy [keV]");
	Eraw_gated->GetYaxis()->SetTitle("Counts");
  
	ELabThetaL = new TH2D("ELabThLab","ELabThLab",300,0,180,3000,-5,40);
        ELabThetaL->GetXaxis()->SetTitle("#theta_{LAB} [deg]");
        ELabThetaL->GetYaxis()->SetTitle("LAB Energy [MeV]");

	//---------Open Cut File----------//
	if(enable_cut){
		CutG_d_file = new TFile("../Data/old/EdE_CutG_d.root");
		CutG_d = (TCutG *)CutG_d_file->Get("EdE_d");
	}
	//--------------------------------//
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
	if(entry%1000 == 0) printProgBar(entry*100.0/nbEntries);

	//	//Filling multiplicity//
	//	nbPmultM2->Fill(*nbParticleM2);
	//	nbPmultMG->Fill(*nbParticleMG);
	//	nbPmultHv->Fill(ThetaHeavy.GetSize());
	//	nbPmultAg->Fill(*nbTrack);
	//	//--------------------//



	//----------------EdE MUST2 matrix--------------//
	for(size_t i=0; i<*nbParticleM2; i++){

		EdE_M2->Fill( E[i], dE[i] );

	}

	//----------------------------------------------//

	//--------------MUST2 Deuton GATE gamma spectrum---------------//
	if(*nbParticleM2 == 1 &&  *nbTrack == 1){
		Eraw->Fill(trackE[0]); 
		if(enable_cut){
			ELabThetaL->Fill(ThetaLab[0],ELab[0]);
			if(CutG_d->IsInside(E[0],dE[0])) {

                        double Beta = 0.08886;  //beta averaged fixed

                        TVector3 BetaVector(0,0,Beta);//heavy direction z axis

                        double Egamma = trackE[0]/1000; //MeV converted

                        TVector3 HitPosition(trackX1[0],trackY1[0],trackZ1[0]+51);

                        TVector3 EmissionPosition(0,0,0);//decay postion at target center

                        TVector3 GammaDirection = HitPosition - EmissionPosition;
                        TVector3 GammaVersor = GammaDirection.Unit();

                        TLorentzVector Gamma;
                        Gamma.SetPx(Egamma*GammaVersor.X());
                        Gamma.SetPy(Egamma*GammaVersor.Y());
                        Gamma.SetPz(Egamma*GammaVersor.Z());
                        Gamma.SetE(Egamma);

                        Gamma.Boost(-1.0*BetaVector);

			Eraw_gated->Fill(Gamma.Energy()*1000);      

			}
		}
		else{
			ELabThetaL->Fill(ThetaLab[0],ELab[0]);
			Eraw_gated->Fill(trackE[0]);     
		}
	}
	//-------------------------------------------------------------//

	return kTRUE;
}

void Analyzer::SlaveTerminate()
{
	// The SlaveTerminate() function is called after all entries or objects
	// have been processed. When running with PROOF SlaveTerminate() is called
	// on each slave server.

}

void Analyzer::Terminate()
{      
	// The Terminate() function is the last function to be called during
	// a query. It always runs on the client, it can be used to present
	// the results graphically or save the results to file.


	TCanvas *C1 = new TCanvas("C1","C1");
	C1->Divide(1,2);
	C1->cd(1);
        EdE_M2->Draw("col");
	CutG_d->Draw("SAME");
	C1->cd(2);
        ELabThetaL->Draw("col");

	TCanvas *C2 = new TCanvas("C2","C2");
	C2->Divide(1,2);
	C2->cd(1);
	Eraw->Draw();
	
	C2->cd(2);
	Eraw_gated->Draw();
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
