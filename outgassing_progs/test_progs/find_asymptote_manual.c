{
TCanvas *c1 = new TCanvas("c1", "c1",650,40,800,500);
c1->SetHighLightColor(10);
c1->SetGrid() ;
c1->SetFillColor(10);
c1->SetBottomMargin(0.15);
c1->SetLeftMargin(0.15);
const int npts = 13;
long int time [npts];

int time_bound_low = 8000;
int time_bound_high = 50;
int p_bound_low = 1;
int p_bound_high = 5;

long int t0 = 1429200420;
for (int i=0; i<npts; i+=1)
{
  time [i] = t0;
  t0 += 60;
}

double pressure [npts] = {1.44e-7, 7.65e-7, 8.93e-7, 8.65e-7, 8.41e-7, 8.30e-7, 
                          8.25e-7, 8.22e-7, 8.21e-7, 8.21e-7, 8.25e-7, 8.31e-7, 8.29e-7};

TGraphErrors *g1 = new TGraphErrors();
for (i=0; i<npts; i++)
{
  g1->SetPoint(i, time[i], pressure[i]);
  g1->SetPointError(i, 0, .05*pressure[i]);
}
TF1 * fitfunc = new TF1("asympt","[0]*(1-exp(-[1]*(x-[2])))");
     
fitfunc->SetLineColor(2);
fitfunc->SetLineStyle(4);
fitfunc->SetParLimits(0,p_bound_low*pressure[npts-1],p_bound_high*pressure[npts-1]);
fitfunc->SetParLimits(1,1e-4,5e-2);
fitfunc->SetParLimits(2,time[0]-time_bound_low,time[0]-time_bound_high);
fitfunc->SetParName(0, "P_final [Torr]");
cerr.setstate(ios::failbit);
// g1->Fit(fitfunc,"m");
cerr.clear();
gStyle->SetOptFit(1);
g1->Draw("ap");
g1->SetMarkerSize(.2);
g1->SetMarkerStyle(8);
g1->SetMarkerColor(1);
g1->SetLineColor(1);
//g->SetMinimum(0.00000001);

// g->SetTitle("Acrylic, initial pump down");
// g->SetTitle("Outgassing System Pressure");
g1->GetXaxis()->CenterTitle(true);
g1->GetXaxis()->SetTimeDisplay(1);
g1->GetXaxis()->SetTitleOffset(2);
g1->GetXaxis()->SetLabelOffset(0.03);
g1->GetXaxis()->SetLabelSize(0.03);
g1->GetXaxis()->SetTimeFormat("#splitline{%y\/%m\/%d}{%H:%M}%F1970-01-01 00:00:00");

g1->GetYaxis()->SetTitle("Pressure [Torr]");
g1->GetXaxis()->SetTitle("Time [Hours]");
g1->GetYaxis()->CenterTitle(true);
}