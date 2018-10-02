#include "sub_macros/plot_outgassing_errors_first.c"
#include "sub_macros/plot_outgassing_errors_second.c"
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
void outgassing_two_samples(TString fname_1, TString fname_2) {
   TString fname = fname_1;
   TLegend *leg = new TLegend(0.8667342,0.845666,0.9976874,0.9936575);
   find_asymptotes(fname, measurement_time, measurement_pressure, fiterr, n_measurements, t_0);
   vector<TString> rga_dats = locate_rga_dat(measurement_time, n_measurements);
   vector<Double_t> meanmass(n_measurements), meanmass_err(n_measurements), throughput(n_measurements);
   vector<long> time_hrs(n_measurements);
//    cout << "RGA Measurement:\t" << "Mean Mass (AMU):" << endl;
   for (int i = 0; i<n_measurements; i++) {
      TString rga_filename = rga_dats[i];
      means = analyze_rga_dat_new(rga_filename, 0);
      meanmass[i] = means.mass;
      meanmass_err[i] = means.err;
      if (meanmass[i] == 0 && i>0) meanmass[i] = meanmass[i-1]; //// in case rga .dat doesn't exist
//       cout << rga_dats[i] << "\t\t" << meanmass[i] << endl;//"\t± "<< meanmass_err[i] <<endl;
      Double_t mass = meanmass[i];
      Double_t conductance = coeff*trans_prob*1./(area*sqrt(mass));
      throughput[i] = conductance * measurement_pressure[i];
      time_hrs[i] = (measurement_time[i] - t_0)/3600.;
   }
   TGraphErrors *g_1 = plot_outgassing_errors_first(n_measurements, time_hrs, throughput, area, fiterr);
   leg->AddEntry(g_1, fname, "pl");
   
   fname = fname_2;
   find_asymptotes(fname, measurement_time, measurement_pressure, fiterr, n_measurements, t_0);
   vector<TString> rga_dats = locate_rga_dat(measurement_time, n_measurements);
   vector<Double_t> meanmass(n_measurements), meanmass_err(n_measurements), throughput(n_measurements);
   vector<long> time_hrs(n_measurements);
//    cout << "RGA Measurement:\t" << "Mean Mass (AMU):" << endl;
   for (int i = 0; i<n_measurements; i++) {
      TString rga_filename = rga_dats[i];
      means = analyze_rga_dat_new(rga_filename, 0);
      meanmass[i] = means.mass;
      meanmass_err[i] = means.err;
      if (meanmass[i] == 0 && i>0) meanmass[i] = meanmass[i-1]; //// in case rga .dat doesn't exist
//       cout << rga_dats[i] << "\t\t" << meanmass[i] << endl;//"\t± "<< meanmass_err[i] <<endl;
      Double_t mass = meanmass[i];
      Double_t conductance = coeff*trans_prob*1./(area*sqrt(mass));
      throughput[i] = conductance * measurement_pressure[i];
      time_hrs[i] = (measurement_time[i] - t_0)/3600.;
   }
   TGraphErrors *g_2 = plot_outgassing_errors_second(n_measurements, time_hrs, throughput, area, fiterr);
   leg->AddEntry(g_2, fname, "pl");
   leg->Draw("same");
}