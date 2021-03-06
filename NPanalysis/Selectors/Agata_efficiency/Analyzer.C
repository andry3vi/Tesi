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

	Edopp = new TH1D("Edopp","Edopp",3000,0,3000);
	Edopp->GetXaxis()->SetTitle("Gamma Energy [keV]");
	Edopp->GetYaxis()->SetTitle("Counts");

	Edopp_TSgated = new TH1D("Edopp_TSgated","Edopp_TSgated",3000,0,3000);
	Edopp_TSgated->GetXaxis()->SetTitle("Gamma Energy [keV]");
	Edopp_TSgated->GetYaxis()->SetTitle("Counts");

	Eraw = new TH1D("Eraw","Eraw",3000,0,3000);
	Eraw->GetXaxis()->SetTitle("Gamma Energy [keV]");
	Eraw->GetYaxis()->SetTitle("Counts");

	Mass = 15829.5; //17O MeV
	SoL  = 0.299792458; // c [mm/ps]
	LifeTime = 178.2; // ps
	//------------Ex Gate-------------//

		gs_Ex = new double[2];
		fs_Ex = new double[2];

		CountExGS = 0;
		CountExFS = 0;
		//------------RUN 256+257 thickness corrected---------------//
				double hwhmgs = 0.29119804; //MeV
				double hwhmfs = 0.36914251; //MeV

				gs_Ex[0] = 0.0 - hwhmgs;
				gs_Ex[1] = 0.0 + hwhmgs;

				fs_Ex[0] = 0.87073 - hwhmfs;
				fs_Ex[1] = 0.87073 + hwhmfs;
		//------------RUN 256+257 thickness corrected---------------//
// //----------------------RUN 256+257-------------------------//
// 		double hwhmgs = 0.29119804; //MeV
// 		double hwhmfs = 0.36914251; //MeV
//
// 		gs_Ex[0] = 0.0424022 - hwhmgs;
// 		gs_Ex[1] = 0.0424022 + hwhmgs;
//
// 		fs_Ex[0] = 0.777442 - hwhmfs;
// 		fs_Ex[1] = 0.777442 + hwhmfs;
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
	for (size_t i = 0; i < *nbAdd; i++) {
		Eraw->Fill(AddE[i]);
	}

	//Doppler correcting and selection of mult. one for agata and mugast//
	if(*nbParticleM2 == 0 && *nbParticleMG == 1 && *nbTrack == 1 ){

		double Beta = TMath::Sqrt(EheavyAfterTg[0]*EheavyAfterTg[0]+2*EheavyAfterTg[0]*Mass)/(EheavyAfterTg[0]+Mass); // Beta reconstructed with kinematics
		//double Beta =0.101;  //beta averaged fixed

		TVector3 BetaVector(-1.0*Beta*sin(ThetaHeavy[0]*M_PI/180.0)*cos(PhiLab[0]*M_PI/180.0),-1.0*Beta*sin(ThetaHeavy[0]*M_PI/180.0)*sin(PhiLab[0]*M_PI/180.0),Beta*cos(ThetaHeavy[0]*M_PI/180.0));
		//TVector3 BetaVector(0,0,Beta);//heavy direction z axis

		//double Egamma = trackE[0]/1000; //MeV converted
		//double Egamma = trackE[0]/1000; //MeV converted
		double Egamma = AddE[0]/1000; //MeV converted

		TVector3 HitPosition(AddX[0],AddY[0],AddZ[0]+51);
		//TVector3 HitPosition(trackX1[0],trackY1[0],trackZ1[0]+51);

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


		if(Ex[0]>fs_Ex[0] && Ex[0]<fs_Ex[1]) Edopp->Fill(Gamma.Energy()*1000);
		Edopp_TSgated->Fill(Egamma*1000);
	}
	//------------------------------------------------------------------//
	return kTRUE;
}

void Analyzer::SlaveTerminate()
{
	// The SlaveTerminate() function is called after all entries or objects
	// have been processed. When running with PROOF SlaveTerminate() is called
	// on each slave server.
	TCanvas *C1 = new TCanvas("C1","C1");
	//C1->Divide(1,2);

	C1->cd();
  //Eraw->Draw();
	Edopp->Draw();

	TSpectrum * back = new TSpectrum();
	TH1D * Edopp_back = (TH1D *) back->Background(Edopp,25);
	Edopp_back->Draw("SAME");
	Edopp->Add(Edopp_back,-1);
	//cout<<endl<<endl<<"gamma 1/2+ counts ->"<<Edopp->Integral(Edopp->FindBin(862.68), Edopp->FindBin(873.87))<<endl;
	cout<<endl<<endl<<"gamma 1/2+ counts ->"<<Edopp->Integral(Edopp->FindBin(850), Edopp->FindBin(885))<<endl;
	//Edopp_TSgated->Draw();

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
