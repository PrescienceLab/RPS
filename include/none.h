#ifndef _NONE
#define _NONE

#include "abstract.h"


class NonePredictor : public Predictor {
 public:
  int Begin();
  int StepsToPrime();
  double Step(double obs);
  int Predict(int maxahead, double *predictions);
  int ComputeVariances(int maxahead, 
		       double *vars, 
		       VarianceType vtype=POINT_VARIANCES);
  void Dump(FILE *out=stdout);
};

class NoneModel : public Model {
 public:
	Predictor * MakePredictor();
        void Dump(FILE *out=stdout);
};

class NoneModeler : public Modeler {
public:
  static Model *Fit();
  static Model *Fit(double *sequence,int len);
  static Model *Fit(double *sequence,int len, const ParameterSet &ps);
};

#endif
