#ifndef _RTA
#define _RTA

#include "Finder.h"
#include "EstimateExecTime.h"


struct RunningTimePredictionRequest {
  Host   host;
  double tnom;
  double conf;
};

struct RunningTimePredictionResponse {
  Host   host;
  double tnom;
  double conf;
  double texp;
  double tlb;
  double tub;
};


int PredictRunningTime(RunningTimePredictionRequest   &req,
		       RunningTimePredictionResponse  &resp);


#if 0

struct AvailableTimePredictionRequest {
  Host   host;
  double time;
  double conf;
};

struct AvailableTimePredictionResponse {
  Host   host;
  doube  time;
  double conf;
  double texp;
  double tlb;
  double tub;
}


int PredictAvailableTime(AvailableTimePredictionRequest   &req,
		         AvailableTimePredictionResponse  &resp);


#endif

#endif
