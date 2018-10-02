#include <math.h>
#include <stdio.h>
#include <sstream>
#include "TSpectrum.h"
#include "TVirtualFitter.h"
#include "TMath.h"
#include "TFile.h"
#include "settings.h"
gROOT->Macro("style.C");

struct rga_scan {
   Double_t mass, err, mass_peak, mass_fraction;
   Double_t *peak_locs;
   int n_peaks;
};


rga_scan fitpeaks(TH1D *h1, Double_t input_mass, TString getpeaks) {

   Double_t meansum = 0;
   Double_t weightsum = 0;
   Double_t *xpeaks,*ypeaks;
   TSpectrum * spect = new TSpectrum(8);
   int npeaks = spect->Search(h1,4,"nodraw",.07);
   const int numnum = npeaks;
   TList *funcs = h1->GetListOfFunctions();
   TPolyMarker *pm = (TPolyMarker*)funcs->FindObject("TPolyMarker");
   ypeaks = pm->GetY();
   Double_t fractions [numnum];
   xpeaks = pm->GetX();

   for (int i=0;i!=npeaks;i++) {
      TF1 * fitfunc = new TF1("fitfunc","gaus",xpeaks[i]-(2*hwhm),xpeaks[i]+(2*hwhm));
      fitfunc->SetParLimits(2,.12,.18);
      h1->Fit("fitfunc","rnq");
      Double_t mean = fitfunc->GetParameter(1);
      meansum += (mean*ypeaks[i]);
      weightsum += ypeaks[i];
      xpeaks[i] = mean;
   }
   Double_t meanmass = meansum/weightsum;
   Double_t count =0;
   for (int i=0;i!=npeaks;i++) {
       Double_t height = ypeaks[i];
       fractions[i] = height/weightsum;
   }
    // error stuff //
   Double_t meanerr = .094 * meanmass;
   rga_scan scan;
   scan.mass = meanmass;
   scan.err = meanerr;
   scan.n_peaks = npeaks;
   if (getpeaks == "getpeaks") scan.peak_locs = xpeaks;
   if (input_mass != 0) {
      int found = 0;
      for (int i=0;i!=npeaks;i++) {
         if ((xpeaks[i] > input_mass-hwhm) && 
         (xpeaks[i] < input_mass +hwhm)) {
            scan.mass_peak = xpeaks[i];
            scan.mass_fraction = fractions[i];
            found++;
         }
      }
      if (found==0) {
         scan.mass_peak = input_mass;
         scan.mass_fraction = 0;
      }
   }
   
   return scan;
}


rga_scan analyze_rga_dat(TString fname, Double_t input_mass = 0, TString getpeaks = "") //gets composition analysis from rga
{
   const int maxbins = 2001;
   int npts=0;
   Double_t xax=.5;
   Double_t y [maxbins], x [maxbins], fy;
   long int num [maxbins],f1,fnum;
   
   string params;
   ifstream infile1;
   infile1.open (file_header + "rga_measurements_2/" + fname,ifstream::in);
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
      cout << endl<<"missing file "<< fname << endl << "using mean mass from previous measurement..." << endl<<endl;
      rga_scan means;
      means.mass = 0;
      means.err = 0;
      return means;
   }
   infile1.close();
   
////  Finding Peaks  ////
   TH1D * h1 = new TH1D("mhist","RGA Measurements "+fname,npts,0,xax);
   for(int i = 0;i<npts;i++)
   {
     h1->Fill(x[i],y[i]);
   }
   rga_scan scan = fitpeaks(h1, input_mass, getpeaks);
//    h1->SetMarkerStyle(20);
//    h1->SetMarkerSize(.5);
   h1->Draw();
   
   return scan;
}