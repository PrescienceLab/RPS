#include <new>
#include "ma.h"
#include "tools.h"
#include "util.h"
#include <string.h>

#include "rps_log.h"

#include "nr-internal.h"
using namespace nrc;

#include "pdqparamsets.h"

MAModel::MAModel()
{
  coeffs=0;
}

MAModel::MAModel(const MAModel &rhs)
{
  Initialize(rhs.order);
  memcpy(coeffs,rhs.coeffs,sizeof(coeffs[0])*order);
  variance=rhs.variance;
  mean=rhs.mean;
}

MAModel::~MAModel()
{
  if (coeffs!=0) {
    delete coeffs;
  }
}

MAModel & MAModel::operator=(const MAModel &rhs)
{
  return *(new(this) MAModel(rhs));
}


void MAModel::Initialize(const int order)
{
  if (coeffs!=0) {
    delete coeffs;
  }
  
  this->order=order;

  coeffs=new double [order];
}

#define CHECK(num) ((num)>=0 && (num)<order)
#define ADJUST(num) ((num))

void MAModel::SetCoeff(const int num, const double value)
{
  if (CHECK(num)) {
    coeffs[ADJUST(num)]=value;
  }
}

double MAModel::GetCoeff(const int num) const 
{
  if (CHECK(num)) {
    return coeffs[ADJUST(num)];
  } else {
	return 0.0;
  }
}
void MAModel::SetVariance(const double var)
{
  variance=var;
}

double MAModel::GetVariance() const 
{
  return variance;
}

double MAModel::EstimateVariance(const double *seq, const int len) const 
{
  Predictor *p=MakePredictor();
  int i;
  double ssd;
  double pred;

  p->Begin();
  // prime
  for (i=0;i<order;i++) {
    p->Step(seq[i]);
  }

  ssd=0.0;
  for (i=order;i<len;i++) {
    p->Predict(1,&pred);
    ssd+=SQUARE(pred-seq[i]);
    p->Step(seq[i]);
  }

  delete p;

  return ssd/((double)len);
}
  

void MAModel::SetMean(const double mn)
{
  mean=mn;
}

double MAModel::GetMean() const 
{
  return mean;
}

int MAModel::GetOrder() const 
{
  return order;
}

void MAModel::Dump(FILE *out) const 
{
  if (out==0) {
    out=stdout;
  }

  fprintf(out,"MA(%d) model with mean=%f and noise variance=%f\n",
              order,mean,variance);
  fprintf(out,"Coeffs:");

  int i;

  for (i=0;i<order;i++) {
    fprintf(out," %f",coeffs[i]);
  }

  fprintf(out,"\n");
}

ostream & MAModel::operator<<(ostream &os) const 
{
  os <<"MAModel(q="<<order<<", mean="<<mean<<", variance="<<variance<<", coeffs=(";
  for (int i=0;i<order;i++) {
    if (i>0) {
      os <<", ";
    }
    os <<coeffs[i];
  }
  os <<"))";
  return os;
}



#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define MIN(x,y) ((x)<(y) ? (x) : (y))


Predictor *MAModel::MakePredictor() const
{
   int i;
   Polynomial et, th;

   // Note that the internal model is of the form
   // z_t = (1 - omega_1B - omega_2B -...) x_t
   // where omega_1 ... are in coeff[0..]
   // The eta-theta representation is identical

   et.SetPower(0);
   et.SetCoeff(0,1);

   th.SetPower(0);
   th.SetCoeff(0,1);
   for (i=0;i<order;i++) {
     th.SetCoeff(i+1,coeffs[i]);
   }

   EtaThetaPredictor *pred = new EtaThetaPredictor;
   
   pred->Initialize(&et,&th,variance,mean);

   return pred;
}

MAModeler::MAModeler()
{
}

MAModeler::MAModeler(const MAModeler &rhs)
{
}

MAModeler::~MAModeler()
{
}

MAModeler & MAModeler:: operator=(MAModeler &rhs)
{
  return *(new(this)MAModeler(rhs));
}

double MAConditionalSumOfSquares(const double *seq, const int len, 
				 const double *coeff, const int numcoeff,
				 const double maxreturn)
{
  double *a = new double [numcoeff];
  double a2, temp;
  int i,j;

  for (i=0;i<numcoeff;i++) {
    a[i]=0;
  }

  a2=0.0;
  for (i=0;i<len;i++) {
    temp=seq[i];
    for (j=1;j<=MIN(numcoeff,i);j++) {
      temp+=coeff[j-1]*a[(i-j)%numcoeff];
      if (fabs(temp)>maxreturn) {
	a2=maxreturn;
	goto done;
      }
    }
    a[i%numcoeff]=temp;
    a2+=SQUARE(a[i%numcoeff]);
    if (a2>maxreturn) { 
      a2=maxreturn;
      goto done;
    }
  }
  done:
  CHK_DEL_MAT(a);
  return a2;
}

// The following is a wrapper to allow calling the NRC "powel()"
// routine for minimization of the likelihood
static int MAConditionalSumOfSquaresNumParams;
static double *MAConditionalSumOfSquaresSeq;
static int MAConditionalSumOfSquaresLen;

extern "C" 
float  MAConditionalSumOfSquaresWrapper(float p[])
{
  double *pd = new double[MAConditionalSumOfSquaresNumParams];
  int i;
  for (i=0;i<MAConditionalSumOfSquaresNumParams;i++) {
    pd[i] = p[i+1];
  }

  float result = (float)MAConditionalSumOfSquares(MAConditionalSumOfSquaresSeq,
					   MAConditionalSumOfSquaresLen,
					   pd, 
					   MAConditionalSumOfSquaresNumParams,	
					   1e20);

  CHK_DEL_MAT(pd);
  return result;
}
				   


// Note: NOT FUNCTIONAL AS OF 5/21/98
double MAUnConditionalSumOfSquares(const double *seq, const int len, 
                                   const double *coeff, const int numcoeff)
{
  double *a = new double [numcoeff];
  double *e = new double [numcoeff];
  double *seq_prev = new double [numcoeff];
  double a2, temp;
  int i,j;

  // Compute es backward
  for (i=0;i<2*numcoeff;i++) {
    e[i]=0;
  }

  for (i=len-1;i>=-numcoeff;i--) {
    temp=seq[i];
    for (j=1;j<=MIN(numcoeff,(len-1)-i);j++) {
       temp+=coeff[j-1]*e[(i+j+numcoeff)%(numcoeff)];
    }
    e[(i+numcoeff)%(numcoeff)]=temp;
  }
  // now use these numcoeff backward predicted values to 
  // estimate the unknown prior values of seq
  for (i=0;i<numcoeff;i++) {
    seq_prev[i]=0;
    for (j=0;j<i+1;j++) {
    }
  }


  for (i=0;i<numcoeff;i++) {
    a[i]=0;
  }

  a2=0.0;
  for (i=0;i<len;i++) {
    temp=seq[i];
    for (j=1;j<=MIN(numcoeff,i);j++) {
      temp+=coeff[j-1]*a[(i-j)%numcoeff];
    }
    a[i%numcoeff]=temp;
    a2+=SQUARE(a[i%numcoeff]);
  }
  CHK_DEL_MAT(a);
  return a2;
}


MAModel *MAModeler::Fit(const double *seq, const int len, const int maxord)
{
  int i,j;
  float *p = nrc::vector(1,maxord);
  float **xi = nrc::matrix(1,maxord,1,maxord);
  double mean;
  
  double *workseq = new double [len];
  memcpy(workseq,seq,sizeof(double)*len);

  mean=MeanifySequence(workseq,len);

  for (i=1;i<=maxord;i++) {
    p[i]=0.0;
    for (j=1;j<=maxord;j++) { 
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

  MAConditionalSumOfSquaresNumParams=maxord;
  MAConditionalSumOfSquaresSeq=workseq;
  MAConditionalSumOfSquaresLen=len;

  powell(p,xi,maxord,ftol,&iter,&fret,MAConditionalSumOfSquaresWrapper);

  RPSLog(CONTEXT,10,"powell() finished after %d iterations\n",iter);

  MAModel *model;

  model = new MAModel;
  model->Initialize(maxord);
  model->SetMean(mean);
  for (i=0;i<maxord;i++) {
     model->SetCoeff(i,(double)(p[i+1]));
  }
  model->SetVariance(((double)fret)/((double)len));

  free_matrix(xi,1,maxord,1,maxord);
  free_vector(p,1,maxord);
  
  CHK_DEL_MAT(workseq);

  return model;

#if 0
  int i,j;
  double mean;

  if (len<=0) { 
    return 0;	
  }

  double *acovf = new double [maxord+1];

  if (acovf==0) {
    return 0;
  }

  mean = ComputeAutoCov(seq,len,acovf,maxord+1);

  MAModel *model = Fit(mean,acovf,maxord+1,maxord);

  CHK_DEL_MAT(acovf);

  return model;
#endif
}

// NOTE: DO NOT USE THIS
MAModel *MAModeler::Fit(const double mean, const double *acovf, const int len, const int maxord)
{
  MAModel *model;
  int i,j,k;

  if (len<=0 || len<maxord) { 
    return 0;	
  }
  if (len<maxord) {
    return 0;
  }

  if (acovf[0]==0.0) { 
  // Yikes, no variation at all, so the right model is all zeros,
  // which we implement here to avoid stability problems with main
  // evaluation loop
    model = new MAModel;
    if (model==0) { 
      return 0;
    }
    model->Initialize(maxord);
    for (i=0;i<maxord;i++) {
      model->SetCoeff(i,0.0);
    }
    model->SetVariance(0.0);
    return model;
  }

  // Otherwise there is some variation and we are set for more
  // exciting things.

  // 1..maxord are our coefficients
  double *omega = new double [maxord+1]; 
  double var, lastvar;
  const double minimprovement=0.001;
  double curimprovement;

  for (i=0;i<maxord+1;i++) {
    omega[i]=0.0;
  }

  var=acovf[0]/minimprovement;
  curimprovement = 1.0;

  for (i=0;curimprovement>minimprovement;i++) {
    lastvar=var;
    var = 1 ;
    for (j=1;j<maxord+1;j++) {
      var += omega[j] * omega[j];
    }
    var = acovf[0] / var;
    for (j=maxord;j>0;j--) {
      omega[j] = -acovf[j]/var;
      for (k=j+1;k<maxord+1;k++) {
	omega[j] += omega[k-j]*omega[k];
      }
    }
    curimprovement=fabs(lastvar-var);
  }
  model = new MAModel;
  model->Initialize(maxord);
  model->SetVariance(var);
  model->SetMean(mean);
  for (i=0;i<maxord;i++) {
     model->SetCoeff(i,omega[i+1]);
  }
  CHK_DEL_MAT(omega);
  return model;
}
			  

Model *MAModeler::Fit(const double *seq, const int len, const ParameterSet &ps)
{
  int p,d,q;

  ((const PDQParameterSet &)ps).Get(p,d,q);
  
  return Fit(seq,len,q);
}


void   MAModeler::Dump(FILE *out) const
{
  fprintf(out,"MAModeler()");
}

ostream & MAModeler::operator<<(ostream &os) const
{
  os <<"MAModeler()";
  return os;
}

  
