/*Production of differential cross section from experimental angular distribution*/

//INCLUDE//
#include <iostream>
#include <fstream>
//INCLUDE//

string EfficiencyFilename = "results/Efficiency.root";    //TH1F filename
string O17_gsFilename     = "results/ADistCMgs.root";     
string O17_fsFilename     = "results/ADistCMfs.root";     


double factor(float theta1, float theta2){

     return 2*M_PI*(cos(M_PI*theta1/180.0)-cos(M_PI*theta2/180.0));

}


//macro
void Differential()
{

   TFile * EfficiencyFile = new TFile( EfficiencyFilename.c_str());
   TFile * O17_gsFile     = new TFile( O17_gsFilename.c_str());
   TFile * O17_fsFile     = new TFile( O17_fsFilename.c_str());

   TH1F * Efficiency = (TH1F*) EfficiencyFile->Get("ThCMEff");
   TH1F * O17_gs     = (TH1F*) O17_gsFile->Get("ThCMgs");
   TH1F * O17_fs     = (TH1F*) O17_fsFile->Get("ThCMfs");
   TH1F * Hfactor = new TH1F("factor","factor",180,0,180);
   
   TCanvas * C1 = new TCanvas("C1","C1");
   C1->cd();
   
   O17_fs->SetLineColor(kBlue);
   O17_fs->SetLineWidth(2);
   O17_fs->Draw("L");

   O17_gs->SetLineColor(kRed);
   O17_gs->SetLineWidth(2);
   O17_gs->Draw("SAME L");

   C1->Update();
   
   Efficiency->Scale(180/10E6); //scaling factor evaluated as #Ev_generated/cross_section_integral
   

   
   
   TH1F * O17_gs_treated = (TH1F*) O17_gs->Clone("ThCMGs_treated");
   TH1F * O17_fs_treated = (TH1F*) O17_fs->Clone("ThCMFs_treated");

   for(size_t i=0 ; i<180 ; i++){
   	  
          Hfactor->SetBinContent(i+1,factor(i,i+1));
	  //O17_fs_treated->SetBinContent(i+1,factor(i,i+1));
	  if(Efficiency->GetBinContent(i+1)!=0){

	  	O17_fs_treated->SetBinContent(i+1,O17_fs_treated->GetBinContent(i+1)/(factor(i,i+1)*Efficiency->GetBinContent(i+1)));
	  	O17_gs_treated->SetBinContent(i+1,O17_gs_treated->GetBinContent(i+1)/(factor(i,i+1)*Efficiency->GetBinContent(i+1)));
	  	//O17_fs_treated->SetBinContent(i+1,O17_fs_treated->GetBinContent(i+1)/(Efficiency->GetBinContent(i+1)));
	  	//O17_gs_treated->SetBinContent(i+1,O17_gs_treated->GetBinContent(i+1)/(Efficiency->GetBinContent(i+1)));
   
	  }
	  else{
	  	O17_fs_treated->SetBinContent(i+1,0);
	  	O17_gs_treated->SetBinContent(i+1,0);

	  }
   }   
 

   TCanvas * Cfactor = new TCanvas("Cfactor","Cfactor");
   Cfactor->Divide(1,2);

   Cfactor->cd(1);
   Efficiency->Draw();

   Cfactor->cd(2);
   Hfactor->Draw();

   Cfactor->Update();

   TCanvas * C = new TCanvas("C","C");
   C->cd();
   
   //Efficiency->Draw();
   
   O17_fs_treated->SetLineColor(kBlue);
   O17_fs_treated->SetLineWidth(2);
   O17_fs_treated->Draw();

   O17_gs_treated->SetLineColor(kRed);
   O17_gs_treated->SetLineWidth(2);
   //O17_gs_treated->Draw("SAME L");

   C->Update();

   return ;
}
