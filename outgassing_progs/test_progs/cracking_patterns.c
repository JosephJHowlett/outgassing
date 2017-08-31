#include <math.h>
#include <stdio.h>
#include <sstream>
#include "TSpectrum.h"
#include "TVirtualFitter.h"
#include "TMath.h"
#include "TFile.h"
#include "../sub_macros/settings.h"
gROOT->Macro("../sub_macros/style.C");

struct rga_scan {
   Double_t mass, err, mass_peak, mass_fraction;
   Double_t *peak_locs;
   Double_t *peak_heights;
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
   gErrorIgnoreLevel = kError;
   scan.err = meanerr;
   scan.n_peaks = npeaks;
   if (getpeaks == "getpeaks") {
      cerr.setstate(ios::failbit);
      scan.peak_locs = xpeaks;
      scan.peak_heights = ypeaks;
      cerr.clear();
   }
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
   cout<<scan.peak_heights[0]<<endl;
   return scan;
}


void load_scan(TString fname,Double_t* x,Double_t* y,int &npts,Double_t &xax)
{
   const int maxbins = 5001;
   int ii = 0;
   Double_t fy;
   long int num [maxbins],f1,fnum;
   string params;
   ifstream infile1;
   infile1.open (file_header + "rga_measurements/" + fname, ifstream::in);
   if (infile1.is_open()) {
     for (int i=0; i<(maxbins-1); i++) {
       while (getline (infile1,params)) {
         if (ii==0) {
            Double_t rga_pressure_tot;
            sscanf(params.c_str(), "%le", &rga_pressure_tot);
            ii++;
            continue;
         }
         if ( (npts+11)%20==0 ) {      // npts + 1 + samples/amu/2  % samples/amu == 0
            sscanf(params.c_str(), "%*s %*i %li %li %le", &f1,&fnum, &fy);
            xax = f1;
         }
         else {
            sscanf(params.c_str(),"%*s %*i %li %le",&fnum,&fy);
            xax += .05; ///  1/(samples/amu)
         }
         num[i] = fnum;
         y[i] = fy;
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
}


Double_t mygaus (Double_t x, Double_t par)
{
   Double_t f = TMath::Exp(-.5*((x-par)/-.1)**2);
//    Double_t f = TMath::Landau(-x, -par, .05);
   return f;
}


Double_t fitfunc (Double_t *x, Double_t *par)
{
   Double_t xx = x[0];
   Double_t mass [] = {18, 17, 16};
   Double_t frac [] = {1., .25, .07};
   Double_t water = par[0] * (frac[0]*mygaus(xx, mass[0]) + frac[1]*mygaus(xx, mass[1]) + frac[2]*mygaus(xx, mass[2]));
   Double_t mass[] = {28, 14};
   Double_t frac[] = {1., .05};
   Double_t nitrogen = par[1] * (frac[0]*mygaus(xx, mass[0]) + frac[1]*mygaus(xx, mass[1]));
   Double_t mass[] = {32, 16};
   Double_t frac[] = {1, .05};
   Double_t oxygen = par[2] * (frac[0]*mygaus(xx, mass[0]) + frac[1]* mygaus(xx, mass[1]));
   Double_t mass [] = {44, 28, 16};
   Double_t frac [] = {1., .05, .05};
   Double_t co2 = par[3] * (frac[0]* mygaus(xx, mass[0]) + frac[1]*mygaus(xx, mass[1]) + frac[2]*mygaus(xx, mass[2]));
   Double_t f = water + nitrogen + oxygen + co2;
   return f;
}
   

void cracking_patterns(TString fname, Double_t input_mass = 0, TString getpeaks = "") //gets composition analysis from rga
{
   int npts=0, ii = 0;
   Double_t y [maxbins], x [maxbins];
   Double_t xax=.5;    // low mass value of rga - .5 amu
   load_scan(fname, x, y, npts, xax);
   TH1D * h1 = new TH1D("mhist","",npts,0,xax);
   TGraphErrors * g1 = new TGraphErrors;
   for(int i = 0;i<npts;i++)
   {
     h1->Fill(x[i], y[i]);
     g1->SetPoint(i, x[i], y[i]);
     g1->SetPointError(i, .025, 0);
   }
   rga_scan scan = fitpeaks(h1, input_mass, "getpeaks");
   TF1 * myfit = new TF1("myfit", fitfunc, 0, 50, 4);
//    myfit->SetParameter(0, scan.peak_heights[1]);
   myfit->SetParLimits(0,1e-9, 1e-7);
   myfit->SetParameter(1, scan.peak_heights[0]);
   myfit->SetParLimits(1, 1e-7, 1e-6);
   myfit->SetParameter(2, scan.peak_heights[2]);
//    myfit->SetParameter(0, 

   h1->Fit(myfit, "mr");
//    myfit->Draw();
   h1->Draw();
}