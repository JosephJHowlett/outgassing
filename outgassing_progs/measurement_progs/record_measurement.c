#include "/Users/josephhowlett/myroot/outgassing_analysis/outgassing_progs/sub_macros/analyze_rga_dat_new.c"
#include "/Users/josephhowlett/myroot/outgassing_analysis/outgassing_progs/sub_macros/plot_outgassing_errors.c"
#include <sstream>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <TString>
#include "TFile.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"
#include "/Users/josephhowlett/myroot/outgassing_analysis/outgassing_progs/sub_macros/settings.h"
#include <ctime>

area = 1;
gROOT->Macro("/Users/josephhowlett/myroot/outgassing_analysis/outgassing_progs/sub_macros/style.C");
//gROOT->Macro("../sub_macros/style.C");
void record_measurement(TString fname, Double_t pressure) {
   rga_scan scan = analyze_rga_dat_new(fname, 0, "getpeaks");
   TDatime calc_time = time(0);
   ofstream logfile;
   logfile.open(file_header + "rga_measurements/measurements_log.txt", ios::app);
//    logfile << "\n";
   logfile << "\nFile opened at " << calc_time.AsSQLString() << " (EDT)" << endl << "RGA scan shows  ";
   const int num = scan.n_peaks;
   int order [num], pos [num];
   Double_t smallest = 300;
   for (int i = 0; i<scan.n_peaks; i++) {
      if (scan.peak_heights[i] < smallest) smallest = scan.peak_heights[i];
      pos[i] = 0;
      for (int j = 0; j < scan.n_peaks; j++) {
         if (scan.peak_locs[i] > scan.peak_locs[j]) pos[i]++;
      }
   }
   for (int i = 0; i<scan.n_peaks; i++) {
      for (int j = 0; j<scan.n_peaks;j++) {
         if (pos[j]==i) {
            logfile << int(scan.peak_locs[j] + .5);
            if (i<scan.n_peaks-1) logfile << ":";
         }
      }
   }
   logfile << " = ";
   for (int i = 0; i<scan.n_peaks; i++) {
      for (int j=0; j<scan.n_peaks; j++) {
         if (pos[j]==i) {
            char buffer [100];
            int n = sprintf(buffer, "%.1f", scan.peak_heights[j]/smallest);
            logfile << buffer;
            if (i<scan.n_peaks-1) logfile << ": ";
         }
      }
   }
   logfile << endl << "The mean molecular mass is " << scan.mass << " amu";
   Double_t conductance = coeff*trans_prob*1./(area*sqrt(scan.mass)); //assuming 293 Kelvin, hole diamter = 5mm
   Double_t outgassing = conductance * pressure;
   logfile << endl << "The system pressure is " << pressure << " Torr" << endl;
   logfile << "The average outgassing is " << outgassing << " Torr*l/s"<< endl;
}
