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

  TFile inFile("/nova/ana/users/wus/root_files/FD_FHC_spectra.root");

  // Load the spectrum...
  std::unique_ptr<Spectrum> nhit = Spectrum::LoadFrom(inFile.GetDirectory("subdir_muE_spectra"));



  //
  // Plot the histo...
  //  
  TCanvas *cNHit = new TCanvas("cNHit","plot muE_spectra",1000,800);
  cNHit->cd();
  TH1D *hNHit = nhit->ToTH1(nhit->POT());
  hNHit->SetLineWidth(2);
  hNHit->SetLineColor(kGreen+2);
  hNHit->Draw("hist");

}
