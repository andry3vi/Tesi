// Mainframe macro generated from application: /usr/local/include/root/bin/root.exe
// By ROOT version 6.16/00 on 2019-04-02 13:55:40

#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TGuiBldNameFrame
#include "TGuiBldNameFrame.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TRootContextMenu
#include "TRootContextMenu.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGMsgBox
#include "TGMsgBox.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGListTree
#include "TGListTree.h"
#endif
#ifndef ROOT_TGuiBldGeometryFrame
#include "TGuiBldGeometryFrame.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TRootEmbeddedCanvas
#include "TRootEmbeddedCanvas.h"
#endif
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif

#include "TFile.h"
#include "Riostream.h"

void gui()
{
	gROOT->SetBatch(kTRUE);
	TFile* file_in = TFile::Open("$NPTOOL/Outputs/Analysis/merged_test.root");	
	TTree *tree = (TTree *)file_in->Get("PhysicsTree");
	tree->Process("MugastSelector.C");
	file_in->Close();

	gROOT->SetBatch(kFALSE);
	TFile *file_spec = TFile::Open("canvas.root");
	std::vector<TCanvas*> plots;
	TCanvas* Cut = (TCanvas*)file_spec->Get("Cut");

	for (UInt_t ii=0;ii<4;ii++) plots.push_back((TCanvas*)file_spec->Get(Form("c_%i",ii)));
		

	int Height=900;
	int Width=900;
	int step=10;
   // main frame
   TGMainFrame *fMainFrame841 = new TGMainFrame(gClient->GetRoot(),Width,Height,kMainFrame | kVerticalFrame);
   fMainFrame841->SetName("Reaction channels");
   fMainFrame841->SetLayoutBroken(kTRUE);

   fMainFrame841->SetSize(*(new TGDimension(Width,Height)));
   fMainFrame841->SetMaxHeight(Height);
   fMainFrame841->SetMinHeight(Height);
   fMainFrame841->SetMaxWidth(Width);
   fMainFrame841->SetMinWidth(Width);

   // tab widget
   TGTab *fTab565 = new TGTab(fMainFrame841,Width,Height);
   fTab565->MoveResize(step, step+20+step,Width-2*step,Height-2*step-20);

   //fTab565->SetSize(*(new TGDimension(1400,900)));

   // container of "Tab1"
   TGCompositeFrame *fCompositeFrame568;
   fCompositeFrame568 = fTab565->AddTab("16O(d,p)");
   fCompositeFrame568->SetLayoutManager(new TGVerticalLayout(fCompositeFrame568));

   // embedded canvas
   TRootEmbeddedCanvas *fRootEmbeddedCanvas597 = new TRootEmbeddedCanvas(0,fCompositeFrame568,Width-2*step,Height-3*step-20,kSunkenFrame);
   Int_t wfRootEmbeddedCanvas597 = fRootEmbeddedCanvas597->GetCanvasWindowId();
   TCanvas *c123 = new TCanvas("c123", Width-2*step,Height-3*step-20, wfRootEmbeddedCanvas597);
   fRootEmbeddedCanvas597->AdoptCanvas(plots[0]);
   fCompositeFrame568->AddFrame(fRootEmbeddedCanvas597, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
 //fRootEmbeddedCanvas597->cd();

   // container of "Tab2"
   TGCompositeFrame *fCompositeFrame570;
   fCompositeFrame570 = fTab565->AddTab("16O(d,d)");
   fCompositeFrame570->SetLayoutManager(new TGVerticalLayout(fCompositeFrame570));

   // embedded canvas
   TRootEmbeddedCanvas *fRootEmbeddedCanvas581 = new TRootEmbeddedCanvas(0,fCompositeFrame570,Width-2*step,Height-3*step-20,kSunkenFrame);
   Int_t wfRootEmbeddedCanvas581 = fRootEmbeddedCanvas581->GetCanvasWindowId();
   TCanvas *c124 = new TCanvas("c124", Width-2*step,Height-3*step-20, wfRootEmbeddedCanvas581);
   fRootEmbeddedCanvas581->AdoptCanvas(plots[1]);
   fCompositeFrame570->AddFrame(fRootEmbeddedCanvas581, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));


   // container of "Tab3"
   TGCompositeFrame *fCompositeFrame571;
   fCompositeFrame571 = fTab565->AddTab("16O(d,t)");
   fCompositeFrame571->SetLayoutManager(new TGVerticalLayout(fCompositeFrame571));

   // embedded canvas
   TRootEmbeddedCanvas *fRootEmbeddedCanvas582 = new TRootEmbeddedCanvas(0,fCompositeFrame571,Width-2*step,Height-3*step-20,kSunkenFrame);
   Int_t wfRootEmbeddedCanvas582 = fRootEmbeddedCanvas582->GetCanvasWindowId();
   TCanvas *c125 = new TCanvas("c125", Width-2*step,Height-3*step-20, wfRootEmbeddedCanvas582);
   fRootEmbeddedCanvas582->AdoptCanvas(plots[2]);
   fCompositeFrame571->AddFrame(fRootEmbeddedCanvas582, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

   // container of "Tab4"
   TGCompositeFrame *fCompositeFrame572;
   fCompositeFrame572 = fTab565->AddTab("No Particle Gate");
   fCompositeFrame572->SetLayoutManager(new TGVerticalLayout(fCompositeFrame572));

   // embedded canvas
   TRootEmbeddedCanvas *fRootEmbeddedCanvas583 = new TRootEmbeddedCanvas(0,fCompositeFrame572,Width-2*step,Height-3*step-20,kSunkenFrame);
   Int_t wfRootEmbeddedCanvas583 = fRootEmbeddedCanvas583->GetCanvasWindowId();
   TCanvas *c126 = new TCanvas("c126", Width-2*step,Height-3*step-20, wfRootEmbeddedCanvas583);
   fRootEmbeddedCanvas583->AdoptCanvas(plots[3]);
   fCompositeFrame572->AddFrame(fRootEmbeddedCanvas583, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

   // container of "Tab5"
   TGCompositeFrame *fCompositeFrame573;
   fCompositeFrame573 = fTab565->AddTab("Gates");
   fCompositeFrame573->SetLayoutManager(new TGVerticalLayout(fCompositeFrame573));

   // embedded canvas
   TRootEmbeddedCanvas *fRootEmbeddedCanvas584 = new TRootEmbeddedCanvas(0,fCompositeFrame573,Width-2*step,Height-3*step-20,kSunkenFrame);
   Int_t wfRootEmbeddedCanvas584 = fRootEmbeddedCanvas584->GetCanvasWindowId();
   TCanvas *c127 = new TCanvas("c127", Width-2*step,Height-3*step-20, wfRootEmbeddedCanvas584);
   fRootEmbeddedCanvas584->AdoptCanvas(Cut);
   fCompositeFrame573->AddFrame(fRootEmbeddedCanvas584, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

   fTab565->SetTab(0);

   fTab565->Resize(fTab565->GetDefaultSize());
   fMainFrame841->AddFrame(fTab565, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   //fTab565->MoveResize(24,128,360,224);

   ULong_t ucolor;        // will reflect user color changes
   gClient->GetColorByName("#ffffff",ucolor);


   fMainFrame841->SetMWMHints(kMWMDecorAll,
                        kMWMFuncAll,
                        kMWMInputModeless);
   fMainFrame841->MapSubwindows();

   //fMainFrame841->Resize(fMainFrame841->GetDefaultSize());
   fMainFrame841->MapWindow();
   //fMainFrame841->Resize(490,372);
}  
