#ifndef _ARFIMA
#define _ARFIMA

#include <stdio.h>
#include "abstract.h"
#include "util.h"

// General Autoregressive Fractionally Integrated Moving Average Model

class ARFIMAModel : public Model {
private:
  int      p,q;
  double   d;
  double   *phis;
  double   *thetas;
  double   variance;
  double   mean;
public:
  ARFIMAModel();
  virtual ~ARFIMAModel();
  int    Initialize(int P, double D, int Q);
  int    GetP();
  double GetD();
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
  virtual void   Dump(FILE *out=stdout);
  virtual Predictor *MakePredictor();
  virtual Predictor *MakePredictor(int truncationlimit);
};


// No State
class ARFIMAModeler : public Modeler {
public:
  ARFIMAModeler();
  virtual ~ARFIMAModeler();
  static Model *Fit(double *sequence, int len, int p, double d, int q);
  static Model *Fit(double *seq, int len, const ParameterSet &ps);
};


#endif
