#include "RConfigure.h"
#include "TMath.h"
#include "TError.h"
#include "TLatex.h"
#include <math.h>
#include <stdio.h>
#include <sstream>
#include "../sub_macros/settings.h"

void plot_pressure_temp()
{
TString fname1 = "entire.dat";
TString fname2 = "bay_temp.dat";
//gROOT->Macro("style.C");
//gErrorIgnoreLevel = 2001;

TCanvas *c1 = new TCanvas("c1", "c1",650,40,800,500);
c1->SetHighLightColor(10);
c1->SetGrid() ;
c1->SetFillColor(10);
c1->SetBottomMargin(0.15);
c1->SetLeftMargin(0.15);
// c1->SetLogy();

const int maxbins = 1000000;
int npts=0;
Double_t pressure [maxbins], fpressure;
long int time [maxbins], ftime, t_0, t_1;
ftime = 0;

string params;
ifstream infile1;
infile1.open (file_header + "pressure_data/"+ fname1,ifstream::in);

if (infile1.is_open()) {
cout <<"scanning file"<<endl;
  for (int i=0; i<(maxbins-1); i++) {
    while (getline (infile1,params)) {
      sscanf(params.c_str(), "%li %le", &ftime, &fpressure);
      time[i] = ftime;
      pressure[i] = fpressure;
      npts++;
      i++;
     }
   }
 }
else cout << "unable to open file"<<endl;

infile1.close();

string params2;
ifstream infile2;
infile2.open (file_header + "pressure_data/"+ fname2,ifstream::in);

int npts_temp=0;
Double_t temp [maxbins], ftemp;
long int time_temp [maxbins], ftime;
ftime = 0;

if (infile2.is_open()) {
cout <<"scanning file"<<endl;
  for (int i=0; i<(maxbins-1); i++) {
    while (getline (infile2,params2)) {
      sscanf(params2.c_str(), "%li %le", &ftime, &ftemp);
      time_temp[i] = ftime;
      temp[i] = ftemp;
      npts_temp++;
      i++;
     }
   }
 }
else cout << "unable to open file"<<endl;

infile2.close();



cout<<"Total number of points: "<<npts<<endl<<npts_temp<<endl;
TMultiGraph *mg = new TMultiGraph();
TLegend *leg = new TLegend(0.7701005,0.8393235,0.951005,0.9513742);
TGraph * g = new TGraph();
TGraph * g2 = new TGraph();

for(int i = 0;i<npts;i++)
{
  g->SetPoint(i,time[i],pressure[i]);
}

for (int i=0;i<npts_temp;i++)
{
  g2->SetPoint(i, time_temp[i], 10e-09*temp[i]);
}

g->SetMarkerSize(.2);
g->SetMarkerStyle(8);
g2->SetMarkerSize(.2);
g2->SetMarkerStyle(8);
g->SetMarkerColor(4);
g2->SetMarkerColor(2);
//g->SetMinimum(0.00000001);

// g->SetTitle("Acrylic, initial pump down");
// g->SetTitle("Outgassing System Pressure");


// g->GetYaxis()->SetTitle("Pressure [Torr]");
g->GetYaxis()->CenterTitle(true);
// g->SetMarkerStyle(2);
// g->SetMarkerSize(.3);

mg->Add(g);
mg->Add(g2);
mg->Draw("ap");
leg->AddEntry(g,"Pressure","p");
leg->AddEntry(g2,"Temperature","p");
leg->Draw();
mg->GetXaxis()->CenterTitle(true);
mg->GetXaxis()->SetTimeDisplay(1);
mg->GetXaxis()->SetLabelOffset(0.03);
mg->GetXaxis()->SetLabelSize(0.03);
mg->GetXaxis()->SetTimeFormat("#splitline{%y\/%m\/%d}{%H:%M}%F1970-01-01 00:00:00");

//c1->Print("test.C");

//tex = new TLatex(0.2,1.75,"Preliminary");
//tex->SetTextColor(17);
//tex->SetTextSize(0.1991525);
//tex->SetTextAngle(26.15998);
//tex->SetLineWidth(2);
//tex->Draw("same");
//g->Draw("p");
//gPad->RedrawAxis();       


}
