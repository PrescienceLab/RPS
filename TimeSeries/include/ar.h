#ifndef _AR
#define _AR

#include <stdio.h>
#include "abstract.h"
#include "etathetapred.h"
#include "pdqparamsets.h"

// Autoregressive Model


// Coeffs are numbered 0..order-1, which is different from the stats literature
class ARModel : public Model {
private:
  int      order;
  double   *coeffs;
  double   variance;
  double   mean;
public:
  ARModel();
  virtual ~ARModel();
  void   Initialize(int order);
  int    GetOrder();
  void   SetCoeff(int num, double value);
  double GetCoeff(int num);
  void   SetVariance(double variance);
  double GetVariance();
  void   SetMean(double mean);
  double GetMean();
  void   Dump(FILE *out=stdout);
  Predictor *MakePredictor();
};


// No State
class ARModeler : public Modeler {
public:
  ARModeler();
  virtual ~ARModeler();
  // Fit an AR model of order maxord to the sequnce and 
  static ARModel *Fit(double *sequence, int len, int maxord);
  // Fit an AR model of order maxord to the mean and acovf
  static ARModel *Fit(double mean, double *acovf, int len, int maxord);

  static Model   *Fit(double *seq, int len, const ParameterSet &ps);
};


#endif

