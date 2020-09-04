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



using namespace ana;


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
        // if no truth info, we can't do anything
        //if (sr->mc.nnu != 1)
        //  return;

        //unsigned int nNeutron = 0;
        //for (const auto & particle : sr->mc.nu[0].prim)
        //{
        //  if (particle.pdg != 2112)
        //    continue;

          // only increase if they were less than 50 MeV to begin with
        //  if (particle.visEinslcBirks + particle.daughterVisEinslcBirks >= 0.050)
        //    continue;

        //  nNeutron++;
        //}

        // now adjust the numu energy.
        // we're going to add 25 MeV of visible energy for each neutron
        // for the +1 sigma shift (and subtract it for -1 sigma, etc.)
        //double addedE = sigma * 0.025 * nNeutron;
        //if (sr->energy.numu.hadcalE + addedE < 0)
        //  addedE = -sr->energy.numu.hadcalE;  // don't let the shift make a negative energy deposit

        //sr->energy.numu.hadcalE += addedE;   // this goes into the numu energy estimator function


        auto &png = sr->vtx.elastic.fuzzyk.png;
        for (size_t i = 0; i < png.size(); i++) {
          // png[i].len // this will give you lenght of the prong number i
          png[i].len *= (1 + sigma * 0.01); 
        }
      }
  };


void prong_length_adjust()
{
  // Environment variables and wildcards work. Most commonly you want a SAM
  // dataset. Pass -ss --limit 1 on the cafe command line to make this take a
  // reasonable amount of time for demo purposes.
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

  const Cut cut    =
        kIsNumuCC
        && (
            kNumuBasicQuality
          && kNumuContainFD2017
          && kNumuLoosePID
        )
        && kTrueEbelow7GeV
        && SanityCut;


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
  TFile *outFile = new TFile("/nova/ana/users/wus/root_files/FD_FHC_spectra_sys5_x_0_10.root","RECREATE");

  muE_spectra.SaveTo(outFile->mkdir("subdir_muE_spectra"));
  hadE_spectra.SaveTo(outFile->mkdir("subdir_hadE_spectra"));
  numuE_spectra.SaveTo(outFile->mkdir("subdir_numuE_spectra"));

  outFile->Close();
}
