#include <vector>
#include "settings.h"
gROOT->Macro("style.C");

void plot_outgassing_errors_oxygen(int n_measurements, vector<Double_t>& time_hrs,
vector<Double_t>& throughput, Double_t Plot_Total, Double_t p_err [] =0) {

TGraph *g = new TGraph();
TGraphErrors * e_stat = new TGraphErrors();
TGraphErrors * e_sys = new TGraphErrors();

TCanvas *c1 = new TCanvas("c1", "c1",650,40,1500,1000);
c1->SetHighLightColor(10);
c1->SetGrid() ;
c1->SetFillColor(10);
c1->SetBottomMargin(0.15);
c1->SetLeftMargin(0.15);
c1->SetLogy();

Double_t error_frac;
for (int i = 0; i<n_measurements; i++) {
   g->SetPoint(i, time_hrs[i], throughput[i]);
   e_stat->SetPoint(i, time_hrs[i], throughput[i]);
   e_sys->SetPoint(i,time_hrs[i], throughput[i]);
   error_frac = TMath::Sqrt((.06**2)+(p_err[i]**2));
//    cout<<error_frac<<endl;
   e_stat->SetPointError(i,0.,error_frac*throughput[i]);
   e_sys->SetPointError(i,0.,0.31*throughput[i]);
}
// e_stat->SetPoint(n_measurements, 40.3, 4.73e-5/area);  // stov_1_drybox
// e_stat->SetPointError(n_measurements, 0., .07*4.73e-5/area);

// e_stat->SetPoint(n_measurements, 16.78, 7.38667e-05/area); // 20141112_ptfe.dat
// e_stat->SetPointError(n_measurements, 0., .07*7.38667e-05/area);

// e_stat->SetPoint(n_measurements, 64.88, 2.519e-05/area); // stov_ 20150206.dat
// e_stat->SetPointError(n_measurements, 0., .07*2.519e-05/area);

// e_stat->SetPoint(n_measurements, 67.80, 1.054e-7/area); // 6 electrodes measurement 4
// e_stat->SetPointError(n_measurements, 0., .07*1.054e-7/area);

///////// PLOTTING STUFF ///////////

//g->SetMinimum(0.00000001);

// e_sys->SetTitle("RG 178 cable, 35\'8\", initial");
gStyle->SetTitleY(0.97);
gStyle->SetTitleX(0.25);
e_stat->GetXaxis()->SetTitle("Time Under Vacuum [hours]");
e_stat->GetXaxis()->CenterTitle(true);
e_stat->GetXaxis()->SetLabelOffset(0.01);
e_stat->GetXaxis()->SetLabelSize(0.03);

if (Plot_Total == 1){
  e_stat->GetYaxis()->SetTitle("Total Outgassing Rate [Torr*l/s]");
}
else {
//   e_stat->GetYaxis()->SetTitle("Specific Outgassing Rate [Torr*l/s*cm]");  // for length
  e_stat->GetYaxis()->SetTitle("Specific Outgassing Rate [Torr*l/s*cm^{2}]");  // for surface area
}

e_stat->GetYaxis()->CenterTitle(true);
e_stat->GetYaxis()->SetTitleOffset(1.5);
e_stat->GetXaxis()->SetRangeUser(-10,100);

Double_t final_val = throughput[n_measurements-1];

TF1 *fitfunc = new TF1("fitexp","expo(0)",10,200);
// fitfunc->SetParameter(0,2*10e-7/area);
// fitfunc->SetParLimits(0,.5*10e-7/area,4*10e-7/area);

// TH2D * hframe = new TH2D("hframe","",100,15,200,100,1e-6,1e-4);
// gStyle->SetOptStat(0);
// hframe->Draw();
// hframe->GetXaxis()->SetTitle("Time [Hrs]");
// hframe->GetYaxis()->SetTitle("Total Outgassing Rate [Torr*l/s]");

fitfunc->SetParLimits(1,-1e-1,-1e-4);  // comment or change this line if problems arise
// fitfunc->FixParameter(2,-3.434e-2);
fitfunc->SetLineColor(my_colors[plot_number]);  //my_colors[5] is decent purp
e_stat->SetMarkerColor(my_colors[plot_number]);
e_stat->SetLineColor(my_colors[plot_number]);
e_stat->SetMarkerSize(1);
e_stat->SetMarkerStyle(my_markers[plot_number]);
fitfunc->SetLineStyle(2);
if (fit==1) {
    cerr.setstate(ios::failbit);
    e_stat->Fit("fitexp","mr");
    cerr.clear();
    e_stat->GetYaxis()->SetMoreLogLabels();
    gStyle->SetOptFit(1);
    // e_sys->Draw("same p []");
    TPaveStats *ptstats = new TPaveStats(0.6457286,0.769556,0.9258794,0.9302326,"brNDC");
    e_stat->GetListOfFunctions()->Add(ptstats);
    ptstats->SetParent(e_stat->GetListOfFunctions());
    ptstats->SetName("stats");
    ptstats->SetBorderSize(1);
    ptstats->SetFillColor(0);
    ptstats->SetTextAlign(12);
    ptstats->SetTextFont(42);
}
// TH2D * hframe = new TH2D("hframe","",100,-10,100,100,0.8*e_stat->GetMinimum(),2.*e_stat->GetMaximum())
// e_stat->SetMarkerColor(2);
// e_stat->SetLineColor(2);
e_stat->Draw("ap");
if (Plot_Total == 1 && print == 1) {
  c1->Print(file_header + "plots/new_data/" + img_name + "_int.png");
  c1->Print(file_header + "plots/new_data/" + img_name + "_int.C");
}
else if (print == 1 ) {
  c1->Print(file_header + "plots/new_data/" + img_name + "_spec.png");
  c1->Print(file_header + "plots/new_data/" + img_name + "_spec.C");
}
}
