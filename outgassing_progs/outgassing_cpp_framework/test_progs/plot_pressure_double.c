#include "RConfigure.h"
#include "TMath.h"
#include "TError.h"
#include "TLatex.h"
#include <math.h>
#include <stdio.h>
#include <sstream>
#include "/Users/josephhowlett/myroot/outgassing_analysis/outgassing_progs/sub_macros/settings.h"

void plot_pressure_double()
{
TString fname1 = "20150128.dat";
TString fname2 = "20150430.dat";
//gROOT->Macro("style.C");
//gErrorIgnoreLevel = 2001;
TMultiGraph * mg = new TMultiGraph();

TCanvas *c1 = new TCanvas("c1", "c1",650,40,800,500);
c1->SetHighLightColor(10);
c1->SetGrid() ;
c1->SetFillColor(10);
c1->SetBottomMargin(0.15);
c1->SetLeftMargin(0.15);
c1->SetLogy();
// c1->SetLogx();

const int maxbins = 1000000;
int npts1=0, npts2=0;
Double_t pressure1 [maxbins], pressure2 [maxbins], fpressure;
Double_t time1 [maxbins], time2 [maxbins];
long int ftime, t_1, t_2;
ftime = 0;

string params1;
ifstream infile1;
infile1.open (file_header + "pressure_data/"+ fname1,ifstream::in);
//infile1.open ("test",ifstream::in);

if (infile1.is_open()) {
cout <<"scanning file"<<endl;
  for (int i=0; i<(maxbins-1); i++) {
    while (getline (infile1,params1)) {
      sscanf(params1.c_str(), "%li %le", &ftime, &fpressure);
      if (i==0) t_1 = ftime;
      time1[i] = double(ftime-t_1)/3600.;
      pressure1[i] = fpressure;
      //cout<<"System time: "<<time[i]<<" Pressure [torr]: "<<pressure[i]<<endl;
      npts1++;
      i++;
     }
   }
 }
else cout << "unable to open file"<<endl;

infile1.close();


string params2;
ifstream infile2;
infile2.open (file_header + "pressure_data/"+ fname2,ifstream::in);
//infile1.open ("test",ifstream::in);

if (infile2.is_open()) {
cout <<"scanning file"<<endl;
  for (int i=0; i<(maxbins-1); i++) {
    while (getline (infile2,params2)) {
      sscanf(params2.c_str(), "%li %le", &ftime, &fpressure);
      if (i==0) t_2 = ftime;
      time2[i] = double(ftime-t_2)/3600.;
      pressure2[i] = fpressure;
      //cout<<"System time: "<<time[i]<<" Pressure [torr]: "<<pressure[i]<<endl;
      npts2++;
      i++;
     }
   }
 }
else cout << "unable to open file"<<endl;

infile2.close();

//t_2 = 1408501020;

TGraph * g1 = new TGraph();
TGraph * g2 = new TGraph();
TGraph * gdiff = new TGraph();

for(int i = 0;i<npts1;i++) g1->SetPoint(i,time1[i],pressure1[i]);
for (int i=0;i<npts2;i++) g2->SetPoint(i,time2[i],pressure2[i]);
g2->SetPoint(npts2+1, 16.5, 1.22e-4);
// for (int i=0;i<npts1;i++) 
// {
//   if (pressure2[i] - pressure1[i] > 0)
//   {
//   gdiff->SetPoint(i, pressure2[i], pressure2[i]-pressure1[i]);
//   }
// }
g1->SetMarkerColor(2);
g1->SetLineColor(2);
g2->SetMarkerColor(4);
g2->SetLineColor(4);
// gdiff->SetMarkerColor(1);
// gdiff->SetMarkerSize(.2);
// gdiff->SetMarkerStyle(8);

g1->SetMarkerSize(.2);
g1->SetMarkerStyle(8);
g2->SetMarkerSize(.2);
g2->SetMarkerStyle(8);
//g->SetMinimum(0.00000001);

// g->SetTitle("Acrylic, initial pump down");
// g->SetTitle("Outgassing System Pressure");
g1->GetXaxis()->CenterTitle(true);
// g1->GetXaxis()->SetTimeDisplay(1);
// g1->GetXaxis()->SetLabelOffset(0.03);
g1->GetXaxis()->SetLabelSize(0.03);
// g1->GetXaxis()->SetTimeFormat("#splitline{%y\/%m\/%d}{%H:%M}%F1970-01-01 00:00:00");

g1->GetYaxis()->SetTitle("Pressure [Torr]");
g1->GetYaxis()->CenterTitle(true);
g1->GetXaxis()->SetTitle("Time [Hours]");
g1->GetXaxis()->CenterTitle(true);
// g->SetMarkerStyle(2);
// g->SetMarkerSize(.3);
//g->Print("test.C");
// mg->Add(g1);
// mg->Add(g2);
// mg->Add(gdiff);
// mg->Draw("ap");
g1->Draw("ap");
g2->Draw("psame");

//tex = new TLatex(0.2,1.75,"Preliminary");
//tex->SetTextColor(17);
//tex->SetTextSize(0.1991525);
//tex->SetTextAngle(26.15998);
//tex->SetLineWidth(2);
//tex->Draw("same");
//g->Draw("p");
//gPad->RedrawAxis();       

// cout << npts1 << '\t' << npts2 << endl;
}
