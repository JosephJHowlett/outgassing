	double correlationpdf(double *corr,double *dof)
	{
		double prob=1.0/sqrt(3.14159265);
		prob*=ROOT::Math::tgamma(0.5*(dof[0]+1.0));
		prob*=1.0/ROOT::Math::tgamma(0.5*dof[0]);
		prob*=pow(1.0-corr[0]*corr[0],0.5*dof[0]-1.0);
		return prob;
	}
