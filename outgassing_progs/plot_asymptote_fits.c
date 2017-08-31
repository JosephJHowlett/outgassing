#include "RConfigure.h"
#include "TMath.h"
#include "TError.h"
#include "TLatex.h"
#include <math.h>
#include <stdio.h>
#include <sstream>
#include <cstring>
#include <string>
#include <iostream>
#include <TString>
#include "/Users/josephhowlett/myroot/outgassing_analysis/outgassing_progs/sub_macros/settings.h"
maxbins = 10000;

void plot_asymptote_fits(TString fname = "", int point=10)
{
gErrorIgnoreLevel = kError;
if (material == "ptfe") {
   time_bound_low = 500;
   time_bound_high = 50;
   p_bound_low = .8;
//    p_bound_high = 1.3;
}
else if (material == "rg178" || material == "mylar" || material == "background") {
   time_bound_low = 8000;
   time_bound_high = 50;
   p_bound_low = 1;
}
else if (material == "al_rings") {
   time_bound_low = 2000;
   time_bound_high = 500;
   p_bound_low = 1;
}

// p_bound_low = .6;
// time_bound_low = 400;
// time_bound_high = 0;
// fname = "20140815_ptfe.dat";
TString dirname = "pressure_data/";
TString filename = file_header + dirname + fname;
long int measurement_time [maxbins];
Double_t measurement_pressure [maxbins];
int n_measurements, t_0;
////  File Read   ////
infile1.open (filename,ifstream::in);
if (infile1.is_open()) {
  for (int i=0; i<(maxbins-1); i++) {
    while (getline (infile1,params)) {
      sscanf(params.c_str(), "%li %le", &ftime, &fpressure);
      if (i==0) t_0 = ftime;
      time[i] = (ftime-t_0);
      pressure[i] = fpressure;
      npts++;
      i++;
     }
   }
 }
else cout << "unable to open file"<<endl;
infile1.close();

////  Find Measurements ////
int n_measurements = 0;
int skipped = 0;
for(int i = 0;i<npts;i++)
{
  if (i<2 || i==npts-2) continue;
  if (pressure[i]-pressure[i-1]>5e-6) continue;
//   if ((pressure[i]>pressure[i-7]) && ( (time[i+1]-time[i]>200) ))
  if (time[i+1]-time[i]>200)
  {
     if (n_measurements == skip_measurement-1 && skipped == 0) {  // override in case non-measurement gets caught
       skipped++;
       continue;
     }
     measurement_time[n_measurements] = time[i];
     // Determine Fit Range //
     Double_t dist=0;
     int k;
     for (int j=1;j<i;j++) {
        if (pressure[j]>1e-3) continue;
        if (n_measurements>0) {
           if (time[j] < measurement_time[n_measurements-1]) continue;
        }
        if (pressure[j] < pressure[i] && (pressure[j] - pressure[j-1]>dist)) {
           if (i-j<50) {
              dist = pressure[j] - pressure[j-1];
              k = j;
           }
        }
     }
     TF1 * fitfunc = new TF1("asympt","[0]*(1-exp(-[1]*(x-[2])))");
     
     fitfunc->SetLineColor(2);
     fitfunc->SetLineStyle(4);
     fitfunc->SetParLimits(0,p_bound_low*pressure[i],p_bound_high*pressure[i]);
     fitfunc->SetParLimits(1,1e-4,5e-2);
     fitfunc->SetParLimits(2,time[k]-time_bound_low,time[k]-time_bound_high);
     TGraphErrors * g = new TGraphErrors();
     for (int j=k;j<i+1;j++) {
        g->SetPoint(j-k,time[j],pressure[j]);
        g->SetPointError(j-k,30.,.05*pressure[j]);
     }
     cerr.setstate(ios::failbit);
     g->Fit(fitfunc,"m");
     cerr.clear();
     Double_t goodness = fitfunc->GetChisquare()/fitfunc->GetNDF();
     Double_t percerr = fitfunc->GetParError(0)/fitfunc->GetParameter(0);
     // find time to wait to reach 99% of asymptote value
     Double_t p1 = fitfunc->GetParameter(1);
     Double_t p2 = fitfunc->GetParameter(2);
     Double_t thresh = .85;
     Double_t time_to_wait_sec = p2 - (TMath::Log(1-thresh)/p1) - time[k];
     int time_to_wait_min = (int)((time_to_wait_sec/60)+2.5);
     cout << "Wait Time = " << time_to_wait_min << " minutes" << endl;
     
     if (percerr<.18) measurement_pressure[n_measurements] = fitfunc->GetParameter(0);
     else {
        measurement_pressure[n_measurements] = pressure[i];
        cout << "Bad Fit - Taking Last Pressure Measured"<<endl;
     }
     if (material == "ptfe") measurement_pressure[n_measurements] = pressure[i];
     
     cout << percerr <<endl;
     cout << goodness << endl;
     n_measurements++;
     cout<<n_measurements<<endl;
     cout << "Equilibrium Pressure = " << fitfunc->GetParameter(0) << "± " << fitfunc->GetParError(0) << endl;
     if (n_measurements==point) break;
  }

}
  g->Draw("ap");
// for mylar, rg178 -> p1 [1e-4, 1e-3], p2 [t-5000, t-100]
// for ptfe -> p1 [1e-4, 5e-2], p2 [t-500, t-50]
}
