#ifndef _RTSA
#define _RTSA

#include "Finder.h"
#include "RTA.h"

struct RTSARequest {
  double tnom;
  double sf;
  double conf;
  int    numhosts;
  Host   **hosts;
  virtual ~RTSARequest();
};

struct RTSAResponse {
  double tnom;
  double sf;
  double conf;
  Host   host;
  RunningTimePredictionResponse runningtime;
};

int RTSAAdviseTask(RTSARequest &req,
		   RTSAResponse &resp);

#endif


