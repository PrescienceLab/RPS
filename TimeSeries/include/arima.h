#ifndef _ARIMA
#define _ARIMA

#include <stdio.h>
#include "abstract.h"
#include "util.h"
#include "linear.h"
// Autoregressive Integrated Moving Average Model



class ARIMAModel : public Model {
private:
  int      p, d, q;
  double   *phis;
  double   *thetas;
  double   variance;
  double   mean;
public:
  ARIMAModel();
  virtual ~ARIMAModel();
  void   Initialize(int P, int D, int Q);
  int    GetP();
  int    GetD();
  int    GetQ();
  void   SetARCoeff(int num, double value);
  double GetARCoeff(int num);
  void   SetMACoeff(int num, double value);
  double GetMACoeff(int num);
  void   SetVariance(double variance);
  double EstimateVariance(double *seq, int len);
  double GetVariance();
  void   SetMean(double m);
  double GetMean();
  virtual void   Dump(FILE *out=stdout);
  virtual Predictor *MakePredictor();
};


// No State
class ARIMAModeler : public Modeler {
public:
  ARIMAModeler();
  virtual ~ARIMAModeler();
  // Fit AR models of order 1..maxord to the sequnce and return these
  // models in an array
  // Assumes a zero mean sequence
  static ARIMAModel *Fit(double *sequence, int len, int P, int D, int Q);
  static Model *Fit(double *seq, int len, const ParameterSet &ps);
};


#endif
