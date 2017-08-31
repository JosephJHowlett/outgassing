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
   int molecule_cracks [] = {1, 2, 16, 17, 18, 27, 28, 32, 44}, tot_cracks =9, tot_molecules=5;
   Double_t crack_amt [] = {0, 0, 0, 0, 0, 0, 0, 0};
   Double_t molecule_frac [] = {0, 0, 0, 0, 0};
   Double_t molecule_masses [] = {2.02, 18.02, 28.02, 32.00, 44.01};
   TString molecule_names [] = {"H2", "H2O", "N2", "O2", "CO2"};
   for (int i=0; i<npeaks; i++) {
      int xpeak_round = int(xpeaks[i]+.5);
      cout<< xpeak_round <<endl;
      for (int j = 0;j<tot_cracks;j++) {
//          cout<<molecule_cracks[j]<<endl;
         if (xpeak_round==molecule_cracks[j]) {
            crack_amt[j] = ypeaks[i]/weightsum;
            cout<<crack_amt[j]<<endl;
         }
      }
   }
   molecule_frac[0] = crack_amt[0] + crack_amt[1];
   molecule_frac[1] = crack_amt[2] + crack_amt[3] + crack_amt[4];
   molecule_frac[2] = crack_amt[5] + crack_amt[6];
   molecule_frac[3] = crack_amt[7];
   molecule_frac[4] = crack_amt[8];
   for (int i=0;i<tot_molecules-1;i++) {
      cout <<molecule_names[i] << "\t"<<molecule_frac[i] <<endl;
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


rga_scan analyze_rga_dat_partial(TString fname, Double_t input_mass = 0, TString getpeaks = "") //gets composition analysis from rga
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
   load_scan(fname, x, y, npts, xax);

////  Finding Peaks  ////
   TH1D * h1 = new TH1D("mhist","",npts,0,xax);
//    TH1D * h2 = new TH1D("hist2","",npts,0,50);
   for(int i = 0;i<npts;i++)
   {
     h1->Fill(x[i],y[i]);
//      h2->Fill(x[i],y[i]);
   }
   rga_scan scan = fitpeaks(h1, input_mass, getpeaks);
//    h1->SetMarkerStyle(20);
//    h1->SetMarkerSize(.3);
//    h1->SetMarkerColor(1);
//    h1->SetLineColor(1);
// //    h2->SetLineWidth(.95);
//    h1->Draw("p");
//    h1->SetStats(kFALSE);
//    h1->GetXaxis()->SetTitle("Mass (AMU)");
//    h1->GetYaxis()->SetTitle("Pressure (Torr)");
//    TPad *inpad = new TPad("inpad","",.3,.3,.9,.88);
//    inpad->Draw();
//    inpad->cd();
//    h2->SetMarkerStyle(20);
//    h2->SetMarkerSize(.3);
//    h2->SetMarkerColor(1);
//    h2->SetLineColor(1);
//    h2->Draw("p");
//    h2->SetStats(kFALSE);
//    cout << scan.mass << endl;
   return scan;
}