// S20-10-30.

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TString.h"
#include "TLegend.h"
#include "TLine.h"
#include <iostream>
#include "CAFAna/Core/Spectrum.h"
#include <string>

using namespace ana;

void Draw_resolution(int file_index = 0)
{

    auto subdir_LSTM = "subdir_LSTM_resolution_spectra";
    auto subdir_STANDARD = "subdir_Standard_resolution_spectra";

    auto *inFile = new TFile("results/100000/spectra.root", "read");

    // Load the spectrum...
    std::unique_ptr<Spectrum> spect_LSTM = Spectrum::LoadFrom(inFile, subdir_LSTM);
    std::unique_ptr<Spectrum> spect_STANDARD = Spectrum::LoadFrom(inFile, subdir_STANDARD);

    //
    // Plot the histo...
    //
    TCanvas *canvas_0 = new TCanvas("canvas_0", "plot numuE_spectra", 1200, 800);

    TH1D *TH1D_LSTM = spect_LSTM->ToTH1(spect_LSTM->POT());
    TH1D *TH1D_STANDARD = spect_STANDARD->ToTH1(spect_STANDARD->POT());

    cout << "INT OF LSTM:" << TH1D_LSTM->Integral() << endl;

    TH1D_LSTM->GetXaxis()->SetRangeUser(-2, 2);
    TH1D_STANDARD->GetXaxis()->SetRangeUser(-2, 2);

    TH1D_LSTM->SetLineWidth(2);
    TH1D_LSTM->SetLineColor(kGreen);
    TH1D_LSTM->SetLineStyle(kSolid);
    TH1D_LSTM->Draw("hist_0");
    TH1D_LSTM->SetName("LSTM");
    TH1D_LSTM->GetXaxis()->SetTitle("Resolution");

    TH1D_STANDARD->SetLineWidth(2);
    TH1D_STANDARD->SetLineColor(kBlue);
    TH1D_STANDARD->SetLineStyle(kSolid);
    TH1D_STANDARD->SetName("STANDARD");
    TH1D_STANDARD->Draw("hist_0 SAME");

    canvas_0->BuildLegend()->SetTextSize(0.025);

    canvas_0->Update();

    canvas_0->Print("./pdf/Resolution.pdf");
    cout << "POT of LSTM:" << spect_LSTM->POT() << endl;
    cout << "POT of standard:" << spect_STANDARD->POT() << endl;
    cout << "Integral of LSTM:" << TH1D_LSTM->Integral() << endl;
    cout << "Integral of standard:" << TH1D_STANDARD->Integral() << endl;
    cout << "RMS OF LSTM:" << TH1D_LSTM->GetRMS() << endl;
    cout << "RMS OF STANDARD:" << TH1D_STANDARD->GetRMS() << endl;
}
