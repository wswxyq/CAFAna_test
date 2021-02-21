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
#include <fstream>
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

void table_spectra_select_fun(int mode_val, int input_FSI, int particle, int p_m ){

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
  

  auto subdir_had = "subdir_hadE_spectra_standard";
  auto subdir_mu = "subdir_muE_spectra_standard";
  auto subdir_numu = "subdir_numuE_spectra_standard";


  //auto * inFile_origin = new TFile("./results/spectra.root");

  //auto * inFile_origin = new TFile(("./results/"+std::to_string(mode_val)+"/spectra.root").c_str());
  auto * inFile_origin = new TFile(("./results/"+std::to_string(mode_val)+"__"+"/spectra.root").c_str());

  auto * inFile_modified_1 = new TFile(("./results/"+std::to_string(mode_val)+"_"+std::to_string(input_FSI)+"_"+pm_list[p_m]+"1/spectra.root").c_str());
  auto * inFile_modified_2 = new TFile(("./results/"+std::to_string(mode_val)+"_"+std::to_string(input_FSI)+"_"+pm_list[p_m]+"2/spectra.root").c_str());
  auto * inFile_modified_3 = new TFile(("./results/"+std::to_string(mode_val)+"_"+std::to_string(input_FSI)+"_"+pm_list[p_m]+"3/spectra.root").c_str());

  // Load the spectrum...
  // assume default particle to be hadron.

  std::unique_ptr<Spectrum> spect_origin = Spectrum::LoadFrom(inFile_origin, "subdir_E_spectra");
  std::unique_ptr<Spectrum> spect_modified_1 = Spectrum::LoadFrom(inFile_modified_1, subdir_numu);
  std::unique_ptr<Spectrum> spect_modified_2 = Spectrum::LoadFrom(inFile_modified_2, subdir_numu);
  std::unique_ptr<Spectrum> spect_modified_3 = Spectrum::LoadFrom(inFile_modified_3, subdir_numu);

  if(particle==2)
  {
    spect_origin = Spectrum::LoadFrom(inFile_origin, "subdir_muE_spectra"); // be careful! the standard EE spectra for muon is not generated yet.
    spect_modified_1 = Spectrum::LoadFrom(inFile_modified_1, subdir_mu);
    spect_modified_2 = Spectrum::LoadFrom(inFile_modified_2, subdir_mu);
    spect_modified_3 = Spectrum::LoadFrom(inFile_modified_3, subdir_mu);
  }else if(particle==1)
  {
    spect_origin = Spectrum::LoadFrom(inFile_origin, "subdir_hadE_spectra"); // be careful! the standard EE spectra for hadron is not generated yet.
    spect_modified_1 = Spectrum::LoadFrom(inFile_modified_1, subdir_had);
    spect_modified_2 = Spectrum::LoadFrom(inFile_modified_2, subdir_had);
    spect_modified_3 = Spectrum::LoadFrom(inFile_modified_3, subdir_had);
  }

  TH1D *TH1D_original = spect_origin->ToTH1(spect_origin->POT());
  TH1D *TH1D_modified_1 = spect_modified_1->ToTH1(spect_modified_1->POT());
  TH1D *TH1D_modified_2 = spect_modified_2->ToTH1(spect_modified_2->POT());
  TH1D *TH1D_modified_3 = spect_modified_3->ToTH1(spect_modified_3->POT());

  //======================================================================

  
  cout << "Original(Green) mean:" << TH1D_original->GetMean()<<endl;
  cout << pm_list[p_m] << "1 sigma mean:" << TH1D_modified_1->GetMean()<<endl;
  cout << pm_list[p_m] << "2 sigma mean:" << TH1D_modified_2->GetMean()<<endl;
  cout << pm_list[p_m] << "3 sigma mean:" << TH1D_modified_3->GetMean()<<endl;



  std::ofstream outfile;

  outfile.open((particle_list[particle]+pm_list[p_m]+".txt").c_str(), std::ios_base::app); // append instead of overwrite

  outfile << 
  (mode_map[mode_val]+"_"+FSI_map[input_FSI]).c_str()<<","
  <<
  TH1D_original->GetMean()<<","
  <<
  TH1D_modified_1->GetMean()<<","
  <<
  TH1D_modified_2->GetMean()<<","
  <<
  TH1D_modified_3->GetMean()<<","
  <<
  (TH1D_modified_1->GetMean()-TH1D_original->GetMean())/TH1D_original->GetMean()<<","
  <<
  (TH1D_modified_2->GetMean()-TH1D_original->GetMean())/TH1D_original->GetMean()<<","
  <<
  (TH1D_modified_3->GetMean()-TH1D_original->GetMean())/TH1D_original->GetMean()
  <<
  endl;
  outfile.close();
  
}


void table_spectra(){

  for (auto const& x : mode_map){
    for (auto const& y : FSI_map){
      
        table_spectra_select_fun(x.first, y.first, 3, 1);
        table_spectra_select_fun(x.first, y.first, 3, -1);
      
    }
  }
}