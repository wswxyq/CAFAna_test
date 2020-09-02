#include <map>

#include "TRandom3.h"

#include "CAFAna/Systs/EnergySysts2018.h"
#include "StandardRecord/Proxy/SRProxy.h"

namespace ana
{
  const NeutronVisEScaleSyst2018 kNeutronVisEScaleProngsSyst2018(true);
  const NeutronVisEScaleSyst2018 kNeutronVisEScalePrimariesSyst2018(false);

  //----------------------------------------------------------------------
  void NeutronVisEScaleSyst2018::
  Shift(double sigma, caf::SRProxy* sr, double& weight) const
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
}
