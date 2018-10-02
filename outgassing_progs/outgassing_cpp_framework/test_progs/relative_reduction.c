void relative_reduction()
{
  Double_t constant_1, p1_1, p2_1, constant_2, p1_2, p2_2;
  
  constant_1 = 3.717e-9;  //  mylar before and after drybox
  p1_1 = -15.38;
  p2_1 = -.08183;
  
  constant_2 = 2.828e-9;
  p1_2 = -16.69;
  p2_2 = -.08352; 
  
/*  constant_1 = 2.388e-8;  // stov_1 before and after drybox
  p1_1 = -15.84;
  p2_1 = -.02351;
  
  constant_2 = 2.095e-8;
  p1_2 = -16.01;
  p2_2 = -.04625; */
  
//   constant_1 = 1.475e-8; //  stov_4
//   p1_1 = -16.69;
//   p2_1 = -.05103;
  
//   constant_2 = 2.388e-8; //  stov_1
//   p1_2 = -15.84;
//   p2_2 = -.02351;
  
//   constant_1 = 2.726e-10;  //  stov_3
//   p1_1 = -20.55;
//   p2_1 = -.1;

constant_1 = 3.312e-5;
p1_1 = -8.139;
p2_1 = -.09332;
  
  TF1 *fit_1 = new TF1("fit_1","[0] + expo(1)");  
  TF1 *fit_2 = new TF1("fit_2","[0] + expo(1)");
  
  fit_1->SetParameters(constant_1, p1_1, p2_1);
  fit_2->SetParameters(constant_2, p1_2, p2_2);
  
  TF1 *rel_red = new TF1("rel_red", "fit_1", 10, 90);
  
  
  TCanvas *c1 = new TCanvas("c1", "c1",650,40,1500,1000);
  c1->SetHighLightColor(10);
  c1->SetGrid() ;
  c1->SetFillColor(10);
  c1->SetBottomMargin(0.15);
  c1->SetLeftMargin(0.15);
  c1->SetLogy();
  rel_red->SetLineColor(1);
  rel_red->GetYaxis()->SetTitleOffset(1.4);
  rel_red->Draw();
  rel_red->SetTitle("");
  rel_red->GetYaxis()->SetTitle("Improvement Factor");
  rel_red->GetXaxis()->SetTitle("Time (Hours)");
}