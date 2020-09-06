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



void draw_spectra_numuE(){

  TFile inFile_0("/nova/ana/users/wus/root_files/FD_FHC_spectra_original_x_0_10.root");
  TFile inFile_5("/nova/ana/users/wus/root_files/FD_FHC_spectra_sys5_x_0_10.root");
  TFile inFile_5_all("/nova/ana/users/wus/root_files/FD_FHC_spectra_sys5_x_0_10_all.root");
  TFile inFile_5_2D("/nova/ana/users/wus/root_files/FD_FHC_spectra_sys5_x_0_10_2D.root");

  // Load the spectrum...
  std::unique_ptr<Spectrum> spect_0 = Spectrum::LoadFrom(inFile_0.GetDirectory("subdir_numuE_spectra"));
  std::unique_ptr<Spectrum> spect_5 = Spectrum::LoadFrom(inFile_5.GetDirectory("subdir_numuE_spectra"));
  std::unique_ptr<Spectrum> spect_5_all = Spectrum::LoadFrom(inFile_5_all.GetDirectory("subdir_numuE_spectra"));
  std::unique_ptr<Spectrum> spect_5_2D = Spectrum::LoadFrom(inFile_5_2D.GetDirectory("subdir_numuE_spectra"));



  //
  // Plot the histo...
  //  
  TCanvas *canvas_0 = new TCanvas("canvas_0","plot muE_spectra",2560,1600);
  canvas_0->cd();

  TH1D *TH1D_0 = spect_0->ToTH1(spect_0->POT());
  TH1D *TH1D_5 = spect_5->ToTH1(spect_5->POT());
  TH1D *TH1D_5_all = spect_5_all->ToTH1(spect_5_all->POT());
  TH1D *TH1D_5_2D = spect_5_2D->ToTH1(spect_5_2D->POT());

  TH1D_0->SetLineWidth(2);
  TH1D_0->SetLineColor(kGreen);
  TH1D_0->SetLineStyle(kSolid);
  TH1D_0->Draw("hist_0");

  TH1D_5->SetLineWidth(2);
  TH1D_5->SetLineColor(kRed);
  TH1D_5->SetLineStyle(kSolid);
  TH1D_5->Draw("SAME");

  TH1D_5_all->SetLineWidth(2);
  TH1D_5_all->SetLineColor(kBlue);
  TH1D_5_all->SetLineStyle(kSolid);
  TH1D_5_all->Draw("SAME");

  TH1D_5_2D->SetLineWidth(2);
  TH1D_5_2D->SetLineColor(kOrange);
  TH1D_5_2D->SetLineStyle(kSolid);
  TH1D_5_2D->Draw("SAME");

  auto legend = new TLegend(0.6, 0.6, 0.8, 0.8);
  legend->SetHeader("Prong-Shifted muon neutrino Energy","C"); // option "C" allows to center the header
  legend->AddEntry(TH1D_0, "Original mean: "+ TString::Format("%f",TH1D_0->GetMean()),"l");
  legend->AddEntry(TH1D_5, "3D -shift mean: "+ TString::Format("%f",TH1D_5->GetMean()),"l");
  legend->AddEntry(TH1D_5_2D, "2D -shift mean: "+ TString::Format("%f",TH1D_5_2D->GetMean()),"l");
  legend->AddEntry(TH1D_5_all, "3D & 2D -shift mean: "+ TString::Format("%f",TH1D_5_all->GetMean()),"l");
  legend->Draw("SAME");



  canvas_0->Print("compare_all_numuE_x.pdf");
  cout << "Original(Green) mean:" << TH1D_0->GetMean()<<endl;
  cout << "5% 3D prong-shift(Red) mean:" << TH1D_5->GetMean()<<endl;
  cout << "5% 2D prong-shift(Orange) mean:" << TH1D_5_2D->GetMean()<<endl;
  cout << "5% 3D & 2D prong-shift(Blue) mean:" << TH1D_5_all->GetMean()<<endl;

}
