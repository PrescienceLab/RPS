#include "tools.h"
#include "util.h"
#include <stdlib.h>
#include <math.h>
#include "maths.h"
#include "random.h"
#include <assert.h>

double Sum(double *seq, int len)
{
  int i;
  double sum;
  
  assert(len>0);

  sum=0.0;
  
  for (i=0;i<len;i++) {
    sum+=seq[i];
  }
  return sum;
}

double SumOfSquares(double *seq, int len)
{
  int i;
  double sum;
  
  assert(len>0);

  sum=0.0;
  
  for (i=0;i<len;i++) {
    sum+=seq[i]*seq[i];
  }
  return sum;
}


  
double Mean(double *seq, int len)
{
  int i;
  double mean;
  
  assert(len>0);

  mean=0.0;
  
  for (i=0;i<len;i++) {
    mean+=seq[i];
  }
  if (len>0) { 
    mean/=len;
  }
  return mean;
}

double StandardDeviation(double *seq, int len)
{
  assert(len>0);
  return sqrt(Variance(seq,len));
}

double Variance(double *seq, int len)
{
  int i;
  double mean;
  double var;

  assert(len>0);

  if (len<2) {
    return 0.0;
  }
  mean=Mean(seq,len);
  var=0.0;
  for (i=0;i<len;i++) {
    var+=SQUARE(seq[i]-mean);
  }
  var/=len-1;
  return var;
}

extern "C" int compare_doubles(const void *leftv, const void *rightv)
{
  const double &left=*((const double*)leftv);
  const double &right=*((const double*)rightv);
  if (left<right) {
    return -1;
  } else {
    if (left>right) {
      return 1;
    } else {
      return 0;
    }
  }
}

double Min(double *seq, int len)
{
  int i;

  assert(len>0);

  double min=seq[0];
  
  for (i=1;i<len;i++) {
    if (seq[i]<min) {
      min=seq[i];
    }
  }
  return min;
}

double Max(double *seq, int len)
{
  int i;

  assert(len>0);

  double max=seq[0];
  
  for (i=1;i<len;i++) {
    if (seq[i]>max) {
      max=seq[i];
    }
  }
  return max;
}
  
// Yes, this is pretty hideous
double Median(double *seq, int len)
{
  assert(len>0);

  double *copy = new double [len];
  memcpy(copy,seq,len*sizeof(double));

  qsort(copy,len,sizeof(double),compare_doubles);	
  
  double med = copy[len/2];
  
  CHK_DEL_MAT(copy);
  
  return med;
}
  
double MeanifySequence(double *seq, int len)
{
  int i;
  double mean;
  
  assert(len>0);
  mean=Mean(seq,len);
  for (i=0;i<len;i++) {
    seq[i]-=mean;
  }
  return mean;
}

void UnMeanifySequence(double *seq, int len, double mean)
{
  int i;

  assert(len>0);
  for (i=0;i<len;i++) {
    seq[i]+=mean;
  }

}

double DifferenceSequence(double *seq, int len)
{
  int i;
  double first=seq[0];

  assert(len>1);
  for (i=0;i<len-1;i++) {
    seq[i]=seq[i+1]-seq[i];
  }
  
  return first;
}

void   UnDifferenceSequence(double *seq, int len, double first)
{
  int i;

  assert(len>1);
  seq[1]=first+seq[0];
  seq[0]=first;

  for (i=2;i<len+1;i++) {
    seq[i]+=seq[i-1];
  }
}

double ComputeAutoCor(double *seq, int len, double *acf, int numlag)
{
  double mean;
  double acf0;
  int i;

  assert(len>numlag && numlag>0);

  mean = ComputeAutoCov(seq,len,acf,numlag);
  acf0 = acf[0]!=0.0 ? acf[0] : 1.0;    // check for zero variance sequence
  for (i=1;i<numlag;i++) {
    acf[i]/=acf0;
  }
  acf[0]=1;
  return mean;
}

double ComputeAutoCov(double *seq, int len, double *acovf, int numlag)
{
  double mean;
  int i,j;

  assert(len>numlag && numlag>0);

  // compute the mean
  mean=0.0;
  for (i=0;i<len;i++) { 
    mean+=seq[i];
  }
  mean/=len;

  // compute autocorrelation estimates

  for (i=0;i<numlag;i++) {
    acovf[i]=0.0;
    for (j=0;j<len-i-1;j++) {
      acovf[i]+=(seq[j]-mean)*(seq[j+i]-mean);
    }
    acovf[i]/=len;
  }

  return mean;
}

double AutoCov2VarianceOfSum(double *autocov, int num, double *sums)
{
  int i,j;

  if (num<=0) { 
    return 0.0;
  }

  sums[0] = autocov[0];

  for (i=1;i<num;i++) { 
    sums[i] = sums[i-1];
    sums[i]+=autocov[0];
    for (j=1;j<=i;j++) { 
      sums[i]+=2*autocov[j];
    }
  }
  return sums[num-1];
}

double AutoCov2VarianceOfSum(double *autocov, int numautocov, 
			     double *sumvars, int numsums)
{
  if (numsums<=numautocov) { 
    return AutoCov2VarianceOfSum(autocov,numsums,sumvars);
  } else {
    int i;
    double add;
    AutoCov2VarianceOfSum(autocov,numautocov,sumvars);
    // Keep adding the final difference for an estimate
    // 
    if (numautocov==1) { 
      add=autocov[0];
    } else {
      add=sumvars[numautocov-1]-sumvars[numautocov-2];
      for (i=numautocov;i<numsums;i++) { 
	sumvars[i]=sumvars[i-1] + ((double)(i)/((double)(numautocov-1)))*add;
      }
    }
    return sumvars[0];
  }
}


double AutoCov2Covariances(double *autocov, int numautocov,
			   double *covars, int numrows)
{
  int i, j;

  for (i=0;i<numrows;i++) { 
    for (j=i;j<numrows;j++) {
      covars[j*numrows+(j-i)]=covars[(j-i)*numrows+j]= i < numautocov ? autocov[i] : 0.0 ;
    }
  }
  return covars[0];
}
  

// Translated from Numerical Recipes in Fortran, Second Edition
// Solve Toeplitz Matrix
#define F(i) ((i)-1)
int SolveToeplitz(double *r, double *x, double *y, int n)
{      
  int j,k,m,m1,m2;
  double pp, pt1, pt2, qq, qt1, qt2, sd, sgd, sgn, shn, sxn;

  double *g = new double [n];
  double *h = new double [n];
  
  if (g==0 || h==0) {
    goto FAIL;
  }

  if (r[F(n)]==0) {
    goto FAIL;
  }

  x[F(1)] = y[F(1)]/r[F(n)];

  if (n==1) {
    return 0;
  }

  g[F(1)] = r[F(n-1)]/r[F(n)];
  
  h[F(1)] = r[F(n+1)]/r[F(n)];

  for (m=1;m<=n;m++) {
    m1=m+1;
    sxn=-y[F(m1)];
    sd=-r[F(n)];
    for (j=1;j<=m;j++) {
      sxn+=r[F(n+m1-j)]*x[F(j)];
      sd+=r[F(n+m1-j)]*g[F(m-j+1)];
    }
    if (sd==0) { 
      goto FAIL;
    }
    x[F(m1)]=sxn/sd;
    for (j=1;j<=m;j++) {
      x[F(j)]-=x[F(m1)]*g[F(m-j+1)];
    }
    if (m1==n) {
      goto SUCCEED;
    }
    sgn=-r[F(n-m1)];
    shn=-r[F(n+m1)];
    sgd=-r[F(n)];
    for (j=1;j<=m;j++) {
      sgn+=r[F(n+j-m1)]*g[F(j)];
      shn+=r[F(n+m1-j)]*h[F(j)];
      sgd+=r[F(n+j-m1)]*h[F(m-j+1)];
    }
    if (sd==0 || sgd==0) {
      goto FAIL;
    }
    g[F(m1)] = sgn/sgd;
    h[F(m1)] = shn/sd;
    k=m;
    m2=(m+1)/2;
    pp=g[F(m1)];
    qq=h[F(m1)];
    for (j=1;j<=m2;j++) {
      pt1=g[F(j)];
      pt2=g[F(k)];
      qt1=h[F(j)];
      qt2=h[F(k)];
      g[F(j)] = pt1-pp*qt2;
      g[F(k)] = pt2-pp*qt1;
      h[F(j)] = qt1-qq*pt2;
      h[F(k)] = qt2-qq*pt1;
      k--;
    }
  }

FAIL:
  CHK_DEL_MAT(g);
  CHK_DEL_MAT(h);
  return -1;

SUCCEED:
  CHK_DEL_MAT(g);
  CHK_DEL_MAT(h);
  return 0;
}


// returns the highest confidence level at which we can reject that the sequence
// is IID by the nunber of turning points
double TurningPointTestForIID(double *seq, int len, int *numtp)
{
  int nt;
  double ntf;
  double ntm, ntv, diff;
  int i;

  // count turning points in sequence
  nt=0;
  for (i=2;i<len;i++) {
    if ((seq[i-2] < seq[i-1] && seq[i-1] > seq[i]) ||
	(seq[i-2] > seq[i-1] && seq[i-1] < seq[i])) {
      ++nt;
    }
  }
  ntf = (double)nt;
  // mean number of turning points and variances
  ntm = 2*(len-2)/3;
  ntv = (16*len-29)/90;

  assert(ntv>0.0);
  
  //fprintf(stderr,"Found %f turning points, expect %f\n",nt,ntm);
  // our difference
  diff= fabs(ntf-ntm);

  if (numtp!=0) {
    *numtp=nt;
  }
  
  return SigmasToConf(diff/sqrt(ntv));
}
 

// returns the highest confidence level at which we can reject that the sequence
// is IID by the nunber of sign changes
double SignTestForIID(double *seq, int len, int *nums)
{
  int ns;
  double nsf, nsm, nsv, diff;
  int i;

  // count sign changes in sequence
  ns=0;
  for (i=1;i<len;i++) {
    if (seq[i] > seq[i-1]) {
      ++ns;
    }
  }
  nsf=(double)ns;
  // mean number of turning points and variances
  nsm = 0.5*(len-1);
  nsv = (len+1)/12.0;
  
  //fprintf(stderr,"Found %f turning points, expect %f\n",nt,ntm);
  // our difference
  diff= fabs(nsf-nsm);
  
  assert(nsv>0.0);

  if (nums!=0) {
    *nums=ns;
  }

  return SigmasToConf(diff/sqrt(nsv));
}

// returns the highest confidence level at which we can reject that the sequence
// is IID by the portmanteau test
// Ljung/Box variant
double PortmanteauTestForIID(double *seq, int len, double *qout, int maxlag)
{
  double Q;
  int i;

  assert(len>maxlag && maxlag>0);

  double *acf = new double [maxlag];

  ComputeAutoCor(seq,len,acf,maxlag);

  Q=0.0;
  for (i=1;i<maxlag;i++) {
    Q+=SQUARE(acf[i])/(len-i);
  }
  Q*=((double)len)*(((double)len)+2.0);

  // Now Q is expected to be distributed as chi^2 with maxlag degrees
  // of freedom
  
  //fprintf(stderr,"Q=%f\n",Q);

  CHK_DEL_MAT(acf);

  if (qout!=0) { 	
    *qout=Q;
  }

  return ChiSquaredCdf(maxlag,Q);
}


// returns the number m of acf component up to lag maxlag outside
// the +/- interval associated with the confidence level.
// To be IID at confidence level F,  m/maxlag should be < F
// This is a large sample test, so len should be at least maxlag+30
int AcfTestForIID(double *seq, int len, int maxlag, double conflevel)
{
  int    lag;
  double interval;
  int num;


  assert(len>maxlag && maxlag>0);

  double *acf= new double [maxlag];
  ComputeAutoCor(seq,len,acf,maxlag);

  interval = ConfToSigmas(conflevel)/sqrt(len-maxlag);

  for (lag=1, num=0;lag<maxlag;lag++) {
    if (fabs(acf[lag])>interval) {
      ++num;
    }
  }
  
  CHK_DEL_MAT(acf);
  return num;
}
  

// Returns the highest confidence level at which we can
// reject that the sequence is IID based on acf
double AcfTestForIID(double *seq, int len, int maxlag, int numiters)
{
  int    lag;
  double ci, interval;
  int i, num;


  assert(len>maxlag && maxlag>0 && numiters>0);

  double *acf= new double [maxlag];
  ComputeAutoCor(seq,len,acf,maxlag);

  for (ci=1.0,i=0;i<numiters;i++,ci-=1.0/(numiters)) {
    interval = ConfToSigmas(ci)/sqrt(len-maxlag);
    for (lag=1, num=0;lag<maxlag;lag++) {
      if (fabs(acf[lag])>interval) {
	//fprintf(stderr,"abs(%f)>%f\n",acf[lag],interval);
	++num;
      }
    }
    //fprintf(stderr,"expect %f found %f\n",1.0-ci, ((double)num)/((double)maxlag));

    if (((double)num)/((double)maxlag) > (1.0-ci)) {
      break;
    }
  }
  CHK_DEL_MAT(acf);
  return ci+1.0/(numiters);	
}


double QuantileQuantile(double *seq1, double *seq2, int len, double *m, double *b)
{  
  assert(len>0);

  double *xx=new double [len];
  double *yy=new double [len];
  double M,B;

  memcpy(xx,seq1,len*sizeof(double));
  memcpy(yy,seq2,len*sizeof(double));

  qsort(xx,len,sizeof(double),compare_doubles);	
  qsort(yy,len,sizeof(double),compare_doubles);	

  double R2 = LeastSquares(xx,yy,len,&M,&B);

  if (m!=0) {
    *m=M;
  }	
  if (b!=0) {
    *b=B;
  }
  
  CHK_DEL_MAT(yy);
  CHK_DEL_MAT(xx);

  return R2;
}

double QuantilesOfNormal(double *seq, int len, double *m, double *b)
{
  assert(len>0);

  double *nrm = new double [len];
  int i;

  for (i=0;i<len;i++) {
    nrm[i] = NormalRandom(0.0,1.0);
  }

  double R2 = QuantileQuantile(seq,nrm,len,m,b);

  CHK_DEL_MAT(nrm);

  return R2;
}


double LeastSquares(double *x, double *y, int len, double *m, double *b)
{
  double sumxy, sumx2, sumy2, meanx, meany;
  int i;

  if (len<2) {
    *m=0;
    if (len==1) {
      *b=y[0];
    } else {
      *b=0;
    }
  }

  sumxy=0.0;
  sumx2=0.0;
  sumy2=0.0;
  meanx=0.0;
  meany=0.0;
  for (i=0;i<len;i++) {
    meanx+=x[i];
    meany+=y[i];
    sumxy+=x[i]*y[i];
    sumx2+=SQUARE(x[i]);
    sumy2+=SQUARE(y[i]);
  }
  meanx/=len;
  meany/=len;

  double denom = (sumx2-len*SQUARE(meanx));

  if (denom==0.0) {
    denom=1e-99;
  }

  *m = (sumxy - len*meanx*meany) / denom;

  *b = meany - *m * meanx;

  double SST = sumy2 - len*meany;
  double SSE = sumy2 - (*b)*len*meany - (*m)*sumxy;

  if (SST==0.0) { 
    return 1.0;
  } else {
    return (SST-SSE)/SST;
  }
}

  








