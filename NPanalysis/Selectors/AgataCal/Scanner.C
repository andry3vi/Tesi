#include <fstream>
#include <vector>

#include <TH1F.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TTree.h>
#include <RooFit.h>
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooGaussian.h"
#include "RooAddPdf.h"
#include "RooPolynomial.h"
#include "RooPlot.h"

#include "colormod.h"



std::vector<double> Fitter(TH1F * histo, double *range, std::string ID){

 using namespace RooFit;
    TCanvas * FitCanvas = new TCanvas(ID.c_str(),ID.c_str());
    	// Declare observable x

	RooRealVar x("x", "x", range[0], range[1]);

	// Create two Gaussian PDFs g1(x,mean1,sigma) anf g2(x,mean2,sigma) and their parameters
	RooRealVar mean("mean", "mean of gaussians",(range[1]+range[0])*0.5, range[0], range[1] );
	RooRealVar sigma("sigma", "width of gaussians", 0.2,0.,0.8);

	RooGaussian gaus("gaus", "Signal", x, mean, sigma);

    // Build Chebychev polynomial p.d.f.
	RooRealVar a0("a0", "a0", 0, 0, 10.);
	RooRealVar a1("a1", "a1", 0, 0, 10.);
	RooRealVar a2("a2", "a2", 0., 0, 10.);
	// RooRealVar a3("a3", "a3", 0.2, 0., 1.);
	// RooRealVar a4("a4", "a4", 0.2, 0., 1.);
	//RooChebychev bkg("bkg", "Background", x, RooArgSet(a0, a1, a2));
   	RooPolynomial bkg("bkg", "Background", x, RooArgSet(a0, a1, a2));

	// Associated nsig/nbkg as expected number of events with sig/bkg _in_the_range_ "signalRange"
	RooRealVar nsig("nsig","number of signal events in signalRange",10000,0.,1E8) ;
	RooRealVar nbkg("nbkg","number of background events in signalRange",1000,0.,1E8) ;

	// Use AddPdf to extend the model. Giving as many coefficients as pdfs switches
	// on extension.
	RooAddPdf  model("model","(gaus+bkg)", RooArgList(bkg,gaus), RooArgList(nbkg,nsig)) ;

  histo->GetXaxis()->SetRangeUser(range[0], range[1] );

	RooDataHist *data = new RooDataHist("dh", "dh", x, Import(*histo));

	// Fit model to data
	model.fitTo(*data,PrintLevel(-1));


	FitCanvas->Divide(1,2);

	// Plot data and PDF overlaid
	RooPlot *xframe = x.frame(Title(ID.c_str()));
	data->plotOn(xframe, XErrorSize(0));

	model.plotOn(xframe);


	// Overlay the background+sig2 components of model with a dotted line
	//model.plotOn(xframe, Components(bkg));
	//model.plotOn(xframe, Components(RooArgSet(gaus, gaus_FS)),  LineStyle(kDashed));
	FitCanvas->cd(1);
	xframe->Draw();

    //bkg subtraction
	FitCanvas->cd(2);
    TH1F *subbkg = (TH1F*) bkg.createHistogram("subbkg",x,Binning(16384,0,3200));

    histo->GetXaxis()->SetRangeUser(range[0],range[1]);
	TH1F *peak = (TH1F*)histo->Clone();

    peak->GetXaxis()->SetRangeUser(range[0],range[1]);
	peak->Add(subbkg,-double(nbkg.getVal()));

    peak->Draw();

    double min = mean.getVal()-2.355*sigma.getVal();
    double max = mean.getVal()+2.355*sigma.getVal();
	int bmin = peak->GetXaxis()->FindBin(min);
    int bmax = peak->GetXaxis()->FindBin(max);

    TH1F *HL_peak = (TH1F*)peak->Clone();
    HL_peak->GetXaxis()->SetRange(bmin,bmax);

	HL_peak->SetFillColor(kRed);
	HL_peak->SetFillStyle(3003);
	HL_peak->Draw("SAME");

    double integral = peak->Integral(bmin,bmax);

    integral -= peak->GetBinContent(bmin)*(min-peak->GetXaxis()->GetBinLowEdge(bmin))/peak->GetXaxis()->GetBinWidth(bmin);
    integral -= peak->GetBinContent(bmax)*(peak->GetXaxis()->GetBinUpEdge(bmax)-max)/peak->GetXaxis()->GetBinWidth(bmax);

    FitCanvas->SaveAs(("Fit_inspect_tmp/"+ID+".png").c_str());

    delete data, FitCanvas, subbkg, peak, HL_peak;

    std::vector<double> results;
    results.push_back(mean.getVal());   //centroid
    results.push_back(sigma.getVal());  //sigma
    results.push_back(integral);        //counts integral
    results.push_back(nsig.getVal());   //fit integral
    results.push_back(sigma.getError());  //sigma error
    results.push_back(nsig.getError());  //fir integral error
    return results;
}

void printProgBar( float percent){
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

void Scanner(){


    string data_Filename = "~/Repository/Tesi/Data/nptreated/nptreated.root";   // tree data from the Commissioning

    TFile *data_File = new TFile(data_Filename.c_str());

    TTree *data_Tree = (TTree*)data_File->Get("PhysicsTree");

    int nbAdd;
    float AddE[100] ;
    int *AddId = new int(100);

    data_Tree->SetBranchAddress("nbAdd",&nbAdd);
  	data_Tree->SetBranchAddress("AddE",AddE);
  	data_Tree->SetBranchAddress("AddId",AddId);
    // data_Tree->SetBranchAddress("nbTrack",&nbAdd);
    // data_Tree->SetBranchAddress("trackE",AddE);
    // data_Tree->SetBranchAddress("trackCrystalID",AddId);

    std::map<int, int> IDlist; //map id-index
    IDlist[0]  = 0;
    IDlist[1]  = 1;
    IDlist[2]  = 2;
    IDlist[3]  = 3;
    IDlist[5]  = 4;
    IDlist[7]  = 5;
    IDlist[8]  = 6;
    IDlist[9]  = 7;
    IDlist[10] = 8;
    IDlist[11] = 9;
    IDlist[12] = 10;
    IDlist[13] = 11;
    IDlist[14] = 12;
    IDlist[15] = 13;
    IDlist[16] = 14;
    IDlist[17] = 15;
    IDlist[19] = 16;
    IDlist[21] = 17;
    IDlist[22] = 18;
    IDlist[23] = 19;
    IDlist[28] = 20;
    IDlist[29] = 21;
    IDlist[30] = 22;
    IDlist[31] = 23;
    IDlist[32] = 24;
    IDlist[33] = 25;
    IDlist[34] = 26;
    IDlist[35] = 27;
    IDlist[36] = 28;
    IDlist[37] = 29;
    IDlist[38] = 30;
    IDlist[39] = 31;
    IDlist[40] = 32;
    IDlist[41] = 33;
    IDlist[42] = 34;
    IDlist[43] = 35;
    IDlist[44] = 36;

    std::vector<TH1F * > HistoCrystal;
    HistoCrystal.reserve(IDlist.size());

    for (auto const& x : IDlist) HistoCrystal.push_back(new TH1F(std::to_string(x.first).c_str(),std::to_string(x.first).c_str(),2400,400,2800));

    Int_t nentries = (Int_t)data_Tree->GetEntries();


  	for (Int_t i=0;i<nentries;i++) {
      if(i%1000 == 0) printProgBar(i*100.0/nentries);
  	   data_Tree->GetEntry(i);

       for (size_t j = 0; j < nbAdd; j++) HistoCrystal[IDlist[AddId[j]]]->Fill(AddE[j]);


    }

    for (auto const& x : IDlist) HistoCrystal[x.second]->SaveAs(("Spectra_tmp/Histo"+std::to_string(x.first)+".root").c_str());

  }
