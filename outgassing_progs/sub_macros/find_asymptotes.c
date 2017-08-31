// #include "RConfigure.h"
// #include "TMath.h"
// #include "TError.h"
// #include "TLatex.h"
// #include <math.h>
// #include <stdio.h>
// #include <sstream>
// #include <cstring>
// #include <string>
// #include <iostream>
// #include <TString>
#include "settings.h"

void find_asymptotes(TString fname, long int measurement_time[], Double_t measurement_pressure[],
Double_t fiterr[], int& n_measurements, int& t_0)
{
if (material == "ptfe") {
   time_bound_low = 500;
   time_bound_high = 50;
   p_bound_low = .8;
   p_bound_high = 1.3;
}
else if (material == "rg178" || material == "mylar" || material == "background") {
   time_bound_low = 5000;
   time_bound_high = 50;
   p_bound_low = 1;
}
else if (material == "al_rings") {
   time_bound_low = 2000;
   time_bound_high = 500;
   p_bound_low = 1;
}
TString dirname;
if (new_data == 1) dirname = "pressure_data/";
else dirname = "pressure_data_old/";
TString filename = file_header + dirname + fname;
////  File Read   ////
infile1.open (filename,ifstream::in);
if (infile1.is_open()) {
  for (int i=0; i<(maxbins-1); i++) {
    while (getline (infile1,params)) {
      sscanf(params.c_str(), "%li %le", &ftime, &fpressure);
      time[i] = ftime;
      pressure[i] = fpressure;
      npts++;
      if (i==0) t_0 = time[i];
      i++;
    }
  }
}
else cout << "unable to open file"<<endl;
infile1.close();
// t_0 = 1405971300;   // stov_1_initial
// t_0 = 1408501020; //stov_4_initial

Double_t goodness;
////  Find Measurements ////
int n_measurements = 0;
int skipped = 0;

for(int i = 0;i<npts;i++)
{
  if (i<2 || i==npts-1) continue;
  if (pressure[i]-pressure[i-1]>5e-6) continue;  /// sometimes needs to be changed
  if (time[i+1]-time[i]>500)
  {
     if (skipped < skip_measurements_number) {
       if (n_measurements+1 == skip_measurements[skipped]-skipped) {
         skipped++;
         continue;
       }
     }
     measurement_time[n_measurements] = time[i];
     // Determine Fit Range //
     Double_t dist=0;
     int k;
     for (int j=1;j<i;j++) {
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
     fitfunc->SetParLimits(0,p_bound_low*pressure[i],p_bound_high*pressure[i]);  // change low to .8p if big err
     fitfunc->SetParLimits(1,1e-4,5e-2);
     fitfunc->SetParLimits(2,time[k]-time_bound_low,time[k]-50);
     TGraphErrors * g = new TGraphErrors();
     for (int j=k;j<i+1;j++) {
        g->SetPoint(j-k,time[j],pressure[j]);
        g->SetPointError(j-k,30.,.05*pressure[j]);
     }
     cerr.setstate(ios::failbit);
     g->Fit(fitfunc,"mq");
     cerr.clear();
     goodness = fitfunc->GetChisquare()/fitfunc->GetNDF();
     measurement_pressure[n_measurements] = fitfunc->GetParameter(0);
     if (goodness < 1e-7 || goodness>2.) {
      fiterr[n_measurements] = .05;
      cout<<"Bad Fit "<<n_measurements+1<<" - Using Last Pressure Reading."<<endl;
      measurement_pressure[n_measurements] = pressure[i];
     }
     else fiterr[n_measurements] = (fitfunc->GetParError(0))/measurement_pressure[n_measurements];
     if (material == "ptfe" && i-k > 30) {
       measurement_pressure[n_measurements] = pressure[i];
       fiterr[n_measurements] = .03;
       cout << "Using Last Pressure Reading. " << endl;
     }
//      cout<<fiterr[n_measurements]<<"\t"<<goodness<<endl;
     measurement_pressure[n_measurements] = pressure[i];
     fiterr[n_measurements] = 0;
     n_measurements++;
//      if (n_measurements==1) break;
  }
}
// g->Draw("ap");
}
