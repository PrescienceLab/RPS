#ifndef _ARMA
#define _ARMA


#include <stdio.h>
#include "abstract.h"
#include "etathetapred.h"

// Mixed Autoregressive and Moving Average Model


// Coeffs are numbered 0..[p|q]-1, which is different from the stats literature
class ARMAModel : public Model {
private:
  int      p, q;
  double   *phis;
  double   *thetas;
  double   variance;
  double   mean;
public:
  ARMAModel();
  virtual ~ARMAModel();
  void   Initialize(int P, int Q);
  int    GetP();
  int    GetQ();
  void   SetARCoeff(int num, double value);
  double GetARCoeff(int num);
  void   SetMACoeff(int num, double value);
  double GetMACoeff(int num);
  void   SetVariance(double variance);
  double EstimateVariance(double *seq, int len);
  double GetVariance();
  void   SetMean(double mean);
  double GetMean();
  void   Dump(FILE *out=stdout);
  Predictor *MakePredictor();
};


// No State
class ARMAModeler : public Modeler {
public:
  ARMAModeler();
  virtual ~ARMAModeler();
  static ARMAModel *Fit(double *sequence, int len, int P, int Q);
  static ARMAModel *Fit(double mean, double *acovf, int len, int P, int Q);
  static Model *Fit(double *seq, int len, const ParameterSet &ps);
};



#endif
