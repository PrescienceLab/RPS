#ifndef _AR
#define _AR

#include <iostream>
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
  ARModel(const ARModel &rhs);
  virtual ~ARModel();

  ARModel & operator=(const ARModel &rhs);

  void   Initialize(const int order);
  int    GetOrder() const ;
  void   SetCoeff(const int num, const double value);
  double GetCoeff(const int num) const;
  void   SetVariance(const double variance);
  double GetVariance() const;
  void   SetMean(const double mean);
  double GetMean() const;
  Predictor *MakePredictor() const;

  void   Dump(FILE *out=stdout) const ;
  ostream &operator<<(ostream &os) const;

};

inline ostream & operator<< (ostream &os, const ARModel &p) {
  return p.operator<<(os);
}



// No State
class ARModeler : public Modeler {
public:
  ARModeler();
  ARModeler(const ARModeler &rhs);
  virtual ~ARModeler();

  ARModeler & operator=(const ARModeler &rhs);
  
  // Fit an AR model of order maxord to the sequnce and 
  static ARModel *Fit(const double *sequence, const int len, const int maxord);
  // Fit an AR model of order maxord to the mean and acovf
  static ARModel *Fit(const double mean, const double *acovf, const int len, const int maxord);

  static Model   *Fit(const double *seq, const int len, const ParameterSet &ps);

  virtual void   Dump(FILE *out=stdout) const;
  virtual ostream & operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const ARModeler &p) {
  return p.operator<<(os);
}


#endif

