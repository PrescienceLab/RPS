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
  ARMAModel(const ARMAModel &rhs);
  virtual ~ARMAModel();

  ARMAModel & operator=(const ARMAModel &rhs);

  void   Initialize(const int P, const int Q);
  int    GetP() const;
  int    GetQ() const;
  void   SetARCoeff(const int num, const double value);
  double GetARCoeff(const int num) const ;
  void   SetMACoeff(const int num, const double value);
  double GetMACoeff(const int num) const ;
  void   SetVariance(const double variance);
  double EstimateVariance(const double *seq, const int len) const ;
  double GetVariance() const ;
  void   SetMean(const double mean);
  double GetMean() const ;
  Predictor *MakePredictor() const;

  void   Dump(FILE *out=stdout) const ;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const ARMAModel &p) {
  return p.operator<<(os);
}


// No State
class ARMAModeler : public Modeler {
public:
  ARMAModeler();
  ARMAModeler(const ARMAModeler &rhs);
  virtual ~ARMAModeler();
  
  ARMAModeler & operator=(const ARMAModeler &rhs);

  static ARMAModel *Fit(const double *sequence, const int len, const int P, const int Q);
  static ARMAModel *Fit(const double mean, const double *acovf, const int len, const int P, const int Q);
  static Model *Fit(const double *seq, const int len, const ParameterSet &ps);

  void Dump(FILE *out=stdout) const;
  ostream & operator<<(ostream &rhs) const;
};

inline ostream & operator<< (ostream &os, const ARMAModeler &p) {
  return p.operator<<(os);
}



#endif
