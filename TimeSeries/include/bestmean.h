
#ifndef _bestmean
#define _bestmean

#include <new>
#include <stdio.h>
#include "abstract.h"
#include "etathetapred.h"

// Mean-of-last-n Model for the best n

class BestMeanModel : public Model {
 private:
  int      order;
  double   variance;
  double   mean;
 public:
  BestMeanModel();
  BestMeanModel(const BestMeanModel &rhs);
  virtual ~BestMeanModel();

  BestMeanModel & operator=(const BestMeanModel &rhs);

  void   Initialize(const int order);
  int    GetOrder() const;
  void   SetMean(const double mean);
  double GetMean() const;
  void   SetVariance(const double var);
  double GetVariance() const;
  Predictor *MakePredictor() const;

  void   Dump(FILE *out=stdout) const;
  ostream & operator<<(ostream &rhs) const;
};

inline ostream & operator<< (ostream &os, const BestMeanModel &p) {
  return p.operator<<(os);
}


class BestMeanPredictor : public Predictor {
private:
  double mult;
  double *samples;
  int order;
  int numsamples;
  double variance;
public:
  BestMeanPredictor();
  BestMeanPredictor(const BestMeanPredictor &rhs);
  ~BestMeanPredictor();

  BestMeanPredictor & operator=(const BestMeanPredictor &rhs);

  int Initialize(const int order, const double variance);
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

inline ostream & operator<< (ostream &os, const BestMeanPredictor &p) {
  return p.operator<<(os);
}

// No State
class BestMeanModeler : public Modeler {
public:
  BestMeanModeler();
  BestMeanModeler(const BestMeanModeler &rhs);
  virtual ~BestMeanModeler();

  BestMeanModeler & operator=(const BestMeanModeler &rhs);

  static BestMeanModel *Fit(const double *sequence, const int len, const int maxord);
  static Model *Fit(const double *sequence, const int len, const ParameterSet &ps);

  void Dump(FILE *out=stdout) const;
  ostream & operator<<(ostream &rhs) const;
};

inline ostream & operator<< (ostream &os, const BestMeanModeler &p) {
  return p.operator<<(os);
}

#endif

