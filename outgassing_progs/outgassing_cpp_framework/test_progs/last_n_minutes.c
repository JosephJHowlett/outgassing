#include "RConfigure.h"
#include "TMath.h"
#include "TError.h"
#include "TLatex.h"
#include <math.h>
#include <stdio.h>
#include <sstream>
#include "../sub_macros/settings.h"
TString command = "scp jhowlett@bellini.astronevis.columbia.edu:/home/jhowlett/log_daemon/logd.log \
                    /Users/josephhowlett/myroot/outgassing_analysis/pressure_data/ > /dev/null";
TString command = "scp -P 7920 outgas@129.236.254.37:/home/outgas/log_daemon/logd.log \
                    /Users/josephhowlett/myroot/outgassing_analysis/pressure_data/ > /dev/null";
                    

void last_n_minutes(const int n_minutes = 20) 
{
long int ftime;
Double_t fpressure;
int inf=0, n=0;
string params;
ifstream infile1;
system(command);
infile1.open (file_header+"pressure_data/logd.log",ifstream::in);
// infile1.open (file_header + "pressure_data/20140709.dat",ifstream::in);
if (infile1.is_open()) {
  for (int i=0; i<(maxbins-1); i++) {
    while (getline (infile1,params)) {
      sscanf(params.c_str(), "%li %le", &ftime, &fpressure);
      npts++;
      i++;
     }
   }
 }
else cout << "unable to open file"<<endl;
infile1.close();

long int time [n_minutes];
Double_t pressure [n_minutes];
infile1.open (file_header+"pressure_data/logd.log",ifstream::in);
// infile1.open (file_header + "pressure_data/20140709.dat",ifstream::in);
if (infile1.is_open()) {
  for (int i=0; i<(maxbins-1); i++) {
    while (getline (infile1,params)) {
      sscanf(params.c_str(), "%li %le", &ftime, &fpressure);
      if (i<npts-n_minutes) {
         i++;
         continue;
      }
      time[n] = ftime;
      pressure[n] = fpressure;
      i++;
      n++;
    }
  }
}
else cout << "unable to open file"<<endl;
infile1.close();
TCanvas *c1 = new TCanvas("c1", "c1",650,40,800,500);
c1->SetHighLightColor(10);
c1->SetGrid() ;
c1->SetFillColor(10);
c1->SetBottomMargin(0.15);
c1->SetLeftMargin(0.15);
// c1->SetLogy();
TGraph *g = new TGraph();

for (int i =0; i<n; i++) g->SetPoint(i, time[i], pressure[i]);
g->SetMarkerStyle(20);
g->SetMarkerSize(.2);
g->GetXaxis()->CenterTitle(true);
g->GetXaxis()->SetTimeDisplay(1);
g->GetXaxis()->SetLabelOffset(0.03);
g->GetXaxis()->SetLabelSize(0.03);
g->GetXaxis()->SetTimeFormat("#splitline{%y\/%m\/%d}{%H:%M}%F1970-01-01 00:00:00");
g->GetYaxis()->SetTitle("Pressure [Torr]");
g->GetYaxis()->CenterTitle(true);
g->Draw("ap");
}