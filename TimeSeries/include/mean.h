#ifndef _mean
#define _mean

#include <stdio.h>
#include "abstract.h"

// last value is next value

class MeanModel : public Model {
 private:
  double *autocovs;
  int     numautocovs;
 public:
  MeanModel(double *autocovs, int num);
  ~MeanModel();
  void   Dump(FILE *out=stdout);
  Predictor *MakePredictor();
};
  

class MeanPredictor : public Predictor {
 private:
  double *autocovs;
  int    numautocovs;
  double sum;
  double sum2;
  int    numsamples;
 public:
  MeanPredictor(double *autocovs, int num);
  ~MeanPredictor();
  int Begin();
  int StepsToPrime();
  double Step(double obs);
  int Predict(int maxahead, double *predictions);
  int ComputeVariances(int maxahead, 
		       double *vars, 
		       VarianceType vtype=POINT_VARIANCES);
  void Dump(FILE *out=stdout);
};

// No State
class MeanModeler : public Modeler {
public:
  static MeanModel *Fit(double *sequence, int len);
  static Model *Fit(double *sequence,int len, const ParameterSet &ps);
};


#endif
