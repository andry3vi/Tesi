/*Production of differential cross section from experimental angular distribution*/

//INCLUDE//
#include <iostream>
#include <fstream>
#include <TPaveStats.h>
//INCLUDE//

string O17_fs_THFilename  = "../../Data/Xsection/16Odp17O_12p.dat";   //.dat filename
string O17_gs_THFilename  = "../../Data/Xsection/16Odp17O_52p.dat";   //.dat filename
string EfficiencyFilename = "../../Data/npsim/EfficiencyAD_VMugast_stripfix.root";    //TH1F filename
string O17_gsFilename     = "../results/ADistCMgs.root";
string O17_fsFilename     = "../results/ADistCMfs.root";
// string O17_gsFilename     = "../results/Angular_Distribution/ADistCMgs_simulated.root";
// string O17_fsFilename     = "../results/Angular_Distribution/ADistCMfs_simulated.root";
TGraph * O17_fs_TH = 0;
TGraph * O17_gs_TH = 0;




Double_t FitFunctionGS(Double_t *v,Double_t *par) {
	//Double_t cross = CrossSection_fs_Spline->Eval(v[0]);
	Double_t cross = O17_gs_TH->Eval(v[0]);
	Double_t value = par[0]*cross;

	return value;
}

Double_t FitFunctionFS(Double_t *v,Double_t *par) {
	//Double_t cross = CrossSection_fs_Spline->Eval(v[0]);
	Double_t cross = O17_fs_TH->Eval(v[0]);
	Double_t value = par[0]*cross;

	return value;
}

double factor(float theta1, float theta2){

	return 2*M_PI*(cos(M_PI*theta1/180.0)-cos(M_PI*theta2/180.0));

}


//macro
void Differential()
{

gStyle->SetOptStat(0);


	TFile * EfficiencyFile = new TFile( EfficiencyFilename.c_str());
	TFile * O17_gsFile     = new TFile( O17_gsFilename.c_str());
	TFile * O17_fsFile     = new TFile( O17_fsFilename.c_str());

	TH1F * Efficiency = (TH1F*) EfficiencyFile->Get("ThCMEff");
	TH1F * O17_gs     = (TH1F*) O17_gsFile->Get("ThCMgs");
	TH1F * O17_fs     = (TH1F*) O17_fsFile->Get("ThCMfs");
	TH1F * Hfactor = new TH1F("factor","factor",180,0,180);

	//O17_gs->SetDefaultSumw2(kFALSE);
	//O17_fs->SetDefaultSumw2(kFALSE);



	TCanvas * C1 = new TCanvas("C1","C1");
	C1->cd();

	O17_fs->SetLineColor(kBlue);
	O17_fs->SetLineWidth(2);
	O17_fs->Draw("L");

	O17_gs->SetLineColor(kRed);
	O17_gs->SetLineWidth(2);
	O17_gs->Draw("SAME L");

	C1->Update();

	//Efficiency->Scale(1.0/10000000); //scaling factor evaluated as #Ev_generated/cross_section_integral




	TH1F * O17_gs_treated = (TH1F*) O17_gs->Clone("ThCMGs_treated");
	TH1F * O17_fs_treated = (TH1F*) O17_fs->Clone("ThCMFs_treated");

	TH1F * O17_gs_eff = (TH1F*) O17_gs->Clone("ThCMGs_treated");
	TH1F * O17_fs_eff = (TH1F*) O17_fs->Clone("ThCMFs_treated");

	//O17_fs_treated->Sumw2(kFALSE);
	//O17_gs_treated->Sumw2(kFALSE);

	for(size_t i=0 ; i<180 ; i++){

		Hfactor->SetBinContent(i+1,factor(i,i+1));
		double Yeld_1      = O17_gs_treated->GetBinContent(i+1);
		double SigmaYeld_1 = O17_gs_treated->GetBinError(i+1);
		double Yeld_2      = O17_fs_treated->GetBinContent(i+1);
		double SigmaYeld_2 = O17_fs_treated->GetBinError(i+1);
		double SigmaEff 	 = Efficiency->GetBinError(i+1);
		double Eff         = Efficiency->GetBinContent(i+1);
		double IntFact     = factor(i,i+1);

		if(Eff !=0){


			O17_fs_treated->SetBinContent(i+1,Yeld_2/(Eff));
			O17_gs_treated->SetBinContent(i+1,Yeld_1/(Eff));

			Efficiency->SetBinContent(i+1,Eff/IntFact);

			O17_fs_eff->SetBinContent(i+1,Yeld_2/(Eff));
			O17_gs_eff->SetBinContent(i+1,Yeld_1/(Eff));

			// O17_fs_treated->SetBinError(i+1,sqrt(pow(SigmaYeld_2/(Eff*IntFact),2)+pow(Yeld_2*SigmaEff/(Eff*IntFact),2)));
			// O17_gs_treated->SetBinError(i+1,sqrt(pow(SigmaYeld_1/(Eff*IntFact),2)+pow(Yeld_1*SigmaEff/(Eff*IntFact),2)));

			O17_fs_treated->SetBinError(i+1,sqrt(pow(SigmaYeld_2/(Eff),2)+pow(Yeld_2*SigmaEff/(Eff*Eff),2)));
			O17_gs_treated->SetBinError(i+1,sqrt(pow(SigmaYeld_1/(Eff),2)+pow(Yeld_1*SigmaEff/(Eff*Eff),2)));

			O17_fs_eff->SetBinError(i+1,0);//sqrt(pow(SigmaYeld_2/(Eff*IntFact),2)+pow(Yeld_2*SigmaEff/(Eff*IntFact),2)));
			O17_gs_eff->SetBinError(i+1,0);//sqrt(pow(SigmaYeld_1/(Eff*IntFact),2)+pow(Yeld_1*SigmaEff/(Eff*IntFact),2)));
			//O17_fs_treated->SetBinContent(i+1,O17_fs_treated->GetBinContent(i+1)/(Efficiency->GetBinContent(i+1)));
			//O17_gs_treated->SetBinContent(i+1,O17_gs_treated->GetBinContent(i+1)/(Efficiency->GetBinContent(i+1)));

		}
		else{
			O17_fs_treated->SetBinContent(i+1,0);
			O17_gs_treated->SetBinContent(i+1,0);

		}
	}

	//O17_fs_treated->Rebin(2);
	//O17_gs_treated->Rebin(2);


	//Retrieving xsec dist from .dat file
	ifstream O17_fs_THFile(O17_fs_THFilename.c_str(),std::ifstream::in);
	ifstream O17_gs_THFile(O17_gs_THFilename.c_str(),std::ifstream::in);

	std::vector<double> X;
	std::vector<double> Y;

	//-------------gs theoretical cross section-------------//
	while(!O17_gs_THFile.eof()){
		if(!O17_gs_THFile.eof()){
			double X_p,Y_p;

			O17_gs_THFile>>X_p;
			O17_gs_THFile>>Y_p;

			X.push_back(X_p);
			Y.push_back(Y_p);

		}
	}

	O17_gs_TH = new TGraph(X.size());

	for(size_t i = 0; i<X.size(); i++){
		O17_gs_TH->SetPoint(i,X[i],Y[i]);
	}

	TF1 * fitGS = new TF1("fitGS",FitFunctionGS,15,35,1);

	fitGS->SetParameter(0,1E3);

	O17_gs_treated->Fit("fitGS","RWN");
	cout<<endl<<endl<<"fitGS chisq-> "<<fitGS->GetChisquare()<<endl<<endl;
	double rescalingGS = fitGS->GetParameter(0);

	X.clear();
	Y.clear();
	//-------------fs theoretical cross section-------------//
	while(!O17_fs_THFile.eof()){
		if(!O17_fs_THFile.eof()){
			double X_p,Y_p;

			O17_fs_THFile>>X_p;
			O17_fs_THFile>>Y_p;

			X.push_back(X_p);
			Y.push_back(Y_p);

		}
	}

	O17_fs_TH = new TGraph(X.size());

	for(size_t i = 0; i<X.size(); i++){
		O17_fs_TH->SetPoint(i,X[i],Y[i]);
	}

	TF1 * fitFS = new TF1("fitFS",FitFunctionFS,15,35,1);

	fitFS->SetParameter(0,1E3);

	O17_fs_treated->Fit("fitFS","RWN");
	//cout<<endl<<endl<<"fitFS chisq-> "<<fitFS->GetChisquare()<<endl<<endl;
	double rescalingFS = fitFS->GetParameter(0);


	O17_fs_treated->Scale(1.0/rescalingFS);
	O17_gs_treated->Scale(1.0/rescalingGS);

	TCanvas * Cfactor = new TCanvas("Cfactor","Cfactor");
	Cfactor->cd();
	// //O17_fs_TH->Draw();
	// fit->Draw();
	// CrossSection_fs_Spline->Draw("SAME");
	//fit->Draw();
	//Cfactor->Divide(1,2);

	//Cfactor->cd(1);
	//Efficiency->Draw();

	//Cfactor->cd(2);
	Hfactor->Draw();

	//Cfactor->Update();

	TCanvas * C = new TCanvas("C","C");
	C->cd();
  C->SetLogy();

	//acceptance lines
	std::vector<TLine *> Acceptance;

	//anular detector
	Acceptance.push_back( new TLine(3.5,0,3.5,1E3));
	Acceptance.push_back( new TLine(7.5,0,7.5,1E3));
	//trapezoidal detector
	Acceptance.push_back( new TLine(8.5,0,8.5,1E3));
	Acceptance.push_back( new TLine(37.5,0,37.5,1E3));


	O17_fs_treated->SetLineColor(kBlue);
	O17_fs_treated->SetLineWidth(2);
	O17_fs_treated->GetYaxis()->SetRangeUser(0.1,5E3);
	O17_fs_treated->GetXaxis()->SetRangeUser(0,45);
	O17_fs_treated->GetXaxis()->SetTitle("#theta_{CM} [deg]");
	O17_fs_treated->GetYaxis()->SetTitle("d#sigma/d#Omega [a.u.]");
	O17_fs_treated->SetStats(kFALSE);
	O17_fs_treated->Draw();

	O17_fs_TH->Draw("SAME");
	O17_gs_TH->Draw("SAME");

	O17_gs_treated->SetLineColor(kRed);
	O17_gs_treated->SetLineWidth(2);
	O17_gs_treated->SetStats(kFALSE);
	O17_gs_treated->Draw("SAME");


	// //Efficiency
	// Efficiency->Scale(0.1);
	// Efficiency->Draw("SAMEHIST");
	for(size_t i=0 ; i<Acceptance.size() ; i++){
		Acceptance[i]->SetLineWidth(2);
		Acceptance[i]->SetLineColor(46);
		Acceptance[i]->SetLineStyle(9);
		//Acceptance[i]->Draw("SAME");
	}

	//Acceptance[0]->Draw("SAME");
	//Acceptance.back()->Draw("SAME");
	C->Update();

	TCanvas * C2 = new TCanvas("C2","C2");

	C2->cd();
  O17_fs_TH->Draw();
	O17_gs_TH->Draw("SAME");

	//O17_gs_eff->Draw();
	//O17_fs_eff->Draw("SAME");

	return ;
}
