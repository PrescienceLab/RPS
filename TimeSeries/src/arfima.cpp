#include <new>
#include <string.h>
#include "arfima.h"
#include "ar.h"
#include "poly.h"
#include "etathetapred.h"
#include "maths.h"
#include "util.h"
#include "tools.h"

#include "rps_log.h"

ARFIMAModel::ARFIMAModel() 
{
  p=q=0;
  d=0.0;
  phis=thetas=0;
  variance=0;
}

ARFIMAModel::ARFIMAModel(const ARFIMAModel &rhs)
{
  Initialize(rhs.p, rhs.d, rhs.q);
  memcpy(phis,rhs.phis,sizeof(phis[0])*p);
  memcpy(thetas,rhs.thetas,sizeof(thetas[0])*q);
  variance=rhs.variance;
  mean=rhs.variance;
}

ARFIMAModel &ARFIMAModel::operator=(const ARFIMAModel &rhs)
{
  this->~ARFIMAModel();
  return *(new(this) ARFIMAModel(rhs));
}
 

ARFIMAModel::~ARFIMAModel()
{
   CHK_DEL_MAT(phis);
   CHK_DEL_MAT(thetas);
}

int ARFIMAModel::Initialize(const int P, const double D, const int Q)
{
   p=P;
   d=D;
   q=Q;
   variance=0.0;

   CHK_DEL_MAT(phis);
   CHK_DEL_MAT(thetas);

   phis = new double [p];
   thetas = new double [q];

   return 0;
}

int ARFIMAModel::GetP() const
{
   return p;
}

int ARFIMAModel::GetQ() const 
{
   return q;
}

double ARFIMAModel::GetD() const
{
   return d;
}


void ARFIMAModel::SetARCoeff(const int num, const double value)
{
   if (num>=0 && num<p) {
      phis[num]=value;
   }
}

void ARFIMAModel::SetMACoeff(const int num, const double value)
{
   if (num>=0 && num<p) {
      thetas[num]=value;
   }
}

double ARFIMAModel::GetARCoeff(const int num) const
{
   if (num<0 || num>=p) {
      return 0.0;
   }
   return phis[num];
}

double ARFIMAModel::GetMACoeff(const int num) const
{
   if (num<0 || num>=q) {
      return 0.0;
   }
   return thetas[num];
}

void ARFIMAModel::SetVariance(const double variance)
{
   this->variance=variance;
}

double ARFIMAModel::GetVariance() const
{
   return variance;
}

double ARFIMAModel::EstimateVariance(const double *seq, const int len) const
{
  Predictor *predictor=MakePredictor();
  int i;
  double ssd;
  double pred;

  predictor->Begin();
  // prime
  for (i=0;i<p+q+d;i++) {
    predictor->Step(seq[i]);
  }

  ssd=0.0;
  for (i=p+q+(int)ceil(d);i<len;i++) {
    predictor->Predict(1,&pred);
    ssd+=SQUARE(pred-seq[i]);
    predictor->Step(seq[i]);
  }

  delete predictor;

  return ssd/((double)len);
}

void ARFIMAModel::SetMean(const double mean)
{
   this->mean=mean;
}

double ARFIMAModel::GetMean() const
{
   return mean;
}


void ARFIMAModel::Dump(FILE *out) const
{
  fprintf(out,"ARFIMA(%d,%f,%d) model\n",p,d,q);
  fprintf(out,"Phis (AR coeffs):");

  int i;

  for (i=0;i<p;i++) {
    fprintf(out," %f",phis[i]);
  }
  fprintf(out,"\n");
  fprintf(out,"Thetas (MA coeffs):");
  for (i=0;i<q;i++) {
    fprintf(out," %f",thetas[i]);
  }
  fprintf(out,"\nNoise Variance=%f\n",variance);
}

ostream &ARFIMAModel::operator<<(ostream &os) const
{
  os << "ARFIMAModel(p="<<p<<", d="<<d<<", q="<<q<<", noisevariance="<<variance<<", phis=(";
  int i;
  for (i=0;i<p;i++) {
    if (i>0) { 
      os << "," << phis[i];
    } else {
      os << phis[i];
    }
  }
  os << "), thetas=(";
  for (i=0;i<q;i++) {
    if (i>0) { 
      os << "," << thetas[i];
    } else {
      os << thetas[i];
    }
  }
  os << "))";
  return os;
}

Predictor * ARFIMAModel::MakePredictor(const int truncationlimit) const
{
   int i;
   Polynomial et,th, *dh;

   // Negate all but the first coefficient since an 
   // in order to do the multiply right, since
   // phi(B) = 1 - phi_1B - phi_2B^2 ...
   // and Polynomials expect 1 + (-phi_1)B + (-phi_2)B^2 ...
   et.SetPower(0);
   et.SetCoeff(0,1); // should be 1
   for (i=1;i<p;i++) {
      et.SetCoeff(i,-phis[i]);
   }
   dh = MakeDeltaFracD(d,truncationlimit);
   et.MultiplyBy(dh);

   // Now, since the eta representation is of form
   // 1 - eta_1B - eta_2B^2... we must negate the coefficients
   for (i=1;i<et.GetNumCoeffs();i++) {
     et.SetCoeff(i,-(et.GetCoeff(i)));
   }

   // Theta representations are the same
   th.SetPower(0);
   for (i=0;i<q;i++) {
      th.SetCoeff(i,thetas[i]);
   }

   EtaThetaPredictor *pred = new EtaThetaPredictor;
   pred->Initialize(&et,&th,variance,mean);

   return pred;
}

Predictor * ARFIMAModel::MakePredictor() const {
   return MakePredictor((int) 100*(p+q));
}


ARFIMAModeler::ARFIMAModeler()
{
}

ARFIMAModeler::ARFIMAModeler(const ARFIMAModeler &rhs)
{
}

ARFIMAModeler::~ARFIMAModeler()
{
}

ARFIMAModeler & ARFIMAModeler::operator=(const ARFIMAModeler &rhs)
{
  this->~ARFIMAModeler();
  return *(new(this) ARFIMAModeler(rhs));
}


// No model is fitted if you don't have the fracdiff package
#ifndef HAVE_FRACDIFF
Model *ARFIMAModeler::Fit(const double *sequence, const int len, const int p, const double d, const int q)
{
  fprintf(stderr,"Attempt to fit ARFIMA model with no fracdiff package\n");
   return 0;
}
#else
// Prototypes for the fortran codes we will call
extern "C"
void fracdf_(float  *x,         // [in] sequence
	     int    *n,         // [in] length of sequence
             int    *M,         // [in] number of terms in likelihood estimate (sug 100)
	     int    *nar,       // [in] number of AR terms
	     int    *nma,       // [in] number of MA terms
	     double *dtol,      // [in] length of final interval of d (<0=> chooses
	     double  drange[2], // [in] min and max values for d
	     double *h,         // [in] finite diff interval (<0 => reset
	     double *hood,      // [out] log max likelihood
             double *d,         // [inout] initial est / optimal fracdiff parameter
	     double *ar,        // [inout] initial est/ optimal AR parameters
             double *ma,        // [inout] initial est/ optimal MA parameters
             double *cov,       // [out] covariance matrix of parameter estimates
	     int    *lcov,      // [in] rows of cov array (min nar+nma+1)
	     double *stderror,  // [out] standard errors of param estimates
             double *cor,       // [out] correlation matrix of paramters
             int    *lcor,      // [in] rows of cor array (min nar+nma+1)
	     double *hess,      // [out] hessian matrix of parameter estimates
	     int    *lhess,     // [in] rows of hess
	     double *w,         // [in] work array
	     int    *lenw,      // [in] size of work array
	     int    *iprint);   // [in] print intermediate results if >0
// minimum  size of work array is
// max(nar+nma+2*(n+M),3*n+(n+6.5)*(nar+nma)+1,(3+2*(nar+nma+1))*(nar+nma+1)+1)

extern "C"
void  fdvar_(float  *x,         // [in] sequence
	     int    *n,         // [in] length of sequence
             int    *M,         // [in] number of terms in likelihood estimate (sug 100)
	     int    *nar,       // [in] number of AR terms
	     int    *nma,       // [in] number of MA terms
	     double *dtol,      // [in] length of final interval of d (<0=> chooses
	     double  drange[2], // [in] min and max values for d
	     double *h,         // [in] finite diff interval (<0 => reset
	     double *hood,      // [out] log max likelihood
             double *d,         // [inout] initial est / optimal fracdiff parameter
	     double *ar,        // [inout] initial est/ optimal AR parameters
             double *ma,        // [inout] initial est/ optimal MA parameters
             double *cov,       // [out] covariance matrix of parameter estimates
	     int    *lcov,      // [in] rows of cov array (min nar+nma+1)
	     double *stderror,  // [out] standard errors of param estimates
             double *cor,       // [out] correlation matrix of paramters
             int    *lcor,      // [in] rows of cor array (min nar+nma+1)
	     double *hess,      // [out] hessian matrix of parameter estimates
	     int    *lhess,     // [in] rows of hess
	     double *w,         // [in] work array
	     int    *lenw);     // [in] size of work array


Model *ARFIMAModeler::Fit(const double *sequence, const int lenlen, const int pp, const double dd, const int qq)
{
  int i;
  int len=lenlen;
  int p=pp;
  int q=qq;
  double d=dd;

  double mean=0.0;
  for (i=0;i<len;i++) {
    mean+=sequence[i];
  }
  mean/=len;

  // Meanify sequence
  float *x = new float[len];
  for (i=0;i<len;i++) {
     x[i]=(float)(sequence[i] - mean);
  }

  int M=100;
  double dtol=-1.0;      
  double drange[2] = {0.0,0.5};
  double h=-1.0; 
  double hood=0.0;
  double *ar = new double[p];
  for (i=0;i<p;i++) {
    ar[i]=0.0;
  }
  double *ma = new double[q];
  for (i=0;i<q;i++) {
    ma[i]=0.0;
  }
  int    matsize=(p+q+1)*(p+q+1);
  double *cov = new double [matsize];
  int    lcov = p+q+1;
  double *stderror = new double[matsize];
  double *cor = new double [matsize];
  int    lcor = p+q+1;
  double *hess = new double[matsize];
  int    lhess= p+q+1;
  int    lenw = (int)ceil(MAX((double)(p+q+2*(len+M)),
		    MAX(3*len+(len+6.5)*(p+q)+1,(double)((3+2*(p+q+1))*(p+q+1)+1))));
  double *w = new double [lenw];
  int    iprint=0;

#if 1
  for (i=0;i<matsize;i++) {
     cov[i]=cor[i]=hess[i]=0.0;
  }
  for (i=0;i<lenw;i++) {
     w[i]=0.0;
  }
#endif

  RPSLog(CONTEXT,10,"Allocated %d bytes of memory for call to fracdf\n",
	 sizeof(double)*(p+q+3*(p+q+1)*(p+q+1) + (p+q+1) + lenw) +
	 sizeof(float)*len);

  fracdf_(x,
	  &len,
          &M,
	  &p,
	  &q,
	  &dtol,
	  drange,
	  &h,
	  &hood, 
          &d,
	  ar,
          ma,
          cov,
	  &lcov,
	  stderror,
          cor,
          &lcor,
	  hess,
	  &lhess,
	  w,
	  &lenw,
	  &iprint);  

  /*
  h=.0001e0;
  fdvar_(x,
	  &len,
          &M,
	  &p,
	  &q,
	  &dtol,
	  drange,
	  &h,
	  &hood, 
          &d,
	  ar,
          ma,
          cov,
	  &lcov,
	  stderror,
          cor,
          &lcor,
	  hess,
	  &lhess,
	  w,
	  &lenw);
          */

  ARFIMAModel *model = new ARFIMAModel;
  
  model->Initialize(p+1,d,q+1);

  model->SetARCoeff(0,1.0);
  for (i=1;i<p+1;i++) {
    model->SetARCoeff(i,ar[i-1]);
  }
  model->SetMACoeff(0,1.0);
  for (i=1;i<q+1;i++) {
    model->SetMACoeff(i,ma[i-1]);
  }
  model->SetMean(mean);

  
  model->SetVariance(model->EstimateVariance(sequence,len));

  CHK_DEL_MAT(x);
  CHK_DEL_MAT(ar);
  CHK_DEL_MAT(ma);
  CHK_DEL_MAT(cov);
  CHK_DEL_MAT(stderror);
  CHK_DEL_MAT(cor);
  CHK_DEL_MAT(hess);
  CHK_DEL_MAT(w);

  return model;
 
  
}
#endif


Model *ARFIMAModeler::Fit(const double *seq, const int len, const ParameterSet &ps)
{
  int p,d,q;

  ((const PDQParameterSet &)ps).Get(p,d,q);
  
  return Fit(seq,len,p,(double)d,q);
}



void ARFIMAModeler::Dump(FILE *out) const
{
  fprintf(out,"ARFIMAModeler\n");
}

ostream &ARFIMAModeler::operator<<(ostream &os) const
{
  os << "ARFIMAModeler()";
  return os;
}

