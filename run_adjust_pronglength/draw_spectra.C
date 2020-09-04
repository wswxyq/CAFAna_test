// Example of how to read spectra saved to disk in and make
// pretty plots...
//
// This is the second half of the demo2p5a script...

#include "TCanvas.h"
#include "TFile.h"  
#include "TH1.h"    

#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Systs/EnergySysts2018.h"



using namespace ana;



void draw_spectra(){

  TFile inFile_0("/nova/ana/users/wus/root_files/FD_FHC_spectra.root");
  TFile inFile_5("/nova/ana/users/wus/root_files/FD_FHC_spectra_x.root");

  // Load the spectrum...
  std::unique_ptr<Spectrum> spect_0 = Spectrum::LoadFrom(inFile_0.GetDirectory("subdir_muE_spectra"));
  std::unique_ptr<Spectrum> spect_5 = Spectrum::LoadFrom(inFile_5.GetDirectory("subdir_muE_spectra"));



  //
  // Plot the histo...
  //  
  TCanvas *canvas_0 = new TCanvas("canvas_0","plot muE_spectra",1000,800);
  canvas_0->cd();

  TH1D *TH1D_0 = spect_0->ToTH1(spect_0->POT());
  TH1D *TH1D_5 = spect_5->ToTH1(spect_5->POT());

  TH1D_0->SetLineWidth(2);
  TH1D_0->SetLineColor(kGreen+2);
  TH1D_0->Draw("hist_0");

  TH1D_5->SetLineWidth(2);
  TH1D_5->SetLineColor(kRed+2);
  TH1D_5->Draw("hist_5");

}
