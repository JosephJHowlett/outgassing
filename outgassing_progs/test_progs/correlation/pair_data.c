#include <iostream>


void pair_data() {
   const int maxbins = 1000000;
   int npts=0;
   Double_t pressure [maxbins], fpressure;
   long int time [maxbins], ftime, t_0, t_1;
   ftime = 0;

   string params;
   ifstream infile1;
   infile1.open ("pressure_time_long_fit.dat",ifstream::in);

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
   
//    ofstream outfile1;
//    outfile1.open("my_pressure.dat", ios::out);
//    for (int i=0;i<npts;i++) {
//       if (time[i]>1405038500 && time[i]<1405360000) {
//          char buffer [100];
//          int n = sprintf(buffer, "%li \t %.2e \n", time[i], pressure[i]);
//          outfile1 << buffer;
//       }
//    }
//    outfile1.close();
   string params2;
   ifstream infile2;
   infile2.open ("temp_time_long.dat",ifstream::in);

   int npts_temp=0;
   Double_t temp [maxbins], ftemp;
   long int time_temp [maxbins], ftime;
   ftime = 0;

   if (infile2.is_open()) {
   cout <<"scanning file"<<endl;
     for (int i=0; i<(maxbins-1); i++) {
       while (getline (infile2,params2)) {
         sscanf(params2.c_str(), "%li %le", &ftime, &ftemp);
         if (ftime>1405038500 && ftime <1405360000) {
            time_temp[npts_temp] = ftime;
            temp[npts_temp] = ftemp;
            npts_temp++;
         }
         i++;
        }
      }
    }
   else cout << "unable to open file"<<endl;
   infile2.close();
   cout<<npts_temp<<endl;
   
   ofstream outfile1;
   outfile1.open("temp_pressure_long.dat",ios::out);
   TGraph *g1 = new TGraph();
   TGraph *g2 = new TGraph();
   
   int dist, closest;
   for (int i=0;i<npts;i++) {
      dist=10000;
      closest=0;
         for (int j=0;j<npts_temp;j++) {
            if (TMath::Abs(time_temp[j]-time[i])<dist) {
               dist = TMath::Abs(time_temp[j]-time[i]);
               closest = j;
            }
         }
      char buffer[100];
      int n = sprintf(buffer, "%.2f \t %.2e \n", temp[closest], pressure[i]);
      outfile1 << buffer;
      g1->SetPoint(i, time[i], pressure[i]*1e8);
      g2->SetPoint(i, time_temp[closest], temp[closest]);
   }
   outfile1.close();
   g1->Draw("ap");
   g2->Draw("psame");

}