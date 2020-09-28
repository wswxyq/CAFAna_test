#include "mode_cut.h"
#include "CAFAna/Core/Binning.h"
#include "CAFAna/Cuts/Cuts.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Vars/Vars.h"

#include "StandardRecord/Proxy/SRProxy.h"

#include "TCanvas.h"
#include "TH2.h"

#include "TensorFlowEvaluator/LSTME/cafana/LSTMEVar.h"

#include "CAFAna/Cuts/NumuCuts.h"
#include "CAFAna/Cuts/NumuCuts2018.h"
#include "TFile.h"  
#include "TH1.h"   

#include "CAFAna/Core/ISyst.h"

#include <string> 
using namespace std;

using namespace ana;
  int input_pdg = -9999;

  class Prong_length_Shift : public ISyst
  {
    public:
      Prong_length_Shift()
        : ISyst("Prong_length_Shift", "Prong_length_Shift ##0")
      {}

      // we'll be modifying the SRProxy this time.
      // (that's why it's passed non-const.)
      void Shift(double sigma, caf::SRProxy* sr, double& weight) const override
      {

        auto &png = sr->vtx.elastic.fuzzyk.png;
        for (size_t i = 0; i < png.size(); i++) {
          // png[i].len // this will give you lenght of the prong number i
          if (abs(png[i].truth.pdg)==input_pdg)
          {
            png[i].len *= (1 + sigma * 0.01); 
          }
        
        }
        auto &png2d = sr->vtx.elastic.fuzzyk.png2d;
        for (size_t i = 0; i < png2d.size(); i++) {
          if (abs(png2d[i].truth.pdg)==input_pdg)
          {
            png2d[i].len *= (1 + sigma * 0.01); 
          }
        }
      }
  };


void select_abs_and_mode_up(int pdg_val, int mode_val)
{
  // Environment variables and wildcards work. Most commonly you want a SAM
  // dataset. Pass -ss --limit 1 on the cafe command line to make this take a
  // reasonable amount of time for demo purposes.


	map<int, string> pdg_map={
    {111,  "pi0"}, {211, "pi+"}, {2212, "p"}, {2112, "n"},{11, "e"}, {13, "mu"}, {15, "tau"}
    };

  // QE, RES, DIS, Coh, MEC cut map

  map<int, string> mode_Cut={
    {0,  "QE"}, {1, "RES"}, {2, "DIS"}, {3, "Coh"},{10, "MEC"}
    };


  std::cout << "Please enter a pdg value(number, negative for antiparticle): "<<std::endl;
  std::cout << "{111,  \"pi0\"}, {211, \"pi+\"}, {2212, \"p\"}, 
        {2112, \"n\"},{11, \"e\"}, {13, \"mu\"}, {15, \"tau\"}" << endl;
  input_pdg=pdg_val;
  
  if ( pdg_map.count(input_pdg) > 0  )
    std::cout<<"Found supported pdg. Now continue... with pdg="<<input_pdg<<std::endl;
  else{
    std::cout<<"NOT FOUND "<<input_pdg<<" ! QUIT..."<<std::endl;
    return;
  }


  const std::string fname = "prod_caf_R19-11-18-prod5reco.f_fd_genie_N1810j0211a_nonswap_fhc_nova_v08_period3_v1";

  SpectrumLoader loader(fname);

  const Binning bins = Binning::Simple(100, 0, 10);

  // Specify variables needed and arbitrary code to extract value from
  // SRProxy
  const Var kTrackLen([](const caf::SRProxy* sr)
                      {
                        if(sr->trk.kalman.ntracks == 0) return 0.0f;
                        return float(sr->trk.kalman.tracks[0].len);
                      });

  // Cut
  const Cut kTrueEbelow7GeV = kTrueE < 7.0;

  const Cut SanityCut(
      [] (const caf::SRProxy *sr)
      {
          return (sr->mc.nnu > 0) && (! sr->mc.nu[0].prim.empty());
      }
  );

  const Cut kNumuLoosePID(
      [] (const caf::SRProxy* sr)
      {
          return (
                (sr->sel.remid.pid > 0.5)
              && (sr->sel.cvnloosepreselptp.numuid > 0.5)
          );
      }
  );


  Cut select_mode;
  if (mode_val==0)
  {
    select_mode=mode_Cut_QE
  }
  else if (mode_val==1)
  {
    select_mode=mode_Cut_RES
  }
  else if (mode_val==2)
  {
    select_mode=mode_Cut_DIS
  }
  else if (mode_val==3)
  {
    select_mode=mode_Cut_Coh
  }
  else if (mode_val==10)
  {
    select_mode=mode_Cut_MEC
  }
  else{
    std::cout << "could not find matching mode: mode_val=" << mode_val << endl;
    return;
  }


  const Cut cut    =
        kIsNumuCC
        && (
            kNumuBasicQuality
          && kNumuContainFD2017
          && kNumuLoosePID
        )
        && kTrueEbelow7GeV
        && SanityCut && select_mode;


  auto model = LSTME::initCAFAnaModel("tf");

  Var muE   = LSTME::muonEnergy(model);
  Var hadE  = LSTME::hadEnergy(model);
  Var numuE = LSTME::numuEnergy(model);

  // Spectrum to be filled from the loader
  const Prong_length_Shift wsw_sys;

  SystShifts shift_2020(&wsw_sys, 5.0);

  Spectrum muE_spectra("muE_spectra", bins, loader, muE, cut, shift_2020);
  Spectrum hadE_spectra("hadE_spectra", bins, loader, hadE, cut, shift_2020);
  Spectrum numuE_spectra("numuE_spectra", bins, loader, numuE, cut, shift_2020);

  // Do it!
  loader.Go();

  // How to scale histograms
  //const double pot = 18e20;

  // We have histograms
  //len.ToTH1(pot)->Draw("hist");
  //new TCanvas;
  //len1.ToTH1(pot)->Draw("hist");
  //new TCanvas;
  //len2.ToTH1(pot)->Draw("hist");

  // Now save to disk...

  TFile *outFile = new TFile(("/nova/ana/users/wus/root_files/new_"+ mode_Cut[mode_val] +"/FD_FHC_spectra_sys5_x_0_10_up_abs_"+pdg_map[input_pdg]+".root").c_str(),"RECREATE");

  muE_spectra.SaveTo(outFile->mkdir("subdir_muE_spectra"));
  hadE_spectra.SaveTo(outFile->mkdir("subdir_hadE_spectra"));
  numuE_spectra.SaveTo(outFile->mkdir("subdir_numuE_spectra"));

  outFile->Close();
}
