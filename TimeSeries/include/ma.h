#ifndef _MA
#define _MA

#include <stdio.h>
#include "abstract.h"
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
  MAModel(const MAModel &rhs);
  virtual ~MAModel();

  MAModel & operator=(const MAModel &rhs);

  void   Initialize(const int order);
  int    GetOrder() const;
  void   SetCoeff(const int num, const double value);
  double GetCoeff(const int num) const;
  void   SetVariance(const double variance);
  double GetVariance() const;
  double EstimateVariance(const double *seq, const int len) const;
  void   SetMean(const double mean);
  double GetMean() const;
  Predictor *MakePredictor() const;

  void   Dump(FILE *out=stdout) const;
  ostream & operator<<(ostream &rhs) const;
};

inline ostream & operator<< (ostream &os, const MAModel &p) {
  return p.operator<<(os);
}


// No State
class MAModeler : public Modeler {
public:
  MAModeler();
  MAModeler(const MAModeler &rhs);
  virtual ~MAModeler();
  
  MAModeler & operator=(MAModeler &rhs);

  static MAModel *Fit(const double *sequence, const int len, const int maxord);
  static MAModel *Fit(const double mean, const double *acovf, const int len, const int maxord);
  static Model *Fit(const double *seq, const int len, const ParameterSet &ps);

  virtual void   Dump(FILE *out=stdout) const;
  virtual ostream & operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const MAModeler &p) {
  return p.operator<<(os);
}



double MAConditionalSumOfSquares(const double *seq, const int len, 
                                 const double *coeff, const int numcoeff, const double maxret);

double MAUnConditionalSumOfSquares(const double *seq, const int len, 
                                   const double *coeff, const int numcoeff);



#endif
