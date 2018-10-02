#include "RConfigure.h"
#include "TMath.h"
#include "TError.h"
#include "TLatex.h"
#include <math.h>
#include <stdio.h>
#include <sstream>


TCanvas *c1 = new TCanvas("c1", "c1",650,40,800,500);
c1->SetHighLightColor(10);
c1->SetGrid() ;
c1->SetFillColor(10);
c1->SetBottomMargin(0.15);
c1->SetLeftMargin(0.15);
c1->SetLogy();

const int maxbins = 1000000;
int npts=0;
Double_t pressure_old [maxbins], error_old [maxbins], fpressure_old, ferror_old;
Double_t pressure_new [maxbins], error_new [maxbins], fpressure_new, ferror_new;
Double_t pressure_diff [maxbins], error_diff [maxbins];

string params;
ifstream infile1;

void plot_gauge_offset_pts ()
{
infile1.open ("offset_points.dat",ifstream::in);
//infile1.open ("test",ifstream::in);

if (infile1.is_open()) {
cout <<"scanning file"<<endl;
  for (int i=0; i<(maxbins-1); i++) {
    while (getline (infile1,params)) {
      if (i==0) {i++; continue;}
      sscanf(params.c_str(), "%le %le %le %le", &fpressure_old, &fpressure_new, &ferror_old, &ferror_new);
      pressure_old[npts] = fpressure_old;
      pressure_new[npts] = fpressure_new;
      error_old[npts] = ferror_old;
      error_new[npts] = ferror_new;
      pressure_diff[npts] = (fpressure_old - fpressure_new);///ferror_new;
      error_diff[npts] = TMath::Sqrt((ferror_old)**2 + (ferror_new)**2);
//       error_diff[npts] = TMath::Sqrt((ferror_old/fpressure_old)**2 + (ferror_new/fpressure_new))*pressure_diff[npts];
      //cout<<"System time: "<<time[i]<<" Pressure [torr]: "<<pressure[i]<<endl;
      npts++;
      i++;
     }
   }
 }
else cout << "unable to open file"<<endl;
cout<<"Npts = " << npts << endl;
TGraphErrors * g1 = new TGraphErrors();
for (int i=0; i<npts; i++) 
{
  g1->SetPoint(i, pressure_new[i], pressure_diff[i]);
  g1->SetPointError(i, 0, error_diff[i]);
}
g1->SetMarkerStyle(8);
g1->SetMarkerSize(.5);
g1->Draw("ap");
g1->GetYaxis()->SetTitle("Pressure Offset [Torr]");
g1->GetXaxis()->SetTitle("New Gauge Pressure [Torr]");


infile1.close();

}


