#ifndef _EstimateExecTime
#define _EstimateExecTime
#include "PredictionRequestResponse.h"
#include "Buffer.h"
#include "Reference.h"

#include <stdio.h>

#define DISCOUNT_LOAD 1
#define DEFAULT_DISCOUNT_TAU 4.5

typedef Reference<BufferDataRequest,BufferDataReply<PredictionResponse> > PredBufferRef;

struct ExecTimeEstimationRequest {
  double confidence;
  double cputime;
  void Print(FILE *out=stdout);
};


struct ExecTimeEstimationReply {
  double confidence;
  double cputime;
  double expectedtime;
  double cilower;
  double ciupper;
  double meanload;
  double discountedmeanload;
  double varload;
  void Print(FILE *out=stdout);
};


int GetLatestPrediction(PredBufferRef &ref, PredictionResponse &resp);

int EstimateExecTime(PredBufferRef &ref, 
		     ExecTimeEstimationRequest &req,
		     ExecTimeEstimationReply   &repl,
		     double discount_tau=DEFAULT_DISCOUNT_TAU);

int EstimateExecTime(PredictionResponse        &presp, 
		     ExecTimeEstimationRequest &req,
		     ExecTimeEstimationReply   &repl,
		     double discount_tau=DEFAULT_DISCOUNT_TAU);


#endif
