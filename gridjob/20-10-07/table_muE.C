#include "TCanvas.h"
#include "TFile.h"  
#include "TH1.h"    
#include "TString.h"

#include "CAFAna/Core/Spectrum.h"
#include <string>
#include <iostream>
#include <fstream>
using namespace ana;

int input_pdg = -9999;
map<int, string> pdg_latex={
  {111, "#pi^{0}"}, {211, "#pi^{+/-}"}, {2212, "p"}, {2112, "n"},{11, "e"}, {13, "#mu"}, {15, "#tau"},
  {-13, "all but #mu"}  };

map<int, string> pdg_map={
  {111,  "pi0"}, {211, "pi"}, {2212, "p"}, {2112, "n"},{11, "e"}, {13, "mu"}, {15, "tau"}, {-13, "nomuon"}
  };
map<int, string> mode_map={
  {0,  "QE"}, {1, "Res"}, {2, "DIS"}, {3, "Coh"},{10, "MEC"}, {100000, "NOCUT"} };



void table_muE_fun(int mode_val, int pdg_val, string filename){

  std::cout << "Please enter a pdg value(number, negative for antiparticle): ";
  input_pdg = pdg_val;
  
  if ( pdg_map.count(input_pdg) > 0  )
    std::cout<<"Found supported pdg. Now continue... with pdg="<<input_pdg<<std::endl;
  else{
    std::cout<<"NOT FOUND "<<input_pdg<<" ! QUIT..."<<std::endl;
    return;
  }


  
  if ( pdg_map.count(input_pdg) > 0  )
    std::cout<<"Found supported pdg. Now continue... with pdg="<<input_pdg<<std::endl;
  else{
    std::cout<<"NOT FOUND "<<input_pdg<<" ! QUIT..."<<std::endl;
    return;
  }

  TString subdir = "subdir_muE_spectra";
  TString percentage = "5%";

  TFile inFile_origin(("./results/"+std::to_string(mode_val)+"__/spectra.root").c_str());
  TFile inFile_modified_up(("./results/"+std::to_string(mode_val)+"_"+std::to_string(input_pdg)+"_1/spectra.root").c_str());
  TFile inFile_modified_down(("./results/"+std::to_string(mode_val)+"_"+std::to_string(input_pdg)+"_-1/spectra.root").c_str());


  // Load the spectrum...
  std::unique_ptr<Spectrum> spect_origin = Spectrum::LoadFrom(inFile_origin.GetDirectory(subdir));
  std::unique_ptr<Spectrum> spect_modified_up = Spectrum::LoadFrom(inFile_modified_up.GetDirectory(subdir));
  std::unique_ptr<Spectrum> spect_modified_down = Spectrum::LoadFrom(inFile_modified_down.GetDirectory(subdir));


  TH1D *TH1D_original = spect_origin->ToTH1(spect_origin->POT());
  TH1D *TH1D_modified_up = spect_modified_up->ToTH1(spect_modified_up->POT());
  TH1D *TH1D_modified_down = spect_modified_down->ToTH1(spect_modified_down->POT());
  


  std::ofstream outfile;

  outfile.open(filename, std::ios_base::app); // append instead of overwrite

  outfile << 
  (mode_map[mode_val]+"_"+pdg_map[input_pdg]).c_str()<<"\t\t"
  <<
  TH1D_original->GetMean()<<"\t\t"
  <<
  TH1D_modified_up->GetMean()<<"\t\t"
  <<
  TH1D_modified_down->GetMean()<<"\t\t"
  <<
  (TH1D_modified_up->GetMean()-TH1D_original->GetMean())/TH1D_original->GetMean()<<"\t\t"
  <<
  (TH1D_modified_down->GetMean()-TH1D_original->GetMean())/TH1D_original->GetMean()
  <<
  endl;
  outfile.close();
}

void table_muE(){
  if (std::ifstream("muE.txt"))
    {
      std::remove("muE.txt");
    }
  std::ofstream outfile ("muE.txt");
  for (auto const& x : mode_map){
    for (auto const& y : pdg_map){
        table_muE_fun(x.first, y.first, "muE.txt");
      }
    }
}