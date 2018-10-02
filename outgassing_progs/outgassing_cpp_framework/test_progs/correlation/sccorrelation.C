sccorrelation(int filenumber=0,int data,int low=0,int switchi=0)
{
	int timeref=1293840000;

	float badstart=1323279939;
	float badend=1323435600;

	// Radon file from Marc Weber
	TString name[13];
	name[0]="pressure_run10";
	name[1]="p1_run10";
    name[2]="barp_run10";
    name[3]="bottom_run10";
    name[4]="top_run10";
    name[5]="insidebell_run10";
    name[6]="belowbell_run10";
    name[7]="t1_run10";
    name[8]="flowrec_run10";
    name[9]="flowpurge_run10";
    name[10]="level1_run10";
    name[11]="level2_run10";
    name[12]="level3_run10";
	
	TString xtitle[13];
	xtitle[0]="Absolote Pressure [atm]";
	xtitle[1]="Relative Pressure P1 [atm]";
	xtitle[2]="Xebox Pressure BarP [mbar]";
	xtitle[3]="Bottom Temperature [C]";
	xtitle[4]="Top Temperature [C]";
	xtitle[5]="InsideBell Temperature [C]";
	xtitle[6]="BelowBell Temperature [C]";
	xtitle[7]="Xebox Temperature [C]";
	xtitle[8]="Recirculation Flow [SLPM]";
	xtitle[9]="N2 Purge Flow [SLPM]";
	xtitle[10]="Level Meter 1 [mm]";
	xtitle[11]="Level Meter 2 [mm]";
	xtitle[12]="Level Meter 3 [mm]";
	
	TString scfile="files/"+name[filenumber]+".txt";
	TString picname="corr_"+name[filenumber]+"_";	
	

	badstart=(badstart-timeref)/(3600.*24);
	badend=(badend-timeref)/(3600.*24);
		
	const int run10files=281;
	float scmean[run10files],scsigma[run10files];
	float scstart[run10files],scend[run10files];
	int cut[run10files];// Removal of bad SC data
	// Loading SC data
	ifstream scfilein(scfile,ios::in);
	for(int i=0;i<run10files;i++)
	{
		scfilein>>scmean[i]>>scsigma[i]>>scstart[i]>>scend[i];	
		scstart[i]-=timeref;
		scend[i]-=timeref;
		scstart[i]/=(3600.*24);
		scend[i]/=(3600.*24);
		// Remove bad data suggested by Sonja
		if(scstart[i]>badstart&&scend[i]<badend&&(filenumber==0||filenumber==1))
			cut[i]=0;
		else
			cut[i]=1;
		// Seperate data
		if(data==0)
		{	
			if(scend[i]<270)
				cut[i]*=1;
			else 
				cut[i]*=0;
		}
		else if(data==1)
		{	
			if(scstart[i]>270)
				cut[i]*=1;
			else
				cut[i]*=0;
		}
	}
	scfilein.close();

	// Loading background data
	TString ratefile="bgrate.txt";
	const int length=run10files,nbins=run10files; // 21bins in total
	int entries[8][length];
	float time[length],livetime[length];
	float starttime[length],endtime[length];
	float start[length],end[length];
	float rate[length],ratesigma[length];
	ifstream infile(ratefile,ios::in);
	for(int i=0;i<length;i++)
	{
		for(int j=0;j<8;j++)
		{
			infile>>entries[j][i];
		}
		infile>>livetime[i]>>starttime[i]>>endtime[i]>>start[i]>>end[i];
		starttime[i]-=timeref;
		endtime[i]-=timeref;
		starttime[i]/=(3600.*24);
		endtime[i]/=(3600.*24);
		}
	infile.close();

	TString title;
	// Switching Data
	switch (switchi)
	{
	case 0: // 34kg SS
		for(int i=0;i<nbins;i++)
		{
			rate[i]=entries[3+low*4][i];
		}
		title+="SS in 34kg Fiducial Volume ";
		picname+="34kg_ss_";
		break;	
	
	case 1: // 34kg DS
		for(int i=0;i<nbins;i++)
		{
			rate[i]=entries[2+low*4][i]-entries[3+low*4][i];
		}
		title+="DS in 34kg Fiducial Volume ";
		picname+="34kg_ds_";
		break;

	case 2: // 34kg SS+DS
		for(int i=0;i<nbins;i++)
		{
			rate[i]=entries[2+low*4][i];
		}
		title+="SS+DS in 34kg Fiducial Volume ";
		picname+="34kg_all_";
		break;

	case 3: // 48-34kg SS
		for(int i=0;i<nbins;i++)
		{
			rate[i]=entries[1+low*4][i]-entries[3+low*4][i];
		}
		title+="SS in 48-34kg Fiducial Volume ";
		picname+="48-34kg_ss_";
		break;

	case 4: // 48-34kg DS
		for(int i=0;i<nbins;i++)
		{
			rate[i]=entries[0+low*4][i]-entries[1+low*4][i]-(entries[2+low*4][i]-entries[3+low*4][i]);
		}
		title+="DS in 48-34kg Fiducial Volume ";
		picname+="48-34kg_ds_";
		break;

	case 5: // 48-34kg SS+DS
		for(int i=0;i<nbins;i++)
		{
			rate[i]=entries[0+low*4][i]-entries[2+low*4][i];
		}
		title+="SS+DS in 48-34kg Fiducial Volume ";
		picname+="48-34kg_all_";
		break;
	}

	if(low==0)
		{
			title+="(3-14PE) ";
			picname+="3-14PE_";
		}
	else if(low==1)
		{
			title+="(14-30PE) ";
			picname+="14-30PE";
		}

	if(data==0)
		{
			title+=" Before Chiller";
			picname+="beforechiller";	
		}

     if(data==1)
		{
			title+=" After Chiller";
			picname+="afterchiller"; 
		}

	cout<<title<<endl;

	int doflinear=0;
	int dofrank=0;
	//Calculating Event Rate [Evts/Day]
	for(int i=0;i<nbins;i++)
	{
		ratesigma[i]=sqrt(rate[i]);
		rate[i]/=livetime[i];
		ratesigma[i]/=livetime[i];
		//cout<<rate[i]<<" +- "<<ratesigma[i]<<endl;	
	}

	// Calculating correlation coefficient
	double sumx=0,sumy=0;
	double sumxy=0;
	double sumxsquare=0,sumysquare=0;
	for(int i=0;i<nbins;i++)
	{
		if(cut[i])
		{
			sumx+=scmean[i];
			sumy+=rate[i];
			sumxy+=scmean[i]*rate[i];
			sumxsquare+=pow(scmean[i],2.0);
			sumysquare+=pow(rate[i],2.0);
			doflinear++;
		}
	}
	double colinear=0;
	colinear=(doflinear*sumxy-sumx*sumy);
	colinear/=sqrt(doflinear*sumxsquare-pow(sumx,2.0));
	colinear/=sqrt(doflinear*sumysquare-pow(sumy,2.0));
	// Rank Coefficient
	int ranksc[nbins],rankrate[nbins],dist[nbins];
	double sumdistsquare=0;
	//Initialization
	for (int i=0;i<nbins;i++)
	{
		if(cut[i]&&rate[i]>0)
		{
			ranksc[i]=1;
			rankrate[i]=1;
			for(int j=0;j<nbins;j++)
			{
				if(scmean[i]>scmean[j]&&rate[j]>0&&cut[j])
					ranksc[i]++;
				if(rate[i]>rate[j]&&rate[j]>0&&cut[j])
					rankrate[i]++;
			}
			dist[i]=ranksc[i]-rankrate[i];
			sumdistsquare+=pow(dist[i],2.0);
			dofrank++;
		}
	}
	double rhorank=1.0-6.0*sumdistsquare/(dofrank*(pow(dofrank,2.0)-1.0));
	
	// For Plots Range
	double xmean=sumx/doflinear;
	double xsigma=sqrt((sumxsquare/doflinear-pow(xmean,2.0))*doflinear/(doflinear-1));
	double ymean=sumy/doflinear;
	double ysigma=sqrt((sumysquare/doflinear-pow(ymean,2.0))*doflinear/(doflinear-1));

	// For Statistical Tests
	gROOT->ProcessLine(".L correlationpdf.h");
	TF1 *pdf=new TF1("pdf",correlationpdf,0,1,1);
	pdf->SetParameter(0,doflinear-2);
	double problinear=2.0*pdf->Integral(TMath::Abs(colinear),1.0);
	pdf->SetParameter(0,dofrank-2);
	double probrank=2.0*pdf->Integral(TMath::Abs(rhorank),1.0);

	// Transform to Student't distribution
	double tlinear=colinear*sqrt(doflinear-2)/sqrt(1-pow(colinear,2.0));
	double trank=rhorank*sqrt(doflinear-2)/sqrt(1-pow(rhorank,2.0));
	double probtlinear=2.0*ROOT::Math::tdistribution_cdf_c(TMath::Abs(tlinear),doflinear-2);
	double probtrank=2.0*ROOT::Math::tdistribution_cdf_c(TMath::Abs(trank),dofrank-2);

	// Fisher Transform
	gROOT->ProcessLine(".L fisher.h");
	double fishermeanlinear=fisher(colinear);
	double fishermeanrank=fisher(rhorank);
	double fishersigma=1./sqrt(doflinear-3);
	double fishersigmarank=1./sqrt(dofrank-3);
	double probflinear=2.0*ROOT::Math::gaussian_cdf_c(TMath::Abs(fishermeanlinear),TMath::Abs(fishersigma),0);
	double probfrank=2.0*ROOT::Math::gaussian_cdf_c(TMath::Abs(fishermeanrank),TMath::Abs(fishersigmarank),0);

	//cout<<"For Web Wiki:"<<endl<<endl;
	cout<<setprecision(3);
	cout<<"| "<<colinear<<" | "<<rhorank<<" | "<<fishermeanlinear<<"+-"<<fishersigma<<" | "<<fishermeanrank<<"+-"<<fishersigmarank<<" | "<<problinear<<" | "<<probrank<<" |"<<endl<<endl;

	
	// Drawing the Background Rate and Radon Rate
	TCanvas *c1 = new TCanvas("c1", "",4,24,800,400);
	gStyle->SetOptStat(0);
	c1->Range(0.4313579,-45.68182,3.107479,-38.86364);
	c1->SetFillColor(0);
	c1->SetBorderMode(0);
	c1->SetBorderSize(2);
	c1->SetTickx();
	c1->SetTicky();
	c1->SetRightMargin(0.04);
	c1->SetLeftMargin(0.08);
	c1->SetTopMargin(0.02);
	c1->SetBottomMargin(0.1);
	c1->SetFrameBorderMode(0);
	c1->SetFrameBorderMode(0);

	double xlow=xmean-4.0*xsigma,xhigh=xmean+4.0*xsigma;
	double ylow=int(ymean-2.0*ysigma),yhigh=int(ymean+8.0*ysigma)+1;
	const char *XTitle=xtitle[filenumber];
	TH1 *frame = new TH2D("frame","",1000,xlow,xhigh,3000,ylow,yhigh);
	frame->SetStats(0);
	frame->GetYaxis()->SetTitle("Rate [Evts/Day]");
	frame->GetXaxis()->CenterTitle(true);
	frame->GetXaxis()->SetMoreLogLabels();
	frame->GetXaxis()->SetNoExponent();
	frame->GetXaxis()->SetLabelFont(132);
	frame->GetXaxis()->SetTitleFont(132);
	frame->GetXaxis()->SetTitle(XTitle);
	frame->GetYaxis()->CenterTitle(true);
	frame->GetYaxis()->SetLabelFont(132);
	frame->GetYaxis()->SetTitleOffset(0.6);
	frame->GetXaxis()->SetTitleOffset(0.8);
	frame->GetYaxis()->SetTitleSize(0.05);
	frame->GetXaxis()->SetTitleSize(0.05);
	frame->GetYaxis()->SetTitleFont(132);
	frame->GetZaxis()->SetLabelFont(132);
	frame->GetZaxis()->SetTitleFont(132);
	frame->Draw("");

	TGraphErrors *scrateplot=new TGraphErrors(nbins);
	for(int i=0;i<nbins;i++)
		{
		scrateplot->SetPoint(i,scmean[i],rate[i]);
		//scrateplot->SetPointError(i,scsigma[i],ratesigma[i]);
		scrateplot->SetPointError(i,0.1*scsigma[i],0);
		}
	scrateplot->SetLineColor(kBlack);
	scrateplot->SetLineWidth(2);
	scrateplot->Draw("Same&&E");

    TLegend *leg = new TLegend(0.1,0.8,0.6,0.95);
	leg->AddEntry(scrateplot,title,"lp"); 
	leg->SetFillColor(kWhite);
    leg->Draw("same");

	TString pdfname="pdf/"+picname+".pdf";
	TString pngname="png/"+picname+".png";
	c1->SaveAs(pdfname);
	c1->SaveAs(pngname);

}
