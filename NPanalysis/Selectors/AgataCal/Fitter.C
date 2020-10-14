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



std::vector<double> Fit(TH1F * histo, double *range, std::string ID){

 using namespace RooFit;
 TCanvas * FitCanvas = new TCanvas(ID.c_str(),ID.c_str());
    	// Declare observable x

	RooRealVar x("x", "x", range[0], range[1]);

	// Create two Gaussian PDFs g1(x,mean1,sigma) anf g2(x,mean2,sigma) and their parameters
	RooRealVar mean("mean", "mean of gaussians",(range[1]+range[0])*0.5, range[0], range[1] );
	RooRealVar sigma("sigma", "width of gaussians", 0.6,0.,2);
  RooRealVar alpha("alpha","alpha", 0.2, -1.0, 1);
  RooRealVar n("n","n", 5, 0,10);

	RooGaussian gaus("gaus", "Signal", x, mean, sigma);

  RooCBShape crystalBall("CB","CB", x, mean, sigma, alpha, n);

    // Build Chebychev polynomial p.d.f.
	RooRealVar a0("a0", "a0", 0., 0, 10.);
	RooRealVar a1("a1", "a1", 0., 0, 10.);
	// RooRealVar a2("a2", "a2", 0., 0, 10.);
	// RooRealVar a3("a3", "a3", 0., 0., 10.);
	// RooRealVar a4("a4", "a4", 0.2, 0., 1.);
	RooChebychev bkg("bkg", "Background", x, RooArgSet(a0, a1));
  //RooPolynomial bkg("bkg", "Background", x, RooArgSet(a0, a1, a2));

	// Associated nsig/nbkg as expected number of events with sig/bkg _in_the_range_ "signalRange"
	RooRealVar nsig("nsig","number of signal events in signalRange",10000,0.,1E8) ;
	RooRealVar nbkg("nbkg","number of background events in signalRange",1000,0.,1E8) ;

	// Use AddPdf to extend the model. Giving as many coefficients as pdfs switches
	// on extension.
  RooAddPdf  model("model","(gaus+bkg)", RooArgList(bkg,gaus), RooArgList(nbkg,nsig)) ;
  //RooAddPdf  model("model","(CB+bkg)", RooArgList(bkg,crystalBall), RooArgList(nbkg,nsig)) ;


	RooDataHist *data = new RooDataHist("dh", "dh", x, Import(*histo));

	// Fit model to data
	model.fitTo(*data,PrintLevel(-1));



	// Plot data and PDF overlaid
	RooPlot *xframe = x.frame(Title(ID.c_str()));
	data->plotOn(xframe, XErrorSize(0));

	model.plotOn(xframe);


	// Overlay the background+sig2 components of model with a dotted line
	//model.plotOn(xframe, Components(bkg));
	//model.plotOn(xframe, Components(RooArgSet(gaus, gaus_FS)),  LineStyle(kDashed));
	FitCanvas->cd();
	xframe->Draw();
  FitCanvas->SaveAs(("Fit_inspect_tmp/"+ID+".png").c_str());

    delete data, FitCanvas;

    std::vector<double> results;
    results.push_back(mean.getVal());   //centroid
    results.push_back(sigma.getVal());  //sigma

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

void Fitter(){

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

  std::vector<TFile * > FileList;
  std::vector<TH1F * > HistoList;

  for (auto const& x : IDlist) {
    std::string filename = "Spectra_tmp/Histo"+std::to_string(x.first)+".root";
    std::string name = std::to_string(x.first);
    FileList.push_back(new TFile(filename.c_str()));
    HistoList.push_back((TH1F *)FileList.back()->Get(name.c_str()));
    //HistoList.back()->Rebin(2);
  }

  std::vector<double> energies = {511,1462};
  std::vector<std::vector<double>> res;
  gROOT->SetBatch(kTRUE);
  for (auto const& x : IDlist) {
    std::vector<double> centroid;

    for (size_t i = 0; i < energies.size(); i++) {

      double range[2] = {energies[i]-20,energies[i]+20};
      std::vector<double> out = Fit(HistoList[x.second], range, "Fit_E"+std::to_string(energies[i])+"_ID"+std::to_string(x.first));
      centroid.push_back(out[0]);
      cout<<out[0]<<endl;
    }
    res.push_back(centroid);
  }
  gROOT->SetBatch(kFALSE);

  std::ofstream lStream( "output.dat" );

  for (auto const& x : IDlist) {
    TGraph * cal = new TGraph(energies.size());
    for (size_t j = 0; j < energies.size(); j++){
      cal->SetPoint(j,res[x.second][j],energies[j]);
    }
    cal->Fit("pol1");
    TF1 * fit  = cal->GetFunction("pol1");
    double m = fit->GetParameters()[1];
    double q = fit->GetParameters()[0];
    lStream<<x.first<<" "<<q<<" "<<m<<endl;;
    cout<<endl<<endl<<"Crystal ->"<<x.first<<" m->"<<m<<" q->"<<q<<endl<<endl;
    delete cal, fit;
  }
  lStream.close();


  }
