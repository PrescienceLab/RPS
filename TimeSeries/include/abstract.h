#ifndef _abstractions
#define _abstractions

#include <stdio.h>

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
		BESTMEDIAN=9};

enum ParameterSetType {PDQ, RefittingPDQ, AwaitingPDQ, ManagedPDQ};

class ParameterSet {
 public:
  virtual ParameterSetType GetType() const =0;
  virtual ParameterSet *Clone() const =0;
};

enum VarianceType {
  POINT_VARIANCES,     // variance of for each lead time
  CO_VARIANCES,        // covariances for cross prod of all lead times
  SUM_VARIANCES        // ith variance is variance for sum to i
};


class Predictor {
public:
	virtual ~Predictor() {} 
	virtual int Begin()=0;
        virtual int StepsToPrime()=0;
	virtual double Step(double obs)=0;
	virtual int Predict(int maxahead, double *predictions)=0;
	virtual int ComputeVariances(int maxahead, 
				     double *vars, 
				     VarianceType vtype=POINT_VARIANCES)=0;
        virtual void Dump(FILE *out=stdout)=0;
};

class Model {
public:
	virtual ~Model() {} ;
	virtual Predictor * MakePredictor()=0;
        virtual void Dump(FILE *out=stdout)=0;
//        virtual ParamterSet *GetParameterSet()=0;
};

class Modeler {
public:
   virtual ~Modeler() {}
   static Model *Fit(double *sequence,int len, const ParameterSet &ps);
};


#endif
