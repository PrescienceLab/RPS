
#ifndef _median
#define _median

#include <deque>
#include <stdio.h>
#include "abstract.h"

using namespace std;

// Median-of-last-n Model for the best n

class BestMedianModel : public Model {
private:
  int      order;
  double   var;
public:
  BestMedianModel();
  BestMedianModel(int order, double var);
  BestMedianModel(const BestMedianModel &rhs);
  virtual ~BestMedianModel();

  BestMedianModel & operator=(const BestMedianModel &rhs);

  void   SetOrder(const int order);
  int    GetOrder() const;
  Predictor *MakePredictor() const;

  void   Dump(FILE *out=stdout) const ;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const BestMedianModel &p) {
  return p.operator<<(os);
}


class BestMedianPredictor : public Predictor {
private:
  double  var;
  deque<double> window;
  int     order;
  int     numsamples;
  double  currentmedian;
public:
  BestMedianPredictor();
  BestMedianPredictor(const int order, const double var);
  BestMedianPredictor(const BestMedianPredictor &rhs);
  ~BestMedianPredictor();

  BestMedianPredictor & operator=(const BestMedianPredictor &rhs);

  double ComputeCurrentMedian();
  void SetOrder(const int order);
  int GetOrder() const;
  int Begin();
  int StepsToPrime() const;
  double Step(const double obs);
  int Predict(const int maxahead, double *predictions) const;
  virtual int ComputeVariances(const int maxahead, 
			       double *vars, 
			       const VarianceType vtype=POINT_VARIANCES) const;
  void Dump(FILE *out=stdout) const;
  ostream & operator<<(ostream &rhs) const;
};

inline ostream & operator<< (ostream &os, const BestMedianPredictor &p) {
  return p.operator<<(os);
}

// No State
class BestMedianModeler : public Modeler {
 private:
  static double TestFit(const int order, const double *sequence, const int len);
 public:
  BestMedianModeler();
  BestMedianModeler(const BestMedianModeler &rhs);
  virtual ~BestMedianModeler();
  
  BestMedianModeler & operator=(const BestMedianModeler &rhs);

  static BestMedianModel *Fit(const double *sequence, const int len, const int maxord);
  static Model *Fit(const double *sequence, const int len, const ParameterSet &ps);

  void Dump(FILE *out=stdout) const;
  ostream & operator<<(ostream &rhs) const;
};

inline ostream & operator<< (ostream &os, const BestMedianModeler &p) {
  return p.operator<<(os);
}


#endif

