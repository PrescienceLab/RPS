
#ifndef _newton
#define _newton

#include <vector>
#include <deque>
#include <stdio.h>
#include "abstract.h"

using namespace std;

// Newton polynomial interpolation of order n-1 done on the last n values of the sqeunc

class NewtonModel : public Model {
private:
  int      order;
  double   var;
public:
  NewtonModel();
  NewtonModel(const int order, const double var);
  NewtonModel(const NewtonModel &rhs);
  virtual ~NewtonModel();

  NewtonModel & operator=(const NewtonModel &rhs);

  void   SetOrder(const int order);
  int    GetOrder() const;
  Predictor *MakePredictor() const;

  void   Dump(FILE *out=stdout) const ;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const NewtonModel &p) {
  return p.operator<<(os);
}


class NewtonPredictor : public Predictor {
private:
  int     order;
  double  var;
  deque<double> window;
  vector<double> coeffs;
public:
  NewtonPredictor();
  NewtonPredictor(const int order, const double var);
  NewtonPredictor(const NewtonPredictor &rhs);
  ~NewtonPredictor();

  NewtonPredictor & operator=(const NewtonPredictor &rhs);

  void SetOrder(const int order);
  int GetOrder() const;
  int Begin();
  int StepsToPrime() const;
  double Step(const double obs);
  int Predict(const int maxahead, double *predictions) const;
  void ComputeCoeffs();
  double ComputePrediction(const int step) const;
  virtual int ComputeVariances(const int maxahead, 
			       double *vars, 
			       const VarianceType vtype=POINT_VARIANCES) const;
  void Dump(FILE *out=stdout) const;
  ostream & operator<<(ostream &rhs) const;
};

inline ostream & operator<< (ostream &os, const NewtonPredictor &p) {
  return p.operator<<(os);
}

// No State
class NewtonModeler : public Modeler {
 public:
  NewtonModeler();
  NewtonModeler(const NewtonModeler &rhs);
  virtual ~NewtonModeler();
  
  NewtonModeler & operator=(const NewtonModeler &rhs);

  static NewtonModel *Fit(const double *sequence, const int len, const int maxord);
  static Model *Fit(const double *sequence, const int len, const ParameterSet &ps);

  void Dump(FILE *out=stdout) const;
  ostream & operator<<(ostream &rhs) const;
};

inline ostream & operator<< (ostream &os, const NewtonModeler &p) {
  return p.operator<<(os);
}


#endif

