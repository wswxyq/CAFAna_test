// Make a simple spectrum plot
#include "CAFAna/Cuts/Cuts.h"
#include "StandardRecord/Proxy/SRProxy.h"



using namespace std;

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