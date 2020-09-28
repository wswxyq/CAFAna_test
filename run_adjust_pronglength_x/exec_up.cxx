#include "select_abs_up.h"
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
void exec_up(){
    select_abs_and_mode_up(111, 0);
}