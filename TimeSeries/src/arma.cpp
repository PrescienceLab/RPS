#include "arma.h"
#include "tools.h"
#include "util.h"
#include "ar.h"
#include "ma.h"

#ifdef HAVE_NUMERICAL_RECIPES
extern "C" {
#include "nr.h"
#include "nrutil.h"
}
#endif //HAVE_NUMERICAL_RECIPES

ARMAModel::ARMAModel()
{
  phis=thetas=0;
}

ARMAModel::~ARMAModel()
{
  CHK_DEL_MAT(phis);
  CHK_DEL_MAT(thetas);
}

void ARMAModel::Initialize(int P, int Q)
{
  CHK_DEL_MAT(phis);
  CHK_DEL_MAT(thetas);
  
  p=P;
  q=Q;

  phis=new double [p];
  thetas= new double [q];
}

#define CHECKP(num) ((num)>=0 && (num)<p)
#define CHECKQ(num) ((num)>=0 && (num)<q)
#define ADJUSTP(num) ((num))
#define ADJUSTQ(num) ((num))

void ARMAModel::SetARCoeff(int num, double value)
{
  if (CHECKP(num)) {
    phis[ADJUSTP(num)]=value;
  }
}

double ARMAModel::GetARCoeff(int num)
{
  if (CHECKP(num)) {
    return phis[ADJUSTP(num)];
  } else {
	return 0.0;
  }
}

void ARMAModel::SetMACoeff(int num, double value)
{
  if (CHECKQ(num)) {
    thetas[ADJUSTQ(num)]=value;
  }
}

double ARMAModel::GetMACoeff(int num)
{
  if (CHECKQ(num)) {
    return thetas[ADJUSTQ(num)];
  } else {
	return 0.0;
  }
}

void ARMAModel::SetVariance(double var)
{
  variance=var;
}

double ARMAModel::GetVariance() 
{
  return variance;
}

double ARMAModel::EstimateVariance(double *seq, int len)
{
  Predictor *predictor=MakePredictor();
  int i;
  double ssd;
  double pred;

  predictor->Begin();
  // prime
  for (i=0;i<p+q;i++) {
    predictor->Step(seq[i]);
  }

  ssd=0.0;
  for (i=p+q;i<len;i++) {
    predictor->Predict(1,&pred);
    ssd+=SQUARE(pred-seq[i]);
    predictor->Step(seq[i]);
  }

  delete predictor;

  return ssd/((double)len);
}



void ARMAModel::SetMean(double mn)
{
  mean=mn;
}

double ARMAModel::GetMean()
{
  return mean;
}

int ARMAModel::GetP() 
{
  return p;
}

int ARMAModel::GetQ() 
{
  return q;
}


void ARMAModel::Dump(FILE *out)
{
  int i;

  if (out==0) {
    out=stdout;
  }

  fprintf(out,"ARMA(%d,%d) model with mean=%lf and noise variance=%lf\n",
              p,q,mean,variance);
  fprintf(out,"AR Coeffs:");

  for (i=0;i<p;i++) {
    fprintf(out," %lf",phis[i]);
  }

  fprintf(out,"\n");
  fprintf(out,"MA Coeffs:");

  for (i=0;i<q;i++) {
    fprintf(out," %lf",thetas[i]);
  }

  fprintf(out,"\n");
}


#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define MIN(x,y) ((x)<(y) ? (x) : (y))


Predictor *ARMAModel::MakePredictor()
{
   int i;
   Polynomial et, th;

   // Note that the internal model is of the form
   // (1-phi_1B-phi_2B^2-...)z_t = (1 - omega_1B - omega_2B -...) x_t
   // where omega_1 ... are in coeff[0..]
   // The eta-theta representation is identical

   et.SetPower(0);
   et.SetCoeff(0,1);
   for (i=0;i<p;i++) {
     et.SetCoeff(i+1,phis[i]);
   }

   th.SetPower(0);
   th.SetCoeff(0,1);
   for (i=0;i<q;i++) {
     th.SetCoeff(i+1,thetas[i]);
   }

   EtaThetaPredictor *pred = new EtaThetaPredictor;
   
   pred->Initialize(&et,&th,variance,mean);

   return pred;
}

ARMAModeler::ARMAModeler()
{
}

ARMAModeler::~ARMAModeler()
{
}





double ARMAConditionalSumOfSquares(double *seq, int len, 
				   double *phi, int numphi,
				   double *theta, int numtheta,
				   double maxreturn)
{
  double *a = new double [numtheta] ;
  double a2, temp;
  int i,j;

  for (i=0;i<numtheta;i++) {
    a[i]=0.0;
  }

  a2=0.0;
  for (i=0;i<len;i++) {
    // ~w_t = phi_1w_t-1 phi_2w_t-2....+theta_t-1a_t-1+theta_t-2a_t-2+...
    
    temp=0.0;

    for (j=0;j<MIN(numphi,i-1);j++) {
      temp += phi[j]*seq[i-j-1];
      if (fabs(temp)>maxreturn) {
	a2=maxreturn;
	goto done;
      }
    }

    for (j=1;j<=MIN(numtheta,i);j++) {
      temp += theta[j-1]*a[(i-j)%numtheta];
      if (fabs(temp)>maxreturn) {
	a2=maxreturn;
	goto done;
      }
    }

    a2+=SQUARE(temp-seq[i]);
    if (a2>maxreturn) { 
      a2=maxreturn;
      goto done;
    }
    if (numtheta>0) {
      a[i%numtheta]=temp-seq[i];
    }
  }
  done:
  CHK_DEL_MAT(a);
  return a2;
}

// The following is a wrapper to allow calling the NRC "powel()"
// routine for minimization of the likelihood
static int     ARMAConditionalSumOfSquaresNumAR;
static int     ARMAConditionalSumOfSquaresNumMA;
static double *ARMAConditionalSumOfSquaresSeq;
static int     ARMAConditionalSumOfSquaresLen;

extern "C" 
float  ARMAConditionalSumOfSquaresWrapper(float p[])
{
  double *pd = new double[ARMAConditionalSumOfSquaresNumAR
			  +ARMAConditionalSumOfSquaresNumMA];
  int i;
  for (i=0;i<ARMAConditionalSumOfSquaresNumMA+
	     ARMAConditionalSumOfSquaresNumAR;i++) {
    pd[i] = p[i+1];
  }

  float result = (float)ARMAConditionalSumOfSquares(ARMAConditionalSumOfSquaresSeq,
					     ARMAConditionalSumOfSquaresLen,
					     &(pd[0]), 
					     ARMAConditionalSumOfSquaresNumAR,
					     &(pd[ARMAConditionalSumOfSquaresNumAR]),
					     ARMAConditionalSumOfSquaresNumMA,
					     1e20);

  CHK_DEL_MAT(pd);
  return result;
}

				   
ARMAModel *ARMAModeler::Fit(double *seq, int len, int P, int Q)
{
#ifndef HAVE_NUMERICAL_RECIPES
  fprintf(stderr,"Attempt to fit ARMA model with no numerical recipes.\n");
  return (ARMAModel *) 0;
#else
  int numparam = P+Q;
  int i,j;
  float *p = vector(1,numparam);
  float **xi = matrix(1,numparam,1,numparam);
  double mean;

  double *workseq = new double [len];
  memcpy(workseq,seq,sizeof(double)*len);

  mean=MeanifySequence(workseq,len);

  for (i=1;i<=numparam;i++) {
    p[i]=0.0;
    for (j=1;j<=numparam;j++) { 
      if (i==j) { 
	xi[i][j]=1.0;
      } else {
	xi[i][j]=0.0;
      }
    }
  }

  float ftol=1e-5;
  float fret;
  int iter;

  ARMAConditionalSumOfSquaresNumAR=P;
  ARMAConditionalSumOfSquaresNumMA=Q;
  ARMAConditionalSumOfSquaresSeq=workseq;
  ARMAConditionalSumOfSquaresLen=len;

  powell(p,xi,numparam,ftol,&iter,&fret,ARMAConditionalSumOfSquaresWrapper);

  //fprintf(stderr,"powell() finished after %d iterations\n",iter);

  // no neeed - we use temporary
  //UnMeanifySequence(seq,len,mean);

  ARMAModel *model;

  model = new ARMAModel;
  model->Initialize(P,Q);
  model->SetMean(mean);
  for (i=0;i<P;i++) {
     model->SetARCoeff(i,(double)(p[i+1]));
  }
  for (i=0;i<Q;i++) {
     model->SetMACoeff(i,(double)(p[P+i+1]));
  }
  model->SetVariance(((double)fret)/((double)len));

  free_matrix(xi,1,numparam,1,numparam);
  free_vector(p,1,numparam);

  CHK_DEL_MAT(workseq);

  return model;

#if 0
  int i,j;
  double mean;
  ARModeler arm;
  MAModeler mam;
  ARModel *ar;
  MAModel *ma;
  

  if (len<=0) { 
    return 0;	
  }

  double *acovf = new double [P+Q+1];

  if (acovf==0) {
    return 0;
  }

  mean = ComputeAutoCov(seq,len,acovf,P+Q+1);

  ARMAModel *model = Fit(mean,acovf,P+Q+1,P,Q);

  CHK_DEL_MAT(acovf);

  return model;
#endif

#endif //HAVE_NUMERICAL_RECIPES

}

// NOTE: DO NOT USE THIS
ARMAModel *ARMAModeler::Fit(double mean, double *acovf, int len, 
			    int P, int Q)
{
#ifndef HAVE_NUMERICAL_RECIPES
  return (ARMAModel*)0;
#else
  ARMAModel *model;
  int i,j,k;
  double akk;

  if (len<=0 || len<(P+Q+1)) { 
    return 0;	
  }

  if (acovf[0]==0.0) { 
  // Yikes, no variation at all, so the right model is all zeros,
  // which we implement here to avoid stability problems with main
  // evaluation loop
    model = new ARMAModel;
    if (model==0) { 
      return 0;
    }
    model->Initialize(P,Q);
    for (i=0;i<P;i++) {
      model->SetARCoeff(i,0.0);
    }
    for (i=0;i<Q;i++) {
      model->SetMACoeff(i,0.0);
    }
    model->SetVariance(0.0);
    return model;
  }

  // Otherwise there is some variation and we are set for more
  // exciting things.

  // First fit the AR components
  double *phis = new double [P];

  SolveToeplitz(&(acovf[Q-P+1]),phis,&(acovf[Q+1]),P);


  // Now compute the adjusted autocovariance
  double *newcovf = new double [len];
  double temp;

  for (j=0;j<len;j++) {
    newcovf[j]=0;
    for (i=0;i<P;i++) {
      newcovf[j]+=phis[i]*phis[i]*acovf[j];
      temp=-phis[i];
      for (k=1;k<P-i;k++) {
	temp+=phis[k]*phis[i+k];
      }
      temp*=acovf[j+i]+acovf[j-i];
      newcovf[j]+=temp;
    }
  }
  
  // Estimate the MA parameters

  MAModeler mamod;
  double *omega = new double [Q]; 

  MAModel *mamodel = mamod.Fit(mean,newcovf,Q,Q);
  model = new ARMAModel;
  model->Initialize(P,Q);
  model->SetVariance(mamodel->GetVariance());
  model->SetMean(mean);
  for (i=0;i<P;i++) {
     model->SetARCoeff(i,phis[i+1]);
  }
  for (i=0;i<Q;i++) {
     model->SetMACoeff(i,mamodel->GetCoeff(i));
  }
  delete mamodel;
  CHK_DEL_MAT(omega);
  return model;
#endif //HAVE_NUMERICAL_RECIPES
}


Model *ARMAModeler::Fit(double *seq, int len, const ParameterSet &ps)
{
  int p,d,q;
  
  ((const PDQParameterSet &)ps).Get(p,d,q);
  
  return Fit(seq,len,p,q);
}
			  
  

  
