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

#include "CAFAna/Systs/ReinteractionSysts.h" // header for rwgt uncertainties

using namespace std;

using namespace ana;

// global vars used.

void exec_rwgt(int rwgt_val, int percentage)
{
    // Environment variables and wildcards work. Most commonly you want a SAM
    // dataset. Pass -ss --limit 1 on the cafe command line to make this take a
    // reasonable amount of time for demo purposes.

    map<int, string> rwgt_map = {
        {0, "kPiplusTotSyst"}, {1, "kPiplusCexSyst"}, {2, "kPiplusDcexSyst"}, {3, "kPiplusQeSyst"}, {4, "kPiplusAbsSyst"}, {5, "kPiplusProdSyst"}};

    if (rwgt_map.count(rwgt_val) > 0)
        std::cout << ">>>> Found supported rwgt uncertainties. Now continue with rwgt = " << rwgt_val << std::endl;
    else
    {
        std::cout << "NOT FOUND " << rwgt_val << " ! QUIT..." << std::endl;
        return;
    }

    const std::string fname = "prod_caf_R20-11-25-prod5.1reco.a_nd_genie_N1810j0211a_nonswap_fhc_nova_v08_full_v1";

    SpectrumLoader loader(fname);

    const Binning bins = Binning::Simple(100, 0, 10);

    // Specify variables needed and arbitrary code to extract value from
    // SRProxy
    const Var kTrackLen([](const caf::SRProxy *sr)
                        {
        if(sr->trk.kalman.ntracks == 0) return 0.0f;
        return float(sr->trk.kalman.tracks[0].len); });

    // Cut
    const Cut kTrueEbelow7GeV = kTrueE < 7.0;

    const Cut SanityCut(
        [](const caf::SRProxy *sr)
        {
            return (sr->mc.nnu > 0) && (!sr->mc.nu[0].prim.empty());
        });

    const Cut kNumuLoosePID(
        [](const caf::SRProxy *sr)
        {
            return (
                (sr->sel.remid.pid > 0.5) && (sr->sel.cvnloosepreselptp.numuid > 0.5));
        });

    const Cut cut_0 =
        kIsNumuCC && (kNumuBasicQuality && kNumuContainFD2017 && kNumuLoosePID) && kTrueEbelow7GeV && SanityCut;

    std::cout << "Setup Cut." << std::endl;

    std::cout << "Load TensorFlow Model." << std::endl;
    auto model = LSTME::initCAFAnaModel((util::EnvExpansion("${SRT_PRIVATE_CONTEXT}") + "/tf").c_str());
    // auto model = LSTME::initCAFAnaModel("tf");

    Var muE = LSTME::primaryEnergy(model);
    Var hadE = LSTME::secondaryEnergy(model);
    Var numuE = LSTME::totalEnergy(model);

    // Spectrum to be filled from the loader

    // By default, set rwgt_val = 0
    SystShifts shift_2020(&kPiplusTotSyst, percentage / 20.0);
    if (rwgt_val == 1)
    {
        shift_2020 = SystShifts(&kPiplusCexSyst, percentage / 20.0);
    }
    else if (rwgt_val == 2)
    {
        shift_2020 = SystShifts(&kPiplusDcexSyst, percentage / 20.0);
    }
    else if (rwgt_val == 3)
    {
        shift_2020 = SystShifts(&kPiplusQeSyst, percentage / 20.0);
    }
    else if (rwgt_val == 4)
    {
        shift_2020 = SystShifts(&kPiplusAbsSyst, percentage / 20.0);
    }
    else if (rwgt_val == 5)
    {
        shift_2020 = SystShifts(&kPiplusProdSyst, percentage / 20.0);
    }

    Spectrum muE_spectra("muE_spectra", bins, loader, muE, cut_0, shift_2020);
    Spectrum hadE_spectra("hadE_spectra", bins, loader, hadE, cut_0, shift_2020);
    Spectrum numuE_spectra("numuE_spectra", bins, loader, numuE, cut_0, shift_2020);

    // Do it!
    loader.Go();

    TFile *outFile = new TFile("spectra.root", "RECREATE");

    muE_spectra.SaveTo(outFile, "subdir_muE_spectra");
    hadE_spectra.SaveTo(outFile, "subdir_hadE_spectra");
    numuE_spectra.SaveTo(outFile, "subdir_numuE_spectra");

    outFile->Close();
}
