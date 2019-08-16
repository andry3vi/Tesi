double * Fitter(TH1D * histo, string name){
        // TSpectrum Peak Search
        TSpectrum *search_peak = new TSpectrum(1);

        int found_peak = search_peak->Search(histo, 2, "", 0.05);

        // Retrieving X positions of the peak

        Double_t peakmax    = search_peak->GetPositionX()[0];
        Double_t peakheight = search_peak->GetPositionY()[0];
	
	TF1 * background = new TF1("back","expo",200,600);
	background->SetLineColor(3);
	histo->Fit(background,"QLNR");
        
	double p0 = background->GetParameter(0);
	double p1 = background->GetParameter(1);

        
	TF1 * peak = new TF1("peak","gaus",peakmax-25,peakmax+25);
	
	peak->SetParameter(2,6);
	peak->SetParameter(0,peakheight);
	
	peak->SetLineColor(2);
	
	histo->Fit(peak,"QNR");

	double mean = peak->GetParameter(1);
	double sigma = peak->GetParameter(2);

        TF1 * peaksub = new TF1("peaksub","gaus+expo(3)",400,1200);
	peaksub->SetLineColor(1);
        
	peaksub->SetParameter(1,mean);
	peaksub->SetParameter(2,sigma);
	peaksub->SetParameter(3,p0);
	peaksub->SetParameter(4,p1);

	histo->Fit(peaksub,"QLNR");

	double RETURN [4];
	RETURN[0] = peaksub->GetParameter(1);
	RETURN[1] = peaksub->GetParError(1);
	RETURN[2] = 2.354*peaksub->GetParameter(2);
	RETURN[3] = 2.354*peaksub->GetParError(2);
	
	TCanvas * tmp = new TCanvas("tmp","tmp");
	tmp->cd();
        histo->GetXaxis()->SetRangeUser(600,1200);	
        histo->GetYaxis()->SetRangeUser(0,40);	
	histo->Draw();
        background->Draw("SAME");
        peak->Draw("SAME");
        peaksub->Draw("SAME");

	tmp->SaveAs(("Fit_tmp/"+name+".png").c_str());

	delete tmp;
	delete background;
	delete peak;
	delete peaksub;

        return RETURN;

}

void Scanner(string filename){
  
	TFile * infile = new TFile(filename.c_str());
	TTree * tree = (TTree *) infile->Get("BETAopt");

	TTreeReader fReader;
        fReader.SetTree(tree);

	TTreeReaderArray<double> dBeta = {fReader,"dBeta"};
	TTreeReaderArray<double> Edopp = {fReader,"Edopp"};
	
	std::vector<double> BetaScan;
	std::vector<double> FWHM;
	std::vector<double> FWHM_sig;
	std::vector<double> MEAN;
	std::vector<double> MEAN_sig;
	std::vector<double> RES_sig;



        int nbEntries = fReader.GetEntries(true);
        
	std::vector<TH1D*> Spectra;


	for(size_t i=0; i<nbEntries; i++){
		
		fReader.SetEntry(i);
		if(i==0){
			Spectra.reserve(dBeta.GetSize());
			for(size_t j=0; j<dBeta.GetSize(); j++){
				string name = "dBeta_"+to_string(dBeta[j]);
				BetaScan.push_back(dBeta[j]);
				Spectra.push_back(new TH1D(name.c_str(),name.c_str(),300,200,1300));
			}
		}
		for(size_t j=0; j<dBeta.GetSize(); j++){
			Spectra[j]->Fill(Edopp[j]);
		}
                 
	}
        
	TGraphErrors * Res = new TGraphErrors(Spectra.size());
	TGraphErrors * Centroid = new TGraphErrors(Spectra.size());
	
	gROOT->SetBatch(kTRUE);
        
	for(size_t i=0; i<BetaScan.size(); i++){
		string name = "dBeta_"+to_string(BetaScan[i]);
		double * result = Fitter(Spectra[i],name);
		MEAN.push_back(result[0]);
		MEAN_sig.push_back(result[1]);
		FWHM.push_back(result[2]);
		FWHM_sig.push_back(result[3]);
	        if(MEAN.back() != 0 ) Res->SetPoint(i,BetaScan[i],FWHM.back()/MEAN.back());
		if(MEAN.back() != 0 && FWHM.back()>1){ 
			RES_sig.push_back(FWHM.back()/MEAN.back()*sqrt((FWHM_sig.back()/FWHM.back())*(FWHM_sig.back()/FWHM.back())+(MEAN_sig.back()/MEAN.back())*(MEAN_sig.back()/MEAN.back())));
	                Res->SetPointError(i,0,RES_sig.back());
		}
		else {
			RES_sig.push_back(0);
		}
		Centroid->SetPoint(i,BetaScan[i],MEAN.back());
	        Centroid->SetPointError(i,0,MEAN_sig.back());
	}

        
        for(size_t i=0; i<BetaScan.size(); i++){
                cout<<"DBeta value -> "<<BetaScan[i]<<" Centroid -> "<<MEAN[i]<<"+/-"<<MEAN_sig[i]<<"----FWHM -> "<<FWHM[i]<<"+/-"<<FWHM_sig[i]<<"----Resolution -> "<<FWHM[i]/MEAN[i]<<"+/-"<<RES_sig[i]<<endl;
	}
        
	gROOT->SetBatch(kFALSE);
	
	TCanvas * C = new TCanvas("C","C");
        C->Divide(1,2);
        C->cd(1);
	Res->GetXaxis()->SetTitle("#beta shift [v/c]");
	Res->GetYaxis()->SetTitle("Resolution");
	Res->Draw();

	C->cd(2);
	Centroid->GetXaxis()->SetTitle("#beta shift [v/c]");
	Centroid->GetYaxis()->SetTitle("Peak position [keV]");
        Centroid->Draw();	
	return;


  }

