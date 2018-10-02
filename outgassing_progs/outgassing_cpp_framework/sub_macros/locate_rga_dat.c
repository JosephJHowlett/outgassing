#include <sstream>
#include <string>
// #include <iostream>
// #include <TString>
// #include <vector>

//// takes in measurement_time array and number of measurements, puts out rga .dat filename
//// in a vector<TString>
vector<TString> locate_rga_dat(long int measurement_time[],int n_measurements)
{
TDatime file_date = measurement_time[n_measurements-1];
Int_t year_asint = file_date.GetYear();
std::ostringstream os;
os << year_asint;
TString year = os.str();

TString mo_day [n_measurements];
vector<TString> rga_dats(n_measurements);
long int nperday = 1;

for (int i=0;i<n_measurements;i++) {

   TDatime measurement_date = measurement_time[i];
   Int_t month_asint = measurement_date.GetMonth();
   Int_t day_asint = measurement_date.GetDay();

///// Getting Dates Right /////
   std::ostringstream os;
   if (day_asint<10 && month_asint<10) os << "0" << month_asint <<"0" << day_asint;
   else if (day_asint<10) os << month_asint <<"0"<< day_asint;
   else if (month_asint<10) os << "0"<<month_asint << day_asint;
   else os << month_asint << day_asint;
   mo_day[i] = os.str();
   if (i==0) {
      rga_dats[i] = mo_day[i] + year + ".dat";
      continue;
   }
   
   if (mo_day[i] == mo_day[i-1]) {
   nperday++;
   std:: ostringstream os;
   os << nperday;
   TString nperday_as_string = os.str();
   rga_dats[i] = mo_day[i] + year + "_" + nperday_as_string + ".dat";
   }
   else {
   rga_dats[i] = mo_day[i] + year + ".dat";
   nperday=1;
   }
}
return rga_dats;
}