#include <string.h>
#include "mean.h"
#include "util.h"
#include "tools.h"

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

void MeanModel::Dump(FILE *out)
{
  fprintf(out,"MeanModel\n");
}

Predictor * MeanModel::MakePredictor()
{
  return new MeanPredictor(autocovs,numautocovs);
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

int MeanPredictor::Begin()
{
  sum=sum2=0;
  numsamples=0;
  return 0;
}

int MeanPredictor::StepsToPrime()
{
  return 0;
}

double MeanPredictor::Step(double obs)
{
  sum+=obs;
  sum2+=obs*obs;
  numsamples++;
  return sum/numsamples;
}

int MeanPredictor::Predict(int maxahead, double *preds)
{
  int i;
  for (i=0;i<maxahead;i++) { 
    preds[i] = (numsamples>0) ? sum / numsamples : 0.0;
  }
  return 0;
}

int MeanPredictor::ComputeVariances(int maxahead, double *vars,
				    VarianceType vtype)
{
  int i,j;
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

void MeanPredictor::Dump(FILE *out)
{
  fprintf(stderr,"MeanPredictor: sum=%lf sum2=%lf numsamples=%d\n",
	  sum,sum2,numsamples);
}

MeanModel * MeanModeler::Fit(double *sequence, int len)
{
  int n = len-30 > 0 ? len-30 : len;
  double *autocovs = new double [n];
  ComputeAutoCov(sequence,len,autocovs,n);
  MeanModel *m= new MeanModel(autocovs,n);
  delete [] autocovs;
  return m;
}

Model *MeanModeler::Fit(double *seq, int len, const ParameterSet &ps)
{
  return Fit(seq,len);
}

