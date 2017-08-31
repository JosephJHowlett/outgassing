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


gErrorIgnoreLevel = kFatal;
gROOT->Macro("sub_macros/style.C");
void outgassing_partial(TString fname, TString option = "") {  // "comp" or "average"
   find_asymptotes(fname, measurement_time, measurement_pressure, fiterr, n_measurements, t_0);
   vector<TString> rga_dats = locate_rga_dat(measurement_time, n_measurements);
   const int nummeas = n_measurements;
   Double_t fraction [nummeas], time_hrs [nummeas], mass;
   Double_t molecule_mass [nummeas];
   TString molecule_name [nummeas];
   Double_t meanmass_err [nummeas], throughput [nummeas], throughput_avg [nummeas], *yplot;
   Double_t err_stat [nummeas], err_sys [nummeas], err_stat_avg [nummeas], err_sys_avg [nummeas];
   TString rga_filename = rga_dats[0];
   cerr.setstate(ios::failbit);
   rga_scan scan_1 = analyze_rga_dat_new(rga_filename,0,"getpeaks");
   const int npeaks = scan_1.n_peaks;
   
   TLegend *leg = new TLegend(0.8667342,0.845666,0.9976874,0.9936575);
   TMultiGraph *mg = new TMultiGraph();
   TGraphErrors **garray = new TGraphErrors*[npeaks];
   for (int j=0;j<npeaks;j++) {
      for (int i=0;i<n_measurements;i++) {
         TString rga_filename = rga_dats[i];
         input_mass = scan_1.peak_locs[j];
         scan = analyze_rga_dat_new(rga_filename, input_mass);
         fraction[i] = scan.mass_fraction;
         time_hrs[i] = (measurement_time[i] - t_0)/3600.;
         mass = input_mass;
         Double_t stat_factor = ( ((.0612/(2*mass))**2) + .0025 + (fiterr[i]**2))**.5;  // dQ/Q^2 = dP/P^2 + dm/2m^2
         Double_t conductance = coeff*trans_prob*1./(area*sqrt(mass));
         Double_t conductance_avg = coeff*trans_prob*1./(area*sqrt(scan.mass));
         throughput[i] = conductance * fraction[i] * measurement_pressure[i];
         err_sys[i] = stat_factor * throughput[i];
         if (option == "average") {
            throughput_avg[i] = conductance_avg*measurement_pressure[i];
            err_sys_avg[i] = .31*throughput_avg[i];
         }
      }
      ostringstream os;
      Int_t mymass = TMath::Nint(scan_1.peak_locs[j]);
      os << "Mass = "<<mymass;
      TString graphname = os.str();
      if (option == "comp") yplot = fraction;
      else yplot = throughput;
      garray[j] = new TGraphErrors(n_measurements, time_hrs, yplot, 0,  err_sys);
      garray[j]->SetTitle(graphname);
      garray[j]->SetMarkerColor(my_colors[j]);
      garray[j]->SetLineColor(my_colors[j]);
      
//       garray[j]->SetMarkerStyle(j+5);
      garray[j]->SetFillColor(0);
      garray[j]->SetMarkerStyle(20);
      garray[j]->SetMarkerSize(.5);
      mg->Add(garray[j]);
      leg->AddEntry(garray[j], graphname, "pl");
      if (option == "average" && (j==npeaks-1)) {
         TGraphErrors *gt = new TGraphErrors(n_measurements, time_hrs, throughput_avg, 0, err_sys_avg);
         gt->SetTitle("Average");
         gt->SetMarkerColor(1);
         gt->SetLineColor(1);
//          gt->SetFillColor(0);
         mg->Add(gt);
         leg->AddEntry(gt, "Average");
      }
   }
   TCanvas *c1 = new TCanvas("c1", "c1",650,40,800,500);
   c1->SetHighLightColor(10);
   c1->SetGrid() ;
   c1->SetFillColor(10);
   c1->SetBottomMargin(0.15);
   c1->SetLeftMargin(0.15);
   c1->SetLogy();
   c1->cd();
   leg->SetFillColor(10);
   mg->Draw("ap");
   leg->Draw("same");
   mg->GetXaxis()->SetTitle("Time Under Vacuum (Hrs)");
   if (option == "comp") {
      mg->SetTitle("Evolution of Gas Composition");
      mg->GetYaxis()->SetTitle("Fraction of Residual Gas");
   }
   else {
//       mg->SetTitle("Partial Outgassing");
      if (area == 1) mg->GetYaxis()->SetTitle("Total Outgassing Rate [Torr*l/s]");
      else mg->GetYaxis()->SetTitle("Specific Outgassing Rate [Torr*l/s*cm^{2}]");
   }
//    mg->GetYaxis()->SetNDivisions(003);
   if (print==1 && area == 1) c1->Print(file_header + "initial_findings/" + img_name + "_partial_int.png");
   else if (print ==1) c1->Print(file_header + "initial_findings/" + img_name + "_partial_spec.png");
   gPad->RedrawAxis();
   cerr.clear();
}
