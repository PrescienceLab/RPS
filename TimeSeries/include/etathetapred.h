#ifndef _etathetapred 
#define _etathetapred

#include "abstract.h"
#include "poly.h"

// Prediction of models of the form
//   etas(B) = theta(B)x_t
//   (1-eta_1B - eta2B^2 + ...) z_t = (1-theta_1B-theta_2B^2- ...) x_t
// using difference equations
//
// The expectation is that there is a leading 1 in the eta and theta
// polynomials passed to Initialize, and that the polynomial power be 0

#define CACHE_VARIANCES   1
#define MINCACHE   100
#define NUMVAR_FACTOR 2

class EtaThetaPredictor : public Predictor {
private:
   int    numeta, numtheta;
   double *etas;
   double *thetas;
   double variance;
   double *values;
   double *errors;
   int    numsamples;
   double next_val;
   double mean;
   int     cachenumvar;
   double *pointvariancecache;
   double *sumvariancecache;
   int RecomputePointAndSumVarianceCaches(const int maxahead);
public:
   EtaThetaPredictor();
   EtaThetaPredictor(const EtaThetaPredictor &rhs);
   virtual ~EtaThetaPredictor();

   EtaThetaPredictor & operator=(const EtaThetaPredictor &rhs);

   int Initialize(const Polynomial *et, const Polynomial *th, const double var, const double mean=0.0);
   int Begin();
   int StepsToPrime() const;
   // Feed the filter with a new observation - returns predicted next value
   double Step(const double observation);
   int Predict(const int maxahead, double *pred) const;
   int ComputeVariances(const int maxahead, double *vars, 
			const enum VarianceType vtype=POINT_VARIANCES) const;
   int ComputeCoVariances(const int maxahead, double *covars) const ;
   int ComputeSumVariances(const int maxahead, double *sumvars) const;
   int ComputePointVariances(const int maxahead, double *pointvars) const;

   void Dump(FILE *out=stdout) const;
   ostream & operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const EtaThetaPredictor &p) {
  return p.operator<<(os);
}

#endif
