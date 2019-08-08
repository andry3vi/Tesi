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

	nbPmultM2 = new TH1F("multM2","multM2",10,-0.5,9.5);
	nbPmultMG = new TH1F("multMG","multMG",10,-0.5,9.5);
	nbPmultHv = new TH1F("multHv","multHv",10,-0.5,9.5);
	nbPmultAg = new TH1F("multAg","multAg",10,-0.5,9.5);

	Edopp = new TH1D("Edopp","Edopp",2000,0,2000);
        Edopp->GetXaxis()->SetTitle("Gamma Energy [keV]");
        Edopp->GetYaxis()->SetTitle("Counts");
	
	Eraw = new TH1D("Eraw","Eraw",2000,0,2000);
        Eraw->GetXaxis()->SetTitle("Gamma Energy [keV]");
        Eraw->GetYaxis()->SetTitle("Counts");


}

void Analyzer::SlaveBegin(TTree * /*tree*/)
{
	// The SlaveBegin() function is called after the Begin() function.
	// When running with PROOF SlaveBegin() is called on each slave server.
	// The tree argument is deprecated (on PROOF 0 is passed).

	TString option = GetOption();

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

	//Filling multiplicity//
	nbPmultM2->Fill(*nbParticleM2);
	nbPmultMG->Fill(*nbParticleMG);
	nbPmultHv->Fill(ThetaHeavy.GetSize());
	nbPmultAg->Fill(*nbTrack);
	//--------------------//
        double Agata_zShift = 51;//51;
        //Doppler correcting and selection of mult. one for agata and mugast//	
	if(*nbParticleMG == 1 && *nbTrack == 1){
                
		double Mass = 15830.5; //17O MeV
		//double Mass = 15000.5; //17O MeV
//
//		double ImpulsionLab    = sqrt(EheavyAfterTg[0]*EheavyAfterTg[0] + 2*EheavyAfterTg[0]*Mass);
//		double ImpulsionLabX   = ImpulsionLab*sin(ThetaHeavy[0]*M_PI/180.0)*cos((PhiLab[0]-180.0)*M_PI/180.0);
//  		double ImpulsionLabY   = ImpulsionLab*sin(ThetaHeavy[0]*M_PI/180.0)*sin((PhiLab[0]-180.0)*M_PI/180.0);
//   		double ImpulsionLabZ   = ImpulsionLab*cos(ThetaHeavy[0]*M_PI/180.0);
//	       
//		TLorentzVector HeavyImpulse(TVector3(ImpulsionLabX, ImpulsionLabY, ImpulsionLabZ),Mass + EheavyAfterTg[0]);
		double Beta = TMath::Sqrt(EheavyAfterTg[0]*EheavyAfterTg[0]+2*EheavyAfterTg[0]*Mass)/(EheavyAfterTg[0]+Mass);	
	        	
		TVector3 BetaVector(-1.0*Beta*sin(ThetaHeavy[0]*M_PI/180.0)*cos(PhiLab[0]*M_PI/180.0),-1.0*Beta*sin(ThetaHeavy[0]*M_PI/180.0)*sin(PhiLab[0]*M_PI/180.0),Beta*cos(ThetaHeavy[0]*M_PI/180.0));
		//TVector3 BetaVector(0,0,Beta);
                
		double Egamma = trackE[0]/1000; //MeV converted
		
		TVector3 HitPosition(trackX1[0],trackY1[0],trackZ1[0]+Agata_zShift);
		TVector3 EmissionPosition(0,0,5.67);
		
		TVector3 GammaDirection = HitPosition - EmissionPosition;
		TVector3 GammaVersor = GammaDirection.Unit();
		
		TLorentzVector Gamma;
                Gamma.SetPx(Egamma*GammaVersor.X());
                Gamma.SetPy(Egamma*GammaVersor.Y());
                Gamma.SetPz(Egamma*GammaVersor.Z());
		Gamma.SetE(Egamma);
                 
		Gamma.Boost(BetaVector);
	        
		Edopp->Fill(Gamma.Energy()*1000);
                Eraw->Fill(Egamma*1000);

	}
        //------------------------------------------------------------------//
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
        Edopp->Draw();

	C1->cd(2);
	Eraw->Draw();

//	TCanvas *C2 = new TCanvas("C2","C2");
//	C2->Divide(1,2);
//
//	C2->cd(1);
//	nbPmultHv->Draw();
//	
//	C2->cd(2);
//	nbPmultAg->Draw();
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
