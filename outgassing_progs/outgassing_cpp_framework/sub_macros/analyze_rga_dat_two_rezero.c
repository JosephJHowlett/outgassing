#include <math.h>
#include <stdio.h>
#include <sstream>
#include "TSpectrum.h"
#include "TVirtualFitter.h"
#include "TMath.h"
#include "TFile.h"
#include "settings.h"
gROOT->Macro("style.C");
Double_t analysis_threshold = 1.0e-10;

struct rga_scan {
   Double_t mass, err, mass_peak, mass_fraction;
   Double_t *peak_locs;
   Double_t *peak_heights;
   int n_peaks;
};

Double_t integral_tgraph(TGraphErrors * graph, Double_t lower_bound, Double_t upper_bound)
{
  int npts = graph->GetN();
  Double_t * x = graph->GetX();
  Double_t * y = graph->GetY();
  Double_t integral = 0.0;
  for (int i=0; i<npts; i++) 
  {
    if (x[i]>lower_bound && x[i]<upper_bound) integral += y[i];
  }
  return integral;
}
    


rga_scan fitpeaks(TH1D *h1, Double_t input_mass, TString getpeaks) {
   Double_t meansum = 0;
   Double_t weightsum = 0;
   Double_t *xpeaks,*ypeaks;
   TSpectrum * spect = new TSpectrum(12);
   int npeaks = spect->Search(h1,4,"nodraw",.05);
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


void load_scan(TString fname,Double_t* x,Double_t* y,int &npts,Double_t &xax, Double_t &rga_pressure_tot=0.0)
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
         if (fy < analysis_threshold) y[i] = 0.0;
         else y[i] = fy;
         x[i] = xax;
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


void analyze_rga_dat_two_rezero(TString fname="07242015_xe.dat", TString fname_bg="07242015_bg.dat", 
                         Double_t recorded_pressure=4.40e-5, Double_t recorded_pressure_bg=5.50e-7, Double_t input_mass = 0, 
                         TString getpeaks = "") //gets composition analysis from rga
{
//    Double_t scan_time;
   ///// Background Subtraction /////

   
//    TString fname_back = "07072014.dat"; /// This needs to be fixed to find a comparable background
//    int npts_back=0, ii_back=0;
//    Double_t y_back [maxbins], x_back[maxbins];
//    Double_t xax_back=.5;
//    load_scan(fname_back, x_back, y_back, npts_back, xax_back);
   /////                  /////
   
   int npts=0, ii = 0, npts_bg=0;
   Double_t y [maxbins], x [maxbins], y_bg [maxbins], x_bg [maxbins];
   Double_t xax=.5;    // low mass value of rga - .5 amu
   Double_t xax_bg=.5;    // low mass value of rga - .5 amu
   Double_t rga_pressure=0.0;
   Double_t rga_pressure_bg=0.0;
   
   load_scan(fname, x, y, npts, xax, rga_pressure);
   load_scan(fname_bg, x_bg, y_bg, npts_bg, xax_bg, rga_pressure_bg);
   if (recorded_pressure==0 && recorded_pressure_bg==0)
   {
      recorded_pressure = rga_pressure;
      recorded_pressure_bg = rga_pressure_bg;
   }
////  Rezeroing ////
   TGraphErrors * g_xe_unzeroed = new TGraphErrors();
   TGraphErrors * g_bg_unzeroed = new TGraphErrors();
   TGraphErrors * g_unzeroed = new TGraphErrors();
   TF1 * linfit = TF1("", "pol0", 150, 200);
   for(int i = 0;i<npts;i++)
   {
     g_xe_unzeroed->SetPoint(i, x[i], y[i]);
     g_xe_unzeroed->SetPointError(i, 0, .05*y[i]);
     g_bg_unzeroed->SetPoint(i, x_bg[i], y_bg[i]);
     g_bg_unzeroed->SetPointError(i, 0, .05*y_bg[i]);
   }
   g_xe_unzeroed->Fit(linfit);
   Double_t xe_offset = linfit->GetParameter(0);
   cout << "Offset Xe " << xe_offset << endl;
   g_bg_unzeroed->Fit(linfit);
   Double_t bg_offset = linfit->GetParameter(0);
   cout << "Offset BG " << bg_offset << endl;
////  Finding Peaks  ////
   TGraphErrors * g_xe = new TGraphErrors();
   TGraphErrors * g_bg = new TGraphErrors();
   TGraphErrors * g = new TGraphErrors();
   Double_t hydrocarbon_peak, hydrocarbon_peak_bg, scaled_subtracted_partial_pressure;
   for(int i = 0;i<npts;i++)
   {
     y[i] += -1.0*xe_offset;
     y_bg[i] += -1.0*bg_offset;
     g_xe->SetPoint(i, x[i], y[i]);
     g_xe->SetPointError(i, 0, .05*y[i]);
     g_bg->SetPoint(i, x_bg[i], y_bg[i]);
     g_bg->SetPointError(i, 0, .05*y_bg[i]);
     if (x[i] == 55.0)
     {
       hydrocarbon_peak = y[i];
       hydrocarbon_peak_bg = y_bg[i];
     }
   }
   Double_t rga_pressure_integral = integral_tgraph(g_xe, 0, 201);
   Double_t rga_pressure_integral_bg = integral_tgraph(g_bg, 0, 201);
   for(int i = 0;i<npts;i++)
   {
//      Double_t scaled_subtracted_partial_pressure = (y[i]*recorded_pressure/rga_pressure_integral) - (y_bg[i]*recorded_pressure_bg/rga_pressure_integral_bg);
     scaled_subtracted_partial_pressure = (y[i]/hydrocarbon_peak*hydrocarbon_peak_bg) - (y_bg[i]);
     g->SetPoint(i, x[i],scaled_subtracted_partial_pressure);   ///2.64e-6);
     g->SetPointError(i, 0, .05*scaled_subtracted_partial_pressure);
   }
   g->SetMarkerStyle(8);
   g->SetMarkerSize(.2);
   g->GetXaxis()->SetTitle("Mass (AMU)");
   g->GetYaxis()->SetTitle("Partial Pressure (Torr)");
   g->Draw("ap");
//    Double_t xenon_frac = (integral_tgraph(g, 63, 69) + integral_tgraph(g, 120, 140))/integral_tgraph(g, 0, 201);
//    cout << 100*xenon_frac << endl;
//    cout << "XENON PERCENTAGE = " << xenon_frac*100.0 << endl;
   }

