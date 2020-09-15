// Example of how to read spectra saved to disk in and make
// pretty plots...
//
// This is the second half of the demo2p5a script...

#include "TCanvas.h"
#include "TFile.h"  
#include "TH1.h"    
#include "TString.h"
#include "TLegend.h"
#include "TLine.h"

#include "CAFAna/Core/Spectrum.h"


using namespace ana;

int input_pdg = -9999;


void draw_spectra_muE_select(){


  map<int, string> pdg_map={
    {111,  "pi0"}, {211, "pi+"}, {2212, "p"}, {2112, "n"},{11, "e"}, {13, "mu"}, {15, "tau"}, 
    {-211, "pi-"}, {-2212, "p-"}, {-2112, "anti-neutron"},{-11, "e+"}, {-13, "mu+"}, {-15, "tau+"}
	};

  map<int, string> pdg_latex={
    {111, "#pi^{0}"}, {211, "#pi^{+}"}, {2212, "p"}, {2112, "n"},{11, "e^{-}"}, {13, "#mu^{-}"}, {15, "#tau^{-}"}, 
    {-211, "#pi^{-}"}, {-2212, "p^{-}"}, {-2112, "#bar{n}"},{-11, "e^{+}"}, {-13, "#mu^{+}"}, {-15, "#tau^{+}"}
	};


  std::cout << "Please enter a pdg value(number, negative for antiparticle): ";
  std::cin >> input_pdg;

  
  if ( pdg_map.count(input_pdg) > 0  )
    std::cout<<"Found supported pdg. Now continue... with pdg="<<input_pdg<<std::endl;
  else{
    std::cout<<"NOT FOUND "<<input_pdg<<" ! QUIT..."<<std::endl;
    return;
  }

  TString subdir = "subdir_muE_spectra";
  TString percentage = "5%";

  TFile inFile_origin("/nova/ana/users/wus/root_files/FD_FHC_spectra_original_x_0_10.root");
  TFile inFile_modified_up(("/nova/ana/users/wus/root_files/up/FD_FHC_spectra_sys5_x_0_10_up_"+pdg_map[input_pdg]+".root").c_str());
  TFile inFile_modified_down(("/nova/ana/users/wus/root_files/down/FD_FHC_spectra_sys5_x_0_10_down_"+pdg_map[input_pdg]+".root").c_str());


  // Load the spectrum...
  std::unique_ptr<Spectrum> spect_origin = Spectrum::LoadFrom(inFile_origin.GetDirectory(subdir));
  std::unique_ptr<Spectrum> spect_modified_up = Spectrum::LoadFrom(inFile_modified_up.GetDirectory(subdir));
  std::unique_ptr<Spectrum> spect_modified_down = Spectrum::LoadFrom(inFile_modified_down.GetDirectory(subdir));



  //
  // Plot the histo...
  //  
  TCanvas *canvas_0 = new TCanvas("canvas_0","plot muE_spectra",1200, 1200);
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

  TH1D *TH1D_original = spect_origin->ToTH1(spect_origin->POT());
  TH1D *TH1D_modified_up = spect_modified_up->ToTH1(spect_modified_up->POT());
  TH1D *TH1D_modified_down = spect_modified_down->ToTH1(spect_modified_down->POT());

  TH1D_original->GetXaxis()->SetRangeUser(0, 5);
  TH1D_modified_up->GetXaxis()->SetRangeUser(0, 5);
  TH1D_modified_down->GetXaxis()->SetRangeUser(0, 5);


  TH1D *TH1D_modified_up_factor = spect_modified_up->ToTH1(spect_modified_up->POT());
  TH1D *TH1D_modified_down_factor = spect_modified_down->ToTH1(spect_modified_down->POT());

  TH1D_modified_up_factor->GetXaxis()->SetRangeUser(0, 5);
  TH1D_modified_down_factor->GetXaxis()->SetRangeUser(0, 5);


  TH1D_original->SetLineWidth(2);
  TH1D_original->SetLineColor(kGreen);
  TH1D_original->SetLineStyle(kSolid);
  TH1D_original->Draw("hist_0");
  TH1D_original->SetTitle(pdg_latex[input_pdg].c_str());

  TH1D_modified_up->SetLineWidth(2);
  TH1D_modified_up->SetLineColor(kRed);
  TH1D_modified_up->SetLineStyle(kSolid);
  TH1D_modified_up->Draw("SAME");

  TH1D_modified_down->SetLineWidth(2);
  TH1D_modified_down->SetLineColor(kOrange);
  TH1D_modified_down->SetLineStyle(kSolid);
  TH1D_modified_down->Draw("SAME");


  auto legend = new TLegend(0.5, 0.6, 0.7, 0.8);
  legend->SetHeader(" ","C"); // option "C" allows to center the header
  legend->AddEntry(TH1D_original, "Original mean: "+ TString::Format("%f",TH1D_original->GetMean()),"l");
  legend->AddEntry(TH1D_modified_up, "5% up -shift mean: "+ TString::Format("%f",TH1D_modified_up->GetMean()),"l");
  legend->AddEntry(TH1D_modified_down, "5% down -shift mean: "+ TString::Format("%f",TH1D_modified_down->GetMean()),"l");
  legend->SetTextSize(0.03);
  legend->Draw("SAME");

  pad2->cd();
  
  //pad2->SetTopMargin(.1);
  //pad2->SetLeftMargin(.1);
  //pad2->SetBottomMargin(.1);
  //pad2->SetRightMargin(.1);
  TH1D_modified_up_factor->Divide(TH1D_original);
  TH1D_modified_up_factor->GetYaxis()->SetRangeUser(0.7, 1.3);

   
  TH1D_modified_down_factor->Divide(TH1D_original);
  TH1D_modified_down_factor->GetYaxis()->SetRangeUser(0.7, 1.3);


  TH1D_modified_up_factor->SetLineWidth(2);
  TH1D_modified_up_factor->SetLineColor(kRed);
  TH1D_modified_up_factor->SetLineStyle(kSolid);
  TH1D_modified_up_factor->GetYaxis()->SetTitle(" ");
  TH1D_modified_up_factor->Draw("DD");

  TH1D_modified_down_factor->SetLineWidth(2);
  TH1D_modified_down_factor->SetLineColor(kOrange);
  TH1D_modified_down_factor->SetLineStyle(kSolid);
  TH1D_modified_up_factor->GetYaxis()->SetTitle(" ");
  TH1D_modified_up_factor->GetXaxis()->SetTitle(" ");
  TH1D_modified_down_factor->Draw("SAME");

  TLine *hline = new TLine(0,1,5,1);
  hline->SetLineColor(kGreen);
  hline->SetLineWidth(2);
  hline->Draw("SAME");

  canvas_0->Update();

  // canvas_0->Print("compare_all_muE_x.pdf");
  canvas_0->Print(("./pdf/compare_muE_"+pdg_map[input_pdg]+".pdf").c_str());
  canvas_0->Print(("./pdf/compare_muE_"+pdg_map[input_pdg]+".png").c_str());

  
  cout << "Original(Green) mean:" << TH1D_original->GetMean()<<endl;
  cout << percentage + " up prong-shift(Red) mean:" << TH1D_modified_up->GetMean()<<endl;
  cout << percentage + " down prong-shift(Orange) mean:" << TH1D_modified_down->GetMean()<<endl;

}
