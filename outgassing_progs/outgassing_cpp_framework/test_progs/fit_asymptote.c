#include "RConfigure.h"
#include "TMath.h"
#include "TError.h"
#include "TLatex.h"
#include <math.h>
#include <stdio.h>
#include <sstream>
#include "settings.h"

void fit_asymptote()
{
TString fname = "20140714_mylar_initial.dat";
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
infile1.open (file_header + "pressure_data/"+ fname,ifstream::in);
//infile1.open ("test",ifstream::in);

if (infile1.is_open()) {
cout <<"scanning file"<<endl;
  for (int i=0; i<(maxbins-1); i++) {
    while (getline (infile1,params)) {
      sscanf(params.c_str(), "%li %le", &ftime, &fpressure);
      time[i] = ftime;
      pressure[i] = fpressure;
      //cout<<"System time: "<<time[i]<<" Pressure [torr]: "<<pressure[i]<<endl;
      npts++;
      i++;
     }
   }
 }
else cout << "unable to open file"<<endl;

infile1.close();

cout<<"Total number of points: "<<npts<<endl;

TGraph * g = new TGraph();

for(int i = 0;i<npts;i++)
{
  g->SetPoint(i,time[i],pressure[i]);
}

g->SetMarkerSize(.2);
g->SetMarkerStyle(8);
//g->SetMinimum(0.00000001);

// g->SetTitle("Acrylic, initial pump down");
// g->SetTitle("Outgassing System Pressure");
g->GetXaxis()->CenterTitle(true);
g->GetXaxis()->SetTimeDisplay(1);
g->GetXaxis()->SetLabelOffset(0.03);
g->GetXaxis()->SetLabelSize(0.03);
g->GetXaxis()->SetTimeFormat("#splitline{%y\/%m\/%d}{%H:%M}%F1970-01-01 00:00:00");

g->GetYaxis()->SetTitle("Pressure [Torr]");
g->GetYaxis()->CenterTitle(true);
// g->SetMarkerStyle(2);
// g->SetMarkerSize(.3);
g->Draw("ap");
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
