#include <sstream>
#include <cstring>
#include <string>
#include <iostream>
#include "TFile.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"

void joeycorrelation()
{
   using namespace std;
	int timeref=1293840000;

	float badstart=1323279939;
	float badend=1323435600;
   const int maxbins = 10000;
   int npts=0;
   double rate [maxbins], fpressure;
   double scmean [maxbins], ftemp;
   string params;
   ifstream infile1;
   infile1.open ("rebin_log.dat",ifstream::in);
   

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
   TGraph *g1 = new TGraph();
   TGraph *g2 = new TGraph();
   
   for (int i=0;i<npts;i++) {
      g1->SetPoint(i,i,scmean[i]);
      g2->SetPoint(i,i,rate[i]*1e8);
   }
   g1->SetMarkerColor(2);
   g1->SetMarkerStyle(20);
   g2->SetMarkerColor(4);
   g2->SetMarkerStyle(20);
   g1->Draw("ap");
   g2->Draw("p same");
   
	int doflinear=0;
	// Calculating correlation coefficient
	double sumx=0,sumy=0;
	double sumxy=0;
	double sumxsquare=0,sumysquare=0;
	for(int i=0;i<npts;i++)
	{
			sumx+=scmean[i];
			sumy+=rate[i];
			sumxy+=scmean[i]*rate[i];
			sumxsquare+=pow(scmean[i],2.0);
			sumysquare+=pow(rate[i],2.0);
			doflinear++;
	}
	double colinear=0;
	colinear=(doflinear*sumxy-sumx*sumy);
	colinear/=sqrt(doflinear*sumxsquare-pow(sumx,2.0));
	colinear/=sqrt(doflinear*sumysquare-pow(sumy,2.0));

	// For Plots Range
	double xmean=sumx/doflinear;
	double xsigma=sqrt((sumxsquare/doflinear-pow(xmean,2.0))*doflinear/(doflinear-1));
	double ymean=sumy/doflinear;
	double ysigma=sqrt((sumysquare/doflinear-pow(ymean,2.0))*doflinear/(doflinear-1));
	// For Statistical Tests
	gROOT->ProcessLine(".L correlationpdf.h");
	TF1 *pdf=new TF1("pdf",correlationpdf,0,1,1);
	pdf->SetParameter(0,doflinear-2);
	cout << "colinear:  " <<colinear << endl;
	double problinear=2.0*pdf->Integral(TMath::Abs(colinear),1.0);
	cout << "problinear:  " << problinear << endl;
	// Transform to Student't distribution
	double tlinear=colinear*sqrt(doflinear-2)/sqrt(1-pow(colinear,2.0));
	double probtlinear=2.0*ROOT::Math::tdistribution_cdf_c(TMath::Abs(tlinear),doflinear-2);
	// Fisher Transform
	gROOT->ProcessLine(".L fisher.h");
	double fishermeanlinear=fisher(colinear);
	double fishersigma=1./sqrt(doflinear-3);
	double probflinear=2.0*ROOT::Math::gaussian_cdf_c(TMath::Abs(fishermeanlinear),TMath::Abs(fishersigma),0);
   
	//cout<<"For Web Wiki:"<<endl<<endl;
	cout<<setprecision(3);
	cout<<"| "<<colinear<<" | "<<fishermeanlinear<<"+-"<<fishersigma<<" | "<<problinear<<" |"<<endl<<endl;
}	
int main() {
   joeycorrelation();
}