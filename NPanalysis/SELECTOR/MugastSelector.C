#define MugastSelector_cxx
// The class definition in MugastSelector.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.


// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// root> T->Process("MugastSelector.C")
// root> T->Process("MugastSelector.C","some options")
// root> T->Process("MugastSelector.C+")
//


#include "MugastSelector.h"
#include <TH2.h>
#include <TStyle.h>

void MugastSelector::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
}

void MugastSelector::SlaveBegin(TTree * /*tree*/)
{
	// The SlaveBegin() function is called after the Begin() function.
	// When running with PROOF SlaveBegin() is called on each slave server.
	// The tree argument is deprecated (on PROOF 0 is passed).

	TString option = GetOption();

	pp_file = TFile::Open("CUT/pp_cut.root");
	dd_file = TFile::Open("CUT/dd_cut.root");
	tt_file = TFile::Open("CUT/tt_cut.root");

	cuts.push_back((TCutG*)pp_file->Get("pp"));
	cuts.push_back((TCutG*)dd_file->Get("dd"));
	cuts.push_back((TCutG*)tt_file->Get("tt"));
	cuts.push_back(NULL);
        
	beta.push_back(0.1069);
	beta.push_back(0.1100);
	beta.push_back(0.1000);
	
	reaction.push_back(new NPL::Reaction());
	reaction.push_back(new NPL::Reaction());
	reaction.push_back(new NPL::Reaction());

        reaction[0]->ReadConfigurationFile("../O16dp.reaction");   
        reaction[1]->ReadConfigurationFile("../O16dd.reaction");   
        reaction[2]->ReadConfigurationFile("../O16dt.reaction");   

	mTOFElab = new TH2D("mTOFElab", "TOF vs E_{LAB}", 600, 0, 6, 1000, 0, 9000);
	mTOFElab->GetXaxis()->SetTitle("E_{LAB} [MeV]");
	mTOFElab->GetYaxis()->SetTitle("TOF [a.u.]");
	fOutput->Add(mTOFElab);

        AGA_XYZ =new TH3D("AGA_XYZ","AGATA HITS",100,-300,300,100,-300,300,100,-350,-100);
	MU_XYZ  =new TH3D("MU_XYZ","MUGAST HITS", 100, -150,150,100,-150,150,100,-150,0);

        AGA_XYZ->GetXaxis()->SetTitle("X [cm]");
        AGA_XYZ->GetYaxis()->SetTitle("Y [cm]");
        AGA_XYZ->GetZaxis()->SetTitle("Z [cm]");
	
        MU_XYZ->GetXaxis()->SetTitle("X [cm]");
        MU_XYZ->GetYaxis()->SetTitle("Y [cm]");
        MU_XYZ->GetZaxis()->SetTitle("Z [cm]");
	
	for(UInt_t ii=0;ii<reaction.size();ii++){
		
		Ex.push_back(0);
		hEx.push_back( new TH1D(Form("hEx_%i",ii), "Excitation Energy",600, 0, 6));
		hEx[ii]->GetXaxis()->SetTitle("E_{X} [MeV]");
		hEx[ii]->GetYaxis()->SetTitle("Counts");
		fOutput->Add(hEx[ii]);

		mExgamma.push_back( new TH2D(Form("mExgamma_%i",ii), "E_{X} vs E_{#gamma}",600, 0, 6, 2000, 0, 2000));
		mExgamma[ii]->GetXaxis()->SetTitle("E_{X} [MeV]");
		mExgamma[ii]->GetYaxis()->SetTitle("E_{#gamma} [keV]");
		fOutput->Add(mExgamma[ii]);
	}

	for(UInt_t ii=0;ii<cuts.size();ii++){

		hgamma.push_back( new TH1D(Form("hgamma_%i",ii), "Non Doppler Corrected Spectrum",2000, 0, 2000));
		hgamma[ii]->GetXaxis()->SetTitle("E_{#gamma} Not DP [keV]");
		hgamma[ii]->GetYaxis()->SetTitle("Counts");
		fOutput->Add(hgamma[ii]);

		hgammaDC.push_back( new TH1D(Form("hgammaDC_%i",ii), "Doppler Corrected Spectrum",2000, 0, 2000));
		hgammaDC[ii]->GetXaxis()->SetTitle("E_{#gamma} DP [keV]");
		hgammaDC[ii]->GetYaxis()->SetTitle("Counts");
		fOutput->Add(hgammaDC[ii]);

		mgammagamma.push_back( new TH2D(Form("mgammagamma_%i",ii), "Non Doppler Corrected Matrix",2000, 0, 2000, 2000, 0, 2000));
		mgammagamma[ii]->GetXaxis()->SetTitle("E_{#gamma} not DP [keV]");
		mgammagamma[ii]->GetYaxis()->SetTitle("E_{#gamma} not DP [keV]");
		fOutput->Add(mgammagamma[ii]);

		mgammagammaDC.push_back( new TH2D(Form("mgammagammaDC_%i",ii), "Doppler Corrected Matrix",2000, 0, 2000, 2000, 0, 2000));
		mgammagammaDC[ii]->GetXaxis()->SetTitle("E_{#gamma} DP [keV]");
		mgammagammaDC[ii]->GetYaxis()->SetTitle("E_{#gamma} DP [keV]");
		fOutput->Add(mgammagammaDC[ii]);

		mElabAlab.push_back( new TH2D(Form("mElabAlab_%i",ii), "E_{LAB} vs #theta_{LAB}", 180, 0, 180, 600, 0, 6));
		mElabAlab[ii]->GetXaxis()->SetTitle("#theta_{LAB} [deg]");
		mElabAlab[ii]->GetYaxis()->SetTitle("E_{LAB} [MeV]");
		fOutput->Add(mElabAlab[ii]);

	}
}  

Bool_t MugastSelector::Process(Long64_t entry)
{
	// The Process() function is called for each entry in the tree (or possibly
	// keyed object in the case of PROOF) to be processed. The entry argument
	// specifies which entry in the currently loaded tree is to be processed.
	// When processing keyed objects with PROOF, the object is already loaded
	// and is available via the fObject pointer.
	//
	// This function should contain the \"body\" of the analysis. It can contain
	// simple or elaborate selection criteria, run algorithms on the data
	// of the event and typically fill histograms.
	//
	// The processing can be stopped by calling Abort().
	//
	// Use fStatus to set the return value of TTree::Process().
	//
	// The return value is currently not used.

	fReader.SetEntry(entry);
	for(UInt_t ii=0;ii<reaction.size();ii++){
		Ex[ii] = reaction[ii]->ReconstructRelativistic(*ELab,*ThetaLab);
		hEx[ii]->Fill(Ex[ii]);
		for(UInt_t kk=0;kk<*nbTrack;kk++){
			mExgamma[ii]->Fill(Ex[ii],CorrectDoppler(trackE[kk], beta[ii], trackX1[kk], trackY1[kk], trackZ1[kk]));
		}
	}	

	if((Mugast->DSSD_T).size()==1){
	       	mTOFElab->Fill(*ELab,(Mugast->DSSD_T)[0]); 
		MU_XYZ->Fill(*X,*Y,*Z);
	}
	
		for(UInt_t ii=0; ii<*nbTrack;ii++){
			AGA_XYZ->Fill(trackX1[ii], trackY1[ii], trackZ1[ii]);
		}
	
	for(UInt_t kk=0; kk<cuts.size();kk++){
		if((((Mugast->DSSD_T).size()==1) && (cuts[kk]==NULL || cuts[kk]->IsInside(*ELab,(Mugast->DSSD_T)[0])))||kk==3){
			mElabAlab[kk]->Fill( *ThetaLab, *ELab);
			for (Int_t ii=0; ii<*nbTrack;ii++){
				hgamma[kk]->Fill(trackE[ii]);
				hgammaDC[kk]->Fill(CorrectDoppler(trackE[ii], beta[kk], trackX1[ii], trackY1[ii], trackZ1[ii]));
				for (Int_t jj=0; jj<*nbTrack;jj++){
					if (ii==jj) continue;
					mgammagamma[kk]->Fill(trackE[ii],trackE[jj]);
					mgammagamma[kk]->Fill(trackE[jj],trackE[ii]);
					mgammagammaDC[kk]->Fill(CorrectDoppler(trackE[ii], beta[kk], trackX1[ii], trackY1[ii], trackZ1[ii]), CorrectDoppler(trackE[jj], beta[kk], trackX1[jj], trackY1[jj], trackZ1[jj]));
					mgammagammaDC[kk]->Fill(CorrectDoppler(trackE[jj], beta[kk], trackX1[jj], trackY1[jj], trackZ1[jj]), CorrectDoppler(trackE[ii], beta[kk], trackX1[ii], trackY1[ii], trackZ1[ii]));
				}
			}
		}
	}
	return kTRUE;
}

Double_t MugastSelector::CorrectDoppler(Double_t En, Double_t beta, Double_t X, Double_t Y, Double_t Z ){
	return En*(1.-beta*Z/(TMath::Sqrt(X*X+Y*Y+Z*Z)));
}

void MugastSelector::SlaveTerminate()
{
	// The SlaveTerminate() function is called after all entries or objects
	// have been processed. When running with PROOF SlaveTerminate() is called
	// on each slave server.
	//
	//
	TFile *file=new TFile("canvas.root", "recreate");
	Cut = new TCanvas("Cut","TOFvsELab",1500,900);
	Cut->cd();
	mTOFElab->Draw("col");

	for(UInt_t kk=0; kk<cuts.size();kk++){
		Cut->cd();
		if(cuts[kk]!=NULL ) cuts[kk]->Draw("same");

		c.push_back(new TCanvas(Form("c_%i",kk),"Kinematics and spectra",1500,900));

		c[kk]->Divide(3,2);
		if(kk<reaction.size()){
			c[kk]->cd(1);
			hEx[kk]->Draw();

			c[kk]->cd(2);
			mExgamma[kk]->Draw();
		}
		else{
			c[kk]->cd(1);
			AGA_XYZ->Draw();

			c[kk]->cd(2);
			MU_XYZ->Draw();

		}
		c[kk]->cd(3);
		mElabAlab[kk]->Draw("colz");
		if( kk<reaction.size()) reaction[kk]->GetKinematicLine3()->Draw("c");

		c[kk]->cd(4);
		hgamma[kk]->Draw();

		c[kk]->cd(5);
		hgammaDC[kk]->Draw();

		c[kk]->cd(6);
		mgammagammaDC[kk]->Draw("colz");

		Cut->Update();
		c[kk]->Update();
		c[kk]->Write();
	}
	Cut->Write();
	file->Write();
	file->Close();

}

void MugastSelector::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

}
