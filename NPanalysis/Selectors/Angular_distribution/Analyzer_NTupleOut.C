#define Analyzer_NTupleOut_cxx
// The class definition in Analyzer_NTupleOut.h has been generated automatically
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
// root> T->Process("Analyzer_NTupleOut.C")
// root> T->Process("Analyzer_NTupleOut.C","some options")
// root> T->Process("Analyzer_NTupleOut.C+")
//


#include "Analyzer_NTupleOut.h"
#include <TH2.h>
#include <TStyle.h>

#include "colormod.h"
#include <iomanip>
#include <iostream>
#include <string>

void Analyzer_NTupleOut::Begin(TTree * /*tree*/)
{
	// The Begin() function is called at the start of the query.
	// When running with PROOF Begin() is only called on the client.
	// The tree argument is deprecated (on PROOF 0 is passed).

	enable_cut = true; //apply graph. cut on EdE MUST2 protons;
	enable_ExGate = true; //apply gate on g.s. and 1st

	//--------Check Boundaries--------//

	ExThetaL = new TH2D("ExThetaL","ExThetaL",100,80,180,1000,-2,3);
	ExThetaL->GetXaxis()->SetTitle("#theta_{LAB} [deg]");
	ExThetaL->GetYaxis()->SetTitle("Ex [MeV]");

	EdE_M2 = new TH2D("EdE","EdE",1000,0,40,1000,0,40);
	EdE_M2->GetXaxis()->SetTitle("CsI Energy [MeV]");
	EdE_M2->GetYaxis()->SetTitle("Si Energy [MeV]");

	nbPmultM2 = new TH1F("multM2","multM2",10,0,10);
	nbPmultMG = new TH1F("multMG","multMG",10,0,10);

	impactMG = new TH2F("impactMG","impactMG",200,-150,150,200,-150,150);
	impactM2 = new TH2F("impactM2","impactM2",200,-150,150,200,-150,150);

	ELabThetaL = new TH2D("ELabThLab","ELabThLab",500,0,180,5000,-5,40);
	ELabThetaL->GetXaxis()->SetTitle("#theta_{LAB} [deg]");
	ELabThetaL->GetYaxis()->SetTitle("LAB Energy [MeV]");

	ELabThetaLSelected = new TH2D("ELabThLabSelected","ELabThLabSelected",300,0,180,3000,-5,40);
	ELabThetaLSelected->GetXaxis()->SetTitle("#theta_{LAB} [deg]");
	ELabThetaLSelected->GetYaxis()->SetTitle("LAB Energy [MeV]");

	ExDist = new TH1D("Ex","Ex",1000,-10,10);
	ExDist->GetXaxis()->SetTitle("Ex [MeV]");
	ExDist->GetYaxis()->SetTitle("Counts");

	ThetaLDist_gs = new TH1F("ThLabgs","Lab angular distribution",180,0,180);
	ThetaLDist_gs->GetXaxis()->SetTitle("#theta_{LAB} [deg]");
	ThetaLDist_gs->GetYaxis()->SetTitle("Counts");
	ThetaLDist_gs->SetLineColor(2);//RED

	ThetaLDist_fs = new TH1F("ThLabfs","Lab angular distribution",180,0,180);
	ThetaLDist_fs->GetXaxis()->SetTitle("#theta_{LAB} [deg]");
	ThetaLDist_fs->GetYaxis()->SetTitle("Counts");
	ThetaLDist_fs->SetLineColor(4);//BLUE


	ThetaCMDist_gs = new TH1F("ThCMgs","CM angular distribution",180,0,180);
	ThetaCMDist_gs->GetXaxis()->SetTitle("#theta_{CM} [deg]");
	ThetaCMDist_gs->GetYaxis()->SetTitle("Counts");
	ThetaCMDist_gs->SetLineColor(2);//RED

	ThetaCMDist_fs = new TH1F("ThCMfs","CM angular distribution",180,0,180);
	ThetaCMDist_fs->GetXaxis()->SetTitle("#theta_{CM} [deg]");
	ThetaCMDist_fs->GetYaxis()->SetTitle("Counts");
	ThetaCMDist_fs->SetLineColor(4);//BLUE

	AddEVsAddId = new TH2F("AddEVsAddId","AddEVsAddId",45,0,45,3000,0,3000);
	AddE_histo = new TH1F("AddE","AddE",3000,0,3000);

	kineGS = new TGraph("../Data/Xsection/KinematicLineGS.txt");
	kineFS = new TGraph("../Data/Xsection/KinematicLineFS.txt");

	Mass = 15829.5; //17O MeV
	SoL  = 0.299792458; // c [mm/ps]
	LifeTime = 178.2; // ps

	Output_NTuple_file = new TFile("COMM19_tmp.root","RECREATE");

 	Output_NTuple = new TNtuple("COMM19", "COMM19", "Ex:Elab:ThLAB:ThCM:ETrack:EAdd:DoppETrack:DoppEAdd:IC1:IC2:MW_Nr");

	califile.open("calibration.dat", ios::in);
	int id;
	double m_tmp, q_tmp;
	while(califile>>id>>q_tmp>>m_tmp){
		m[id]=m_tmp;
		q[id]=q_tmp;
	}
	califile.close();

	TString option = GetOption();

}

void Analyzer_NTupleOut::SlaveBegin(TTree * /*tree*/)
{
	// The SlaveBegin() function is called after the Begin() function.
	// When running with PROOF SlaveBegin() is called on each slave server.
	// The tree argument is deprecated (on PROOF 0 is passed).

	TString option = GetOption();

}

Bool_t Analyzer_NTupleOut::Process(Long64_t entry)
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

	nbEntries = fReader.GetEntries(true);
	if(entry%1000 == 0) printProgBar(entry*100.0/nbEntries);

	//Default_Val
	Ex_NT = Elab_NT = ThLAB_NT = ThCM_NT = ETrack_NT = EAdd_NT = DoppETrack_NT = DoppEAdd_NT = IC1_NT = IC2_NT = MW_Nr_NT = -1000;

	//--------------Mugast---------------//
	for(size_t i=*nbParticleM2; i<(*nbParticleM2+*nbParticleMG); i++){

		Ex_NT = Ex[i];
		Elab_NT = ELab[i];
		ThLAB_NT = ThetaLab[i];
		ThCM_NT = ThetaCM[i];

	}

	for(size_t i= 0; i<*nbAdd; i++){
		if((*VTS-AddTS[i])>= -185 && (*VTS-AddTS[i])< -172 ){
			//double Beta = TMath::Sqrt(EheavyAfterTg[0]*EheavyAfterTg[0]+2*EheavyAfterTg[0]*Mass)/(EheavyAfterTg[0]+Mass); // Beta reconstructed with kinematics
			double Beta = 0.101;  //beta averaged fixed

			//TVector3 BetaVector(-1.0*Beta*sin(ThetaHeavy[0]*M_PI/180.0)*cos((PhiLab[0]+125)*M_PI/180.0),-1.0*Beta*sin(ThetaHeavy[0]*M_PI/180.0)*sin((PhiLab[0]+125)*M_PI/180.0),Beta*cos(ThetaHeavy[0]*M_PI/180.0));
			//Vector3 BetaVector(-1.0*Beta*sin(ThetaHeavy[0]*M_PI/180.0)*cos(PhiLab[0]*M_PI/180.0),-1.0*Beta*sin(ThetaHeavy[0]*M_PI/180.0)*sin(PhiLab[0]*M_PI/180.0),Beta*cos(ThetaHeavy[0]*M_PI/180.0));
			TVector3 BetaVector(0,0,Beta);//heavy direction z axis

			TVector3 EmissionPosition(0,0,0);//decay postion at target center

			//AddBack
			double EgammaAdd = (AddE[i]*m[AddId[i]]+q[AddId[i]])/1000; //MeV converted
			TVector3 HitPositionAdd(AddX[i],AddY[i],AddZ[i]+51);
			TVector3 GammaDirectionAdd = HitPositionAdd - EmissionPosition;
			TVector3 GammaVersorAdd = GammaDirectionAdd.Unit();
			TLorentzVector GammaAdd;
			GammaAdd.SetPx(EgammaAdd*GammaVersorAdd.X());
			GammaAdd.SetPy(EgammaAdd*GammaVersorAdd.Y());
			GammaAdd.SetPz(EgammaAdd*GammaVersorAdd.Z());
			GammaAdd.SetE(EgammaAdd);
			GammaAdd.Boost(-1.0*BetaVector);

			AddE_histo->Fill(GammaAdd.Energy()*1000);
			AddEVsAddId->Fill(AddId[i],EgammaAdd*1000);
		}

	}

	if(*nbParticleM2 == 0 && *nbParticleMG == 1 && *nbTrack == 1 ){

		double Beta = TMath::Sqrt(EheavyAfterTg[0]*EheavyAfterTg[0]+2*EheavyAfterTg[0]*Mass)/(EheavyAfterTg[0]+Mass); // Beta reconstructed with kinematics
		//double Beta =0.101;  //beta averaged fixed

		// TVector3 BetaVector(-1.0*Beta*sin(ThetaHeavy[0]*M_PI/180.0)*cos(PhiLab[0]*M_PI/180.0),-1.0*Beta*sin(ThetaHeavy[0]*M_PI/180.0)*sin(PhiLab[0]*M_PI/180.0),Beta*cos(ThetaHeavy[0]*M_PI/180.0));
		TVector3 BetaVector(-1.0*Beta*sin(ThetaHeavy[0]*M_PI/180.0)*cos((PhiLab[0]+125)*M_PI/180.0),-1.0*Beta*sin(ThetaHeavy[0]*M_PI/180.0)*sin((PhiLab[0]+125)*M_PI/180.0),Beta*cos(ThetaHeavy[0]*M_PI/180.0));
		//TVector3 BetaVector(0,0,Beta);//heavy direction z axis

		TVector3 EmissionPosition(0,0,0);//decay postion at target center

		//AddBack
		double EgammaAdd = (AddE[0]*m[AddId[0]]+q[AddId[0]])/1000; //MeV converted
		TVector3 HitPositionAdd(AddX[0],AddY[0],AddZ[0]+51);
		TVector3 GammaDirectionAdd = HitPositionAdd - EmissionPosition;
		TVector3 GammaVersorAdd = GammaDirectionAdd.Unit();
		TLorentzVector GammaAdd;
		GammaAdd.SetPx(EgammaAdd*GammaVersorAdd.X());
		GammaAdd.SetPy(EgammaAdd*GammaVersorAdd.Y());
		GammaAdd.SetPz(EgammaAdd*GammaVersorAdd.Z());
		GammaAdd.SetE(EgammaAdd);
		GammaAdd.Boost(-1.0*BetaVector);

		//Tracking
		double EgammaTrack = (trackE[0]*m[AddId[0]]+q[AddId[0]])/1000; //MeV converted
		TVector3 HitPositionTrack(trackX1[0],trackY1[0],trackZ1[0]+51);
		TVector3 GammaDirectionTrack = HitPositionTrack - EmissionPosition;
		TVector3 GammaVersorTrack = GammaDirectionTrack.Unit();
		TLorentzVector GammaTrack;
		GammaTrack.SetPx(EgammaTrack*GammaVersorTrack.X());
		GammaTrack.SetPy(EgammaTrack*GammaVersorTrack.Y());
		GammaTrack.SetPz(EgammaTrack*GammaVersorTrack.Z());
		GammaTrack.SetE(EgammaTrack);
		GammaTrack.Boost(-1.0*BetaVector);


		ETrack_NT		 	= trackE[0];
		EAdd_NT 			= AddE[0];
		DoppETrack_NT = GammaTrack.Energy()*1000;
		DoppEAdd_NT 	= GammaAdd.Energy()*1000;

	}


	if (IC[1] > 0) IC1_NT = IC[1];
	if (IC[2] > 0) IC2_NT = IC[2];
	if (*MW_Nr > 0) MW_Nr_NT = *MW_Nr;
	// if(*nbParticleMG > 0){
	// cout<<"	Ex_NT - Elab_NT - ThLAB_NT - ThCM_NT - ETrack_NT - EAdd_NT - DoppETrack_NT - DoppEAdd_NT - IC1_NT - IC2_NT - MW_Nr_NT"<<endl;
	// cout<<	Ex_NT<<" - "<<Elab_NT<<" - "<<ThLAB_NT<<" - "<<ThCM_NT<<" - "<<ETrack_NT<<" - "<<EAdd_NT<<" - "<<DoppETrack_NT<<" - "<<DoppEAdd_NT<<" - "<<IC1_NT<<" - "<<IC2_NT<<" - "<<MW_Nr_NT<<endl;
	// }
	Output_NTuple->Fill(Ex_NT, Elab_NT, ThLAB_NT, ThCM_NT, ETrack_NT, EAdd_NT, DoppETrack_NT, DoppEAdd_NT, IC1_NT, IC2_NT, MW_Nr_NT);

	//--------------------------//
	return kTRUE;

}

void Analyzer_NTupleOut::SlaveTerminate()
{
	// The SlaveTerminate() function is called after all entries or objects
	// have been processed. When running with PROOF SlaveTerminate() is called
	// on each slave server.

}

void Analyzer_NTupleOut::Terminate()
{
	// The Terminate() function is the last function to be called during
	// a query. It always runs on the client, it can be used to present
	// the results graphically or save the results to file.
	Output_NTuple->Write();
	Output_NTuple_file->Close();
	AddE_histo->SaveAs("AddE_Spectrum.root");
	TCanvas *C = new TCanvas("c","c");
	C->cd();
	AddE_histo->Draw();



}


void Analyzer_NTupleOut::printProgBar( float percent){
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
