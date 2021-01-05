//S20-10-30

#include "TCanvas.h"
#include "TFile.h"  
#include "TH1.h"    
#include "TString.h"
#include "TLegend.h"
#include "TLine.h"

#include "CAFAna/Core/Spectrum.h"
#include <string>
#include <iostream>

using namespace ana;


map<int, string> FSI_map={
  {1,  "khNFSISyst2020_EV1"}, {2, "khNFSISyst2020_EV2"}, {3, "khNFSISyst2020_EV3"}, {0, "khNFSISyst2020_MFP"}
  };
map<int, string> mode_map={ 
  {100000, "NOCUT"} 
  };

map<int, string> particle_list={ 
  {1, "had"}, {2, "mu"}, {3, "numu"} 
  };

map<int, string> pm_list={ 
  {1, ""}, {-1, "-"} 
  };

void draw_spectra_select_fun(int mode_val, int input_FSI, int particle, int p_m ){

  if ( mode_map.count(mode_val) > 0  )
    std::cout<<"Found supported mode. Now continue... with mode cut="<< mode_val <<std::endl;
  else{
    std::cout<<"NOT FOUND "<< mode_val <<" ! QUIT..."<< std::endl;
    return;
  }

  if ( FSI_map.count(input_FSI) > 0  )
    std::cout<<"Found supported FSI. Now continue... with FSI="<< input_FSI <<std::endl;
  else{
    std::cout<<"NOT FOUND "<<input_FSI<<" ! QUIT..."<<std::endl;
    return;
  }

  if ( particle_list.count(particle) > 0  )
    std::cout<<"Found supported particle. Now continue... with particle="<< particle <<std::endl;
  else{
    std::cout<<"NOT FOUND "<< particle <<" ! QUIT..."<<std::endl;
    return;
  }

  if (p_m != 1 && p_m != -1)
  {
    std::cout<< "p_m should be either 1 or -1. Input p_m=" << p_m << " . Skipping..." <<std::endl;
    return;
  }
  

  auto subdir_had = "subdir_hadE_spectra";
  auto subdir_mu = "subdir_muE_spectra";
  auto subdir_numu = "subdir_numuE_spectra";


  auto * inFile_origin = new TFile(("./results/"+std::to_string(mode_val)+"__/spectra.root").c_str());
  auto * inFile_modified_1 = new TFile(("./results/"+std::to_string(mode_val)+"_"+std::to_string(input_FSI)+"_"+pm_list[p_m]+"1/spectra.root").c_str());
  auto * inFile_modified_2 = new TFile(("./results/"+std::to_string(mode_val)+"_"+std::to_string(input_FSI)+"_"+pm_list[p_m]+"2/spectra.root").c_str());
  auto * inFile_modified_3 = new TFile(("./results/"+std::to_string(mode_val)+"_"+std::to_string(input_FSI)+"_"+pm_list[p_m]+"3/spectra.root").c_str());

  // Load the spectrum...
  // assume default particle to be hadron.
  std::unique_ptr<Spectrum> spect_origin = Spectrum::LoadFrom(inFile_origin, subdir_had);
  std::unique_ptr<Spectrum> spect_modified_1 = Spectrum::LoadFrom(inFile_modified_1, subdir_had);
  std::unique_ptr<Spectrum> spect_modified_2 = Spectrum::LoadFrom(inFile_modified_2, subdir_had);
  std::unique_ptr<Spectrum> spect_modified_3 = Spectrum::LoadFrom(inFile_modified_3, subdir_had);
  if(particle==2)
  {
    spect_origin = Spectrum::LoadFrom(inFile_origin, subdir_mu);
    spect_modified_1 = Spectrum::LoadFrom(inFile_modified_1, subdir_mu);
    spect_modified_2 = Spectrum::LoadFrom(inFile_modified_2, subdir_mu);
    spect_modified_3 = Spectrum::LoadFrom(inFile_modified_3, subdir_mu);
  }else if(particle==3)
  {
    spect_origin = Spectrum::LoadFrom(inFile_origin, subdir_numu);
    spect_modified_1 = Spectrum::LoadFrom(inFile_modified_1, subdir_numu);
    spect_modified_2 = Spectrum::LoadFrom(inFile_modified_2, subdir_numu);
    spect_modified_3 = Spectrum::LoadFrom(inFile_modified_3, subdir_numu);
  }
  

  //
  // Plot the histo...
  //  
  TCanvas *canvas_0 = new TCanvas("canvas_0", ("plot "+particle_list[particle]+"E_spectra").c_str(), 1200, 1200);
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
  TH1D *TH1D_modified_1 = spect_modified_1->ToTH1(spect_modified_1->POT());
  TH1D *TH1D_modified_2 = spect_modified_2->ToTH1(spect_modified_2->POT());
  TH1D *TH1D_modified_3 = spect_modified_3->ToTH1(spect_modified_3->POT());

  TH1D_original->GetXaxis()->SetRangeUser(0, 5);
  TH1D_modified_1->GetXaxis()->SetRangeUser(0, 5);
  TH1D_modified_2->GetXaxis()->SetRangeUser(0, 5);
  TH1D_modified_3->GetXaxis()->SetRangeUser(0, 5);


  TH1D *TH1D_modified_1_factor = spect_modified_1->ToTH1(spect_modified_1->POT());
  TH1D *TH1D_modified_2_factor = spect_modified_2->ToTH1(spect_modified_2->POT());
  TH1D *TH1D_modified_3_factor = spect_modified_3->ToTH1(spect_modified_3->POT());

  TH1D_modified_1_factor->GetXaxis()->SetRangeUser(0, 5);
  TH1D_modified_2_factor->GetXaxis()->SetRangeUser(0, 5);
  TH1D_modified_3_factor->GetXaxis()->SetRangeUser(0, 5);


  TH1D_original->SetLineWidth(2);
  TH1D_original->SetLineColor(kGreen);
  TH1D_original->SetLineStyle(kSolid);
  TH1D_original->Draw("hist_0");
  TH1D_original->GetYaxis()->SetTitle(" ");
  TH1D_original->GetXaxis()->SetTitle(" ");
  TH1D_original->SetTitle(("E_{"+ particle_list[particle] + "} spectrum").c_str());

  TH1D_modified_1->SetLineWidth(2);
  TH1D_modified_1->SetLineColor(kRed);
  TH1D_modified_1->SetLineStyle(kSolid);
  TH1D_modified_1->Draw("SAME");

  TH1D_modified_2->SetLineWidth(2);
  TH1D_modified_2->SetLineColor(kOrange);
  TH1D_modified_2->SetLineStyle(kSolid);
  TH1D_modified_2->Draw("SAME");

  TH1D_modified_3->SetLineWidth(2);
  TH1D_modified_3->SetLineColor(kBlue);
  TH1D_modified_3->SetLineStyle(kSolid);
  TH1D_modified_3->Draw("SAME");


  auto legend = new TLegend(0.5, 0.6, 0.7, 0.8);
  legend->SetHeader((FSI_map[input_FSI] + " uncertainties, "+ mode_map[mode_val]+" mode").c_str(),"C"); // option "C" allows to center the header
  legend->AddEntry(TH1D_original, "Original mean: "+ TString::Format("%f",TH1D_original->GetMean()),"l");
  legend->AddEntry(TH1D_modified_1, pm_list[p_m] + "1 sigma -shift mean: "+ TString::Format("%f",TH1D_modified_1->GetMean()),"l");
  legend->AddEntry(TH1D_modified_2, pm_list[p_m] + "2 sigma -shift mean: "+ TString::Format("%f",TH1D_modified_2->GetMean()),"l");
  legend->AddEntry(TH1D_modified_3, pm_list[p_m] + "3 sigma -shift mean: "+ TString::Format("%f",TH1D_modified_3->GetMean()),"l");
  legend->SetTextSize(0.03);
  legend->Draw("SAME");

  pad2->cd();
  
  //pad2->SetTopMargin(.1);
  //pad2->SetLeftMargin(.1);
  //pad2->SetBottomMargin(.1);
  //pad2->SetRightMargin(.1);
  TH1D_modified_1_factor->Divide(TH1D_original);
  TH1D_modified_1_factor->GetYaxis()->SetRangeUser(0.85, 1.15);

   
  TH1D_modified_2_factor->Divide(TH1D_original);
  TH1D_modified_2_factor->GetYaxis()->SetRangeUser(0.85, 1.15);

  TH1D_modified_3_factor->Divide(TH1D_original);
  TH1D_modified_3_factor->GetYaxis()->SetRangeUser(0.85, 1.15);


  TH1D_modified_1_factor->SetLineWidth(2);
  TH1D_modified_1_factor->SetLineColor(kRed);
  TH1D_modified_1_factor->SetLineStyle(kSolid);
  TH1D_modified_1_factor->GetYaxis()->SetTitle(" ");
  TH1D_modified_2_factor->GetXaxis()->SetTitle(" ");
  TH1D_modified_1_factor->Draw("DD");

  TH1D_modified_2_factor->SetLineWidth(2);
  TH1D_modified_2_factor->SetLineColor(kOrange);
  TH1D_modified_2_factor->SetLineStyle(kSolid);
  TH1D_modified_2_factor->GetYaxis()->SetTitle(" ");
  TH1D_modified_2_factor->GetXaxis()->SetTitle(" ");
  TH1D_modified_2_factor->Draw("SAME");

  TH1D_modified_3_factor->SetLineWidth(2);
  TH1D_modified_3_factor->SetLineColor(kBlue);
  TH1D_modified_3_factor->SetLineStyle(kSolid);
  TH1D_modified_3_factor->GetYaxis()->SetTitle(" ");
  TH1D_modified_3_factor->GetXaxis()->SetTitle(" ");
  TH1D_modified_3_factor->Draw("SAME");

  TLine *hline = new TLine(0,1,5,1);
  hline->SetLineColor(kGreen);
  hline->SetLineWidth(2);
  hline->Draw("SAME");

  canvas_0->Update();

  canvas_0->Print(("./pdf/"+particle_list[particle]+"E_"+mode_map[mode_val]+"_"+FSI_map[input_FSI]+pm_list[p_m]+".pdf").c_str());
  canvas_0->Print(("./png/"+particle_list[particle]+"E_"+mode_map[mode_val]+"_"+FSI_map[input_FSI]+pm_list[p_m]+".png").c_str());

  
  cout << "Original(Green) mean:" << TH1D_original->GetMean()<<endl;
  cout << pm_list[p_m] << "1 sigma mean:" << TH1D_modified_1->GetMean()<<endl;
  cout << pm_list[p_m] << "2 sigma mean:" << TH1D_modified_2->GetMean()<<endl;
  cout << pm_list[p_m] << "3 sigma mean:" << TH1D_modified_3->GetMean()<<endl;

  delete canvas_0;

}


void draw_spectra(){

  for (auto const& x : mode_map){
    for (auto const& y : FSI_map){
      draw_spectra_select_fun(x.first, y.first, 1, 1);
      draw_spectra_select_fun(x.first, y.first, 2, 1);
      draw_spectra_select_fun(x.first, y.first, 3, 1);
      draw_spectra_select_fun(x.first, y.first, 1, -1);
      draw_spectra_select_fun(x.first, y.first, 2, -1);
      draw_spectra_select_fun(x.first, y.first, 3, -1);
    }
  }
}