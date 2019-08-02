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
#include <iomanip>
#include <iostream>
#include <string>

void Analyzer::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   enable_cut = true; //apply graph. cut on EdE MUST2 protons;
   enable_ExGate = true; //apply gate on g.s. and 1st

   //--------Check Boundaries--------//
   EdE_M2 = new TH2D("EdE","EdE",1000,0,40,1000,0,40);
   EdE_M2->GetXaxis()->SetTitle("CsI Energy [MeV]");
   EdE_M2->GetYaxis()->SetTitle("Si Energy [MeV]");

   nbPmultM2 = new TH1F("multM2","multM2",10,0,10);
   nbPmultMG = new TH1F("multMG","multMG",10,0,10);

   impactMG = new TH2F("impactMG","impactMG",200,-150,150,200,-150,150);
   impactM2 = new TH2F("impactM2","impactM2",200,-150,150,200,-150,150);

   ELabThetaL = new TH2D("ELabThLab","ELabThLab",250,0,180,1000,-5,40);
   ELabThetaL->GetXaxis()->SetTitle("#theta_{LAB} [deg]");
   ELabThetaL->GetYaxis()->SetTitle("LAB Energy [MeV]");
   
   ExDist = new TH1D("Ex","Ex",1000,-10,10);
   ExDist->GetXaxis()->SetTitle("Ex [MeV]");
   ExDist->GetYaxis()->SetTitle("Counts");

   ThetaLDist_gs = new TH1F("ThLabGs","Lab angular distribution",180,0,180);
   ThetaLDist_gs->GetXaxis()->SetTitle("#theta_{LAB} [deg]");
   ThetaLDist_gs->GetYaxis()->SetTitle("Counts"); 
   ThetaLDist_gs->SetLineColor(2);//RED 

   ThetaLDist_fs = new TH1F("ThLabfs","Lab angular distribution",180,0,180);
   ThetaLDist_fs->GetXaxis()->SetTitle("#theta_{LAB} [deg]");
   ThetaLDist_fs->GetYaxis()->SetTitle("Counts"); 
   ThetaLDist_fs->SetLineColor(4);//BLUE 


   ThetaCMDist_gs = new TH1F("ThCMGs","CM angular distribution",180,0,180);
   ThetaCMDist_gs->GetXaxis()->SetTitle("#theta_{CM} [deg]");
   ThetaCMDist_gs->GetYaxis()->SetTitle("Counts"); 
   ThetaCMDist_gs->SetLineColor(2);//RED 
   
   ThetaCMDist_fs = new TH1F("ThCMfs","CM angular distribution",180,0,180);
   ThetaCMDist_fs->GetXaxis()->SetTitle("#theta_{CM} [deg]");
   ThetaCMDist_fs->GetYaxis()->SetTitle("Counts"); 
   ThetaCMDist_fs->SetLineColor(4);//BLUE 

   //--------------------------------//
   //---------Open Cut File----------//
   if(enable_cut){
	   CutG_p_file = new TFile("EdE_CutG_p.root");
           CutG_p = (TCutG *)CutG_p_file->Get("EdE_p");
   }
   //--------------------------------//
   //------------Ex Gate-------------//
   if(enable_ExGate){
	  gs_Ex = new double[2]; 
	  fs_Ex = new double[2];
         
	  double res = 0.05; //MeV
	 
//          gs_Ex[0] = 0.0 - res;
//          gs_Ex[1] = 0.0 + res;
//
//          fs_Ex[0] = 0.870 - res;
//          fs_Ex[1] = 0.870 + res;
//
          gs_Ex[0] = 0.13;
          gs_Ex[1] = 0.3;

          fs_Ex[0] = 0.67;
          fs_Ex[1] = 1.1;

   }
   //--------------------------------//
   
   TString option = GetOption();
   
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

   nbEntries = fReader.GetEntries(true);
   if(entry%1000 == 0) printProgBar(entry*100.0/nbEntries);

   //Filling EdE matrix//
   for(size_t i=0; i<*nbParticleM2; i++){

	   EdE_M2->Fill( E[i], dE[i] );
           
   }
   //---------------------//
  
   //Filling impact matrix//
   for(size_t i=0; i<(*nbParticleM2); i++){

	impactM2->Fill(X[i],Y[i]);	   
           
   }
   
   for(size_t i=*nbParticleM2; i<(*nbParticleM2+*nbParticleMG); i++){

	impactMG->Fill(X[i],Y[i]);	   
           
   } 
   //-----------------------//
   
   //Filling multiplicity//
   nbPmultM2->Fill(*nbParticleM2);
   nbPmultMG->Fill(*nbParticleMG);
   //--------------------//
   
   //Filling ELab matrix and dist//
   
   //--------------MUST2---------------//
   for(size_t i=0; i<(*nbParticleM2); i++){
        if(enable_cut && enable_ExGate){
        	if(CutG_p->IsInside(E[i],dE[i])){
			ExDist->Fill(Ex[i]);
		}
		//GS check
        	if(CutG_p->IsInside(E[i],dE[i]) && Ex[i]>gs_Ex[0] && Ex[i]<gs_Ex[1]){
			ELabThetaL->Fill(ThetaLab[i],ELab[i]);
			ThetaLDist_gs->Fill(ThetaLab[i]);
			ThetaCMDist_gs->Fill(ThetaCM[i]);
		}
		//1S check
        	if(CutG_p->IsInside(E[i],dE[i]) && Ex[i]>fs_Ex[0] && Ex[i]<fs_Ex[1]){
			ELabThetaL->Fill(ThetaLab[i],ELab[i]);
			ThetaLDist_fs->Fill(ThetaLab[i]);
			ThetaCMDist_fs->Fill(ThetaCM[i]);
		}
	}
	else{
        	ELabThetaL->Fill(ThetaLab[i],ELab[i]);
                ExDist->Fill(Ex[i]);	
	}
   } 
  
    
   //--------------Mugast---------------//
   for(size_t i=*nbParticleM2; i<(*nbParticleM2+*nbParticleMG); i++){
        
        if(enable_ExGate){

		ExDist->Fill(Ex[i]);
		//GS check
        	if(Ex[i]>gs_Ex[0] && Ex[i]<gs_Ex[1]){
			ELabThetaL->Fill(ThetaLab[i],ELab[i]);
			ThetaLDist_gs->Fill(ThetaLab[i]);
			ThetaCMDist_gs->Fill(ThetaCM[i]);
		}
		//1S check
        	if(Ex[i]>fs_Ex[0] && Ex[i]<fs_Ex[1]){
			ELabThetaL->Fill(ThetaLab[i],ELab[i]);
			ThetaLDist_fs->Fill(ThetaLab[i]);
			ThetaCMDist_fs->Fill(ThetaCM[i]);
		}
	}
	else{
        	ELabThetaL->Fill(ThetaLab[i],ELab[i]);
                ExDist->Fill(Ex[i]);	
		ThetaLDist_gs->Fill(ThetaLab[i]);
		ThetaCMDist_gs->Fill(ThetaCM[i]);
		ThetaLDist_fs->Fill(ThetaLab[i]);
		ThetaCMDist_fs->Fill(ThetaCM[i]);
	}
           
   } 
   //--------------------------//
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
   //impactM2->Draw("col");
   //nbPmultM2->Draw();
   EdE_M2->Draw("col");
   if(enable_cut) CutG_p->Draw("same");

   C1->cd(2);
   //impactMG->Draw("col");
   //nbPmultMG->Draw();
   ExDist->Draw("");

   TCanvas *C2 = new TCanvas("C2","C2");
   C2->Divide(1,2);
   
   C2->cd(1);
   ELabThetaL->Draw("col");
   
   C2->cd(2);
//   ThetaLDist_gs->Draw();
//   ThetaLDist_fs->Draw("same");
   ThetaCMDist_gs->Draw();
   ThetaCMDist_fs->Draw("same");

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
