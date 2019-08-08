	//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Aug  8 07:22:10 2019 by ROOT version 6.14/04
// from TTree PhysicsTree/Data created / analysed with the NPTool package
// found on file: run256_wElThHeavy.root
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
#include "TMust2Physics.h"

#include "TMugastPhysics.h"

#include <vector>



class Analyzer : public TSelector {
public :
   TTreeReader     fReader;  //!the tree reader
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

   // Readers to access the data (delete the ones you do not need).
   TTreeReaderValue<TMust2Physics> MUST2 = {fReader, "MUST2"};
   TTreeReaderValue<TMugastPhysics> Mugast = {fReader, "Mugast"};
   TTreeReaderArray<double> Ex = {fReader, "Ex"};
   TTreeReaderArray<double> ELab = {fReader, "ELab"};
   TTreeReaderArray<double> ThetaLab = {fReader, "ThetaLab"};
   TTreeReaderArray<double> PhiLab = {fReader, "PhiLab"};
   TTreeReaderArray<double> ThetaCM = {fReader, "ThetaCM"};
   TTreeReaderArray<double> X = {fReader, "X"};
   TTreeReaderArray<double> Y = {fReader, "Y"};
   TTreeReaderArray<double> Z = {fReader, "Z"};
   TTreeReaderArray<double> E = {fReader, "E"};
   TTreeReaderArray<double> dE = {fReader, "dE"};
   TTreeReaderValue<Int_t> nbParticleM2 = {fReader, "nbParticleM2"};
   TTreeReaderValue<Int_t> nbParticleMG = {fReader, "nbParticleMG"};
   TTreeReaderArray<double> ThetaHeavy = {fReader, "ThetaHeavy"};
   TTreeReaderArray<double> EheavyAfterTg = {fReader, "EheavyAfterTg"};
   TTreeReaderValue<Int_t> Run = {fReader, "Run"};
   TTreeReaderValue<ULong64_t> LTS = {fReader, "LTS"};
   TTreeReaderValue<ULong64_t> TStrack = {fReader, "TStrack"};
   TTreeReaderValue<Int_t> nbTrack = {fReader, "nbTrack"};
   TTreeReaderArray<Float_t> trackE = {fReader, "trackE"};
   TTreeReaderArray<Float_t> trackX1 = {fReader, "trackX1"};
   TTreeReaderArray<Float_t> trackY1 = {fReader, "trackY1"};
   TTreeReaderArray<Float_t> trackZ1 = {fReader, "trackZ1"};
   TTreeReaderArray<Float_t> trackT = {fReader, "trackT"};
   TTreeReaderArray<Int_t> trackCrystalID = {fReader, "trackCrystalID"};
   TTreeReaderValue<Int_t> nbAdd = {fReader, "nbAdd"};
   TTreeReaderValue<ULong64_t> TSHit = {fReader, "TSHit"};
   TTreeReaderArray<Float_t> AddE = {fReader, "AddE"};
   TTreeReaderArray<Float_t> AddX = {fReader, "AddX"};
   TTreeReaderArray<Float_t> AddY = {fReader, "AddY"};
   TTreeReaderArray<Float_t> AddZ = {fReader, "AddZ"};

   //Desired plot//
   TH1F * nbPmultM2;
   TH1F * nbPmultMG;
   TH1F * nbPmultHv;
   TH1F * nbPmultAg;
   TH1D * Edopp;
   TH1D * Eraw;



   //------------//

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
