#include <math.h>
#include <stdio.h>
#include <sstream>
#include "TSpectrum.h"
#include "TVirtualFitter.h"
#include "TMath.h"
#include "TFile.h"
#include "/Users/josephhowlett/myroot/outgassing_analysis/outgassing_progs/settings.h"
gROOT->Macro("style.C");

void plot_background_scans() //gets composition analysis from rga
{
TH1D **harray = new TH1D*[2];
Double_t input_mass = 0;
TString fname [2] = {"10052012.dat","10222012.dat"};

for (int j=0;j<2;j++) {
   // TString fname = "10282012.dat";
   const int maxbins = 2001;
   int npts=0;
   Double_t xax=.5;
   Double_t y [maxbins], x [maxbins], fy;
   long int num [maxbins],f1,fnum;
   
   string params;
   ifstream infile1;
   infile1.open ("rga_measurements_2/" + fname[j],ifstream::in);
   if (infile1.is_open()) {
     for (int i=0; i<(maxbins-1); i++) {
       while (getline (infile1,params)) {
         if ( ((npts+1) % 5 == 0)&&((npts+1)%10!=0) )
            sscanf(params.c_str(), "%*s %*i %li %li %le", &f1,&fnum, &fy);
         else sscanf(params.c_str(),"%*s %*i %li %le",&fnum,&fy);
         num[i] = fnum;
         y[i] = fy;
         xax += .1;
         x[i] = xax;
         npts++;
         i++;
        }
      }
    }
   else {
//       cout << endl<<"missing file "<< fname << endl << "using mean mass from previous measurement..." << endl<<endl;
   }
   infile1.close();

////  Finding Peaks  ////
   
   TH1D * harray[j] = new TH1D("mhist","RGA Measurements "+fname,npts,0,200);
   for(int i = 0;i<npts;i++)
   {
     harray[j]->Fill(x[i],y[i]);
   }
   harray[j]->Draw("same");
}
}