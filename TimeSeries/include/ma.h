#ifndef _MA
#define _MA

#include <stdio.h>
#include "abstract.h"
#include "linear.h"
#include "etathetapred.h"

// Moving Average Model


// Coeffs are numbered 0..order-1, which is different from the stats literature
class MAModel : public Model {
private:
  int      order;
  double   *coeffs;
  double   variance;
  double   mean;
public:
  MAModel();
  virtual ~MAModel();
  void   Initialize(int order);
  int    GetOrder();
  void   SetCoeff(int num, double value);
  double GetCoeff(int num);
  void   SetVariance(double variance);
  double GetVariance();
  double EstimateVariance(double *seq, int len);
  void   SetMean(double mean);
  double GetMean();
  void   Dump(FILE *out=stdout);
  Predictor *MakePredictor();
};


// No State
class MAModeler : public Modeler {
public:
  MAModeler();
  virtual ~MAModeler();
  static MAModel *Fit(double *sequence, int len, int maxord);
  static MAModel *Fit(double mean, double *acovf, int len, int maxord);
  static Model *Fit(double *seq, int len, const ParameterSet &ps);
};


double MAConditionalSumOfSquares(double *seq, int len, 
                                 double *coeff, int numcoeff, double maxret);

double MAUnConditionalSumOfSquares(double *seq, int len, 
                                   double *coeff, int numcoeff);



#endif
