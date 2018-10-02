#include "RConfigure.h"
#include "TMath.h"
#include "TError.h"
#include "TLatex.h"
#include <math.h>
#include <stdio.h>
#include <sstream>
#include "../sub_macros/settings.h"
#include <ctime>

int CLOCKS_PER_SEC = 1000000;
void breakTime( int seconds)
{
    clock_t temp;
    temp = clock () + seconds * CLOCKS_PER_SEC ;
    while (clock() < temp) {}
}

void continuous_plot_pressure() 
{
long int time, ftime;
Double_t pressure, fpressure;
TCanvas *c1 = new TCanvas("c1", "c1",650,40,800,500);
c1->SetHighLightColor(10);
c1->SetGrid() ;
c1->SetFillColor(10);
c1->SetBottomMargin(0.15);
c1->SetLeftMargin(0.15);
// c1->SetLogy();
c1->cd();
TGraph *g = new TGraph();
g->SetMarkerStyle(3);
g->SetMarkerSize(2);
int inf=0, n=0;
while (inf<1) {
   string params;
   ifstream infile1;
   system("scp jhowlett@bellini.astronevis.columbia.edu:/home/jhowlett/log_daemon/logd.log . > /dev/null");
   infile1.open ("logd.log",ifstream::in);
   // infile1.open (file_header + "pressure_data/20140709.dat",ifstream::in);
   if (infile1.is_open()) {
     for (int i=0; i<(maxbins-1); i++) {
       while (getline (infile1,params)) {
         sscanf(params.c_str(), "%li %le", &ftime, &fpressure);
         //cout<<"System time: "<<time[i]<<" Pressure [torr]: "<<pressure[i]<<endl;
         npts++;
         i++;
        }
      }
    }
   else cout << "unable to open file"<<endl;
   infile1.close();
   g->SetPoint(n, ftime, fpressure);
   system("rm logd.log");
   if (n==0) g->Draw("ap");
   c1->Modified();
   c1->Update();
   n++;
   breakTime(59);
}


}