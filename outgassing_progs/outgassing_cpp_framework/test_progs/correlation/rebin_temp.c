#include <iostream>
const int maxbins = 1000000;


void rebin_temp() {
   double rate [maxbins], fpressure;
   double scmean [maxbins], ftemp;
   string params;
   ifstream infile1;
   infile1.open ("temp_log.dat",ifstream::in);
   
   int npts=0;

   if (infile1.is_open()) {
     for (int i=0; i<(maxbins-1); i++) {
       while (getline (infile1,params)) {
         sscanf(params.c_str(), "%lf %le", &ftemp, &fpressure);
         scmean[i] = ftemp;
         rate[i] = fpressure;
         npts++;
         i++;
        }
      }
    }
    
   cout<<npts<<endl;
   ofstream logfile;
   logfile.open("rebin_log.dat", ios::out);
   
   double new_rate [maxbins], new_scmean [maxbins];
   
   const int binsize = 40;
   Double_t mean = 0;
   Double_t mean2 = 0;
   int count = 0;
   
   for (int i=0;i<npts-1;i++) {
      for (int j=i;j<i+binsize;j++) {
         mean += scmean[j]/binsize;
         mean2 += rate[j]/binsize;
      }
      new_scmean[count] = mean;
      new_rate[count] = mean2;
      count++;
      i+=binsize;
      char buffer [100];
      int n = sprintf(buffer, "%.2f \t %.2e \n", mean, mean2);
      logfile<<buffer;
      mean = 0;
      mean2 = 0;
   }
   cout<<count<<endl;
   logfile.close();
   
}