#include <algorithm>
#include <string.h>

#include "bestmedian.h"
#include "util.h"
#include "tools.h"
#include "pdqparamsets.h"

BestMedianModel::BestMedianModel() : order(0), var(0)
{
}

BestMedianModel::BestMedianModel(const BestMedianModel &rhs) : order(rhs.order), var(rhs.var)
{
}

BestMedianModel::BestMedianModel(int o, double v) : order(o), var(v)
{
}

BestMedianModel::~BestMedianModel()
{
}

BestMedianModel & BestMedianModel::operator=(const BestMedianModel &rhs)
{
  return *(new(this)BestMedianModel(rhs));
}

void BestMedianModel::Dump(FILE *out) const
{
  fprintf(out,"BestMedianModel(windowsize=%d, variance=%f)\n",order,var);
}

ostream & BestMedianModel::operator<<(ostream &os) const
{
  os <<"BestMedianModel(order="<<order<<", variance="<<var<<")";
  return os;
}

Predictor * BestMedianModel::MakePredictor() const
{
  return new BestMedianPredictor(order, var);
}

BestMedianPredictor::BestMedianPredictor() :  var(0), order(0), numsamples(0), currentmedian(0)
{
}

BestMedianPredictor::BestMedianPredictor(const BestMedianPredictor &rhs) : var(rhs.var), window(rhs.window), 
									   order(rhs.order), numsamples(rhs.numsamples), 
									   currentmedian(rhs.currentmedian)
{}

BestMedianPredictor::BestMedianPredictor(int order, double var)
{
  this->order=order;
  this->var=var;
  numsamples=0;
  currentmedian=0;
}

BestMedianPredictor::~BestMedianPredictor()
{
  numsamples=0;
  order=0;
}

BestMedianPredictor & BestMedianPredictor::operator=(const BestMedianPredictor &rhs)
{
  return *(new(this)BestMedianPredictor(rhs));
}

int BestMedianPredictor::Begin()
{
  numsamples=0;
  currentmedian=0;
  return 0;
}

int BestMedianPredictor::StepsToPrime() const
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


  return currentmedian;
}

double BestMedianPredictor::Step(const double obs)
{
  assert(window.size()<=(unsigned)order);

  if (window.size()==(unsigned)order) {
    window.pop_front();
  }
  window.push_back(obs);

  numsamples++;
  
  return ComputeCurrentMedian();
}

int BestMedianPredictor::Predict(const int maxahead, double *preds) const
{
  int i;
  for (i=0;i<maxahead;i++) { 
    preds[i] = currentmedian;
  }
  return 0;
}


int BestMedianPredictor::ComputeVariances(const int maxahead, double *vars,
					  const VarianceType vtype) const
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

void BestMedianPredictor::Dump(FILE *out) const
{
  fprintf(out,"BestMedianPredictor: order=%d, variance=%f, numsamples=%d, samples are:\n",
	  order,var,numsamples);
  for (deque<double>::const_iterator i=window.begin(); i!=window.end(); ++i) {
    fprintf(out, "%f\n",*i);
  }
}

ostream & BestMedianPredictor::operator<<(ostream &os) const
{
  os << "BestMedianPredictor(order="<<order<<", variance="<<var<<", numsamples="<<numsamples<<", samples=(";
  for (deque<double>::const_iterator i=window.begin(); i!=window.end(); ++i) {
    if (i!=window.begin()) { 
      os <<", ";
    } 
    os << *i;
  }
  os << "))";
  return os;
}


BestMedianModeler::BestMedianModeler()
{}

BestMedianModeler::BestMedianModeler(const BestMedianModeler &rhs)
{}

BestMedianModeler::~BestMedianModeler()
{}

BestMedianModeler & BestMedianModeler::operator=(const BestMedianModeler &rhs)
{
  return *(new(this)BestMedianModeler(rhs));
}


double BestMedianModeler::TestFit(const int order, const double *sequence, const int len) 
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


BestMedianModel * BestMedianModeler::Fit(const double *sequence, const int len, const int maxord)
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


Model *BestMedianModeler::Fit(const double *seq, const int len, const ParameterSet &ps)
{
  int p,d,q;
  ((const PDQParameterSet &)ps).Get(p,d,q);
  return Fit(seq,len,p);
}


void BestMedianModeler::Dump(FILE *out) const
{
  fprintf(out,"BestMedianModeler()\n");
}

ostream & BestMedianModeler::operator<<(ostream &os) const
{
  os << "BestMedianModeler()";
  return os;
}
