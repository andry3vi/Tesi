std::vector<double> Fitter(TH1D * histo, string name){

  TSpectrum *back = new TSpectrum();
  TH1D * ExDist_back = (TH1D *) back->Background(histo,26);
  histo->Add(ExDist_back,-1);

  // TSpectrum Peak Search
  TSpectrum *search_peak = new TSpectrum(2);

  int found_peak = search_peak->Search(histo, 4, "", 0.30);

  // Retrieving positions of the peaks

  Double_t peakmax_0    = search_peak->GetPositionX()[0];
  Double_t peakheight_0 = search_peak->GetPositionY()[0];
  Double_t peakmax_1    = search_peak->GetPositionX()[1];
  Double_t peakheight_1 = search_peak->GetPositionY()[1];

  Double_t distance = (peakmax_1-peakmax_0)/2.0;

  //Fitting first Ex peak
  TF1 * peak_0 = new TF1("peak_0","gaus",peakmax_0-0.8*distance,peakmax_0+0.8*distance);

  peak_0->SetParameter(2,6);
  //peak_0->SetParameter(0,peakheight_0);
  peak_0->SetLineColor(2);


  //Fitting second Ex peak
  TF1 * peak_1 = new TF1("peak_1","gaus",peakmax_1-0.8*distance,peakmax_1+0.8*distance);

  peak_1->SetParameter(2,6);
  //peak_1->SetParameter(0,peakheight_1);
  peak_1->SetLineColor(2);

  histo->Fit(peak_0,"QNR");
  histo->Fit(peak_1,"QNR");

  TF1 * lin_back = new TF1("lin_back","pol1",-4,-1);

  histo->Fit(lin_back,"QNR");

  peakheight_0 = peak_0->GetParameter(0);
  double mean_0 = peak_0->GetParameter(1);
  double sigma_0 = peak_0->GetParameter(2);
  peakheight_1 = peak_1->GetParameter(0);
  double mean_1 = peak_1->GetParameter(1);
  double sigma_1 = peak_1->GetParameter(2);

  distance = (mean_1-mean_0)/2.0;
  //TF1 * peaksum = new TF1("peaksum","gaus+gaus(3)+pol1(6)",-2,3);//peakmax_0-1.7*distance,peakmax_1+2*distance);
  TF1 * peaksum = new TF1("peaksum","gaus+gaus(3)",peakmax_0-0.8*distance,peakmax_1+1.8*distance);

  peaksum->SetLineColor(1);
  peaksum->FixParameter(0,peakheight_0);
  peaksum->SetParameter(1,mean_0);
  peaksum->SetParameter(2,sigma_0);
  peaksum->SetParameter(3,peakheight_1);
  peaksum->SetParameter(4,mean_1);
  peaksum->SetParameter(5,sigma_1);
  //peaksum->SetParameter(6,lin_back->GetParameter(0));
  //peaksum->SetParameter(7,lin_back->GetParameter(1));

  histo->Fit(peaksum,"QMNR");

  std::vector<double> RETURN;
  RETURN.reserve(6);

  RETURN.push_back(peaksum->GetParameter(1));        //0
  RETURN.push_back(peaksum->GetParError(1));          //1
  RETURN.push_back(2.355*peaksum->GetParameter(2));    //2
  //RETURN.push_back(peaksum->GetParameter(4));
  RETURN.push_back(mean_1);                           //3
  RETURN.push_back(peaksum->GetParError(4));          //4
  RETURN.push_back(2.355*peaksum->GetParameter(5));    //5

  for (size_t i = 0; i < RETURN.size(); i++) {
    cout<<RETURN[i]<<"-";
  }
  cout<<endl<<endl;
  TCanvas * tmp = new TCanvas("tmp","tmp");

  tmp->cd();
  histo->GetYaxis()->SetRangeUser(0,260);
  histo->Draw();
  peak_0->Draw("SAME");
  peak_1->Draw("SAME");
  peaksum->Draw("SAME");
  //lin_back->Draw("SAME");

  tmp->SaveAs(("Fit_tmp/"+name+".png").c_str());

  delete tmp;
  delete search_peak;
  delete back;
  delete ExDist_back;
  delete peak_0;
  delete peak_1;
  delete peaksum;

  return RETURN;

}

std::vector<double> Scanner(string filename){

	TFile * infile = new TFile(filename.c_str());
	TTree * tree = (TTree *) infile->Get("PhysicsTree");

  TTreeReader fReader;
  fReader.SetTree(tree);

  TTreeReaderArray<double> Ex = {fReader, "Ex"};
  TTreeReaderArray<double> Target_dThickness = {fReader, "Target_dThickness"};
  TTreeReaderValue<Int_t> nbParticleMG = {fReader, "nbParticleMG"};

	std::vector<double> Thick_scan;

  int nbEntries = fReader.GetEntries(true);

	std::vector<TH1D*> Spectra;

  std::vector<double> FoM;
  std::vector<double> Ex_0;
  std::vector<double> Ex_1;
  std::vector<double> Ex_0_sigma;
  std::vector<double> Ex_1_sigma;


	for(size_t i=0; i<nbEntries; i++){

		fReader.SetEntry(i);
		if(i==0){
			Spectra.reserve(Target_dThickness.GetSize());

      for(size_t j=0; j<Target_dThickness.GetSize(); ++j){
				string name = "dThick_"+to_string(Target_dThickness[j]);
				Thick_scan.push_back(Target_dThickness[j]);
				Spectra.push_back(new TH1D(name.c_str(),name.c_str(),800,-4, 4));
      }
		}
		for(size_t j=0; j<Ex.GetSize(); j++){
        Spectra[j]->Fill(Ex[j]);
      }
	}

  FoM.reserve(Thick_scan.size());
  Ex_0.reserve(Thick_scan.size());
  Ex_1.reserve(Thick_scan.size());
  Ex_0_sigma.reserve(Thick_scan.size());
  Ex_1_sigma.reserve(Thick_scan.size());


  gROOT->SetBatch(kTRUE);

	TGraphErrors * FigureofMerit = new TGraphErrors(Spectra.size());
	TGraphErrors * Centroid_0 = new TGraphErrors(Spectra.size());
  TGraphErrors * Centroid_1 = new TGraphErrors(Spectra.size());
  TLine *        Ex_0_nominal = new TLine(1000*Thick_scan[0],0,1000*Thick_scan.back(),0);
  TLine *        Ex_1_nominal = new TLine(1000*Thick_scan[0],0.87073,1000*Thick_scan.back(),0.87073);

  Ex_0_nominal->SetLineWidth(2);
  Ex_0_nominal->SetLineColor(46);
  Ex_0_nominal->SetLineStyle(9);
  Ex_1_nominal->SetLineWidth(2);
  Ex_1_nominal->SetLineColor(46);
  Ex_1_nominal->SetLineStyle(9);



  for (size_t i = 0; i < Spectra.size(); ++i) {
      string name = "dThick_"+to_string(Thick_scan[i]);
      Spectra[i]->Rebin(4);
      std::vector<double> results = Fitter(Spectra[i],name);
      FoM.push_back((results[3]-results[0])/results[2]+results[5]);
      Ex_0.push_back(results[0]);
      Ex_1.push_back(results[3]);
      Ex_0_sigma.push_back(results[1]);
      Ex_1_sigma.push_back(results[4]);

      FigureofMerit->SetPoint(i,Thick_scan[i],FoM.back());
      Centroid_0->SetPoint(i,1000*Thick_scan[i],Ex_0.back());
      Centroid_1->SetPoint(i,1000*Thick_scan[i],Ex_1.back());
      Centroid_0->SetPointError(i,0,Ex_0_sigma.back());
      Centroid_1->SetPointError(i,0,Ex_1_sigma.back());

  }

	for(size_t i=0; i<Spectra.size(); i++){
      cout<<"dThick_shift value -> "<<Thick_scan[i]<<" Ex_0 -> "<<Ex_0[i]<<" Ex_1 -> "<<Ex_1[i]<<"----FoM -> "<<FoM[i]<<endl;
	}

	gROOT->SetBatch(kFALSE);

	TCanvas * C = new TCanvas("C","C");

  C->Divide(1,2);
  C->cd(1);
  Centroid_0->GetXaxis()->SetTitle("Target Thickness correction [#mu\m]");
  Centroid_0->GetYaxis()->SetTitle("Peak position [MeV]");
  Centroid_0->GetYaxis()->SetRangeUser(-0.2,0.2);
  Centroid_0->Draw();
  Ex_0_nominal->Draw("SAME");
	//FigureofMerit->GetXaxis()->SetTitle("#Thick shift []");
	//FigureofMerit->GetYaxis()->SetTitle("FoM");
	//FigureofMerit->Draw();

	C->cd(2);
  Centroid_1->GetXaxis()->SetTitle("Target Thickness correction [#mu\m]");
	Centroid_1->GetYaxis()->SetTitle("Peak position [MeV]");
  Centroid_1->GetYaxis()->SetRangeUser(0.82,0.92);
  Centroid_1->Draw();
  Ex_1_nominal->Draw("SAME");

	std::vector<double> result;
  return result;



  }

void Main(){
	std::vector<string> filename;
  filename.push_back("../Data/optimized/Thickness_opt_final.root");

	std::vector<double> centroids;
	std::vector<double> errors;

	for(size_t i = 0; i<filename.size(); i++){

		std::vector<double> tmp_res = Scanner(filename[i]);

	}

	return;
}
