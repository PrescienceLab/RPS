#include <algo.h>
#include <string.h>

#include "bestmedian.h"
#include "util.h"
#include "tools.h"
#include "pdqparamsets.h"

BestMedianModel::BestMedianModel(int order, double var)
{
  this->order=order;
  this->var=var;
}

BestMedianModel::~BestMedianModel()
{
}

void BestMedianModel::Dump(FILE *out)
{
  fprintf(out,"BestMedianModel(windowsize=%d, variance=%f)\n",order,var);
}

Predictor * BestMedianModel::MakePredictor()
{
  return new BestMedianPredictor(order, var);
}

BestMedianPredictor::BestMedianPredictor(int order, double var)
{
  this->order=order;
  this->var=var;
  numsamples=0;
}

BestMedianPredictor::~BestMedianPredictor()
{
  numsamples=0;
  order=0;
}

int BestMedianPredictor::Begin()
{
  numsamples=0;
  currentmedian=0;
  return 0;
}

int BestMedianPredictor::StepsToPrime()
{
  return 0;
}

double BestMedianPredictor::ComputeCurrentMedian()
{
  if (window.size()==0) { 
    return 0;
  }
  deque<double> temp=window;
  sort(temp.begin(),temp.end());

  if (temp.size()%2) {
    currentmedian=temp[temp.size()/2];
  } else {
    currentmedian=0.5*(temp[temp.size()/2]+temp[temp.size()/2-1]);
  }

#if 0
  fprintf(stderr,"Median of (");
  for (deque<double>::const_iterator v=window.begin(); v!=window.end(); ++v) {
    fprintf(stderr," %f",*v);
  }
  fprintf(stderr,") is %f\n",currentmedian);
#endif

  return currentmedian;
}

double BestMedianPredictor::Step(double obs)
{
  assert(window.size()<=(unsigned)order);

  if (window.size()==(unsigned)order) {
    window.pop_front();
  }
  window.push_back(obs);

  numsamples++;
  
  return ComputeCurrentMedian();
}

int BestMedianPredictor::Predict(int maxahead, double *preds)
{
  int i;
  for (i=0;i<maxahead;i++) { 
    preds[i] = currentmedian;
  }
  return 0;
}


int BestMedianPredictor::ComputeVariances(int maxahead, double *vars,
				      VarianceType vtype)
{
  int i,j;

  switch (vtype) { 
  case POINT_VARIANCES:
    for (i=0;i<maxahead;i++) { 
      vars[i]=var;
    }
    return 0;
    break;
  case SUM_VARIANCES: 
    for (i=0;i<maxahead;i++) {
      vars[i]=var;
    }
    return 0;
    break;
  case CO_VARIANCES:
    for (i=0;i<maxahead;i++) {
      for (j=0;j<maxahead;j++) {
	vars[i*maxahead+j] = (i==j) ? var : 0.0;
      }
    }
    return 0;
    break;
  }
  return -1;
}

void BestMedianPredictor::Dump(FILE *out)
{
  fprintf(stderr,"BestMedianPredictor: order=%d, variance=%f, numsamples=%d\n",
	  order,var,numsamples);
}

double BestMedianModeler::TestFit(int order, double *sequence, int len)
{
  assert(len>=2);

  BestMedianPredictor pred(order,0);
  double *errs = new double[len-1];
  double next;

  for (int i=0;i<len;i++) {
    if (i==0) {
      next=pred.Step(sequence[i]);
    } else {
      errs[i-1]=sequence[i]-next;
      next=pred.Step(sequence[i]);
    }
  }

  double var = Variance(errs,len-1);
  delete [] errs;
  return var;
   
}


BestMedianModel * BestMedianModeler::Fit(double *sequence, int len, int maxord)
{
  double bestvar=99e99, thisvar;
  int bestord;

  for (int i=1;i<=maxord;i++) {
    thisvar=TestFit(i,sequence,len);
    if (thisvar<bestvar) {
      bestvar=thisvar;
      bestord=i;
    }
  }
  return new BestMedianModel(bestord,bestvar);
}


Model *BestMedianModeler::Fit(double *seq, int len, const ParameterSet &ps)
{
  int p,d,q;
  ((const PDQParameterSet &)ps).Get(p,d,q);
  return Fit(seq,len,p);
}
