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
#include "TAnnularS1Physics.h"
#include "TInitialConditions.h"
#include <TRandom3.h>
#include <TVector3.h>
#include <TMath.h>
#include <vector>
#include <set>
#include <string>
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
  
  std::set<std::string> BadStripSet;
  std::set<std::string>::iterator StripIt;
 
  int trapeX[128] = {64,57,62,55,60,53,58,51,56,49,54,59,52,61,50,63,47,48,45,46,43,44,41,42,39,40,37,38,35,36,33,34,29,30,27,28,25,26,23,24,21,22,19,20,17,18,32,16,13,2,11,4,9,6,1,8,3,10,7,12,5,14,15,31,127,128,125,126,123,124,121,122,119,120,117,118,115,116,113,114,111,112,109,110,107,108,105,106,103,104,101,102,99,100,97,98,93,94,91,92,89,90,87,88,85,86,83,84,81,82,79,80,77,78,75,76,73,74,71,72,69,70,67,68,65,66,96,95 };

  int trapeY[128] = {4,128,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,37,39,41,43,45,47,49,51,53,55,57,59,61,63,2,65,67,69,71,73,75,77,79,81,83,85,87,89,91,93,95,97,99,101,103,105,107,109,111,113,115,117,119,121,123,125,127,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,106,108,110,112,114,116,118,120,122,124,126};

  int anularX[64] = {64,62,63,60,61,58,59,56,57,54,55,52,53,50,51,48,49,46,47,44,45,42,43,40,41,38,39,36,37,34,32,30,65,67,71,69,75,73,79,77,83,81,87,85,91,89,95,93,99,97,103,101,107,105,111,109,115,113,119,117,123,121,125,127};
 
  int anularY[16] = {46,50,48,44,40,36,26,22,18,14,16,20,24,28,38,42};

  int GaspardMap[5] = {7,3,1,5,4};
  
  int nbParticleM2;
  int nbParticleMG;
  int nbParticleAN;

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
  double ThetaANSurface ;
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
  TAnnularS1Physics* AN;
  GaspardTracker* GDtracker;
  TGaspardTrackerPhysics *GD;
  TInitialConditions* myInit ;

};
#endif