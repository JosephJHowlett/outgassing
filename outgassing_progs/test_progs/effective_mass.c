#include "sub_macros/find_asymptotes.c"
#include "sub_macros/locate_rga_dat.c"
#include "sub_macros/analyze_rga_dat.c"
#include "sub_macros/analyze_rga_dat_new.c"
#include "sub_macros/plot_outgassing_errors.c"
#include <sstream>
#include <cstring>
#include <string>
#include <iostream>
#include <TString>
#include "TFile.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"
#include "sub_macros/settings.h"

gROOT->Macro("sub_macros/style.C");
void effective_mass(TString fname, Double_t input_mass =0) {
   find_asymptotes(fname, measurement_time, measurement_pressure, fiterr, n_measurements, t_0);
   vector<TString> rga_dats = locate_rga_dat(measurement_time, n_measurements);
   
   vector<Double_t> meanmass(n_measurements), meanmass_err(n_measurements), throughput(n_measurements);
   vector<long> time_hrs(n_measurements);
   cout << "RGA Measurement:\t" << "Mean Mass (AMU):" << endl;
   for (int i = 0; i<n_measurements; i++) {
      TString rga_filename = rga_dats[i];
      means = analyze_rga_dat_new(rga_filename, input_mass);
      meanmass[i] = means.mass;
      meanmass_err[i] = means.err;
      if (meanmass[i] == 0 && i>0) meanmass[i] = meanmass[i-1]; //// in case rga .dat doesn't exist
      cout << rga_dats[i] << "\t\t" << meanmass[i] <<endl;//<< "\t± "<< meanmass_err[i] <<endl;
      Double_t mass = meanmass[i];
      Double_t conductance = coeff*trans_prob*1./(area*sqrt(mass));
      throughput[i] = conductance * measurement_pressure[i];
      time_hrs[i] = (measurement_time[i] - t_0)/3600.;
   }
   TGraphErrors *g = new TGraphErrors();
   for (int i=0;i<n_measurements;i++) {
      g->SetPoint(i, time_hrs[i], meanmass[i]);
      g->SetPointError(i, .25, .06);
   }
//    g->SetPoint(n_measurements+1, 40.3, 25.86); stov_1_drybox
//    g->SetPointError(n_measurements+1, .25, .06);
   TCanvas *c1 = new TCanvas("c1", "c1",650,40,800,500);
   c1->SetHighLightColor(10);
   c1->SetGrid() ;
   c1->SetFillColor(10);
   c1->SetBottomMargin(0.15);
   c1->SetLeftMargin(0.15);
   g->Draw("ap");
   g->GetXaxis()->SetTitle("Time Under Vacuum [hours]");
   g->GetYaxis()->SetTitle("Effective (Average) Molecular Mass [AMU]");
   ///////// PLOTTING STUFF ///////////
   g->SetMarkerSize(.5);
   g->SetMarkerStyle(8);
   gStyle->SetTitleY(0.97);
   gStyle->SetTitleX(0.25);
   g->GetXaxis()->CenterTitle(true);
   g->GetXaxis()->SetLabelOffset(0.01);
   g->GetXaxis()->SetLabelSize(0.03);
   g->GetYaxis()->CenterTitle(true);
   
//   c1->Print(file_header + "initial_findings/" + img_name + "_mass.png");
}
