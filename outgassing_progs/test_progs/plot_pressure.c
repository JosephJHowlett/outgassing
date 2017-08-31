#include "RConfigure.h"
#include "TMath.h"
#include "TError.h"
#include "TLatex.h"
#include <math.h>
#include <stdio.h>
#include <sstream>
#include "sub_macros/settings.h"

void plot_pressure(TString fname = "", int dateaxis = 1)
{
if (fname == "") TString fname = "20140725_ptfe.dat";
//gROOT->Macro("style.C");
//gErrorIgnoreLevel = 2001;

TCanvas *c1 = new TCanvas("c1", "c1",650,40,800,500);
c1->SetHighLightColor(10);
c1->SetGrid() ;
c1->SetFillColor(10);
c1->SetBottomMargin(0.15);
c1->SetLeftMargin(0.15);
c1->SetLogy();

const int maxbins = 1000000;
int npts=0;
Double_t pressure [maxbins], fpressure;
long int time [maxbins], ftime, t_0, t_1;
ftime = 0;

string params;
ifstream infile1;
const char* full_fn = file_header + "pressure_data/"+ fname;
infile1.open (full_fn ,ifstream::in);
//infile1.open ("test",ifstream::in);

if (infile1.is_open()) {
cout <<"scanning file"<<endl;
  for (int i=0; i<(maxbins-1); i++) {
    while (getline (infile1,params)) {
      sscanf(params.c_str(), "%li %le", &ftime, &fpressure);
      if (i==0) t_0 = ftime;
      time[i] = ftime;
      pressure[i] = fpressure;
      //cout<<"System time: "<<time[i]<<" Pressure [torr]: "<<pressure[i]<<endl;
      npts++;
      i++;
     }
   }
 }
else cout << "unable to open file " << full_fn <<endl;

infile1.close();

cout<<"Total number of points: "<<npts<<endl;
cout<< "T0 = "<<t_0<<endl;
TGraph * g = new TGraph();

for(int i = 0;i<npts;i++)
{
 if (dateaxis == 1)
 {
  g->SetPoint(i,time[i],pressure[i]);
 }
 else
 {
  g->SetPoint(i,double(time[i]-t_0)/3600.0,pressure[i]);
 }
}

g->SetMarkerSize(.2);
g->SetMarkerStyle(8);
g->SetMarkerColor(1);
g->SetLineColor(1);
//g->SetMinimum(0.00000001);

// g->SetTitle("Outgassing System Pressure");



g->GetYaxis()->SetTitle("Pressure [Torr]");
g->GetXaxis()->SetTitle("Time [Hours]");
g->GetYaxis()->CenterTitle(true);
g->GetXaxis()->CenterTitle(true);
// g->SetMarkerStyle(2);
// g->SetMarkerSize(.3);
g->Draw("ap");
//c1->Print("test.C");

if (dateaxis==1)
{
  g->GetXaxis()->SetTimeDisplay(1);
  g->GetXaxis()->SetTitleOffset(2);
  g->GetXaxis()->SetLabelOffset(0.03);
  g->GetXaxis()->SetLabelSize(0.03);
  g->GetXaxis()->SetTimeFormat("#splitline{%y\/%m\/%d}{%H:%M}%F1970-01-01 00:00:00");
  g->GetXaxis()->SetTitle("Time");
}

//tex = new TLatex(0.2,1.75,"Preliminary");
//tex->SetTextColor(17);
//tex->SetTextSize(0.1991525);
//tex->SetTextAngle(26.15998);
//tex->SetLineWidth(2);
//tex->Draw("same");
//g->Draw("p");
//gPad->RedrawAxis();       


}
