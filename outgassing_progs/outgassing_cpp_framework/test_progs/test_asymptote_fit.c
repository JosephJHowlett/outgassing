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
#include "/Users/josephhowlett/myroot/outgassing_analysis/outgassing_progs/settings.h"


void test_asymptote_fit()
{

TCanvas *c1 = new TCanvas("c1", "c1",650,40,800,500);
c1->SetHighLightColor(10);
c1->SetGrid() ;
c1->SetFillColor(10);
c1->SetBottomMargin(0.15);
c1->SetLeftMargin(0.15);
// gStyle->SetOptFit(1);
// c1->SetLogy();

TString fname = "20140714_mylar_initial.dat";
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
      time[i] = ftime-t_0;
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
for(int i = 0;i<npts;i++)
{
  if (i<200 || i==npts-1) continue;
  if (pressure[i]-pressure[i-1]>5e-7) continue;
  if ((pressure[i]>pressure[i-7]) && ( (time[i+1]-time[i]>200) ))
  {
     measurement_time[n_measurements] = time[i];
     measurement_pressure[n_measurements] = pressure[i];
     
     // Determine Fit Range //
     Double_t dist=0;
     int k;
     int last_pt;
     for (int j=0;j<i;j++) {
        if (n_measurements>0) {
           if (time[j] < measurement_time[n_measurements-1]) continue;
        }
        if (pressure[j] < pressure[i] && (pressure[j] - pressure[j-1]>dist)) {
           if (i-j<40) {
              dist = pressure[j] - pressure[j-1];
              k = j;
              last_pt = i;
           }
        }
     }
     n_measurements++;
     if (n_measurements==6) break;
  }
}
  
TGraphErrors * g = new TGraphErrors();
for (int i=k;i<last_pt+1;i++) {
   g->SetPoint(i-k,time[i],pressure[i]);
   g->SetPointError(i-k,30.,.05*pressure[i]);
}
// for(int i = 0;i<npts-k;i++)
// {
//   g->SetPoint(i,time[i],pressure[i]);
// }
TF1 * fitfunc = new TF1("asympt","[0]*(1-exp(-[1]*(x-[2])))");
fitfunc->SetLineColor(2);
fitfunc->SetLineStyle(4);
fitfunc->SetParLimits(0,pressure[last_pt],1.5*pressure[last_pt]);           //(0,4.2e-6,4.7e-6);
fitfunc->SetParLimits(1,1e-4,5e-2);           //(1,7e-4,9e-4);
if (material == "ptfe") time_bound_low = 500;
else if (material == "rg178" || material == "mylar") time_bound_low = 5000;
fitfunc->SetParLimits(2,time[k]-time_bound_low,time[k]-50);            //(2,254000,254500);
cout<<time_bound_low<<endl;
// for mylar, rg178 -> p1 [1e-4, 1e-3], p2 [t-5000, t-100]
// for ptfe -> p1 [1e-4, 5e-2], p2 [t-500, t-50]



cerr.setstate(ios::failbit);
g->Fit(fitfunc,"m");
cerr.clear();

Double_t chi = fitfunc->GetChisquare();
cout << chi<<endl;
Double_t asymptote = fitfunc->GetParameter(0);
g->SetMarkerSize(.2);
g->SetMarkerStyle(8);
//g->SetMinimum(0.00000001);
g->GetXaxis()->CenterTitle(true);
// g->GetXaxis()->SetTimeDisplay(1);
g->GetXaxis()->SetLabelOffset(0.03);
g->GetXaxis()->SetLabelSize(0.03);
// g->GetXaxis()->SetTimeFormat("#splitline{%y\/%m\/%d}{%H:%M}%F1970-01-01 00:00:00");

g->GetYaxis()->SetTitle("Pressure [Torr]");
g->GetYaxis()->CenterTitle(true);
// g->SetMarkerStyle(2);
// g->SetMarkerSize(.3);
g->Draw("ap");
// ideal: measurement_pressure[n_measurements] = fit_asymptote(measurement_time[n_measurements]);

}
