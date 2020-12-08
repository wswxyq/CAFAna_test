// S20-10-30.

#include "TCanvas.h"
#include "TFile.h"  
#include "TH1.h"    
#include "TString.h"
#include "TLegend.h"
#include "TLine.h"
#include<iostream>
#include "CAFAna/Core/Spectrum.h"
#include <string>

using namespace ana;

void GEANT_MCNP_numuE(){


  const std::string subdir = "subdir_numuE_spectra";

  //TFile inFile_GEANT("../20-10-07/results/100000__/spectra.root");
  //TFile inFile_MCNP("MCNP_LSTM/results/100000/spectra.root");

  TFile * inFile_GEANT = new TFile("../20-10-07/results/100000__/spectra.root", "read");
  TFile * inFile_MCNP = new TFile("MCNP_LSTM/results/100000/spectra.root", "read");


  // Load the spectrum...
  std::unique_ptr<Spectrum> spect_GEANT = Spectrum::LoadFrom(inFile_GEANT, subdir);
  std::unique_ptr<Spectrum> spect_MCNP = Spectrum::LoadFrom(inFile_MCNP, subdir);


  //
  // Plot the histo...
  //  
  TCanvas *canvas_0 = new TCanvas("canvas_0","plot numuE_spectra",1200, 1200);
	//canvas_0->Divide(1, 2, 0, 0);
  TPad *pad1 = new TPad("pad1", " ",0.1,0.35,0.9,0.92);
  TPad *pad2 = new TPad("pad2", " ",0.1,0.1,0.9,0.35);

  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  //pad1->SetTopMargin(.1);
  //pad1->SetBottomMargin(.1);
  //pad1->SetLeftMargin(.1);
  //pad1->SetRightMargin(.1);

  TH1D *TH1D_GEANT = spect_GEANT->ToTH1(spect_GEANT->POT());
  TH1D *TH1D_MCNP = spect_MCNP->ToTH1(spect_MCNP->POT());
  TH1D_MCNP->Scale(TH1D_GEANT->Integral()/TH1D_MCNP->Integral());

  TH1D_GEANT->GetXaxis()->SetRangeUser(0, 5);
  TH1D_MCNP->GetXaxis()->SetRangeUser(0, 5);

  //TH1D *TH1D_difference_factor = spect_MCNP->ToTH1(spect_MCNP->POT());
  TH1D* TH1D_difference_factor = static_cast<TH1D*>(TH1D_MCNP->Clone());
  TH1D_difference_factor->GetXaxis()->SetRangeUser(0, 5);
  TH1D_difference_factor->Scale(TH1D_GEANT->Integral()/TH1D_MCNP->Integral());


  TH1D_GEANT->SetLineWidth(2);
  TH1D_GEANT->SetLineColor(kGreen);
  TH1D_GEANT->SetLineStyle(kSolid);
  TH1D_GEANT->Draw("hist_0");
  TH1D_GEANT->SetTitle("E_{#upsilon_{#mu}} spectrum");

  TH1D_MCNP->SetLineWidth(2);
  TH1D_MCNP->SetLineColor(kRed);
  TH1D_MCNP->SetLineStyle(kSolid);
  TH1D_MCNP->Draw("SAME");



  auto legend = new TLegend(0.5, 0.6, 0.7, 0.8);
  legend->SetHeader("GEANT vs MCNP","C"); // option "C" allows to center the header
  legend->AddEntry(TH1D_GEANT, "GEANT","l");
  legend->AddEntry(TH1D_MCNP, "MCNP","l");
  legend->SetTextSize(0.03);
  legend->Draw("SAME");

  pad2->cd();
  
  //pad2->SetTopMargin(.1);
  //pad2->SetLeftMargin(.1);
  //pad2->SetBottomMargin(.1);
  //pad2->SetRightMargin(.1);
  TH1D_difference_factor->Divide(TH1D_GEANT);
  TH1D_difference_factor->GetYaxis()->SetRangeUser(0.85, 1.15);


  TH1D_difference_factor->SetLineWidth(2);
  TH1D_difference_factor->SetLineColor(kRed);
  TH1D_difference_factor->SetLineStyle(kSolid);
  TH1D_difference_factor->GetYaxis()->SetTitle(" ");
  TH1D_difference_factor->Draw("DD");


  TLine *hline = new TLine(0,1,5,1);
  hline->SetLineColor(kGreen);
  hline->SetLineWidth(2);
  hline->Draw("SAME");

  canvas_0->Update();

  canvas_0->Print("./pdf/GEANT_MCNP.pdf");
  //canvas_0->Print("./png/numuE_.png");
  cout << "Entries of GEANT:" << spect_GEANT->POT()<<endl;
  cout << "Entries of MCNP:" << spect_MCNP->POT()<<endl;
  cout << "INt of GEANT:" << TH1D_GEANT->Integral()<<endl;
  cout << "INt of MCNP:" << TH1D_MCNP->Integral()<<endl;

  
}

