
#ifndef _median
#define _median

#include <deque>
#include <stdio.h>
#include "abstract.h"

using namespace std;

// Median-of-last-n Model for the best n

class BestMedianModel : public Model {
private:
  int      order;
  double   var;
public:
  BestMedianModel(int order, double var);
  virtual ~BestMedianModel();
  void   SetOrder(int order);
  int    GetOrder();
  void   Dump(FILE *out=stdout);
  Predictor *MakePredictor();
};

class BestMedianPredictor : public Predictor {
private:
  double  var;
  deque<double> window;
  int     order;
  int     numsamples;
  double  currentmedian;
public:
  BestMedianPredictor(int order, double var);
  ~BestMedianPredictor();
  double ComputeCurrentMedian();
  void SetOrder(int order);
  int GetOrder();
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
class BestMedianModeler : public Modeler {
 private:
  static double TestFit(int order, double *sequence, int len);
 public:
  BestMedianModeler();
  virtual ~BestMedianModeler();
  static BestMedianModel *Fit(double *sequence, int len, int maxord);
  static Model *Fit(double *sequence,int len, const ParameterSet &ps);
};


#endif

