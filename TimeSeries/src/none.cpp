#include "none.h"

int NonePredictor::Begin()
{
  return 0;
}

int NonePredictor::StepsToPrime()
{
  return 0;
}

double NonePredictor::Step(double obs)
{
  return 0.0;
}

int NonePredictor::Predict(int maxahead, double *predictions)
{
  int i;
  for (i=0;i<maxahead;i++) { 
    predictions[i]=0.0;
  }
  return 0;
}

int NonePredictor::ComputeVariances(int maxahead, double *vars, 
				       VarianceType vtype) 
{
  int i;
  int max = vtype==CO_VARIANCES ? maxahead*maxahead : maxahead;
  for (i=0;i<max;i++) { 
    vars[i]=0.0;
  }
  return 0;
}

void NonePredictor::Dump(FILE *out)
{
  fprintf(out,"NonePredictor");
}

Predictor *NoneModel::MakePredictor()
{
  return new NonePredictor;
}

void NoneModel::Dump(FILE *out)
{
  fprintf(out,"NoneModel");
}


Model *NoneModeler::Fit()
{
  return new NoneModel;
}

Model *NoneModeler::Fit(double *seq, int len)
{
  return Fit();
}

Model *NoneModeler::Fit(double *sequence,int len, const ParameterSet &ps)
{
  return Fit();
}
