#ifndef _etathetapred 
#define _etathetapred

#include "abstract.h"
#include "poly.h"

// Prediction of models of the form
//   eta(B)z_t = theta(B)x_t
// using difference equations

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
public:
   ARIMAPredictor();
   virtual ~ARIMAPredictor();
   int Initialize(int numeta, numtheta);
   int SetEta(Polynomial *eta);
   int SetTheta(Polynomial *th);
   int Begin();
   // Feed the filter with a new observation - returns predicted next value
   double Step(double observation);
   int Predict(int maxahead, double *pred);
   int ComputeVarianceEstimates(int maxahead, double *vars);

   void Dump(FILE *out=stdout);
};
