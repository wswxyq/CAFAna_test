//S20-10-30
#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TString.h"
#include "TLegend.h"
#include "TLine.h"

#include "CAFAna/Core/Spectrum.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace ana;
using namespace std;


map<int, string> FSI_map= {
    {1,  "khNFSISyst2020_EV1"}, {2, "khNFSISyst2020_EV2"}, {3, "khNFSISyst2020_EV3"}, {0, "khNFSISyst2020_MFP"}
};
map<int, string> mode_map= {
    {100000, "NOCUT"}
};


void draw_resolution_select_fun(int mode_val, int input_FSI) {

    if ( mode_map.count(mode_val) > 0  )
        std::cout<<"Found supported mode. Now continue... with mode cut="<< mode_map[mode_val] <<std::endl;
    else {
        std::cout<<"NOT FOUND "<< mode_val <<" ! QUIT..."<< std::endl;
        return;
    }

    if ( FSI_map.count(input_FSI) > 0  )
        std::cout<<"Found supported FSI. Now continue... with FSI="<< FSI_map[input_FSI] <<std::endl;
    else {
        std::cout<<"NOT FOUND "<<input_FSI<<" ! QUIT..."<<std::endl;
        return;
    }


    auto subdir_LSTM = "subdir_LSTM_resolution_spectra";

    auto * inFile_origin = new TFile(("./results_resolution/"+std::to_string(mode_val)+"__/spectra.root").c_str());
    auto * inFile_modified_up = new TFile(("./results_resolution/"+std::to_string(mode_val)+"_"+std::to_string(input_FSI)+"_1/spectra.root").c_str());
    auto * inFile_modified_down = new TFile(("./results_resolution/"+std::to_string(mode_val)+"_"+std::to_string(input_FSI)+"_-1/spectra.root").c_str());

    std::cout<< "Load the spectrum..." << std::endl;
    std::unique_ptr<Spectrum> spect_origin = Spectrum::LoadFrom(inFile_origin, subdir_LSTM);
    std::unique_ptr<Spectrum> spect_modified_up = Spectrum::LoadFrom(inFile_modified_up, subdir_LSTM);
    std::unique_ptr<Spectrum> spect_modified_down = Spectrum::LoadFrom(inFile_modified_down, subdir_LSTM);

    //
    std::cout<<  "Plot the histo..." <<std::endl;
    //
    TCanvas *canvas_0 = new TCanvas("canvas_0", "plot resolution", 1200, 1200);
    //canvas_0->Divide(1, 2, 0, 0);
    TPad *pad1 = new TPad("pad1", " ",0.1,0.35,0.9,0.92);
    TPad *pad2 = new TPad("pad2", " ",0.1,0.1,0.9,0.35);

    pad1->Draw();
    pad2->Draw();

    pad1->cd();
    //pad1->SetTopMargin(.1);
    //pad1->SetBottomMargin(.1);
    //pad1->SetLeftMargin(.1);
    //pad1->SetRightMargin(.1);

    TH1D *TH1D_original = spect_origin->ToTH1(spect_origin->POT());
    TH1D *TH1D_modified_up = spect_modified_up->ToTH1(spect_modified_up->POT());
    TH1D *TH1D_modified_down = spect_modified_down->ToTH1(spect_modified_down->POT());

    TH1D_original->GetXaxis()->SetRangeUser(-2, 2);
    TH1D_modified_up->GetXaxis()->SetRangeUser(-2, 2);
    TH1D_modified_down->GetXaxis()->SetRangeUser(-2, 2);

    TH1D *TH1D_modified_up_factor = spect_modified_up->ToTH1(spect_modified_up->POT());
    TH1D *TH1D_modified_down_factor = spect_modified_down->ToTH1(spect_modified_down->POT());

    TH1D_modified_up_factor->GetXaxis()->SetRangeUser(-2, 2);
    TH1D_modified_down_factor->GetXaxis()->SetRangeUser(-2, 2);


    TH1D_original->SetLineWidth(2);
    TH1D_original->SetLineColor(kGreen);
    TH1D_original->SetLineStyle(kSolid);
    TH1D_original->Draw("hist_0");
    TH1D_original->GetYaxis()->SetTitle(" ");
    TH1D_original->GetXaxis()->SetTitle(" ");
    TH1D_original->SetTitle("resolution");

    TH1D_modified_up->SetLineWidth(2);
    TH1D_modified_up->SetLineColor(kRed);
    TH1D_modified_up->SetLineStyle(kSolid);
    TH1D_modified_up->Draw("SAME");

    TH1D_modified_down->SetLineWidth(2);
    TH1D_modified_down->SetLineColor(kOrange);
    TH1D_modified_down->SetLineStyle(kSolid);
    TH1D_modified_down->Draw("SAME");


    auto legend = new TLegend(0.55, 0.6, 0.8, 0.8);
    legend->SetHeader((FSI_map[input_FSI] + " , "+ mode_map[mode_val]+" mode").c_str(),"C"); // option "C" allows to center the header
    legend->AddEntry(TH1D_original, "Original mean: "+ TString::Format("%f",TH1D_original->GetMean()),"l");
    legend->AddEntry(TH1D_modified_up, "+1 sigma -shift mean: "+ TString::Format("%f",TH1D_modified_up->GetMean()),"l");
    legend->AddEntry(TH1D_modified_down, "-1 sigma -shift mean: "+ TString::Format("%f",TH1D_modified_down->GetMean()),"l");
    legend->SetTextSize(0.03);
    legend->Draw("SAME");

    pad2->cd();

    //pad2->SetTopMargin(.1);
    //pad2->SetLeftMargin(.1);
    //pad2->SetBottomMargin(.1);
    //pad2->SetRightMargin(.1);
    TH1D_modified_up_factor->Divide(TH1D_original);
    TH1D_modified_up_factor->GetYaxis()->SetRangeUser(0.85, 1.15);

    TH1D_modified_down_factor->Divide(TH1D_original);
    TH1D_modified_down_factor->GetYaxis()->SetRangeUser(0.85, 1.15);

    TH1D_modified_up_factor->SetLineWidth(2);
    TH1D_modified_up_factor->SetLineColor(kRed);
    TH1D_modified_up_factor->SetLineStyle(kSolid);
    TH1D_modified_up_factor->GetYaxis()->SetTitle(" ");
    TH1D_modified_up_factor->GetXaxis()->SetTitle(" ");
    TH1D_modified_up_factor->Draw("DD");

    TH1D_modified_down_factor->SetLineWidth(2);
    TH1D_modified_down_factor->SetLineColor(kOrange);
    TH1D_modified_down_factor->SetLineStyle(kSolid);
    TH1D_modified_down_factor->GetYaxis()->SetTitle(" ");
    TH1D_modified_down_factor->GetXaxis()->SetTitle(" ");
    TH1D_modified_down_factor->Draw("SAME");

    TLine *hline = new TLine(0,1,5,1);
    hline->SetLineColor(kGreen);
    hline->SetLineWidth(2);
    hline->Draw("SAME");

    canvas_0->Update();

    canvas_0->Print(("./pdf/"+mode_map[mode_val]+"_"+FSI_map[input_FSI]+"_resolution.pdf").c_str());
    canvas_0->Print(("./png/"+mode_map[mode_val]+"_"+FSI_map[input_FSI]+"_resolution.png").c_str());
    
    ofstream logfile;
    logfile.open("./log_resolution.log", std::ios::app);
    logfile << mode_map[mode_val] << "\t" << FSI_map[input_FSI] << endl;
    logfile << "Original(Green) mean:" << TH1D_original -> GetMean() << endl;
    logfile << "+1 sigma mean:" << TH1D_modified_up -> GetMean() << 
            "  " << std::setprecision(3) << 100. * ( TH1D_modified_up -> GetMean()-TH1D_original -> GetMean()) / TH1D_original -> GetMean() << "%" << endl;
    logfile << "-1 sigma mean:" << TH1D_modified_down -> GetMean() << 
            "  " << std::setprecision(3) << 100. * ( TH1D_modified_down -> GetMean()-TH1D_original -> GetMean()) / TH1D_original -> GetMean() << "%" << endl;
    logfile << "Original(Green) RMS:" << TH1D_original -> GetRMS()<<endl;
    logfile << "+1 sigma mean:" << TH1D_modified_up -> GetRMS() << 
            "  " << std::setprecision(3) << 100. * ( TH1D_modified_up -> GetRMS()-TH1D_original -> GetRMS()) / TH1D_original -> GetRMS() << "%" << endl;
    logfile << "-1 sigma mean:" << TH1D_modified_down -> GetRMS() << 
            "  " << std::setprecision(3) << 100. * ( TH1D_modified_down -> GetRMS()-TH1D_original -> GetRMS()) / TH1D_original -> GetRMS() << "%" << endl;
    logfile << endl;
    logfile.close();
    delete canvas_0;

}


void draw_resolution() {
    for (auto const& x : mode_map) {
        for (auto const& y : FSI_map) {
            draw_resolution_select_fun(x.first, y.first);
        }
    }
}