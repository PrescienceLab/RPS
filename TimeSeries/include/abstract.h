#ifndef _abstractions
#define _abstractions

#include <iostream>
#include <stdio.h>

using namespace std;

// This file includes the core abstractions

enum ModelType {NONE=0,
		MEAN=1,
		LAST=2,
		BESTMEAN=3,
		AR=4, 
		MA=5,	
		ARMA=6, 
		ARIMA=7, 
		ARFIMA=8,
		BESTMEDIAN=9,
                NEWTON=10};

enum ParameterSetType {PDQ, RefittingPDQ, AwaitingPDQ, ManagedPDQ};

class ParameterSet {
 public:
  virtual ~ParameterSet() {};
  virtual ParameterSetType GetType() const =0;
  virtual ParameterSet *Clone() const =0;

  virtual void Dump(FILE *out=stdout) const =0;
  virtual ostream & operator<<(ostream &os) const = 0;
};

inline ostream & operator<<(ostream &os, const ParameterSet &p) { return p.operator<<(os);}

enum VarianceType {
  POINT_VARIANCES,     // variance of for each lead time
  CO_VARIANCES,        // covariances for cross prod of all lead times
  SUM_VARIANCES        // ith variance is variance for sum to i
};


class Predictor {
public:
	virtual ~Predictor() {} 
	virtual int Begin()=0;
        virtual int StepsToPrime() const =0;
	virtual double Step(const double obs)=0;
	virtual int Predict(const int maxahead, double *predictions) const =0;
	virtual int ComputeVariances(const int maxahead, 
				     double *vars, 
				     const VarianceType vtype=POINT_VARIANCES) const =0;
        virtual void Dump(FILE *out=stdout) const =0;
	virtual ostream & operator<<(ostream &os) const = 0;
};

inline ostream & operator<<(ostream &os, const Predictor &p) { return p.operator<<(os);}

class Model {
public:
	virtual ~Model() {} ;
	virtual Predictor * MakePredictor() const =0;
        virtual void Dump(FILE *out=stdout) const =0;
	virtual ostream & operator<<(ostream &os) const =0;
//        virtual ParamterSet *GetParameterSet()=0;
};

inline ostream & operator<<(ostream &os, const Model &p) { return p.operator<<(os);}


class Modeler {
public:
   virtual ~Modeler() {}
   static Model *Fit(const double *sequence, const int len, const ParameterSet &ps);
   virtual void Dump(FILE *out=stdout) const =0;
   virtual ostream & operator<<(ostream &os) const = 0;
};

inline ostream & operator<<(ostream &os, const Modeler &p) { return p.operator<<(os);}

#endif
