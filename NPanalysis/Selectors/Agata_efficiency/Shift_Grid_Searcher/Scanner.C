std::vector<double> Fitter(TH1D * histo, string name){
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

	std::vector<double> RETURN;
	RETURN.resize(4);
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
	TTree * tree = (TTree *) infile->Get("SHIFTopt");

	TTreeReader fReader;
        fReader.SetTree(tree);

	TTreeReaderArray<double> dBeta = {fReader,"dBeta"};
	TTreeReaderArray<double> dShift = {fReader,"dShift"};
	TTreeReaderValue<int> dShiftNb = {fReader,"dShiftNb"};
	TTreeReaderValue<int> dBetaNb = {fReader,"dBetaNb"};
	TTreeReaderArray<double> Edopp = {fReader,"Edopp"};
	
	std::vector<double> BetaScan;
	std::vector<double> ShiftScan;
	
	int ShiftNb;
	int BetaNb;

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
			ShiftNb=*dShiftNb;
			BetaNb=*dBetaNb;
			
			Spectra.reserve(dBeta.GetSize());
			
			for(size_t j=0; j<dBeta.GetSize(); j++){
				string name = "dBeta_"+to_string(dBeta[j])+"dShift_"+to_string(dShift[j]);
				BetaScan.push_back(dBeta[j]);
				ShiftScan.push_back(dShift[j]);
				Spectra.push_back(new TH1D(name.c_str(),name.c_str(),300,200,1300));
			}
		}
		for(size_t j=0; j<dBeta.GetSize(); j++){
			Spectra[j]->Fill(Edopp[j]);
		}
                 
	}
        
	
	double BetaHalfStep =(BetaScan[ShiftNb]-BetaScan[0])/2.0;
	double ShiftHalfStep =(ShiftScan[1]-ShiftScan[0])/2.0;

        TH2D * Res      = new TH2D("Resolution","Resolution",ShiftNb,ShiftScan[0]-ShiftHalfStep,ShiftScan.back()+ShiftHalfStep,BetaNb,BetaScan[0]-BetaHalfStep,BetaScan.back()+BetaHalfStep);
        TH2D * Centroid = new TH2D("Centroid",  "Centroid",  ShiftNb,ShiftScan[0]-ShiftHalfStep,ShiftScan.back()+ShiftHalfStep,BetaNb,BetaScan[0]-BetaHalfStep,BetaScan.back()+BetaHalfStep);	
	
	gROOT->SetBatch(kTRUE);
        
	for(size_t i=0; i<BetaScan.size(); i++){
		string name = "dBeta_"+to_string(dBeta[i])+"dShift_"+to_string(dShift[i]);
		std::vector<double> result = Fitter(Spectra[i],name);
		MEAN.push_back(result[0]);
		MEAN_sig.push_back(result[1]);
		FWHM.push_back(result[2]);
		FWHM_sig.push_back(result[3]);
	        if(MEAN.back() != 0 ){
		       	Res->Fill(ShiftScan[i],BetaScan[i],FWHM.back()/MEAN.back());
		}
		if(MEAN.back() != 0 && FWHM.back()>1){ 
			RES_sig.push_back(FWHM.back()/MEAN.back()*sqrt((FWHM_sig.back()/FWHM.back())*(FWHM_sig.back()/FWHM.back())+(MEAN_sig.back()/MEAN.back())*(MEAN_sig.back()/MEAN.back())));
		}
		else {
			RES_sig.push_back(0);
		}
		Centroid->Fill(ShiftScan[i],BetaScan[i],MEAN.back());
	}

        
        for(size_t i=0; i<BetaScan.size(); i++){
                cout<<"DBeta value -> "<<BetaScan[i]<<" DShift value -> "<<ShiftScan[i]<<" Centroid -> "<<MEAN[i]<<"+/-"<<MEAN_sig[i]<<" FWHM -> "<<FWHM[i]<<"+/-"<<FWHM_sig[i]<<" Resolution -> "<<FWHM[i]/MEAN[i]<<"+/-"<<RES_sig[i]<<endl;
		
	}
        
	gROOT->SetBatch(kFALSE);
        gStyle->SetPalette(53,0);	
	
	TCanvas * C = new TCanvas("C","C");
        C->Divide(1,2);
        C->cd(1);
	Res->GetXaxis()->SetTitle("#Agata shift [mm]");
	Res->GetYaxis()->SetTitle("#beta shift [v/c]");
	Res->Draw("col");
        
 
	C->cd(2);
	

	Centroid->GetXaxis()->SetTitle("#Agata shift [mm]");
	Centroid->GetYaxis()->SetTitle("#beta shift [v/c]");
        Centroid->Draw("colz");	
        
        filename.erase(0,8);
        filename.erase(filename.end()-5,filename.end());
        string cname = "Fit_tmp/"+filename+".png";
        C->SaveAs(cname.c_str());	

	return;
 


  }

