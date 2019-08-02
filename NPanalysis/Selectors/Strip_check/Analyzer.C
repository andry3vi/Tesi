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
#include <TCanvas.h>

#include "colormod.h"
#include <iomanip>
#include <iostream>
#include <string>

void Analyzer::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

   //-------index-------//
   Mugast_index.push_back(1);//trapezoid
   Mugast_index.push_back(3);//trapezoid
   Mugast_index.push_back(4);//trapezoid
   Mugast_index.push_back(5);//trapezoid
   Mugast_index.push_back(7);//trapezoid
   Mugast_index.push_back(11);//anular

   MUST_index.push_back(1);
   MUST_index.push_back(2);
   MUST_index.push_back(3);
   MUST_index.push_back(4);
   //-------------------//
   
   
   //-----Strip matrix initialization-----//
   
   //MUGAST//
   for(size_t i = 0; i<Mugast_index.size(); i++){
   	string nameX="MugastNb_"+to_string(Mugast_index[i])+"_X";
   	string nameY="MugastNb_"+to_string(Mugast_index[i])+"_Y";

	MugastDetector_X.push_back(new TH2D(nameX.c_str(),nameX.c_str(),128,0.5,128.5,1000,0,10000));
	MugastDetector_Y.push_back(new TH2D(nameY.c_str(),nameY.c_str(),128,0.5,128.5,1000,0,10000));
   }
   //MUST//
   for(size_t i = 0; i<MUST_index.size(); i++){
   	string nameX="MUSTNb_"+to_string(MUST_index[i])+"_X";
   	string nameY="MUSTNb_"+to_string(MUST_index[i])+"_Y";

	MUSTDetector_X.push_back(new TH2D(nameX.c_str(),nameX.c_str(),128,0.5,128.5,1000,0,10000));
	MUSTDetector_Y.push_back(new TH2D(nameY.c_str(),nameY.c_str(),128,0.5,128.5,1000,0,10000));
   }


   //-------------------------------------//   

   
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

   size_t nbEntries = fReader.GetEntries(true);
   if(entry%1000 == 0) printProgBar(entry*100.0/nbEntries);

   
    
   //------Mugast spectra filling------//
   for(size_t i = 0; i<Mugast->GetDSSDXEMult(); i++){
   	for(size_t j = 0; j<Mugast_index.size(); j++){
	
		if( (Mugast->fMG_DSSDXE_DetectorNbr)[i]==Mugast_index[j]){
	           
			MugastDetector_X[j]->Fill( (Mugast->fMG_DSSDXE_StripNbr)[i] , (Mugast->fMG_DSSDXE_Energy)[i] );

		}

	}
   } 
   
   for(size_t i = 0; i<Mugast->GetDSSDYEMult(); i++){
   	for(size_t j = 0; j<Mugast_index.size(); j++){
	
		if( (Mugast->fMG_DSSDYE_DetectorNbr)[i]==Mugast_index[j]){
	           
			MugastDetector_Y[j]->Fill( (Mugast->fMG_DSSDYE_StripNbr)[i] , (Mugast->fMG_DSSDYE_Energy)[i] );

		}

	}
   } 
   //----------------------------------//
   
   //------MUST spectra filling------//
   for(size_t i = 0; i<MUST2->GetMMStripXEMult(); i++){
   	for(size_t j = 0; j<MUST_index.size(); j++){
	
		if( (MUST2->fMM_StripXE_DetectorNbr)[i]==MUST_index[j]){
	           
			MUSTDetector_X[j]->Fill( (MUST2->fMM_StripXE_StripNbr)[i] , (MUST2->fMM_StripXE_Energy)[i] );

		}

	}
   } 
   
   for(size_t i = 0; i<MUST2->GetMMStripYEMult(); i++){
   	for(size_t j = 0; j<MUST_index.size(); j++){
	
		if( (MUST2->fMM_StripYE_DetectorNbr)[i]==MUST_index[j]){
	           
			MUSTDetector_Y[j]->Fill( (MUST2->fMM_StripYE_StripNbr)[i] , (MUST2->fMM_StripYE_Energy)[i] );

		}

	}
   } 
   //----------------------------------//
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
 
   std::vector<TCanvas*> Canvas;
   for(size_t i = 0; i<Mugast_index.size(); i++){
	string name="MugastNb_"+to_string(Mugast_index[i]);
	Canvas.push_back(new TCanvas(name.c_str(),name.c_str()));
        Canvas.back()->Divide(1,2);
       
       	Canvas.back()->cd(1);
        MugastDetector_X[i]->Draw("col");
        
       	Canvas.back()->cd(2);
        MugastDetector_Y[i]->Draw("col");

   } 
   
   for(size_t i = 0; i<MUST_index.size(); i++){
	string name="MUSTNb_"+to_string(MUST_index[i]);
	Canvas.push_back(new TCanvas(name.c_str(),name.c_str()));
        Canvas.back()->Divide(1,2);
       
       	Canvas.back()->cd(1);
        MUSTDetector_X[i]->Draw("col");
        
       	Canvas.back()->cd(2);
        MUSTDetector_Y[i]->Draw("col");

   } 
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

