// Make a simple spectrum plot

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
#include "Utilities/func/EnvExpand.cxx"

using namespace std;

using namespace ana;

  // cut

  const Cut mode_Cut_QE(
    [] (const caf::SRProxy* sr)
    {
        return (sr->mc.nu[0].mode == 0);
    }
  );

  const Cut mode_Cut_Res(
    [] (const caf::SRProxy* sr)
    {
        return (sr->mc.nu[0].mode == 1);
    }
  );

  const Cut mode_Cut_DIS(
    [] (const caf::SRProxy* sr)
    {
        return (sr->mc.nu[0].mode == 2);
    }
  );

  const Cut mode_Cut_Coh(
    [] (const caf::SRProxy* sr)
    {
        return (sr->mc.nu[0].mode == 3);
    }
  );

  const Cut mode_Cut_MEC(
    [] (const caf::SRProxy* sr)
    {
        return (sr->mc.nu[0].mode == 10);
    }
  );
  // global vars used.
  
  int input_pdg = -9999;
  double shift_pm = 1.0;  // bool var that determine whether positive or negative shift is used.
                      // by default we choose positive.


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
        double shift_ratio = (1 + shift_pm * sigma * 0.01);
        auto &png = sr->vtx.elastic.fuzzyk.png;
        for (size_t i = 0; i < png.size(); i++) {
          // png[i].len // this will give you lenght of the prong number i
          if (abs(png[i].truth.pdg)==input_pdg)
          {
            png[i].len *= shift_ratio; // 0.01 = 1%
            png[i].bpf.muon.energy *= shift_ratio;
            png[i].bpf.pion.energy *= shift_ratio;
            png[i].bpf.proton.energy *= shift_ratio;
            png[i].bpf.muon.momentum.x *= shift_ratio;
            png[i].bpf.muon.momentum.y *= shift_ratio;
            png[i].bpf.muon.momentum.z *= shift_ratio;
            png[i].bpf.pion.momentum.x *= shift_ratio;
            png[i].bpf.pion.momentum.y *= shift_ratio;
            png[i].bpf.pion.momentum.z *= shift_ratio;
            png[i].bpf.proton.momentum.x *= shift_ratio;
            png[i].bpf.proton.momentum.y *= shift_ratio;
            png[i].bpf.proton.momentum.z *= shift_ratio;
          }
        }
        auto &png2d = sr->vtx.elastic.fuzzyk.png2d;
        for (size_t i = 0; i < png2d.size(); i++) {
          if (abs(png2d[i].truth.pdg)==input_pdg)
          {
            png2d[i].len *= shift_ratio; 
          }
        }
      }
  };


  class Prong_length_Shift_exclude : public ISyst
  {
    public:
      Prong_length_Shift_exclude()
        : ISyst("Prong_length_Shift_exclude", "Prong_length_Shift_exclude ##0")
      {}

      // we'll be modifying the SRProxy this time.
      // (that's why it's passed non-const.)
      void Shift(double sigma, caf::SRProxy* sr, double& weight) const override
      {
        double shift_ratio = (1 + shift_pm * sigma * 0.01);
        auto &png = sr->vtx.elastic.fuzzyk.png;
        for (size_t i = 0; i < png.size(); i++) {
          // png[i].len // this will give you lenght of the prong number i
          if (abs(png[i].truth.pdg)!=abs(input_pdg))
          {
            png[i].len *= shift_ratio; // 0.01 = 1%
            png[i].bpf.muon.energy *= shift_ratio;
            png[i].bpf.pion.energy *= shift_ratio;
            png[i].bpf.proton.energy *= shift_ratio;
            png[i].bpf.muon.momentum.x *= shift_ratio;
            png[i].bpf.muon.momentum.y *= shift_ratio;
            png[i].bpf.muon.momentum.z *= shift_ratio;
            png[i].bpf.pion.momentum.x *= shift_ratio;
            png[i].bpf.pion.momentum.y *= shift_ratio;
            png[i].bpf.pion.momentum.z *= shift_ratio;
            png[i].bpf.proton.momentum.x *= shift_ratio;
            png[i].bpf.proton.momentum.y *= shift_ratio;
            png[i].bpf.proton.momentum.z *= shift_ratio;
          }
        }
        auto &png2d = sr->vtx.elastic.fuzzyk.png2d;
        for (size_t i = 0; i < png2d.size(); i++) {
          if (abs(png2d[i].truth.pdg)!=abs(input_pdg))
          {
            png2d[i].len *= shift_ratio; 
          }
        }
      }
  };



void exec_mode(int mode_val, int pdg_val, int p_m)
{
  // Environment variables and wildcards work. Most commonly you want a SAM
  // dataset. Pass -ss --limit 1 on the cafe command line to make this take a
  // reasonable amount of time for demo purposes.
  string up_down;

  if (p_m == 1)
  {
    shift_pm = p_m;
    up_down = "up";
  }else if (p_m==-1)
  {
    shift_pm = p_m;
    up_down = "down";
  }else  
  {
    cout << "Please choose valid value for p_m. Your value is" << p_m << endl;
    return;
  }
  
  // positive pdg value means modifying the selected particle prong length
  // negative pdg value means modifying the prong length of all particles except the selected particle
  
	map<int, string> pdg_map={
    {111,  "pi0"}, {211, "pi"}, {2212, "p"}, {2112, "n"},{11, "e"}, {13, "mu"}, {15, "tau"}, {-13, "nomuon"}
    };
  map<int, string> mode_map={
    {0,  "QE"}, {1, "Res"}, {2, "DIS"}, {3, "Coh"}, {10, "MEC"}, {100000, "NOCUT"} };

  std::cout << "Please enter a pdg value(number, negative for exclusion): ";
  input_pdg = pdg_val;
  
  if ( pdg_map.count(input_pdg) > 0  )
    std::cout<<">>>>Found supported pdg. Now continue with pdg="<<input_pdg<<std::endl;
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


  const Cut cut_0    =
        kIsNumuCC
        && (
            kNumuBasicQuality
          && kNumuContainFD2017
          && kNumuLoosePID
        )
        && kTrueEbelow7GeV
        && SanityCut;

  Cut cut=cut_0;
  if (mode_val==0)
  {
    cut=cut_0 && mode_Cut_QE;
  }else if (mode_val==1)
  {
    cut=cut_0 && mode_Cut_Res;
  }else if (mode_val==2)
  {
    cut=cut_0 && mode_Cut_DIS;
  }else if (mode_val==3)
  {
    cut=cut_0 && mode_Cut_Coh;
  }else if (mode_val==10)
  {
    cut=cut_0 && mode_Cut_MEC;
  }else if (mode_val==100000)
  {
    cut=cut_0;
  }else
  {
    return;
  }
  
  
  auto model = LSTME::initCAFAnaModel((util::EnvExpansion("${SRT_PRIVATE_CONTEXT}")+"/tf").c_str());

  Var muE   = LSTME::muonEnergy(model);
  Var hadE  = LSTME::hadEnergy(model);
  Var numuE = LSTME::numuEnergy(model);

  // Spectrum to be filled from the loader

  
  Prong_length_Shift wsw_sys;
  Prong_length_Shift_exclude wsw_sys_ex;

  SystShifts shift_2020(&wsw_sys, 5.0);
  if (pdg_val<0)
  {
    shift_2020 = SystShifts(&wsw_sys_ex, 5.0)
  }
  

  Spectrum muE_spectra("muE_spectra", bins, loader, muE, cut, shift_2020);
  Spectrum hadE_spectra("hadE_spectra", bins, loader, hadE, cut, shift_2020);
  Spectrum numuE_spectra("numuE_spectra", bins, loader, numuE, cut, shift_2020);

  // Do it!
  loader.Go();

  TFile *outFile = new TFile("spectra.root", "RECREATE");

  muE_spectra.SaveTo(outFile->mkdir("subdir_muE_spectra"));
  hadE_spectra.SaveTo(outFile->mkdir("subdir_hadE_spectra"));
  numuE_spectra.SaveTo(outFile->mkdir("subdir_numuE_spectra"));

  outFile->Close();
}


