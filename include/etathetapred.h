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
public:
   EtaThetaPredictor();
   virtual ~EtaThetaPredictor();
   int Initialize(Polynomial *et, Polynomial *th, double var, double mean=0.0);
   int Begin();
   int StepsToPrime();
   // Feed the filter with a new observation - returns predicted next value
   double Step(double observation);
   int Predict(int maxahead, double *pred);
   int ComputeVariances(int maxahead, double *vars, 
			enum VarianceType vtype=POINT_VARIANCES);
   int ComputeCoVariances(int maxahead, double *covars);
   int ComputeSumVariances(int maxahead, double *sumvars);
   int ComputePointVariances(int maxahead, double *pointvars);
   void Dump(FILE *out=stdout);
};
#endif
