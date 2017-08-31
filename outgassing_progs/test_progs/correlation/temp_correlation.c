#include "RConfigure.h"
#include "TMath.h"
#include "TError.h"
#include "TLatex.h"
#include <math.h>
#include <stdio.h>
#include <sstream>
#include "/Users/josephhowlett/myroot/outgassing_analysis/outgassing_progs/sub_macros/settings.h"

void temp_correlation()
{
TString fname1 = "20140714_baseline.dat";
TString fname2 = "just_temp.dat";
//gROOT->Macro("style.C");
//gErrorIgnoreLevel = 2001;


const int maxbins = 1000000;

infile1.close();

string params2;
ifstream infile2;
infile2.open (file_header + "pressure_data/"+ fname2,ifstream::in);

int npts_temp=0;
Double_t temp [maxbins], ftemp;
long int time_temp [maxbins], ftime, t_0, t_1;
ftime = 0;

if (infile2.is_open()) {
cout <<"scanning file"<<endl;
  for (int i=0; i<(maxbins-1); i++) {
    while (getline (infile2,params2)) {
      sscanf(params2.c_str(), "%li %le", &ftime, &ftemp);
//       if (i==0) t_0 = ftime;
      time_temp[i] = ftime;
      temp[i] = ftemp;
      npts_temp++;
      i++;
     }
   }
 }
else cout << "unable to open file"<<endl;
cout<<time_temp[0]<<"\t"<<time_temp[npts_temp-1]<<"\t"<<temp[0]<<endl;
infile2.close();
int npts=0;
Double_t pressure [maxbins], fpressure;
long int time [maxbins], ftime;
ftime = 0;

string params;
ifstream infile1;
infile1.open (file_header + "pressure_data/"+ fname1,ifstream::in);

if (infile1.is_open()) {
cout <<"scanning file"<<endl;
  for (int i=0; i<(maxbins-1); i++) {
    while (getline (infile1,params)) {
      sscanf(params.c_str(), "%li %le", &ftime, &fpressure);
      if (ftime<time_temp[0]-60) continue;
      else if (ftime>time_temp[npts_temp-1]) continue;
      else {
         time[i] = ftime;
         pressure[i] = fpressure;
         npts++;
         i++;
      }
     }
   }
 }
 
 infile1.close();
 cout<<npts_temp<<endl<<npts<<endl;
TGraph * g1 = new TGraph();
TGraph * g2 = new TGraph();
TLegend * leg = new TLegend(0.8667342,0.845666,0.9976874,0.9936575);
g1->SetMarkerColor(2);
g1->SetMarkerStyle(8);
g1->SetMarkerSize(.2);
g2->SetMarkerSize(.2);
g2->SetMarkerColor(4);
g2->SetMarkerStyle(8);
for (int i=0;i<npts;i++) g1->SetPoint(i, time[i], pressure[i]);
for (int i=0;i<npts_temp;i++) g2->SetPoint(i, time_temp[i], 1e-8*temp[i]);
TCanvas *c1 = new TCanvas("c1", "c1",650,40,800,500);
c1->SetHighLightColor(10);
c1->SetGrid() ;
c1->SetFillColor(10);
c1->SetBottomMargin(0.15);
c1->SetLeftMargin(0.15);
// c1->SetLogy();
c1->cd();
g1->GetXaxis()->SetTimeDisplay(1);
g1->GetXaxis()->SetLabelOffset(0.03);
g1->GetXaxis()->SetLabelSize(0.03);
// g1->GetYaxis()->SetLabelOffset(999);
// g1->GetYaxis()->SetLabelSize(0);
g1->GetXaxis()->SetTimeFormat("#splitline{%y\/%m\/%d}{%H:%M}%F1970-01-01 00:00:00");
g2->GetXaxis()->SetTimeFormat("#splitline{%y\/%m\/%d}{%H:%M}%F1970-01-01 00:00:00");

g1->Draw("ap");
g2->Draw("p same");
leg->AddEntry(g1, "Pressure [Torr]","p");
leg->AddEntry(g2, "Temperature ","p");
leg->SetFillColor(10);
leg->Draw();
// const int data_len = npts;
// Double_t temp_dat [data_len];
// ofstream logfile;
// logfile.open("temp_log.dat");
// for (int i=0;i<npts;i++) {
//    temp_dat[i] = temp[i];
//    char buffer [100];
//    int n = sprintf(buffer, "%.2f\t%.2e", temp_dat[i], pressure[i]);
//    logfile << buffer << endl;
// }


}
