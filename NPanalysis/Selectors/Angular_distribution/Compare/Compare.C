/*Comparison of Efficiency angular distribution*/

//INCLUDE//
#include <iostream>
#include <fstream>
#include <TPaveStats.h>
//INCLUDE//


string EfficiencyFilename_1 = "../../Data/npsim/EfficiencyAD_VMugast.root";    //TH1F filename
string EfficiencyFilename_2 = "../../Data/npsim/EfficiencyAD_VGaspard.root";    //TH1F filename


//macro
void Compare()
{

gStyle->SetOptStat(0);


	TFile * EfficiencyFile_1 = new TFile( EfficiencyFilename_1.c_str());
	TFile * EfficiencyFile_2 = new TFile( EfficiencyFilename_2.c_str());


	TH1F * Efficiency_1 = (TH1F*) EfficiencyFile_1->Get("ThCMEff");
	TH1F * Efficiency_2 = (TH1F*) EfficiencyFile_2->Get("ThCMEff");

	TCanvas * C1 = new TCanvas("C1","C1");
	C1->cd();

	Efficiency_1->Scale(1.0/1.0E7); //scaling factor evaluated as #Ev_generated/cross_section_integral
	Efficiency_2->Scale(1.0/1.0E7);
  Efficiency_1->SetStats(kFALSE);
	Efficiency_2->SetStats(kFALSE);
	Efficiency_2->GetXaxis()->SetTitle("#theta_{CM} [deg]");
  Efficiency_2->GetYaxis()->SetTitle("Efficiency [n.u.]");
	Efficiency_2->GetXaxis()->SetRangeUser(0,180);

	Efficiency_2->Draw();
	Efficiency_1->Draw("SAME");


	return ;
}
