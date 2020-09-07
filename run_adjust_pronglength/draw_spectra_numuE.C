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

  TString subdir = "subdir_numuE_spectra";
  TString percentage = "5%";

  TFile inFile_origin("/nova/ana/users/wus/root_files/FD_FHC_spectra_original_x_0_10.root");
  TFile inFile_modified_3D("/nova/ana/users/wus/root_files/FD_FHC_spectra_sys5_x_0_10_3D.root");
  TFile inFile_modified_all("/nova/ana/users/wus/root_files/FD_FHC_spectra_sys5_x_0_10_all.root");
  TFile inFile_modified_2D("/nova/ana/users/wus/root_files/FD_FHC_spectra_sys5_x_0_10_2D.root");

  // Load the spectrum...
  std::unique_ptr<Spectrum> spect_origin = Spectrum::LoadFrom(inFile_origin.GetDirectory(subdir));
  std::unique_ptr<Spectrum> spect_modified_3D = Spectrum::LoadFrom(inFile_modified_3D.GetDirectory(subdir));
  std::unique_ptr<Spectrum> spect_modified_2D = Spectrum::LoadFrom(inFile_modified_2D.GetDirectory(subdir));
  std::unique_ptr<Spectrum> spect_modified_all = Spectrum::LoadFrom(inFile_modified_all.GetDirectory(subdir));



  //
  // Plot the histo...
  //  
  TCanvas *canvas_0 = new TCanvas("canvas_0","plot numuE_spectra",2560,1600);
  canvas_0->cd();

  TH1D *TH1D_original = spect_origin->ToTH1(spect_origin->POT());
  TH1D *TH1D_modified_3D = spect_modified_3D->ToTH1(spect_modified_3D->POT());
  TH1D *TH1D_modified_2D = spect_modified_2D->ToTH1(spect_modified_2D->POT());
  TH1D *TH1D_modified_all = spect_modified_all->ToTH1(spect_modified_all->POT());

  TH1D_original->SetLineWidth(2);
  TH1D_original->SetLineColor(kGreen);
  TH1D_original->SetLineStyle(kSolid);
  TH1D_original->Draw("hist_0");

  TH1D_modified_3D->SetLineWidth(2);
  TH1D_modified_3D->SetLineColor(kRed);
  TH1D_modified_3D->SetLineStyle(kSolid);
  TH1D_modified_3D->Draw("SAME");

  TH1D_modified_2D->SetLineWidth(2);
  TH1D_modified_2D->SetLineColor(kOrange);
  TH1D_modified_2D->SetLineStyle(kSolid);
  TH1D_modified_2D->Draw("SAME");

  TH1D_modified_all->SetLineWidth(2);
  TH1D_modified_all->SetLineColor(kBlue);
  TH1D_modified_all->SetLineStyle(kSolid);
  TH1D_modified_all->Draw("SAME");

  auto legend = new TLegend(0.6, 0.6, 0.8, 0.8);
  legend->SetHeader("Prong-Shifted muon neutrino Energy","C"); // option "C" allows to center the header
  legend->AddEntry(TH1D_original, "Original mean: "+ TString::Format("%f",TH1D_original->GetMean()),"l");
  legend->AddEntry(TH1D_modified_3D, "3D -shift mean: "+ TString::Format("%f",TH1D_modified_3D->GetMean()),"l");
  legend->AddEntry(TH1D_modified_2D, "2D -shift mean: "+ TString::Format("%f",TH1D_modified_2D->GetMean()),"l");
  legend->AddEntry(TH1D_modified_all, "3D & 2D -shift mean: "+ TString::Format("%f",TH1D_modified_all->GetMean()),"l");
  legend->Draw("SAME");



  canvas_0->Print("compare_all_numuE_x.pdf");

  
  cout << "Original(Green) mean:" << TH1D_original->GetMean()<<endl;
  cout << percentage + " 3D prong-shift(Red) mean:" << TH1D_modified_3D->GetMean()<<endl;
  cout << percentage + " 2D prong-shift(Orange) mean:" << TH1D_modified_2D->GetMean()<<endl;
  cout << percentage + " 3D & 2D prong-shift(Blue) mean:" << TH1D_modified_all->GetMean()<<endl;

}
