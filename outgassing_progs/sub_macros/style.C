{
///// FORMATTING /////
gROOT->SetStyle("Plain");
//gStyle->SetFilleStyle(4000);
//gStyle->SetTitleStyle(2001);
//gStyle->SetGrid();
gStyle->SetStatBorderSize(1);
gStyle->SetTitleBorderSize(1);
//gStyle->SetPalette(1,0);
////gStyle->SetPalette(0,0);
TGaxis::SetMaxDigits(4);
gStyle->SetOptStat(0);
gStyle->SetOptFit(1);
gStyle->SetFillColor(10);


///// COLORS /////
const Int_t NRGBs = 5;
const Int_t NCont = 255;
Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00};
TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont); 
gStyle->SetNumberContours(NCont);

///// FONT /////
gStyle->SetTitleFont(42);
gStyle->SetStatFont(42);
gStyle->SetLabelFont(42,"xy");
	 
///// TITLES /////	  
gStyle->SetTitleStyle(0);
//gStyle->SetTitleX(0.15);
gStyle->SetTitleY(0.95);
gStyle->SetTitleXSize(0.04);
gStyle->SetTitleYSize(0.04);
gStyle->SetTitleOffset(1.0,"X");
gStyle->SetTitleOffset(1.2,"Y");
gStyle->SetLabelSize(0.04,"xy");
gStyle->SetMarkerStyle(20);
gStyle->SetMarkerSize(0.4);

///// STATS /////
//gStyle->SetStatStyle(0);
//Style->SetStatX(0.9);
//gStyle->SetStatY(0.9);
//gStyle->SetStatW(0.3);
//gStyle->SetStatH(1.3);
//gStyle->SetStatColor(0);

}
