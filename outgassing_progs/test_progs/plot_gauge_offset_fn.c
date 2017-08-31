#include <sstream>
#include <cstring>
#include <string>
#include <iostream>
#include <TString>
#include "TFile.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"
#include "TMath.h"
gROOT->Macro("style.C");

TCanvas *c1 = new TCanvas("c1", "c1",650,40,1500,1000);
c1->SetHighLightColor(10);
c1->SetGrid() ;
c1->SetFillColor(10);
c1->SetBottomMargin(0.15);
c1->SetLeftMargin(0.15);

void plot_gauge_offset_fn()
{
const int npts = 100;
Double_t max_hrs = 92;
Double_t min_hrs = 10;
Double_t pressure_old [npts], pressure_new [npts], pressure_diff [npts], pressure_new_corrected [npts];
  for (int i = 0; i<npts; i++)
  {
    Double_t time_hrs = double(i)/double(npts)*(max_hrs-min_hrs) + min_hrs;
    pressure_old[i] = 7.33e-5 + TMath::Exp(-8.21 + -3.25e-2*time_hrs);
    pressure_new[i] = 2.11e-5 + TMath::Exp(-8.61 + -3.25e-2*time_hrs);
    pressure_new_corrected[i] = pressure_new[i]*.491825 + pressure_new[i] + 4.182e-5;
    pressure_diff[i] = pressure_old[i] - pressure_new[i];
  }

TGraph * g1 = new TGraph(npts, pressure_new, pressure_new_corrected);
g1->SetMarkerSize(.5);
g1->SetMarkerStyle(8);
g1->Draw("ap");
}