#ifndef _ARFIMA
#define _ARFIMA

#include <iostream>
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
  ARFIMAModel(const ARFIMAModel &rhs);
  virtual ~ARFIMAModel();

  ARFIMAModel &operator=(const ARFIMAModel &rhs);

  int    Initialize(const int P, const double D, const int Q);
  int    GetP() const ;
  double GetD() const ;
  int    GetQ() const;
  void   SetARCoeff(const int num, const double value);
  double GetARCoeff(const int num) const;
  void   SetMACoeff(const int num, const double value);
  double GetMACoeff(const int num) const;
  void   SetVariance(const double variance);
  double EstimateVariance(const double *seq, const int len) const;
  double GetVariance() const;
  void   SetMean(const double mean);
  double GetMean() const;

  virtual void   Dump(FILE *out=stdout) const;
  virtual ostream & operator<<(ostream &os) const;

  virtual Predictor *MakePredictor() const;
  virtual Predictor *MakePredictor(const int truncationlimit) const;
};


inline ostream & operator<< (ostream &os, const ARFIMAModel &p) {
  return p.operator<<(os);
}


// No State
class ARFIMAModeler : public Modeler {
public:
  ARFIMAModeler();
  ARFIMAModeler(const ARFIMAModeler &rhs);
  virtual ~ARFIMAModeler();

  ARFIMAModeler & operator=(const ARFIMAModeler &rhs);

  static Model *Fit(const double *sequence, const int len, const int p, const double d, const int q);
  static Model *Fit(const double *seq, const int len, const ParameterSet &ps);

  virtual void   Dump(FILE *out=stdout) const;
  virtual ostream & operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const ARFIMAModeler &p) {
  return p.operator<<(os);
}

#endif
