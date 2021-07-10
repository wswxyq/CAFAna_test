// This script fills spectra using G4Rwgt weights.
// To make plots run example_plot.C over the output of this script.
//
// Author: Cathal Sweeney - csweeney@fnal.gov

#include "CAFAna/Core/Binning.h"
#include "CAFAna/Core/Spectrum.h"
#include "CAFAna/Core/SpectrumLoader.h"
#include "CAFAna/Core/Var.h"

#include "TFile.h"

#include "NDAna/numucc_1Pi/NumuCC1PiVars.h"
#include "NDAna/numucc_1Pi/NumuCC1PiCuts.h"

#include "G4Rwgt/plotting_scripts/Geant4WeightVars.h"

using namespace ana;

void example_macro() {
    SpectrumLoader* loader = new SpectrumLoader("prod_caf_R20-11-25-prod5.1reco.a_nd_genie_N1810j0211a_nonswap_fhc_nova_v08_full_v1");

    std::string outName =  "outFile.root";

    int nUniv = 36;

    std::vector<std::string> myLabel_vec{
        "calE",
        "nuE",
        "piTrkLen"
    };
    std::vector<std::string> myTitle_vec{
        "Piplus 100 universes",
        "Piplus 100 universes",
        "Piplus 100 universes"
    };
    std::vector<std::string> myName_vec  = {
        "piplus_calE",
        "piplus_nuE",
        "piplus_piTrkLen"
    };
    std::vector<Binning> myBins_vec{
        Binning::Simple(50, 0., 5.),
        Binning::Simple(50, 0., 5.),
        Binning::Simple(50, 0., 600.)
    };
    std::vector<const Var *> kMyVar_vec{
        new Var(kCalE),
        new Var(kNuE),
        new Var(kLongestPionLen)
    };


    const Cut * kSignal = new Cut( CutFromNuTruthCut(kIsNumuCC1Pi_NT && kTrueDetector_NT)
                                   && kSimpleProngContainment_tight );

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
                                       *kSignal);

        mySpec_vec.push_back(sSig);

        for(int iUniv=0; iUniv<nUniv; iUniv++) {
            Spectrum * sUniv = new Spectrum(myLabel, myBins,
                                            *loader, *kMyVar,
                                            *kSignal, kNoShift,
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



}

