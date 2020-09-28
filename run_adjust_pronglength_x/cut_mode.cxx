// Make a simple spectrum plot
#include "CAFAna/Cuts/Cuts.h"
#include "StandardRecord/Proxy/SRProxy.h"



using namespace std;

using namespace ana;

void cut_mode(){
    const Cut mode_Cut_QE(
        [] (const caf::SRProxy* sr)
        {
            return (sr->mc.nu[0].mode == 0);
        }
    );
}

