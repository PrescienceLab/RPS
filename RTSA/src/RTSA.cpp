#include "RTSA.h"
#include "RTA.h"
#include "Finder.h"
#include "random.h"



RTSARequest::~RTSARequest()
{
  for (int i=0;i<numhosts;i++) { 
    delete hosts[i];
  }
  delete [] hosts;
}


int RTSAAdviseTask(RTSARequest &req, RTSAResponse &resp)
{
  int i;
  bool valid[req.numhosts];
  PredBufferRef           pref[req.numhosts];
  BufferDataRequest                   breq;
  BufferDataReply<PredictionResponse> brepl[req.numhosts];


  breq.num=1;
  for (i=0;i<req.numhosts;i++) {
    valid[i]=false;
    Mapping *m=FindComponent(req.hosts[i]->name,RPS_PREDBUFFER);
    if (m==0) {
    } else {
      if (pref[i].ConnectTo(m->endpoints[0])) { 
	delete m;
      } else {
	delete m;
	if (!pref[i].SendRequest(breq)) { 
	  valid[i]=true;
	}
      }
    }
  }
   
  for (i=0;i<req.numhosts;i++) { 
    if (valid[i]) { 
      if (pref[i].RecvResponse(brepl[i])) { 
	valid[i]=false;
      }
      pref[i].Disconnect();
    }
  }

  ExecTimeEstimationRequest rtareq;
  ExecTimeEstimationReply   rtaresp[req.numhosts];
  rtareq.confidence=req.conf;
  rtareq.cputime=req.tnom;
  int numvalid=0;
  for (i=0;i<req.numhosts;i++) {
    if (valid[i]) { 
      if (EstimateExecTime(brepl[i].data[0],rtareq,rtaresp[i])) { 
	valid[i]=false;
      } else {
	numvalid++;
      }
    }
  }
   
  if (numvalid==0) {
    return -1;
  }

  /* Schedule - first try to find possible hosts */
  int possiblehosts[req.numhosts];
  int numpossiblehosts;
  int minexpecthost;
  int selectedhost;
  double minexpect;

  numpossiblehosts=0;
  minexpect=99e99;
  for (i=0;i<req.numhosts;i++) { 
    if (valid[i]) {
      if (rtaresp[i].ciupper < (1+req.sf)*req.tnom) { 
	possiblehosts[numpossiblehosts]=i;
	numpossiblehosts++;
      }
      if (rtaresp[i].expectedtime < minexpect) { 
	minexpect=rtaresp[i].expectedtime;
	minexpecthost = i;
      }
    }
  }
      
  /* If there are possible hosts, pick one at random */
  if (numpossiblehosts>0) {
    selectedhost = possiblehosts[UnsignedRandom()%numpossiblehosts];
  } else {
    /* Pick the host with the minimum expected time */
    selectedhost = minexpecthost;
  }

  resp.tnom=req.tnom;
  resp.sf=req.sf;
  resp.conf=req.conf;
  resp.host = *(req.hosts[selectedhost]);
  resp.runningtime.tnom=req.tnom;
  resp.runningtime.conf=req.conf;
  resp.runningtime.texp=rtaresp[selectedhost].expectedtime;
  resp.runningtime.tlb=rtaresp[selectedhost].cilower;
  resp.runningtime.tub=rtaresp[selectedhost].ciupper;

  return 0;

}
