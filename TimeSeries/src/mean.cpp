#include <string.h>
#include "mean.h"
#include "util.h"
#include "tools.h"

MeanModel::MeanModel() : autocovs(0), numautocovs(0)
{}

MeanModel::MeanModel(const MeanModel &rhs)
{
  numautocovs=rhs.numautocovs;
  autocovs=new double [numautocovs];
  memcpy(autocovs,rhs.autocovs,numautocovs*sizeof(autocovs[0]));
}

MeanModel::MeanModel(double *autocovs, int n)
{
  this->autocovs = new double [n];
  memcpy(this->autocovs,autocovs,n*sizeof(double));
  numautocovs=n;
}

MeanModel::~MeanModel()
{
  CHK_DEL_MAT(this->autocovs);
  numautocovs=0;
}

MeanModel & MeanModel::operator=(const MeanModel &rhs)
{
  return *(new(this)MeanModel(rhs));
}


void MeanModel::Dump(FILE *out) const
{
  fprintf(out,"MeanModel: numautocovs=%d, autocovs follow:",numautocovs); 
  for (int i=0;i<numautocovs;i++) {
    fprintf(out,"%f\n",autocovs[i]);
  }
}

ostream & MeanModel::operator<<(ostream &os) const
{
  os << "MeanModel(numautocovs="<<numautocovs<<", autocovs=(";
  for (int i=0;i<numautocovs;i++) {
    if (i>0) { 
      os << ", ";
    }
    os << autocovs[i];
  }
  os <<"))";
  return os;
}


Predictor * MeanModel::MakePredictor() const
{
  return new MeanPredictor(autocovs,numautocovs);
}

MeanPredictor::MeanPredictor() : autocovs(0), numautocovs(0), sum(0), sum2(0), numsamples(0)
{}

MeanPredictor::MeanPredictor(const MeanPredictor &rhs) 
{
  numautocovs=rhs.numautocovs;
  autocovs=new double [numautocovs];
  memcpy(autocovs,rhs.autocovs,sizeof(autocovs[0])*numautocovs);
  sum=rhs.sum;
  sum2=rhs.sum2;
  numsamples=rhs.numsamples;
}

MeanPredictor::MeanPredictor(double *autocovs, int n)
{
  this->autocovs = new double[n];
  memcpy(this->autocovs,autocovs,n*sizeof(double));
  numautocovs=n;
  sum=sum2=0;
  numsamples=0;
}

MeanPredictor::~MeanPredictor()
{
  CHK_DEL_MAT(autocovs);
  numautocovs=0;
  sum=sum2=0;
  numsamples=0;
}

MeanPredictor & MeanPredictor::operator=(const MeanPredictor &rhs) 
{
  return *(new(this)MeanPredictor(rhs));
}

int MeanPredictor::Begin()
{
  sum=sum2=0;
  numsamples=0;
  return 0;
}

int MeanPredictor::StepsToPrime() const
{
  return 0;
}

double MeanPredictor::Step(const double obs)
{
  sum+=obs;
  sum2+=obs*obs;
  numsamples++;
  return sum/numsamples;
}

int MeanPredictor::Predict(const int maxahead, double *preds) const
{
  int i;
  for (i=0;i<maxahead;i++) { 
    preds[i] = (numsamples>0) ? sum / numsamples : 0.0;
  }
  return 0;
}

int MeanPredictor::ComputeVariances(const int maxahead, double *vars,
				    const VarianceType vtype) const
{
  int i;
  double var;

  if (numsamples>=2) {
    var = (sum2 - sum*sum/numsamples) / (numsamples-1);
  } else {
    var = 0;
  }

  switch (vtype) { 
  case POINT_VARIANCES:
    for (i=0;i<maxahead;i++) { 
      vars[i]=autocovs[0];
    }
    return 0;
    break;
  case SUM_VARIANCES: 
    AutoCov2VarianceOfSum(autocovs,numautocovs,vars,maxahead);
    return 0;
    break;
  case CO_VARIANCES:
    AutoCov2Covariances(autocovs,numautocovs,vars,maxahead);
    return 0;
    break;
  }
  return -1;
}

void MeanPredictor::Dump(FILE *out) const
{
  fprintf(out,"MeanPredictor: sum=%f sum2=%f numsamples=%d numautocov=%d autocovs follow:\n",
	  sum,sum2,numsamples,numautocovs);
  for (int i=0;i<numautocovs;i++) {
    fprintf(out,"%f\n",autocovs[i]);
  }
}

ostream & MeanPredictor::operator<<(ostream &os) const
{
  os <<"MeanPredictor(sum="<<sum<<", sum2="<<sum2<<", numsamples="<<numsamples<<", numautocovs="<<numautocovs<<", autocovs=(";
  for (int i=0;i<numautocovs;i++) {
    if (i>0) { 
      os << ", ";
    }
    os << autocovs[i];
  }
  os <<"))";
  return os;
}

MeanModeler::MeanModeler()
{}

MeanModeler::MeanModeler(const MeanModeler &rhs)
{}

MeanModeler::~MeanModeler()
{}

MeanModeler &MeanModeler::operator=(const MeanModeler &rhs)
{
  return *(new(this)MeanModeler(rhs));
}


MeanModel * MeanModeler::Fit(const double *sequence, const int len)
{
  int n = len-30 > 0 ? len-30 : len;
  double *autocovs = new double [n];
  ComputeAutoCov(sequence,len,autocovs,n);
  MeanModel *m= new MeanModel(autocovs,n);
  delete [] autocovs;
  return m;
}

Model *MeanModeler::Fit(const double *seq, const int len, const ParameterSet &ps)
{
  return Fit(seq,len);
}


void MeanModeler::Dump(FILE *out) const
{
  fprintf(out,"MeanModeler\n");
}

ostream & MeanModeler::operator<<(ostream &os) const
{
  return (os<<"MeanModeler()");
}

