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
  static bool inited=false;
  int i;
  int rc;
  bool *valid = new bool [req.numhosts];
  PredBufferRef           *pref = new PredBufferRef[req.numhosts];
  BufferDataRequest                   breq;
  BufferDataReply<PredictionResponse> *brepl = new BufferDataReply<PredictionResponse>[req.numhosts];
  ExecTimeEstimationRequest rtareq;
  ExecTimeEstimationReply   *rtaresp = new ExecTimeEstimationReply[req.numhosts];
  int *possiblehosts = new int[req.numhosts];

  if (!inited) {
    InitRandom();
    inited=true;
  }

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
    rc=-1;
    goto LEAVE;
  }

  /* Schedule - first try to find possible hosts */

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

  rc=0;

LEAVE:
  delete [] valid;
  delete [] pref;
  delete [] brepl;
  delete [] rtaresp;
  delete [] possiblehosts;

  return rc;

}
