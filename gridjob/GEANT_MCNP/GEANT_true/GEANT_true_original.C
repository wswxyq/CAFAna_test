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

#include "Utilities/func/EnvExpand.cxx"


using namespace ana;


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




void GEANT_true_original(int mode_val)
{
    // Environment variables and wildcards work. Most commonly you want a SAM
    // dataset. Pass -ss --limit 1 on the cafe command line to make this take a
    // reasonable amount of time for demo purposes.


    map<int, string> mode_map= {
        {0,  "QE"}, {1, "Res"}, {2, "DIS"}, {3, "Coh"},{10, "MEC"}
    };

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
    } else if (mode_val==1)
    {
        cut=cut_0 && mode_Cut_Res;
    } else if (mode_val==2)
    {
        cut=cut_0 && mode_Cut_DIS;
    } else if (mode_val==3)
    {
        cut=cut_0 && mode_Cut_Coh;
    } else if (mode_val==10)
    {
        cut=cut_0 && mode_Cut_MEC;
    } else if (mode_val==100000)
    {
        cut=cut_0;
    } else
    {
        return;
    }


    // Spectrum to be filled from the loader

    Spectrum TrueE_spectra("TrueE_spectra", bins, loader, kTrueE, cut);

    // Do it!
    loader.Go();

    // Now save to disk...
    TFile *outFile = new TFile("spectra.root","RECREATE");

    TrueE_spectra.SaveTo(outFile, "subdir_TrueE_spectra");

    outFile->Close();
}
