#ifndef Analysis_h 
#define Analysis_h
/*****************************************************************************
 * Copyright (C) 2009-2014    this file is part of the NPTool Project        *
 *                                                                           *
 * For the licensing terms see $NPTOOL/Licence/NPTool_Licence                *
 * For the list of contributors see $NPTOOL/Licence/Contributors             *
 *****************************************************************************/

/*****************************************************************************
 * Original Author: Adrien MATTA  contact address: a.matta@surrey.ac.uk      *
 *                                                                           *
 * Creation Date  : march 2025                                               *
 * Last update    :                                                          *
 *---------------------------------------------------------------------------*
 * Decription:                                                               *
 * Class describing the property of an Analysis object                       *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Comment:                                                                  *
 *                                                                           *
 *                                                                           *
 *****************************************************************************/
#include"NPVAnalysis.h"
#include"NPEnergyLoss.h"
#include"NPReaction.h"
#include"RootOutput.h"
#include"RootInput.h"
#include "TMust2Physics.h"
#include "TMugastPhysics.h"
#include "TInitialConditions.h"
#include <TRandom3.h>
#include <TVector3.h>
#include <TMath.h>
#include <vector>
#include "GaspardTracker.h"

class Analysis: public NPL::VAnalysis{
  public:
    Analysis();
    ~Analysis();

  public: 
    void Init();
    void TreatEvent();
    void End();

  void InitOutputBranch();
  void InitInputBranch();
  void ReInitValue();
  static NPL::VAnalysis* Construct();
 
  private:
  std::vector<double> Ex;
  std::vector<double> ELab;
  std::vector<double> ThetaLab;
  std::vector<double> ThetaCM;
  std::vector<double> X ;
  std::vector<double> Y ;
  std::vector<double> Z ;
  
  std::vector<double> E;
  std::vector<double> dE;
  
  int nbParticleM2;
  int nbParticleMG;

  NPL::Reaction myReaction;
    //	Energy loss table: the G4Table are generated by the simulation
  NPL::EnergyLoss LightTarget;
  NPL::EnergyLoss LightAl;
  NPL::EnergyLoss LightSi;
  NPL::EnergyLoss BeamCD2;
  NPL::EnergyLoss* BeamWindow;
  NPL::EnergyLoss* LightWindow;

  double TargetThickness ;
  double WindowsThickness;
  // Beam Energy
  double OriginalBeamEnergy ; // AMEV
  double FinalBeamEnergy; 
  
  // intermediate variable
  TVector3 BeamDirection;
  TVector3 BeamImpact;
  TRandom3 Rand ;
  int Run;
  int DetectorNumber  ;
  double ThetaNormalTarget;
  double ThetaM2Surface ;
  double ThetaMGSurface ;
  double Si_E_M2 ;
  double CsI_E_M2  ;
  double Energy ;
  double ThetaGDSurface ;

  //Agata Branches
  int nbHits;
  int nbTrack;  
  float *trackE= new float(100);
  float *trackX1= new float(100);
  float *trackY1= new float(100);
  float *trackZ1= new float(100);
  float *trackT= new float(100);
  int *trackCrystalID = new int(100);
  int nbCores;
  int *coreId= new int(100);
  ULong64_t *coreTS= new ULong64_t(100);
  float *coreE0= new float(100);
  ULong64_t VTS; 

  double dTheta;
  // Branches and detectors
  TMust2Physics* M2;
  //TMugastPhysics* MG;
  GaspardTracker* GD;
  TInitialConditions* myInit ;

};
#endif
