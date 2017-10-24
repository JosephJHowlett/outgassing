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
   Double_t *peak_heights;
   int n_peaks;
};



rga_scan fitpeaks(TH1D *h1, Double_t input_mass, TString getpeaks) {
   Double_t meansum = 0;
   Double_t weightsum = 0;
   Double_t *xpeaks,*ypeaks;
   TSpectrum * spect = new TSpectrum(10);
   int npeaks = spect->Search(h1,4.0,"nodraw",.05);
   //cout << npeaks << endl;
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
      //cout << mean << endl;
   }
   Double_t meanmass = meansum/weightsum;
   Double_t count =0;
   for (int i=0;i!=npeaks;i++) {
       Double_t height = ypeaks[i];
       fractions[i] = height/weightsum;
//        cout<<xpeaks[i]<<"\t"<<fractions[i]<<endl;
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
   return scan;
}


void load_scan(TString fname,Double_t* x,Double_t* y,int &npts,Double_t &xax, Double_t &rga_pressure_tot)
{
   const int maxbins = 5001;
   int ii = 0;
   Double_t fx;
   Double_t fy;
   long int num [maxbins],f1,fnum;
   string params;
   ifstream infile1;
   infile1.open (file_header + "rga_measurements/" + fname, ifstream::in);
   if (infile1.is_open()) {
     for (int i=0; i<(maxbins-1); i++) {
       while (getline (infile1,params)) {
         if (ii==0) {
            sscanf(params.c_str(), "%le", &rga_pressure_tot);
            ii++;
            continue;
         }
         else {
            sscanf(params.c_str(),"%le %le",&fx,&fy);
         }
         y[i] = fy;
         x[i] = fx;
         npts++;
         i++;
        }
      }
    }
   else {
      cout << endl<<"missing file "<< fname << endl << "using mean mass from previous measurement..." << endl<<endl;
      x[0]=10;
   }
   infile1.close();
//    cout << "RGA Total Pressure = " << rga_pressure_tot << endl;
}


rga_scan analyze_rga_dat_oxygen(TString fname, Double_t input_mass, TString getpeaks = "") //gets composition analysis from rga
{
//    Double_t scan_time;
   ///// Background Subtraction /////

   
//    TString fname_back = "07072014.dat"; /// This needs to be fixed to find a comparable background
//    int npts_back=0, ii_back=0;
//    Double_t y_back [maxbins], x_back[maxbins];
//    Double_t xax_back=.5;
//    load_scan(fname_back, x_back, y_back, npts_back, xax_back);
   /////                  /////
   
   int npts=0, ii = 0;
   Double_t y [maxbins], x [maxbins];
   Double_t xax=.5;    // low mass value of rga - .5 amu
   Double_t rga_pressure_tot = 0.0;
   load_scan(fname, x, y, npts, xax, rga_pressure_tot);
   xax = x[npts-1];
   cout << rga_pressure_tot << endl;
   if (x[0]==10) {
      rga_scan means;
      means.mass=0;
      means.err=0;
      return means;
   }
   else {
////  Finding Peaks  ////
   //cout << xax << endl;
   TH1D * h1 = new TH1D("mhist","",npts,0,xax);
   TGraph * g = new TGraph();
   for(int i = 0;i<npts;i++)
   {
     h1->Fill(x[i],y[i]);
     g->SetPoint(i, x[i], y[i]);
   }
   rga_scan scan = fitpeaks(h1, input_mass, getpeaks);
   if (draw_rga_scan==1) {
   h1->SetMarkerStyle(20);
   h1->SetMarkerSize(.3);
   h1->SetMarkerColor(my_colors[0]);
   h1->SetLineColor(my_colors[0]);
//    h2->SetLineWidth(.95);
//    cout << scan.mass << endl;
   g->SetMarkerStyle(8);
   g->SetMarkerSize(.2);
   g->GetXaxis()->SetTitle("Mass (AMU)");
   g->GetYaxis()->SetTitle("Partial Pressure (Torr)");
   g->Draw("ap");
   h1->Draw("plsame");
   h1->SetStats(kFALSE);
   h1->GetXaxis()->SetTitle("Mass (AMU)");
   h1->GetYaxis()->SetTitle("Partial Pressure (Torr)");
   }
   return scan;
   }
}
