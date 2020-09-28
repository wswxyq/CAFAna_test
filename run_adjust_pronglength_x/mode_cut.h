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

#include <string> 

// QE, RES, DIS, Coh, MEC cut

using namespace ana;

  // Cut

  const Cut mode_Cut_QE(
      [] (const caf::SRProxy* sr)
      {
          return (sr->mc.nu[0].mode == 0);
      }
  );

  const Cut mode_Cut_RES(
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