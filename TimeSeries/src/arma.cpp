#include <new>
#include "arma.h"
#include "tools.h"
#include "util.h"
#include "ar.h"
#include "ma.h"

#include "rps_log.h"

#include <string.h>

#include "nr-internal.h"
using namespace nrc;

ARMAModel::ARMAModel(const ARMAModel &rhs)
{
  phis=thetas=0;
  Initialize(rhs.p, rhs.q);
  memcpy(phis,rhs.phis,sizeof(phis[0])*p);
  memcpy(thetas,rhs.thetas,sizeof(thetas[0])*q);
  variance=rhs.variance;
  mean=rhs.mean;
}

ARMAModel::ARMAModel()
{
  phis=thetas=0;
}

ARMAModel::~ARMAModel()
{
  CHK_DEL_MAT(phis);
  CHK_DEL_MAT(thetas);
}

ARMAModel & ARMAModel::operator=(const ARMAModel &rhs)
{
  this->~ARMAModel();
  return *(new(this)ARMAModel(rhs));
}

void ARMAModel::Initialize(const int P, const int Q)
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

void ARMAModel::SetARCoeff(const int num, const double value)
{
  if (CHECKP(num)) {
    phis[ADJUSTP(num)]=value;
  }
}

double ARMAModel::GetARCoeff(const int num) const 
{
  if (CHECKP(num)) {
    return phis[ADJUSTP(num)];
  } else {
	return 0.0;
  }
}

void ARMAModel::SetMACoeff(const int num, const double value)
{
  if (CHECKQ(num)) {
    thetas[ADJUSTQ(num)]=value;
  }
}

double ARMAModel::GetMACoeff(const int num) const 
{
  if (CHECKQ(num)) {
    return thetas[ADJUSTQ(num)];
  } else {
	return 0.0;
  }
}

void ARMAModel::SetVariance(const double var)
{
  variance=var;
}

double ARMAModel::GetVariance() const 
{
  return variance;
}

double ARMAModel::EstimateVariance(const double *seq, const int len) const 
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



void ARMAModel::SetMean(const double mn)
{
  mean=mn;
}

double ARMAModel::GetMean() const 
{
  return mean;
}

int ARMAModel::GetP() const 
{
  return p;
}

int ARMAModel::GetQ() const 
{
  return q;
}


void ARMAModel::Dump(FILE *out) const 
{
  int i;

  if (out==0) {
    out=stdout;
  }

  fprintf(out,"ARMA(%d,%d) model with mean=%f and noise variance=%f\n",
              p,q,mean,variance);
  fprintf(out,"AR Coeffs:");

  for (i=0;i<p;i++) {
    fprintf(out," %f",phis[i]);
  }

  fprintf(out,"\n");
  fprintf(out,"MA Coeffs:");

  for (i=0;i<q;i++) {
    fprintf(out," %f",thetas[i]);
  }

  fprintf(out,"\n");
}

ostream & ARMAModel::operator<<(ostream &os) const 
{
  os << "ARMAModel(p="<<p<<", q="<<q<<", mean="<<mean<<", variance="<<variance<<", phis=(";
  int i;
  for (i=0;i<p;i++) {
    if (i>0) {
      os <<", ";
    }
    os << phis[i];
  }
  os << "), thetas=(";
  for (i=0;i<q;i++) {
    if (i>0) {
      os <<", ";
    } 
    os <<thetas[i];
  }
  os << "))";
  return os;
}


#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define MIN(x,y) ((x)<(y) ? (x) : (y))


Predictor *ARMAModel::MakePredictor() const
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

ARMAModeler::ARMAModeler(const ARMAModeler &rhs) 
{
}

ARMAModeler::~ARMAModeler()
{
}


double ARMAConditionalSumOfSquares(const double *seq, const int len, 
				   const double *phi, const int numphi,
				   const double *theta, const int numtheta,
				   const double maxreturn)
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

				   
ARMAModel *ARMAModeler::Fit(const double *seq, const int len, const int P, const int Q)
{
  int numparam = P+Q;
  int i,j;
  float *p = nrc::vector(1,numparam);
  float **xi = nrc::matrix(1,numparam,1,numparam);
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

  float ftol=(float)1e-5;
  float fret;
  int iter;

  ARMAConditionalSumOfSquaresNumAR=P;
  ARMAConditionalSumOfSquaresNumMA=Q;
  ARMAConditionalSumOfSquaresSeq=workseq;
  ARMAConditionalSumOfSquaresLen=len;

  powell(p,xi,numparam,ftol,&iter,&fret,ARMAConditionalSumOfSquaresWrapper);

  RPSLog(CONTEXT,10,"powell() finished after %d iterations\n",iter);

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

}

// NOTE: DO NOT USE THIS
ARMAModel *ARMAModeler::Fit(const double mean, const double *acovf, const int len, 
			    const int P, const int Q)
{
  ARMAModel *model;
  int i,j,k;

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
}


Model *ARMAModeler::Fit(const double *seq, const int len, const ParameterSet &ps)
{
  int p,d,q;
  
  ((const PDQParameterSet &)ps).Get(p,d,q);
  
  return Fit(seq,len,p,q);
}
			  

void ARMAModeler::Dump(FILE *out) const
{
  fprintf(out,"ARMAModeler()\n");
}

ostream & ARMAModeler::operator<<(ostream &os) const
{
  os << "ARMAModeler()";
  return os;
}
  

  
