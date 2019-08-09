double * Fitter(TH1D * histo, string name){
	
	TF1 * background = new TF1("back","expo",200,600);
	background->SetLineColor(3);
	histo->Fit(background,"QLNR");
        
	double p0 = background->GetParameter(0);
	double p1 = background->GetParameter(1);

        TF1 * peak = new TF1("peak","gaus",830,940);
	peak->SetLineColor(2);
	histo->Fit(peak,"QLNR");

	double mean = peak->GetParameter(1);
	double sigma = peak->GetParameter(2);

        TF1 * peaksub = new TF1("peaksub","gaus+expo(3)",400,1200);
	peaksub->SetLineColor(1);
        
	peaksub->SetParameter(1,mean);
	peaksub->SetParameter(2,sigma);
	peaksub->SetParameter(3,p0);
	peaksub->SetParameter(4,p1);

	histo->Fit(peaksub,"QLNR");

	double RETURN [2];
	RETURN[1] = 2.354*peaksub->GetParameter(2);
	RETURN[0] = peaksub->GetParameter(1);
	
	TCanvas * tmp = new TCanvas("tmp","tmp");
	tmp->cd();
        histo->GetXaxis()->SetRangeUser(800,1000);	
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
	std::vector<double> MEAN;



        int nbEntries = fReader.GetEntries(true);
        
	std::vector<TH1D*> Spectra;


	for(size_t i=0; i<nbEntries; i++){
		
		fReader.SetEntry(i);
		if(i==0){
			Spectra.reserve(dBeta.GetSize());
			for(size_t j=0; j<dBeta.GetSize(); j++){
				string name = "dBeta_"+to_string(dBeta[j]);
				BetaScan.push_back(dBeta[j]);
				Spectra.push_back(new TH1D(name.c_str(),name.c_str(),250,200,1300));
			}
		}
		for(size_t j=0; j<dBeta.GetSize(); j++){
			Spectra[j]->Fill(Edopp[j]);
		}
                 
	}
        TH1D * Res = new TH1D("Resolution","Resolution",BetaScan.size(),BetaScan[0],BetaScan.back());
        TH1D * Centroid = new TH1D("Centroid","Centroid",BetaScan.size(),BetaScan[0],BetaScan.back());
	
	gROOT->SetBatch(kTRUE);
        
	for(size_t i=0; i<BetaScan.size(); i++){
		string name = "dBeta_"+to_string(BetaScan[i]);
		double * result = Fitter(Spectra[i],name);
		MEAN.push_back(result[0]);
		FWHM.push_back(result[1]);
	        if(MEAN.back() != 0) Res->SetBinContent(i+1,FWHM.back()/MEAN.back());
	        Centroid->SetBinContent(i+1,MEAN.back());
	}

        
        for(size_t i=0; i<BetaScan.size(); i++){
                cout<<"DBeta value -> "<<BetaScan[i]<<" Centroid -> "<<MEAN[i]<<" FWHM -> "<<FWHM[i]<<" Resolution -> "<<FWHM[i]/MEAN[i]<<endl;
	}
        
	gROOT->SetBatch(kFALSE);
	
	TCanvas * C = new TCanvas("C","C");
        C->Divide(1,2);
        C->cd(1);
	Res->Draw();
        C->cd(2);
        Centroid->Draw();	
	return;


  }

