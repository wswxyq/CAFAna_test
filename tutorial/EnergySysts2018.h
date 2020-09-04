#pragma once

#include "CAFAna/Core/ISyst.h"
#include "CAFAna/Systs/EvtRNGSyst.h"

namespace ana
{
  //----------------------------------------------------------------------
  class NeutronVisEScaleSyst2018: public EvtRNGSyst
  {
  public:
    NeutronVisEScaleSyst2018(bool useProngs, double threshE=0.040, double sf=3.6, double prob=0.33)
      : EvtRNGSyst(std::string("NeutronEvis") + (useProngs ? "Prongs" : "Primaries") + "Syst2018", "Neutron visible energy systematic 2018"),
        fUseProngs(useProngs), fThreshE(threshE), fScale(sf), fProb(prob)
    {}

    void Shift(double sigma, caf::SRProxy* sr, double& weight) const override;

  private:
    bool   fUseProngs;
    double fThreshE;
    double fScale;
    double fProb;
  };

  extern const NeutronVisEScaleSyst2018 kNeutronVisEScaleProngsSyst2018;
  extern const NeutronVisEScaleSyst2018 kNeutronVisEScalePrimariesSyst2018;
}
