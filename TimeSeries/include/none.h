#ifndef _NONE
#define _NONE

#include "abstract.h"


class NonePredictor : public Predictor {
 public:
  NonePredictor();
  NonePredictor(const NonePredictor &rhs);
  ~NonePredictor();
  
  NonePredictor & operator=(const NonePredictor &rhs);

  int Begin();
  int StepsToPrime() const;
  double Step(const double obs);
  int Predict(const int maxahead, double *predictions) const;
  int ComputeVariances(const int maxahead, 
		       double *vars, 
		       const VarianceType vtype=POINT_VARIANCES) const ;

  void Dump(FILE *out=stdout) const ;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const NonePredictor &p) {
  return p.operator<<(os);
}


class NoneModel : public Model {
 public:
  NoneModel();
  NoneModel(const NoneModel &rhs);
  ~NoneModel();
  
  NoneModel & operator=(const NoneModel &rhs);

  Predictor * MakePredictor() const;
  
  void Dump(FILE *out=stdout) const;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const NoneModel &p) {
  return p.operator<<(os);
}

class NoneModeler : public Modeler {
public:
  NoneModeler();
  NoneModeler(const NoneModeler &rhs);
  ~NoneModeler();

  NoneModeler & operator=(const NoneModeler &rhs);

  static Model *Fit();
  static Model *Fit(const double *sequence, const int len);
  static Model *Fit(const double *sequence, const int len, const ParameterSet &ps);

  void Dump(FILE *out=stdout) const;
  ostream & operator<<(ostream &rhs) const;
};

inline ostream & operator<< (ostream &os, const NoneModeler &p) {
  return p.operator<<(os);
}

#endif
