#ifndef _ARIMA
#define _ARIMA

#include <stdio.h>
#include "abstract.h"
#include "util.h"
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
  ARIMAModel(const ARIMAModel &rhs);
  virtual ~ARIMAModel();
  
  ARIMAModel & operator=(const ARIMAModel &rhs);

  void   Initialize(const int P, const int D, const int Q);
  int    GetP() const ;
  int    GetD() const ;
  int    GetQ() const ;
  void   SetARCoeff(const int num, const double value);
  double GetARCoeff(const int num) const ;
  void   SetMACoeff(const int num, const double value);
  double GetMACoeff(const int num) const ;
  void   SetVariance(const double variance);
  double EstimateVariance(const double *seq, const int len) const ;
  double GetVariance() const ;
  void   SetMean(const double m);
  double GetMean() const ;
  virtual Predictor *MakePredictor() const ;

  virtual void   Dump(FILE *out=stdout) const ;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const ARIMAModel &p) {
  return p.operator<<(os);
}


// No State
class ARIMAModeler : public Modeler {
public:
  ARIMAModeler();
  ARIMAModeler(const ARIMAModeler &rhs);
  virtual ~ARIMAModeler();

  ARIMAModeler & operator=(const ARIMAModeler &rhs);

  static ARIMAModel *Fit(const double *sequence, const int len, const int P, const int D, const int Q);
  static Model *Fit(const double *seq, const int len, const ParameterSet &ps);

  void Dump(FILE *out=stdout) const;
  ostream & operator<<(ostream &rhs) const;
};

inline ostream & operator<< (ostream &os, const ARIMAModeler &p) {
  return p.operator<<(os);
}


#endif
