// Example of how to read spectra saved to disk in and make
// pretty plots...
//
// This is the second half of the demo2p5a script...

#include "TCanvas.h"
#include "TFile.h"  
#include "TH1.h"    
#include "TString.h"
#include "TLegend.h"

#include "CAFAna/Core/Spectrum.h"


using namespace ana;



void draw_spectra_muE(){

  TString subdir = "subdir_muE_spectra";
  TString percentage = "5%";

  TFile inFile_origin("/nova/ana/users/wus/root_files/FD_FHC_spectra_original_x_0_10.root");
  TFile inFile_modified_up("/nova/ana/users/wus/root_files/FD_FHC_spectra_sys5_x_0_10_all_up.root");
  TFile inFile_modified_down("/nova/ana/users/wus/root_files/FD_FHC_spectra_sys5_x_0_10_all_down.root");

  // Load the spectrum...
  std::unique_ptr<Spectrum> spect_origin = Spectrum::LoadFrom(inFile_origin.GetDirectory(subdir));
  std::unique_ptr<Spectrum> spect_modified_up = Spectrum::LoadFrom(inFile_modified_up.GetDirectory(subdir));
  std::unique_ptr<Spectrum> spect_modified_down = Spectrum::LoadFrom(inFile_modified_down.GetDirectory(subdir));



  //
  // Plot the histo...
  //  
  TCanvas *canvas_0 = new TCanvas("canvas_0","plot muE_spectra",1800, 2000);
	c->Divide(1, 2);

  canvas_0->cd(1);

  TH1D *TH1D_original = spect_origin->ToTH1(spect_origin->POT());
  TH1D *TH1D_modified_up = spect_modified_up->ToTH1(spect_modified_up->POT());
  TH1D *TH1D_modified_down = spect_modified_down->ToTH1(spect_modified_down->POT());

  TH1D *TH1D_modified_up_factor = TH1D_modified_up->Clone();
  TH1D *TH1D_modified_down_factor = TH1D_modified_down->Clone();

  TH1D_original->SetLineWidth(1);
  TH1D_original->SetLineColor(kGreen);
  TH1D_original->SetLineStyle(kSolid);
  TH1D_original->Draw("hist_0");

  TH1D_modified_up->SetLineWidth(1);
  TH1D_modified_up->SetLineColor(kRed);
  TH1D_modified_up->SetLineStyle(kSolid);
  TH1D_modified_up->Draw("SAME");

  TH1D_modified_down->SetLineWidth(1);
  TH1D_modified_down->SetLineColor(kOrange);
  TH1D_modified_down->SetLineStyle(kSolid);
  TH1D_modified_down->Draw("SAME");


  auto legend = new TLegend(0.6, 0.6, 0.8, 0.8);
  legend->SetHeader("Prong-Shifted muon Energy","C"); // option "C" allows to center the header
  legend->AddEntry(TH1D_original, "Original mean: "+ TString::Format("%f",TH1D_original->GetMean()),"l");
  legend->AddEntry(TH1D_modified_up, "up -shift mean: "+ TString::Format("%f",TH1D_modified_up->GetMean()),"l");
  legend->AddEntry(TH1D_modified_down, "down -shift mean: "+ TString::Format("%f",TH1D_modified_down->GetMean()),"l");
  legend->Draw("SAME");

  canvas_0->cd(2);
  TH1D_modified_up_factor->Divide(TH1D_original);
  TH1D_modified_down_factor->Divide(TH1D_original);


  TLine *hline = new TLine(gPad->GetUxmin(), 1, gPad->GetUxmax(), 1);
  hLine->SetLineColor(kGreen);
  hline->Draw("hist_1");

  TH1D_modified_up_factor->SetLineWidth(1);
  TH1D_modified_up_factor->SetLineColor(kRed);
  TH1D_modified_up_factor->SetLineStyle(kSolid);
  TH1D_modified_up_factor->Draw("SAME");

  TH1D_modified_down_factor->SetLineWidth(1);
  TH1D_modified_down_factor->SetLineColor(kOrange);
  TH1D_modified_down_factor->SetLineStyle(kSolid);
  TH1D_modified_down_factor->Draw("SAME");



  canvas_0->Print("compare_all_muE_x.pdf");

  
  cout << "Original(Green) mean:" << TH1D_original->GetMean()<<endl;
  cout << percentage + " up prong-shift(Red) mean:" << TH1D_modified_up->GetMean()<<endl;
  cout << percentage + " down prong-shift(Orange) mean:" << TH1D_modified_down->GetMean()<<endl;

}
