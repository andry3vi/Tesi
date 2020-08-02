//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jul 31 12:30:47 2019 by ROOT version 6.14/04
// from TTree PhysicsTree/Data created / analysed with the NPTool package
// found on file: run256_wrong_det_pos.root
//////////////////////////////////////////////////////////

#ifndef Analyzer_GCutGate_h
#define Analyzer_GCutGate_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

// Headers needed by this particular selector
//#include "TMust2Physics.h"

//#include "TMugastPhysics.h"
#include <TH1F.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TCutG.h>
#include <vector>


class Analyzer_GCutGate : public TSelector {
public :
   TTreeReader     fReader;  //!the tree reader
   TTree          *fChain = 0;   //!pointer to the analyzed TTree or TChain

   // Readers to access the data (delete the ones you do not need).
   //TTreeReaderValue<TMust2Physics> MUST2 = {fReader, "MUST2"};
   //TTreeReaderValue<Double_t> GATCONFMASTER = {fReader, "GATCONFMASTER"};
   //TTreeReaderValue<Double_t> GATCONFSLAVE = {fReader, "GATCONFSLAVE"};
   //TTreeReaderValue<Double_t> GATCONFSLAVE2 = {fReader, "GATCONFSLAVE2"};
   //TTreeReaderValue<TMugastPhysics> Mugast = {fReader, "Mugast"};
   TTreeReaderArray<double> Ex = {fReader, "Ex"};
   TTreeReaderArray<double> ELab = {fReader, "ELab"};
   TTreeReaderArray<double> ThetaLab = {fReader, "ThetaLab"};
   TTreeReaderArray<double> ThetaCM = {fReader, "ThetaCM"};
   TTreeReaderArray<double> X = {fReader, "X"};
   TTreeReaderArray<double> Y = {fReader, "Y"};
   TTreeReaderArray<double> Z = {fReader, "Z"};
   TTreeReaderArray<double> E = {fReader, "E"};
   TTreeReaderArray<double> dE = {fReader, "dE"};
   TTreeReaderValue<Int_t> nbParticleM2 = {fReader, "nbParticleM2"};
   TTreeReaderValue<Int_t> nbParticleMG = {fReader, "nbParticleMG"};
   TTreeReaderValue<Int_t> Run = {fReader, "Run"};

   //Desired plot//
   TH2D * ExThetaL;

   TH2D * EdE_M2;
   TH1F * nbPmultM2;
   TH1F * nbPmultMG;

   TH2F * impactMG;
   TH2F * impactM2;

   TH2D * ELabThetaL;
   TH2D * ELabThetaLSelected;
   TH1D * ExDist;

   TH1F * ThetaLDist_gs;
   TH1F * ThetaCMDist_gs;

   TH1F * ThetaLDist_fs;
   TH1F * ThetaCMDist_fs;

   TGraph * kineGS;
   TGraph * kineFS;

   Long64_t nbEntries;

   bool enable_cut;
   TFile * CutG_p_file;
   TCutG * CutG_p;

   TFile * CutG_12p_file;
   TCutG * CutG_12p;
   TFile * CutG_52p_file;
   TCutG * CutG_52p;


   bool enable_ExGate;
   double * gs_Ex;
   double * fs_Ex;

   int CountExGS;
   int CountExFS;
   //------------//

   Analyzer_GCutGate(TTree * /*tree*/ =0) { }
   virtual ~Analyzer_GCutGate() { }
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

   ClassDef(Analyzer_GCutGate,0);

};

#endif

#ifdef Analyzer_GCutGate_cxx
void Analyzer_GCutGate::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the reader is initialized.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   fReader.SetTree(tree);
}

Bool_t Analyzer_GCutGate::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}


#endif // #ifdef Analyzer_GCutGate_cxx
