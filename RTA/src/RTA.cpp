#include "RTA.h"


int PredictRunningTime(RunningTimePredictionRequest   &req,
		       RunningTimePredictionResponse  &resp)
{
  Mapping *m=FindComponent(req.host,RPS_PREDBUFFER);
  if (!m) {
    return -1;
  } else {
    PredBufferRef ref;
    int rc;
    if ((rc=ref.ConnectTo(m->endpoints[0]))==0) {
	ExecTimeEstimationRequest eq = {req.conf,req.tnom};
	ExecTimeEstimationReply   er;
	rc=EstimateExecTime(ref,eq,er);
	if (rc==0) { 
	  resp.host=req.host;
	  resp.tnom=req.tnom;
	  resp.conf=req.conf;
	  resp.texp=er.expectedtime;
	  resp.tlb=er.cilower;
	  resp.tub=er.ciupper;
	} 
	ref.Disconnect();
	delete m;
	return rc;
    } else {
      delete m;
      return rc;
    }
  }
}
