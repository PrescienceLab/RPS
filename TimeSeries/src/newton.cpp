#include <algorithm>
#include <string.h>

#include "newton.h"
#include "util.h"
#include "tools.h"
#include "pdqparamsets.h"

NewtonModel::NewtonModel() : order(0)
{
}

NewtonModel::NewtonModel(const NewtonModel &rhs) : order(rhs.order), var(rhs.var)
{
}

NewtonModel::NewtonModel(const int o, const double v) : order(o), var(v)
{
}

NewtonModel::~NewtonModel()
{
}

NewtonModel & NewtonModel::operator=(const NewtonModel &rhs)
{
  this->~NewtonModel();
  return *(new(this)NewtonModel(rhs));
}

void NewtonModel::Dump(FILE *out) const
{
  fprintf(out,"NewtonModel(order=%d, var=%f)\n",order,var);
}

ostream & NewtonModel::operator<<(ostream &os) const
{
  os <<"NewtonModel(order="<<order<<", var="<<var<<")";
  return os;
}

Predictor * NewtonModel::MakePredictor() const
{
  return new NewtonPredictor(order,var);
}

NewtonPredictor::NewtonPredictor() :  order(0), var(0)
{
}

NewtonPredictor::NewtonPredictor(const NewtonPredictor &rhs) :  order(rhs.order), var(rhs.var), window(rhs.window), coeffs(rhs.coeffs)
{}

NewtonPredictor::NewtonPredictor(const int order, const double var) : window(order), coeffs(order)
{
  this->order=order;
  this->var=var;
}

NewtonPredictor::~NewtonPredictor()
{
  window.clear();
  coeffs.clear();
}

NewtonPredictor & NewtonPredictor::operator=(const NewtonPredictor &rhs)
{
  this->~NewtonPredictor();
  return *(new(this)NewtonPredictor(rhs));
}

int NewtonPredictor::Begin()
{
  return 0;
}

int NewtonPredictor::StepsToPrime() const
{
  return 0;
}
 
void NewtonPredictor::ComputeCoeffs() 
{
  int n=window.size();
  int i,j;

  // window from 0..n-1 corresponds to values  1, 2, 3, 3, 4, .. n

  for (i=0;i<n;i++) { 
    coeffs[i]=window[i];
  }

  for (j=0;j<(n-1);j++) { 
    for (i=n-1;i>=(j+1);i--) { 
      coeffs[i]=(coeffs[i]-coeffs[i-1])/((i+1)-((i+1)-(j+1)));
    }
  }
}

double NewtonPredictor::ComputePrediction(const int step) const
{
  int n=window.size();

  double x=coeffs[n-1];

  // go to location n+step-1
  int loc=n+step;

  for (int i=n-2;i>=0;i--) { 
    x =  x*((loc - (i+1) ))  + coeffs[i];
  }
  
  return x;
}


double NewtonPredictor::Step(const double obs)
{
  if (window.size()==(unsigned)order) {
    window.pop_front();
  }
  window.push_back(obs);

  ComputeCoeffs();

  return ComputePrediction(1);
}

int NewtonPredictor::Predict(const int maxahead, double *preds) const
{
  int i;
  for (i=0;i<maxahead;i++) { 
    preds[i] = ComputePrediction(i+1);
  }
  return 0;
}


int NewtonPredictor::ComputeVariances(const int maxahead, double *vars,
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

void NewtonPredictor::Dump(FILE *out) const
{
  fprintf(out,"NewtonPredictor: order=%d, variance=%f, numsamples=%d, numcoeffs=%d samples are:\n",
	  order,var,window.size(),coeffs.size());
  for (deque<double>::const_iterator i=window.begin(); i!=window.end(); ++i) {
    fprintf(out, "%f\n",*i);
  }
  fprintf(out,"coeffs are:\n");
  for (vector<double>::const_iterator i=coeffs.begin(); i!=coeffs.end(); ++i) {
    fprintf(out, "%f\n",*i);
  }
}

ostream & NewtonPredictor::operator<<(ostream &os) const
{
  os << "NewtonPredictor(order="<<order<<", variance="<<var<<", numsamples="<<window.size()
     <<", numcoeffs="<<coeffs.size()<<", samples=(";
  for (deque<double>::const_iterator i=window.begin(); i!=window.end(); ++i) {
    if (i!=window.begin()) { 
      os <<", ";
    } 
    os << *i;
  }
  os << "), coeffs=(";
  for (vector<double>::const_iterator i=coeffs.begin(); i!=coeffs.end(); ++i) {
    if (i!=coeffs.begin()) { 
      os <<", ";
    } 
    os << *i;
  }
  os << "))";
  return os;
}

NewtonModeler::NewtonModeler()
{}

NewtonModeler::NewtonModeler(const NewtonModeler &rhs)
{}

NewtonModeler::~NewtonModeler()
{}

NewtonModeler & NewtonModeler::operator=(const NewtonModeler &rhs)
{
  this->~NewtonModeler();
  return *(new(this)NewtonModeler(rhs));
}



NewtonModel * NewtonModeler::Fit(const double *sequence, const int len, const int maxord)
{
  return new NewtonModel(maxord,len>2 ? Variance(sequence,len) : 0);
}


Model *NewtonModeler::Fit(const double *seq, const int len, const ParameterSet &ps)
{
  int p,d,q;
  ((const PDQParameterSet &)ps).Get(p,d,q);
  return Fit(seq,len,p);
}


void NewtonModeler::Dump(FILE *out) const
{
  fprintf(out,"NewtonModeler()\n");
}

ostream & NewtonModeler::operator<<(ostream &os) const
{
  os << "NewtonModeler()";
  return os;
}
