
#ifndef _bestmean
#define _bestmean

#include <stdio.h>
#include "abstract.h"
#include "linear.h"
#include "etathetapred.h"

// Mean-of-last-n Model for the best n

class BestMeanModel : public Model {
private:
  int      order;
  double   variance;
  double   mean;
public:
  BestMeanModel();
  virtual ~BestMeanModel();
  void   Initialize(int order);
  int    GetOrder();
  void   SetMean(double mean);
  double GetMean();
  void   SetVariance(double var);
  double GetVariance();
  void   Dump(FILE *out=stdout);
  Predictor *MakePredictor();
};

class BestMeanPredictor : public Predictor {
private:
  double mult;
  double *samples;
  int order;
  int numsamples;
  double variance;
public:
  BestMeanPredictor();
  ~BestMeanPredictor();
  int Initialize(int order,double variance);
  int Begin();
  int StepsToPrime();
  double Step(double obs);
  int Predict(int maxahead, double *predictions);
  virtual int ComputeVariances(int maxahead, 
			       double *vars, 
			       VarianceType vtype=POINT_VARIANCES);
  void Dump(FILE *out=stdout);
};

// No State
class BestMeanModeler : public Modeler {
public:
  BestMeanModeler();
  virtual ~BestMeanModeler();

  static BestMeanModel *Fit(double *sequence, int len, int maxord);
  static Model *Fit(double *sequence,int len, const ParameterSet &ps);
};


#endif

