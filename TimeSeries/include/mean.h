#ifndef _mean
#define _mean

#include <stdio.h>
#include "abstract.h"

// last value is next value

class MeanModel : public Model {
 private:
  double *autocovs;
  int     numautocovs;
 public:
  MeanModel();
  MeanModel(const MeanModel &rhs);
  MeanModel(double *autocovs, int num);
  ~MeanModel();

  MeanModel & operator=(const MeanModel &rhs);

  Predictor *MakePredictor() const;

  void   Dump(FILE *out=stdout) const;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const MeanModel &p) {
  return p.operator<<(os);
}
  

class MeanPredictor : public Predictor {
 private:
  double *autocovs;
  int    numautocovs;
  double sum;
  double sum2;
  int    numsamples;
 public:
  MeanPredictor();
  MeanPredictor(const MeanPredictor &rhs);
  MeanPredictor(double *autocovs, int num);
  ~MeanPredictor();
  
  MeanPredictor & operator=(const MeanPredictor &rhs);

  int Begin();
  int StepsToPrime() const;
  double Step(const double obs);
  int Predict(const int maxahead, double *predictions) const; 
  int ComputeVariances(const int maxahead, 
		       double *vars, 
		       const VarianceType vtype=POINT_VARIANCES) const;
  void Dump(FILE *out=stdout) const;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const MeanPredictor &p) {
  return p.operator<<(os);
}

// No State
class MeanModeler : public Modeler {
public:
  MeanModeler();
  MeanModeler(const MeanModeler &rhs);
  ~MeanModeler();
  
  MeanModeler & operator=(const MeanModeler &rhs);

  static MeanModel *Fit(const double *sequence, const int len);
  static Model *Fit(const double *sequence, const int len, const ParameterSet &ps);

  void Dump(FILE *out=stdout) const;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const MeanModeler &p) {
  return p.operator<<(os);
}

#endif
