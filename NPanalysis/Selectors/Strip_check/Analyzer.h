//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Aug  2 09:44:20 2019 by ROOT version 6.14/04
// from TTree TreeMaster/TreeMaster
// found on file: /agata07_data5/comMugast/data_ganil/MERGED_DATA/MergedTree_run_0256_0000_Final.root
//////////////////////////////////////////////////////////

#ifndef Analyzer_h
#define Analyzer_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

// Headers needed by this particular selector
#include "TMust2Data.h"
#include "TMugastData.h"

#include <TH2D.h>
#include <vector>

class Analyzer : public TSelector {
public :
   TTreeReader     fReader;  //!the tree reader
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

   // Readers to access the data (delete the ones you do not need).
   TTreeReaderValue<TMust2Data> MUST2 = {fReader, "MUST2"};
   TTreeReaderValue<TMugastData> Mugast = {fReader, "Mugast"};
   
   //------------Strip Matrix----------//
   std::vector<int> Mugast_index;
   std::vector<int> MUST_index;
   
   std::vector<TH2D*> MugastDetector_X;
   std::vector<TH2D*> MUSTDetector_X;
   std::vector<TH2D*> MugastDetector_Y;
   std::vector<TH2D*> MUSTDetector_Y;
   //----------------------------------//

   Analyzer(TTree * /*tree*/ =0) { }
   virtual ~Analyzer() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();
   void printProgBar( float);
   
   ClassDef(Analyzer,0);

};

#endif

#ifdef Analyzer_cxx
void Analyzer::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the reader is initialized.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   fReader.SetTree(tree);
}

Bool_t Analyzer::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


#endif // #ifdef Analyzer_cxx
