//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Apr  2 10:54:44 2019 by ROOT version 6.14/04
// from TTree PhysicsTree/Data created / analysed with the NPTool package
// found on file: test4.root
//////////////////////////////////////////////////////////

#ifndef MugastSelector_h
#define MugastSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

// Headers needed by this particular selector
#include "TMust2Physics.h"

#include "TMugastPhysics.h"

#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TCutG.h>

 //nptool headers
 #include "TInitialConditions.h"
 #include "TInteractionCoordinates.h"
 #include "NPDetectorManager.h"
 #include "NPReaction.h"
 
using namespace NPL;



class MugastSelector : public TSelector {
public :
   TTreeReader     fReader;  //!the tree reader
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

   // Readers to access the data (delete the ones you do not need).
   TTreeReaderValue<TMust2Physics> MUST2 = {fReader, "MUST2"};
   TTreeReaderValue<Double_t> GATCONFMASTER = {fReader, "GATCONFMASTER"};
   TTreeReaderValue<Double_t> GATCONFSLAVE = {fReader, "GATCONFSLAVE"};
   TTreeReaderValue<Double_t> GATCONFSLAVE2 = {fReader, "GATCONFSLAVE2"};
   TTreeReaderValue<TMugastPhysics> Mugast = {fReader, "Mugast"};
 //  TTreeReaderValue<Double_t> Ex = {fReader, "Ex"};
   TTreeReaderValue<Double_t> ELab = {fReader, "ELab"};
   TTreeReaderValue<Double_t> ThetaLab = {fReader, "ThetaLab"};
   TTreeReaderValue<Double_t> ThetaCM = {fReader, "ThetaCM"};
   TTreeReaderValue<Int_t> Run = {fReader, "Run"};
   TTreeReaderValue<Double_t> X = {fReader, "X"};
   TTreeReaderValue<Double_t> Y = {fReader, "Y"};
   TTreeReaderValue<Double_t> Z = {fReader, "Z"};
   TTreeReaderValue<Double_t> dE = {fReader, "dE"};
   TTreeReaderValue<Int_t> nbTrack = {fReader, "nbTrack"};
   TTreeReaderArray<Float_t> trackE = {fReader, "trackE"};
   TTreeReaderArray<Float_t> trackX1 = {fReader, "trackX1"};
   TTreeReaderArray<Float_t> trackY1 = {fReader, "trackY1"};
   TTreeReaderArray<Float_t> trackZ1 = {fReader, "trackZ1"};
   TTreeReaderArray<Float_t> trackT = {fReader, "trackT"};
   TTreeReaderArray<Int_t> trackCrystalID = {fReader, "trackCrystalID"};
   TTreeReaderValue<Int_t> nbCores = {fReader, "nbCores"};
   TTreeReaderArray<Int_t> coreId = {fReader, "coreId"};
   TTreeReaderArray<ULong64_t> coreTS = {fReader, "coreTS"};
   TTreeReaderArray<Float_t> coreE0 = {fReader, "coreE0"};
   //vector of Ex to be calculated with every reaction
   std::vector<Double_t> Ex;

   MugastSelector(TTree * /*tree*/ =0) { }
   virtual ~MugastSelector() { }
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
   Double_t CorrectDoppler(Double_t En, Double_t beta, Double_t X, Double_t Y, Double_t Z );
    
   TFile *pp_file;
   TFile *dd_file;
   TFile *tt_file;
   std::vector<TCutG*> cuts;//Order: pp, dd, tt

   std::vector<TH1D*> hEx;
   std::vector<TH1D*> hgamma;
   std::vector<TH1D*> hgammaDC;
   std::vector<TH2D*> mgammagamma;
   std::vector<TH2D*> mgammagammaDC;
   std::vector<TH2D*> mElabAlab;
   std::vector<TH2D*> mExgamma;
   
   TH2D* mTOFElab;
   TH3D* AGA_XYZ;
   TH3D* MU_XYZ;

   std::vector<NPL::Reaction*> reaction;       

   std::vector<Double_t> beta;

   std::vector<TCanvas*> c;
   TCanvas* Cut;
   ClassDef(MugastSelector,0);

};

#endif

#ifdef MugastSelector_cxx
void MugastSelector::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the reader is initialized.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   fReader.SetTree(tree);
}

Bool_t MugastSelector::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


#endif // #ifdef MugastSelector_cxx
