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
#include "CAFAna/Systs/EvtRNGSyst.h"

using namespace ana;

class Prong_length_Sys_2020 : publc ISyst
{
  public:
    Prong_length_Sys_2020()
      : ISyst("Prong_length_Sys_2020", "Prong_length_Sys_2020 #1")
    {}
  void Shift(double sigma, caf::SRProxy* sr, double& weight) const override
  {
    if (sr->mc.nnu != 1)
      return;

    // only do anything if at least 1 MeV of neutron-caused visE
    if (sr->mc.nu[0].visEinslcNeutronBirks < 0.001)
      return;

    // todo:  what should sigma apply to?
    // here I assume it's the scale factor, but it could reasonably be
    // any of the three parameters, or some combination of all of them
    double scale = 1 + sigma * (fScale - 1);

    std::vector<caf::SRProngProxy*> prongs;
    if (sr->vtx.elastic.IsValid){
      for (caf::SRFuzzyKProngProxy & png : sr->vtx.elastic.fuzzyk.png)
        prongs.push_back(static_cast<caf::SRProngProxy*>(&png));  // this is a slicing operation, but we don't need the 3D part of it below anyway
      for (caf::SRProngProxy & png : sr->vtx.elastic.fuzzyk.png2d)
        prongs.push_back(&png);
    }
    double addedE = 0;
    if (fUseProngs){
      for (auto & png : prongs){
        // is this right?  do we want to search the whole mother list, or just the immediate mother? hmm.
        // for now, no choice -- motherlist is filled with 0s until ART 2.11 comes in with its fix to ROOT
        //            bool neutronFound = false;
        ////            std::cout << "  prong mother list size = " << png->truth.motherlist.size() << std::endl;
        //            for (const auto & m : png->truth.motherlist)
        //            {
        ////              std::cout << "   ancestor pdg = " << m << std::endl;
        //              if (m == 2112)
        //              {
        //                neutronFound = true;
        //                break;
        //              }
        //            }
        //            if (!neutronFound)
        //              continue;
        if (png->truth.motherpdg != 2112)
          continue;


        // yes: adjust all neutrons below some threshold.
        // skip the ones above it.
        double visE = png->truth.visEinslc; //visEinslcBirks;
        if (visE > fThreshE)
          continue;

        double moreE = (RNG(sr)->Uniform() < fProb) ? visE * (scale - 1) : 0;
        addedE += moreE;

        png->calE += moreE;
        if (png->calE < 0)
          png->calE = 0;
        png->weightedCalE += moreE;
        if (png->weightedCalE < 0)
          png->weightedCalE = 0;
      } // for (png)
    } // if (fUseProngs)
    else{
      std::map<int, double> addedEbyTrkID;
      for (auto & prim : sr->mc.nu[0].prim){
        if (prim.pdg != 2112)
          continue;

        // neutrons won't leave any energy directly (probably) but best be safe
        double visE = prim.visEinslcBirks + prim.daughterVisEinslcBirks;
        // concession to rapidly changing CAF structure
        if (std::isnan(visE))
          visE = prim.visEinslc + prim.daughterVisEinslc;
        if (visE > fThreshE)
          continue;

        double moreE = (RNG(sr)->Uniform() < fProb) ? visE * (scale - 1) : 0;
        addedE += moreE;
        addedEbyTrkID[prim.trkID] += moreE;
      }

      // try to redistribute energy to the prongs.
      // first: find any prongs that are directly associated with the primary neutrons
      // that we tagged above.
      std::vector<caf::SRProngProxy*> otherNeutronProngs; // for the next loop
      for (auto & png : prongs){
        int motherTrk = 0;
        for (const auto & mother : png->truth.motherlist){
          if (addedEbyTrkID.find(mother) != addedEbyTrkID.end()){
            motherTrk = mother;
            break;
          }
        }
        if (!motherTrk){
          // save other neutron prongs for a second pass.
          // note: threshold originally CAME from calE, so better respect it here!
          if (png->truth.motherpdg == 2112 && png->calE <= fThreshE)
            otherNeutronProngs.push_back(png);
          continue;
        }
        // todo: what if there are multiple prongs that descend from this neutron??
        double E = addedEbyTrkID.at(motherTrk);

        png->calE += E;
        if (png->calE < 0)
          png->calE = 0;
        png->weightedCalE += E;
        if (png->weightedCalE < 0)
          png->weightedCalE = 0;

        addedEbyTrkID.erase(motherTrk);
      }

      // now distribute any energy left to the other neutron-descendants...
      // since we don't know which ones they "should" be,
      // do something dumb and assign them "randomly"
      for (auto & png : otherNeutronProngs){
        if (addedEbyTrkID.empty())
          break;

        png->calE += addedEbyTrkID.begin()->second;
        if (png->calE < 0)
          png->calE = 0;
        png->weightedCalE += addedEbyTrkID.begin()->second;
        if (png->weightedCalE < 0)
          png->weightedCalE = 0;

        addedEbyTrkID.erase(addedEbyTrkID.begin());
//            std::cout << "  added energy vector size is now: " << addedEbyTrkID.size() << std::endl;
      }

    }

    // add the neutron energy back to the energy estimator inputs.
    //   * for numu, we posit that the reco muon is never affected.  so we only adjust Ehad.
    //   * for nue, we've already modified neutron-descended prongs above,
    //     but most of those won't be judged "electromagnetic" by CVN,
    //     so we care about the "hadronic" part, which is just slice calE - (sum of "EM" prong calEs).
    //     Thus modifying slice calE is good enough.
    sr->energy.numu.hadcalE += addedE;   // this goes into the numu energy estimator function
    sr->slc.calE += addedE;              // this guy is used by the nue energy estimator function
  }

};


void prong_length_adjust()
{
  // Environment variables and wildcards work. Most commonly you want a SAM
  // dataset. Pass -ss --limit 1 on the cafe command line to make this take a
  // reasonable amount of time for demo purposes.
  const std::string fname = "prod_caf_R19-11-18-prod5reco.f_fd_genie_N1810j0211a_nonswap_fhc_nova_v08_period3_v1";

  SpectrumLoader loader(fname);

  const Binning bins = Binning::Simple(100, 0, 20);

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
  SystShifts shift_2020(Prong_length_Sys_2020, 1.0);

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
  TFile *outFile = new TFile("/nova/ana/users/wus/root_files/FD_FHC_spectra_x.root","RECREATE");

  muE_spectra.SaveTo(outFile->mkdir("subdir_muE_spectra"));
  hadE_spectra.SaveTo(outFile->mkdir("subdir_hadE_spectra"));
  numuE_spectra.SaveTo(outFile->mkdir("subdir_numuE_spectra"));

  outFile->Close();
}
