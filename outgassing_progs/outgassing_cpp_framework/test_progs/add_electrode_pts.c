// #include "/Users/josephhowlett/myroot/outgassing_analysis/outgassing_progs/sub_macros/settings.h"

{
TGraphErrors * g2 = new TGraphErrors();
const int npts = 2;
int electrode_color = kMagenta+2;
long int time_electrode [npts] = {13.0, 36.0};
double pressure_electrode [npts] = {1.4e-7, 6.5e-8};

for (int i=0; i<npts; i++)
{
  g2->SetPoint(i, time_electrode [i], pressure_electrode [i]);
  g2->SetPointError(i, 0, .05*pressure_electrode [i]);
  g2->SetMarkerColor(electrode_color);
  g2->SetLineColor(electrode_color);
  g2->Draw("psame");
}
}