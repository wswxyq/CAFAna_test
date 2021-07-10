// This script fills spectra using G4Rwgt weights.
// To make plots run example_plot.C over the output of this script.
//
// Author: Cathal Sweeney - csweeney@fnal.gov

// This script requires CAFAna release after 1/15/2021

#include "CAFAna/Core/Binning.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Core/Var.h"
#include "CAFAna/Cuts/Cuts.h"


#include "TFile.h"

#include "NDAna/numucc_1Pi/NumuCC1PiVars.h"
#include "NDAna/numucc_1Pi/NumuCC1PiCuts.h"

#include "G4Rwgt/plotting_scripts/Geant4WeightVars.h"

#include "3FlavorAna/Cuts/NumuCuts.h"
#include "3FlavorAna/Cuts/NumuCuts2018.h"
#include "3FlavorAna/Vars/NumuEFxs.h"

#include "TensorFlowEvaluator/LSTME/cafana/LSTMEVar.h"
#include "Utilities/func/EnvExpand.cxx"

using namespace ana;

void LSTM_macro() {
    SpectrumLoader* loader = new SpectrumLoader("prod_caf_R20-11-25-prod5.1reco.a_nd_genie_N1810j0211a_nonswap_fhc_nova_v08_full_v1");
    std::string outName =  "spectra.root";

    int nUniv = 36;

    std::vector<std::string> myLabel_vec{
        "muE",
        "hadE",
        "numuE"
    };
    std::vector<std::string> myTitle_vec{
        "Piplus 100 universes",
        "Piplus 100 universes",
        "Piplus 100 universes"
    };
    std::vector<std::string> myName_vec  = {
        "piplus_muE",
        "piplus_hadE",
        "piplus_numuE"
    };
    std::vector<Binning> myBins_vec{
        Binning::Simple(100, 0, 10),
        Binning::Simple(100, 0, 10),
        Binning::Simple(100, 0, 10)
    };


    std::cout<<"Load TensorFlow Model."<<std::endl;
    auto model = LSTME::initCAFAnaModel((util::EnvExpansion("${SRT_PRIVATE_CONTEXT}")+"/tf").c_str());
    //auto model = LSTME::initCAFAnaModel("tf");

    std::vector<const Var *> kMyVar_vec{
        new Var(LSTME::primaryEnergy(model)),
        new Var(LSTME::secondaryEnergy(model)),
        new Var(LSTME::totalEnergy(model))
    };

    //=======================================
    // cut

    const Var kTrackLen([](const caf::SRProxy* sr) {
        if(sr->trk.kalman.ntracks == 0) return 0.0f;
        return float(sr->trk.kalman.tracks[0].len);
    });

    // Cut
    const Cut kTrueEbelow7GeV = kTrueE < 7.0;

    const Cut SanityCut(
    [] (const caf::SRProxy *sr) {
        return (sr->mc.nnu > 0) && (! sr->mc.nu[0].prim.empty());
    }
    );

    const Cut kNumuLoosePID(
    [] (const caf::SRProxy* sr) {
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

    //=======================================


    std::vector<Spectrum *> mySpec_vec; // Holds unweighted (nominal) spectra
    std::vector<Spectrum *> myUniv_vec; // Holds weighted spectra

    for(uint i=0; i<myName_vec.size(); ++i) {

        std::string myLabel = myLabel_vec[i];
        Binning myBins = myBins_vec[i];

        const Var * kMyVar = kMyVar_vec[i];

        const HistAxis * axis =  new HistAxis(myLabel,
                                              myBins,
                                              *kMyVar);


        Spectrum * sSig = new Spectrum(myLabel, myBins,
                                       *loader, *kMyVar,
                                       cut_0);

        mySpec_vec.push_back(sSig);

        for(int iUniv=0; iUniv<nUniv; iUniv++) {
            Spectrum * sUniv = new Spectrum(myLabel, myBins,
                                            *loader, *kMyVar,
                                            cut_0, kNoShift,
                                            kGeantPiPlusSingleUnivWeight(iUniv));
            myUniv_vec.push_back(sUniv);
        }

    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    loader->Go();

    TFile * outFile = new TFile( outName.c_str(), "recreate");

    for(uint i=0; i< myName_vec.size(); ++i) {

        TDirectory* base_dir = outFile->mkdir(myName_vec[i].c_str());

        mySpec_vec[i]->SaveTo( base_dir, "nom" );

        for(int iUniv=0; iUniv<nUniv; iUniv++) {

            std::string outDir = "univ_" + std::to_string(iUniv);

            myUniv_vec[(i*nUniv) + iUniv ]->SaveTo(base_dir, outDir.c_str());

        }//end for(iUniv)

    }//end for(i)
    outFile->Close();



}

