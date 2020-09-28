#include "CAFAna/Cuts/Cuts.h"
#include "CAFAna/Vars/Vars.h"
#include "StandardRecord/Proxy/SRProxy.h"

// QE, RES, DIS, Coh, MEC cut

using namespace ana;

  // Cut
  const Cut kTrueEbelow7GeV = kTrueE < 7.0;

  const Cut SanityCut(
      [] (const caf::SRProxy *sr)
      {
          return (sr->mc.nnu > 0) && (! sr->mc.nu[0].prim.empty());
      }
  );

  const Cut C(
      [] (const caf::SRProxy* sr)
      {
          return (
                (sr->sel.remid.pid > 0.5)
              && (sr->sel.cvnloosepreselptp.numuid > 0.5)
          );
      }
  );

  const Cut 
  sr->mc.nu[0].mode == 0