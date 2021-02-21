// Plot the histograms made using example_macro.C
//
// Each of the 36 "universes" we have weights for corresponds to 
// varying one of the 6 pion interaction knobs up/down by 5, 10, or 20%.
//
// It is as follows:
// Universes  0-5  : Total Inelastic (reac) Knob {0.80, 0.90, 0.95, 1.05, 1.10, 1.20} 
// Universes  6-11 : Charge Exchange (cex) Knob {0.80, 0.90, 0.95, 1.05, 1.10, 1.20} 
// Universes 12-17 : Double Charge Exchange (dcex) Knob {0.80, 0.90, 0.95, 1.05, 1.10, 1.20} 
// Universes 18-23 : Quasi-elastic (inel) Knob {0.80, 0.90, 0.95, 1.05, 1.10, 1.20} 
// Universes 24-29 : Absorption (abs) Knob {0.80, 0.90, 0.95, 1.05, 1.10, 1.20} 
// Universes 30-25 : Production (prod) Knob {0.80, 0.90, 0.95, 1.05, 1.10, 1.20} 
//
// For example, to get an error band due to a 20% uncertainty on the total inelastic
// pion scattering cross section, use the weights from universe 0 and 5.
//
// Author: Cathal Sweeney - csweeney@fnal.gov


#include "CAFAna/Core/Spectrum.h"

#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TGraphAsymmErrors.h"               
#include "TString.h"      
#include "TColor.h"
#include "TGaxis.h"
#include "TLegend.h"
#include "TLegendEntry.h"

#include <iostream>
#include <fstream>

// for plotting
#include "CAFAna/Analysis/Plots.h"

namespace ana
{

  std::vector<std::string> knobName_list = {
    "Total inelastic",
    "Charge exchange",
    "Double charge exchange",
    "Quasi-elastic",
    "Absorption",
    "Pion production"
  };

  std::vector<std::string> knob_list = {
    "reac",
    "cex",
    "dcex",
    "inel",
    "abs",
    "prod"
  };



  std::map<std::string, int> knob_map = {
    {"reac",  0},
    {"cex",   6},
    {"dcex", 12},
    {"inel", 18},
    {"abs",  24},
    {"prod", 30}
  };

  std::map<float, int> variation_map = {
    {0.2,  0},
    {0.1,  1},
    {0.05, 2}
  };

  std::vector<double> variantion_vec = {0.80, 0.90, 0.95, 1.05, 1.10, 1.20};

 
  TGraphAsymmErrors* Hypercross_plots(TH1* nom,
                                      std::vector<TH1*> univs,
                                      std::string knob,
                                      int colour,
                                      float variation)
	// Plots spectra with error band due to varying a given knob by
	// the specified variation (5, 10, 20%) 
 {
    int down_idx = knob_map[knob] + variation_map[variation];
    int up_idx = knob_map[knob] + 5 - variation_map[variation];

    std::vector<TH1*> hDown_vec{univs[down_idx]};
    std::vector<TH1*> hUp_vec{univs[up_idx]};
     
    TGraphAsymmErrors* graph = PlotWithSystErrorBand(nom,
                                                    hUp_vec,
                                                    hDown_vec, 
                                                    kBlack,
                                                    colour);

    return graph;
  }
  // end Hypercross_plots()




  TH1* FracUncert(TH1* hSig, std::string knob,
                  std::vector<TH1*> univs, 
                  float variation,
                  std::string myFullLabel)
  {

    int down_idx = knob_map[knob] + variation_map[variation];
    int up_idx = knob_map[knob] + 5 - variation_map[variation];

    TH1* hDown = univs[down_idx];
    TH1* hUp   = univs[up_idx];
   
    // Get fractional uncertainty (take larger of +/-)                                                                              
    int nBins = hSig->GetNbinsX();
    double xLow = hSig->GetXaxis()->GetXmin();
    double xHigh = hSig->GetXaxis()->GetXmax();

    TH1D * fracUncert = new TH1D("frac", "", nBins, xLow, xHigh);
    for(int i=1; i <= nBins; ++i){
      double up   = hUp->GetBinContent(i);
      double down = hDown->GetBinContent(i);
      double nom  = hSig->GetBinContent(i);

      double abs_shift = std::max( std::abs(up - nom), std::abs(down - nom) );
      double frac = abs_shift / nom;

      if( isnan(frac) ) fracUncert->SetBinContent(i, 0.f);
      else fracUncert->SetBinContent(i, frac);

    }

    fracUncert->GetYaxis()->SetTitle("#frac{#deltaN}{N}");
    fracUncert->GetXaxis()->SetTitle(myFullLabel.c_str());
    
    return fracUncert;

  }//end FracUncert()


}


using namespace ana;  

void LSTM_plot()
{

  TGaxis::SetMaxDigits(5);

  std::string inName = "spectra.root";

  TFile* inFile = TFile::Open(inName.c_str(), "read");

  int nUnivs = knob_map.size() * variation_map.size() * 2;

  std::vector<std::string> myLabel_vec{
    "muE", 
    "hadE", 
    "numuE" 
  };
  std::vector<std::string> myTitle_vec{
    "Piplus 100 universes",  
    "Piplus 100 universes",
    "Piplus 100 universes"
  };
  std::vector<std::string> myName_vec  = {
    "piplus_muE",
    "piplus_hadE",
    "piplus_numuE"
  }; 



  for(uint i=0; i<myName_vec.size(); ++i){

    std::string myName = myName_vec[i];
    std::string myLabel = myLabel_vec[i];
    std::string myTitle = myTitle_vec[i];

    std::string myFullLabel;

    if(myLabel.compare("muE") == 0){
      myFullLabel = "muon Energy";
    }
    else if(myLabel.compare("hadE") == 0){
      myFullLabel = "hadron Energy";
    }
    else{
      myFullLabel = "muon neutrino Energy";
    }


    std::string base_dir = myName_vec[i];
		// Load in nominal spectrum
    Spectrum * sSig = Spectrum::LoadFrom( inFile->GetDirectory(base_dir.c_str()) , "nom" ).release();


    double pot = sSig->POT();
    TH1 * hSig = sSig->ToTH1(pot);
    hSig->GetXaxis()->SetTitle(myFullLabel.c_str());

    std::ofstream outfile;

    outfile.open(myLabel_vec[i], std::ios::in | std::ios::out | std::ios::app);

    outfile << "nom: " << hSig->GetMean() << std::endl;

    std::vector<TH1*> hUnivs;
    for(int iUniv=0; iUniv<nUnivs; iUniv++){
      std::string univ_dir = "univ_" + std::to_string(iUniv);
      Spectrum* sUniv = Spectrum::LoadFrom(inFile->GetDirectory(base_dir.c_str()), univ_dir.c_str()).release();
      TH1* hUniv = sUniv->ToTH1(pot);
      hUniv->GetXaxis()->SetTitle(myFullLabel.c_str());
      hUnivs.push_back(hUniv);
      outfile << knob_list[iUniv/6] << "\t\t" << variantion_vec[iUniv%6] << "\t\t" << hUniv->GetMean() << std::endl;
    }

    outfile.close();



    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    for(int iKnob=0; iKnob< (int)knob_list.size(); iKnob++){

      std::string outName = "./g4rwgt_";

      outName += myName;
      outName += "_";


      std::string knob_name = knob_list[iKnob];
      hSig->SetTitle((knobName_list[iKnob]).c_str());

      outName += knob_name;


      TGraphAsymmErrors * graph_1 =  Hypercross_plots(hSig, hUnivs, 
                                                      knob_name, kRed,
                                                      0.2);
      TH1* frac_1 = FracUncert(hSig, knob_name, hUnivs, 0.2, myFullLabel);
      frac_1->SetLineColor(kRed);

      TGraphAsymmErrors * graph_2 =  Hypercross_plots(hSig, hUnivs, 
                                                      knob_name, kBlue,
                                                      0.1);
      TH1* frac_2 = FracUncert(hSig, knob_name, hUnivs, 0.1, myFullLabel);
      frac_2->SetLineColor(kBlue);
      
      TGraphAsymmErrors * graph_3 =  Hypercross_plots(hSig, hUnivs, 
                                                      knob_name, kGreen+2,
                                                      0.05);
      TH1* frac_3 = FracUncert(hSig, knob_name, hUnivs, 0.05, myFullLabel);
      frac_3->SetLineColor(kGreen +2);

      auto legend = new TLegend(0.6,0.5,0.95,0.9);
      legend->AddEntry(graph_1, "#pm 20% error band", "f");
      legend->AddEntry(graph_2, "#pm 10% error band", "f");
      legend->AddEntry(graph_3, "#pm  5% error band", "f");
      legend->AddEntry(hSig, "Nominal", "l");

      /*
      TLegendEntry *header = (TLegendEntry*)legend->GetListOfPrimitives()->First();
      //header->SetTextAlign(22);
      //header->SetTextColor(2);
      header->SetTextSize(.2);
      */

      //.........................................
      TString rCname = "rC"; 

      TCanvas* rC = new TCanvas(rCname, rCname);
  
      rC -> SetBottomMargin(0.);
      double Spl = 0.3;
      TPad* P1 = new TPad( "Temp_1", "", 0.0, Spl, 1.0, 1.0, 0 );
      TPad* P2 = new TPad( "Temp_2", "", 0.0, 0.0, 1.0, Spl, 0 );
      P2 -> SetRightMargin (.03);
      P2 -> SetTopMargin   (.00);
      P2 -> SetBottomMargin(.3);
      P2 -> SetLeftMargin  (.13);
      P2 -> Draw();
      P1 -> SetRightMargin (.03);
      P1 -> SetLeftMargin  (.13);
      P1 -> SetTopMargin   (.1);
      P1 -> SetBottomMargin(.00);
      P1 -> Draw();
      // Set some label sizes.                                                                                
      double Lb1 = 0.07;
      double Lb2 = 0.13;
      // --- First, draw the fracUncert so cd onto Pad2                                                           
      P2 -> cd();
      
      // Set axis ranges etc.                                                                                 
      frac_1->GetYaxis()->SetTitleSize( Lb2 );
      frac_1->GetYaxis()->SetTitleOffset(0.4);
      //frac_1->GetYaxis()->SetLabelOffset(0.05);
      frac_1->GetYaxis()->SetLabelSize( Lb2 );
      frac_1->GetXaxis()->SetTitleSize( Lb2 );
      frac_1->GetXaxis()->SetLabelSize( Lb2 );
      frac_1->Draw("");
      frac_2->Draw("SAME");
      frac_3->Draw("SAME");

      
      P1->cd();

      hSig->GetYaxis()->SetTitleSize( Lb1 );
      hSig->GetYaxis()->SetLabelSize( Lb1 );
      hSig->GetYaxis()->SetTitleOffset( 0.7 );
      // Remove the x axis labels                                                                         
      hSig->GetXaxis()->SetLabelSize  (0 );
      hSig->GetXaxis()->SetLabelOffset(99);



      //.........................................



      hSig->Draw("hist ][");
      graph_1->Draw("e2 same");
      graph_2->Draw("e2 same");
      graph_3->Draw("e2 same");
      hSig->Draw("hist ][ same");

      legend->Draw();
      
      rC->SaveAs((outName + ".png").c_str());

      delete rC;
      delete frac_1;
      delete frac_2;
      delete frac_3;
    }

    /*    
    TGraphAsymmErrors * graph = PlotWithSystErrorBand(hSig, hUp_vec, hDown_vec);  
    graph->SetTitle(myTitle.c_str());
  
    //TCanvas * canv = new TCanvas("canv", "canv", 800, 600);
  
    hSig->SetLineColor(1);
    hSig->SetLineWidth(2);
    TString rCname = "rC";
    TCanvas* rC = new TCanvas(rCname, rCname);
  
    rC -> SetBottomMargin(0.);
    double Spl = 0.3;
    TPad* P1 = new TPad( "Temp_1", "", 0.0, Spl, 1.0, 1.0, 0 );
    TPad* P2 = new TPad( "Temp_2", "", 0.0, 0.0, 1.0, Spl, 0 );
    P2 -> SetRightMargin (.03);
    P2 -> SetTopMargin   (.00);
    P2 -> SetBottomMargin(.3);
    P2 -> SetLeftMargin  (.13);
    P2 -> Draw();
    P1 -> SetRightMargin (.03);
    P1 -> SetLeftMargin  (.13);
    P1 -> SetTopMargin   (.1);
    P1 -> SetBottomMargin(.00);
    P1 -> Draw();
    // Set some label sizes.                                                                                
    double Lb1 = 0.07;
    double Lb2 = 0.15;
    // --- First, draw the fracUncert so cd onto Pad2                                                           
    P2 -> cd();
  
    // Set axis ranges etc.                                                                                 
    fracUncert->GetYaxis()->SetTitleSize( Lb2 );
    fracUncert->GetYaxis()->SetTitleOffset(0.4);
    fracUncert->GetYaxis()->SetLabelSize( Lb2 );
    fracUncert->GetXaxis()->SetTitleSize( Lb2 );
    fracUncert->GetXaxis()->SetLabelSize( Lb2 );
    fracUncert->Draw("");
  
    //fracUncert->Draw("axis same");
    P1->cd();
    // graph goes on P1    
    hSig->SetTitle(myTitle.c_str());
    hSig->GetYaxis()->SetTitleSize( Lb1 );
    hSig->GetYaxis()->SetLabelSize( Lb1 );
    hSig->GetYaxis()->SetTitleOffset( 0.7 );
    // Remove the x axis labels                                                                         
    hSig->GetXaxis()->SetLabelSize  (0 );
    hSig->GetXaxis()->SetLabelOffset(99);
    hSig->SetLineColor(4);
    hSig->DrawCopy("hist");
  
    hSig->Draw("hist ][");
    graph->Draw("e2 same");
    hSig->Draw("hist ][ same");
  
    rC->SaveAs((outName + ".png").c_str());
  
    */


  }//end for(myName_vec)  
  
  inFile->Close();

}

