// S20-10-30.

#include "CAFAna/Core/Binning.h"
#include "CAFAna/Cuts/Cuts.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Vars/Vars.h"

#include "StandardRecord/Proxy/SRProxy.h"

#include "TCanvas.h"
#include "TH2.h"

#include "TensorFlowEvaluator/LSTME/cafana/LSTMEVar.h"

#include "3FlavorAna/Cuts/NumuCuts.h"
#include "3FlavorAna/Cuts/NumuCuts2018.h"
#include "3FlavorAna/Vars/NumuEFxs.h"
#include "TFile.h"  
#include "TH1.h"   

#include "CAFAna/Core/ISyst.h"

#include <string> 
#include "Utilities/func/EnvExpand.cxx"

#include "CAFAna/Systs/FSISysts.h"  // header for Final State Interaction uncertainties

using namespace std;

using namespace ana;

extern const NOvARwgtSyst khNFSISyst2020_EV1;
extern const NOvARwgtSyst khNFSISyst2020_EV2;
extern const NOvARwgtSyst khNFSISyst2020_EV3;
extern const NOvARwgtSyst khNFSISyst2020_MFP;

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
  
  int input_FSI = -9999;
  double shift_pm = 1.0;  // bool var that determine whether positive or negative shift is used.
                      // by default we choose positive.


void exec_FSI(int mode_val, int FSI_val, double p_m)
{
  // Environment variables and wildcards work. Most commonly you want a SAM
  // dataset. Pass -ss --limit 1 on the cafe command line to make this take a
  // reasonable amount of time for demo purposes.
  string up_down;
  shift_pm = p_m;

  if (p_m > 0 && p_m <= 10 )
  {
    up_down = "up";
  }else if (p_m<0 && p_m >= -10 )
  {
    up_down = "down";
  }else  
  {
    cout << "p_m value out of suggested range. Your value is: " << p_m << endl;
    return;
  }
  
  
	map<int, string> FSI_map={
    {1,  "khNFSISyst2020_EV1"}, {2, "khNFSISyst2020_EV2"}, {3, "khNFSISyst2020_EV3"}, {0, "khNFSISyst2020_MFP"}
    };
  map<int, string> mode_map={
    {0,  "QE"}, {1, "Res"}, {2, "DIS"}, {3, "Coh"}, {10, "MEC"}, {100000, "NOCUT"} };

  input_FSI = FSI_val;
  
  if ( FSI_map.count(input_FSI) > 0 )
    std::cout<<">>>> Found supported FSI uncertainties. Now continue with FSI = "<< input_FSI <<std::endl;
  else{
    std::cout<<"NOT FOUND "<< input_FSI <<" ! QUIT..."<<std::endl;
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

  std::cout<<"Setup Cut."<<std::endl;
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
  
  //std::cout<<"Load TensorFlow Model."<<std::endl;
  //auto model = LSTME::initCAFAnaModel((util::EnvExpansion("${SRT_PRIVATE_CONTEXT}")+"/tf").c_str());
  //auto model = LSTME::initCAFAnaModel("tf");



  // Spectrum to be filled from the loader

  // By default, set FSI_val = 0
  SystShifts shift_2020(&ana::khNFSISyst2020_EV2, p_m);
  if (FSI_val == 1)
  {
    shift_2020 = SystShifts(&ana::khNFSISyst2020_EV1, p_m);
  }else if (FSI_val == 2)
  {
    shift_2020 = SystShifts(&ana::khNFSISyst2020_EV2, p_m);
  }else if (FSI_val == 3)
  {
    shift_2020 = SystShifts(&ana::khNFSISyst2020_EV3, p_m);
  }
  

  Spectrum numuE_spectra_standard("numuE_spectra_standard", bins, loader, kNumuE2020, cut, shift_2020);

  // Do it!
  loader.Go();

  TFile *outFile = new TFile("spectra.root", "RECREATE");

  numuE_spectra_standard.SaveTo(outFile, "subdir_numuE_spectra_standard");

  outFile->Close();
}


