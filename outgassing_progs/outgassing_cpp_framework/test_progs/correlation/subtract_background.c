void subtract_background() {
   const int maxbins = 1000000;
   int npts=0;
   Double_t pressure [maxbins], fpressure;
   long int time [maxbins], ftime, t_0, t_1;
   ftime = 0;

   string params;
   ifstream infile1;
   infile1.open ("pressure_time_long_prefit.dat",ifstream::in);

   if (infile1.is_open()) {
   cout <<"scanning file"<<endl;
     for (int i=0; i<(maxbins-1); i++) {
       while (getline (infile1,params)) {
         sscanf(params.c_str(), "%li %le", &ftime, &fpressure);
         time[i] = ftime;
         pressure[i] = fpressure;
         npts++;
         i++;
        }
      }
    }
   else cout << "unable to open file"<<endl;
   cout<<npts<<endl;
   infile1.close();
   Double_t p0 = 1.55033e-3;
   Double_t p1 = -1.10272e-12;
   TF1 *fitfunc = new TF1("fitexp","pol1");
//    fitfunc->SetParName(0,"constant");
   ofstream outfile1;
   outfile1.open("pressure_time_long_fit.dat",ios::out);
      
   for (int i=0;i<npts;i++) {
      char buffer [100];
      int n = sprintf(buffer, "%li \t %.2e \n", time[i], pressure[i]-(p0 + (p1*time[i])));
      outfile1<<buffer;
   }
   outfile1.close();
//    fitfunc->SetParLimits(0,.6*pressure[npts],.9*pressure[npts]);
//    fitfunc->SetParLimits(2,-1e-1,-1e-3);  // comment or change this line if problems arise
   // fitfunc->SetParLimits(1, -1e-13, -1e-11);
//    g->Fit("fitexp", "m");

}