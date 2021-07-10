
#include "TCanvas.h"
#include "TLegend.h"
#include "TPad.h"

#include "CAFAna/Analysis/Plots.h"
#include "CAFAna/Analysis/Prod4Loaders.h"
#include "CAFAna/Core/ISyst.h"
#include "CAFAna/Core/Ratio.h"
#include "CAFAna/Core/Spectrum.h"
#include "3FlavorAna/Cuts/NumuCuts.h"
#include "CAFAna/Cuts/SpillCuts.h"
#include "CAFAna/Prediction/PredictionInterp.h"
#include "CAFAna/Prediction/PredictionNoOsc.h"
#include "CAFAna/Vars/HistAxes.h"
#include "OscLib/OscCalc.h"
#include "StandardRecord/Proxy/SRProxy.h"
#include "Utilities/func/MathUtil.h"

using namespace ana;

namespace demo {
//  Systs are classes derived from ana::ISyst.
//  Instances of each Syst class must be unique:
//  if you instantiate more than one, you'll get a complaint
//  from the SystRegistry that they're stored in.
//  (More on that later.)

//  Let's take a simple first example:
//    imagine you thought that there was a beam-related uncertainty
//    that depends on Enu: +1sigma = fixed 10% for Enu < 5 GeV,
//    and 10% + a function that blows up smoothly from 10% to 50%
//    from 5 to 15 GeV:
//    y(E) = 0.1 + 0.4*Heaviside(E-5)*(1-exp(-(E-5)**2/10))
class DemoSyst1 : public ISyst {
  public:
    DemoSyst1()
    // they have to supply a "short" name (used for lookup internally)
    // and a "Latex" name (which is used on plots).
    // The "short" name MUST be unique or you'll get runtime errors...
        : ISyst("DemoSyst1", "Demo syst ##1")
    {}

    // the magic happens in TruthShift() (when truth-only) or Shift() (when reco involved).
    // here, we're just reweighting events according to the function
    // described above, so we just adjust the weight
    void TruthShift(double sigma, caf::SRNeutrinoProxy* sr, double& weight) const override {

        // first calculate the function.
        double additionalWgt = 0.1;
        double Enu = sr->E;
        if (Enu > 5)
            additionalWgt += 0.4 * (1 - exp(-util::sqr(Enu-5)/10.));

        // we just constructed the +1sigma function.
        // now compute the actual shift requested.
        additionalWgt *= sigma;

        // obviously the nominal (0 sigma) means leave the event alone,
        // so that's weight = 1.
        // increasing by 10% is weight = 1.1, etc.
        // so we add the change to 1 to get the weight to use.
        additionalWgt += 1;

        // we don't want events with negative weights...
        if (additionalWgt < 0)
            additionalWgt = 0;

        // remember to multiply it into the weight that was already there...
        weight *= additionalWgt;
    }
};

// In CAFAna/Syst, you'll find that the instances of the ISyst classes
// in .h files are all declared 'extern', which tells the compiler
// not to instantiate them based on the header declaration.
// They get instantiated by the corresponding lines in the .cxx files.
// This way, even if a .h is #included in multiple places, there
// will actually only ever be a single instance of the Syst object
// in the library that's ultimately made -- the one coming from the .cxx.
// This demo is standalone, so the implicit second copy that gets made
// by ACLiC (don't worry about it) and you see warnings about in the output
// won't hurt anything.
const DemoSyst1 kDemoSyst1;

// ---------------------------------------

// Now imagine a systematic that needs to change the values
// of some part of the reconstructed event
// (for instance, an energy scale shift.)
// That (usually) can't be implemented just by reweighting.
// In this example we'll make one that adds a fixed amount of visible energy
// for each neutron in the event.
class DemoSyst2 : public ISyst {
  public:
    DemoSyst2()
        : ISyst("DemoSyst2", "Demo syst ##2")
    {}

    // we'll be modifying the SRProxy this time.
    // (that's why it's passed non-const.)
    void Shift(double sigma, caf::SRProxy* sr, double& weight) const override {
        // if no truth info, we can't do anything
        if (sr->mc.nnu != 1)
            return;

        unsigned int nNeutron = 0;
        for (const auto & particle : sr->mc.nu[0].prim) {
            if (particle.pdg != 2112)
                continue;

            // only increase if they were less than 50 MeV to begin with
            if (particle.visEinslcBirks + particle.daughterVisEinslcBirks >= 0.050)
                continue;

            nNeutron++;
        }

        // now adjust the numu energy.
        // we're going to add 25 MeV of visible energy for each neutron
        // for the +1sigma shift (and subtract it for -1sigma, etc.)
        double addedE = sigma * 0.025 * nNeutron;
        if (sr->energy.numu.hadcalE + addedE < 0)
            addedE = -sr->energy.numu.hadcalE;  // don't let the shift make a negative energy deposit

        sr->energy.numu.hadcalE += addedE;   // this goes into the numu energy estimator function

    }
};

// now instantiate
const DemoSyst2 kDemoSyst2;

// ---------------------------------------
// makes drawing easier later
TCanvas * DrawUpDownRatioCanvas(const PredictionInterp * pred,
                                osc::IOscCalc * calc,
                                const ISyst* syst) {
    SystShifts shifts;

    // get the spectra from the predictions
    auto spec_nom = pred->PredictUnoscillated();
    shifts.SetShift(syst, 1.0);
    auto spec_shift_up = pred->PredictSyst(calc, shifts);
    Ratio rat_shift_up(spec_shift_up, spec_nom);
    shifts.SetShift(syst, -1.0);
    auto spec_shift_down = pred->PredictSyst(calc, shifts);
    Ratio rat_shift_down(spec_shift_down, spec_nom);

    double pot = spec_nom.POT();

    // prep the drawing area
    auto c = new TCanvas;  // just leak this in the demo.
    TPad * top;
    TPad * bottom;
    SplitCanvas(0.3, top, bottom);
    auto leg = new TLegend(0.6, 0.6, 0.9, 0.9);  // also leaked
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);

    // compare the event rate spectra in the top panel
    top->cd();
    auto hup = spec_shift_up.ToTH1(pot, kRed, kDashed);
    hup->GetXaxis()->SetTitleSize(0); // since it will also be drawn on the bottom panel
    hup->GetXaxis()->SetLabelSize(0);
    hup->Draw("hist");
    auto hnom = spec_nom.ToTH1(pot);
    hnom->Draw("hist same");
    auto hdown = spec_shift_down.ToTH1(pot, kBlue, kDashed);
    hdown->Draw("hist same");
    leg->AddEntry(hnom, "Nominal", "l");
    leg->AddEntry(hup, "+1#sigma shift");
    leg->AddEntry(hdown, "-1#sigma shift");
    leg->Draw();

    // compare the ratios to nominal in the bottom
    bottom->cd();
    auto hbottom = rat_shift_up.ToTH1(kRed);
    hbottom->GetYaxis()->SetRangeUser(0.45, 1.55);
    hbottom->Draw("hist");
    rat_shift_down.ToTH1(kBlue)->Draw("hist same");

    return c;
}

}

// ---------------------------------------

void demoSysts() {
    // let's use the systs to create some shifted distributions to prove they work.

    Prod4NomLoaders loaders(ana::kNumuConcat, ana::Loaders::kFHC);
    loaders.SetSpillCut(ana::kStandardSpillCuts);

    // we'll use ND predictions for simplicity.  (no oscillations to confuse us)
    osc::NoOscillations noOscCalc;
    auto & NDLoader = loaders.GetLoader(caf::kNEARDET, Loaders::kMC);

    // ------
    // predictions for syst #1

    // we just want to see the effect on the true spectrum, so don't bother with the reco cuts here
    std::vector<const ISyst*> systs1 { &demo::kDemoSyst1 };
    SystShifts shifts1(&demo::kDemoSyst1, 1.0);
    HistAxis trueEaxis("True neutrino energy (GeV)", Binning::Simple(60, 0, 15), kTrueE);
    NoOscPredictionGenerator predGen_shift1(NDLoader, trueEaxis, kNoCut, kUnweighted);
    PredictionInterp pred_trueE_syst1(systs1, &noOscCalc, predGen_shift1, loaders, shifts1);

    // ------
    // predictions for syst #2

    // for this one we want to see the effect on the events with neutrons separately
    const Cut kHasNeutron([](const caf::SRProxy * sr) {
        if (sr->mc.nnu != 1)
            return false;

        for (const auto & particle : sr->mc.nu[0].prim) {
            if (particle.pdg == 2112)
                return true;
        }

        return false;
    });

    // here we need the reco cuts and all.
    std::vector<const ISyst*> systs2 { &demo::kDemoSyst2 };
    SystShifts shifts2(&demo::kDemoSyst2, 1.0);
    NoOscPredictionGenerator predGen_numuCC_ND_withNeutron(NDLoader, kNumuCCAxis, kNumuND && kHasNeutron, kUnweighted);
    PredictionInterp pred_recoE_withNeutron_syst2(systs2, &noOscCalc, predGen_numuCC_ND_withNeutron, loaders, shifts2);
    NoOscPredictionGenerator predGen_numuCC_ND_noNeutron(NDLoader, kNumuCCAxis, kNumuND && !kHasNeutron, kUnweighted);
    PredictionInterp pred_recoE_noNeutron_syst2(systs2, &noOscCalc, predGen_numuCC_ND_noNeutron, loaders, shifts2);

    // ------
    // make predictions now
    loaders.Go();


    // ------
    // draw comparison plots
    demo::DrawUpDownRatioCanvas(&pred_trueE_syst1, &noOscCalc, &demo::kDemoSyst1)->Print("test_syst1.png");
    demo::DrawUpDownRatioCanvas(&pred_recoE_withNeutron_syst2, &noOscCalc, &demo::kDemoSyst2)->Print("test_syst2_neutron.png");
    demo::DrawUpDownRatioCanvas(&pred_recoE_noNeutron_syst2, &noOscCalc, &demo::kDemoSyst2)->Print("test_syst2_noneutron.png");
};