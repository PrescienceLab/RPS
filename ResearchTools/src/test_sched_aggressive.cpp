#include "Spin.h"
#include "RTA.h"
#include "random.h"
#include "TimeStamp.h"
#include "LoadMeasurement.h"

#include "glarp.h"

typedef Reference<BufferDataRequest,BufferDataReply<LoadMeasurement> > LoadMeasureBufferRef;


void usage()
{
  fprintf(stderr,"usage: test_sched numtestcases confidence minint:maxint minsecs:macsecs minslack:maxslack numhost numpred [host spin load [predtag pred]#numpred]#numhost\n");
}



#ifdef ALPHA
extern "C" int usleep(int);
#endif



struct HostHandler {
  char          *name;
  SpinServerRef *ssref;
  PredBufferRef **psrefs;
  LoadMeasureBufferRef *lmref;

  HostHandler(char *name, int numpred) { 
    this->name = new char [strlen(name)+1]; strcpy(this->name,name);
    psrefs = new PredBufferRef * [numpred];
  }
    
  virtual ~HostHandler() { 
    delete [] name;
    delete [] psrefs;
  }

  virtual int RequestMeasurement() {
    BufferDataRequest req;
    req.num=1;
    if (lmref->SendRequest(req)) { 
      return -1;
    }
    return 0;
  }
  
  virtual double ReceiveMeasurement() { 
    BufferDataReply<LoadMeasurement> lmb;
    if (lmref->RecvResponse(lmb)) { 
      return -1;
    } 
    return lmb.data[0].avgs[0];
  }

  virtual double GetMeasurement() { 
    if (RequestMeasurement()) { 
      return -1;
    } else {
      return ReceiveMeasurement();
    }
  }

  virtual int GetPrediction(int    predictor,
			    ExecTimeEstimationRequest &req,
			    ExecTimeEstimationReply   &repl) {
    if (EstimateExecTime(*(psrefs[predictor]),req,repl)) { 
      return -1;
    }
    return 0;
  }

  virtual int RequestPrediction(int    predictor,
				ExecTimeEstimationRequest &req) {
    BufferDataRequest breq;
    breq.num=1;
    if (psrefs[predictor]->SendRequest(breq)) { 
      return -1;
    } 
    return 0;
  }

  virtual int ReceivePrediction(int   predictor,
			    ExecTimeEstimationRequest &req,
			    ExecTimeEstimationReply   &repl) {
    BufferDataReply<PredictionResponse> bresp;
    if (psrefs[predictor]->RecvResponse(bresp)) { 
      return -1;
    } 
    if (EstimateExecTime(bresp.data[0],req,repl)) { 
      return -1;
    }
    return 0;
  }
    

  virtual int Run(SpinRequest &req, SpinReply &repl) {
    if (ssref->Call(req,repl)) { 
      return -1;
    }
    return 0;
  }
};



int main(int argc, char *argv[]) 
{
  int numtestcases;
  int numhosts;
  int numpreds;
  int firsthostarg;
  int predictor;
  double confidence;
  double minint,maxint;
  double minsecs, maxsecs;
  double minslack, maxslack;
  int i,j;
  char       **prednames;
  HostHandler       **hosts;

  if (argc<12) {
    usage();
    exit(-1);
  }

  numtestcases = atoi(argv[1]);
  confidence = atof(argv[2]);

  if ((sscanf(argv[3],"%lf:%lf",&minint,&maxint))!=2) { 
    fprintf(stderr,"Can't parse %s\n",argv[2]);
    exit(-1);
  }

  if ((sscanf(argv[4],"%lf:%lf",&minsecs,&maxsecs))!=2) { 
    fprintf(stderr,"Can't parse %s\n",argv[3]);
    exit(-1);
  }

  if ((sscanf(argv[5],"%lf:%lf",&minslack,&maxslack))!=2) { 
    fprintf(stderr,"Can't parse %s\n",argv[4]);
    exit(-1);
  }

  numhosts = atoi(argv[6]);
  numpreds = atoi(argv[7]);


  ExecTimeEstimationRequest  ereq;
  ExecTimeEstimationReply   *erepl = new ExecTimeEstimationReply [numhosts];
  int *possiblehosts = new int [numhosts];

  hosts = new HostHandler * [numhosts];
  prednames = new char * [numpreds+2];
  for (i=0;i<numpreds+2;i++) { prednames[i]=0; }

  firsthostarg=8;

  for (i=0;i<numhosts;i++ ) {
    EndPoint ep;
    hosts[i] = new HostHandler(argv[firsthostarg+i*(3+numpreds*2)+0],numpreds);
    if (ep.Parse(argv[firsthostarg+i*(3+numpreds*2)+1])) { 
      fprintf(stderr,"Can't parse spinserver %s\n",argv[firsthostarg+i*(3+numpreds*2)+1]);
      goto fail;
    }
    hosts[i]->ssref = new SpinServerRef;
    if (hosts[i]->ssref->ConnectTo(ep)) { 
      fprintf(stderr,"Can't connect to spinserver %s\n",argv[firsthostarg+i*(3+numpreds*2)+1]);
      goto fail;
    }
    if (ep.Parse(argv[firsthostarg+i*(3+numpreds*2)+2])) { 
      fprintf(stderr,"Can't parse loadbuffer %s\n",argv[firsthostarg+i*(3+numpreds*2)+2]);
      goto fail;
    }
    hosts[i]->lmref = new LoadMeasureBufferRef;
    if (hosts[i]->lmref->ConnectTo(ep)) { 
      fprintf(stderr,"Can't connect to loadbuffer %s\n",argv[firsthostarg+i*(3+numpreds*2)+2]);
      goto fail;
    }
    for (j=0;j<numpreds;j++) {
      if (prednames[j]==0) { 
	prednames[j] = new char [strlen(argv[firsthostarg+i*(3+numpreds*2)+3+2*j])+1];
	strcpy(prednames[j],argv[firsthostarg+i*(3+numpreds*2)+3+2*j]);
      } else {
	if (strcasecmp(prednames[j],argv[firsthostarg+i*(3+numpreds*2)+3+2*j])) { 
	  fprintf(stderr,"Host '%s' has wrong predictor\n",hosts[j]->name);
	  goto fail;
	}
      }
      if (ep.Parse(argv[firsthostarg+i*(3+numpreds*2)+3+2*j+1])) { 
	fprintf(stderr,"Can't parse predictor %s\n",argv[firsthostarg+i*(3+numpreds*2)+3+2*j+1]);
	goto fail;
      }
      hosts[i]->psrefs[j] = new PredBufferRef;
      if (hosts[i]->psrefs[j]->ConnectTo(ep)) { 
	fprintf(stderr,"Can't connect to predictor %s\n",argv[firsthostarg+i*(3+numpreds*2)+3+2*j+1]);
	goto fail;
      }
    }
  }

  prednames[numpreds]="random";
  prednames[numpreds+1]="measure";

  double intsec;
  double computesec;
  double slack;
  int host;
  int numpossiblehosts;
  double minexpect;
  int minexpecthost;
  int choseminexp;

  int numinrange;
  int nummet;
  int numfailed;



  InitRandom();

  fprintf(stdout,"%%timestamp hostname predname tnom slack numpossible tlb texp tub tact inrange deadlinemet\n");

  numinrange=0;
  nummet=0;
  numfailed=0;

  for (i=0;i<numtestcases;i++) { 
    SpinRequest spinreq;
    SpinReply   spinrepl;
    TimeStamp now(0);

    intsec=UniformRandom(minint,maxint);
    computesec=UniformRandom(minsecs,maxsecs);
    slack = UniformRandom(minslack,maxslack);
    predictor =UnsignedRandom()%(numpreds+2);

    
    ereq.cputime=computesec;
    ereq.confidence=confidence;

    if (predictor < numpreds) {
      // Use predictor
      
      usleep((int)(intsec*1e6));
      
      for (j=0;j<numhosts;j++) {
	if (hosts[j]->RequestPrediction(predictor,ereq)) { 
	  fprintf(stderr,"Request Prediction Failed for Host %s Predictor %s\n",
		  hosts[j]->name,prednames[predictor]);
	  goto fail;
	}
      }
      for (j=0;j<numhosts;j++) { 
	if (hosts[j]->ReceivePrediction(predictor,ereq,erepl[j])) { 
	  fprintf(stderr,"Receive Prediction Failed for Host %s Predictor %s\n",
		  hosts[j]->name,prednames[predictor]);
	  goto fail;
	}
      }
      
      /* Schedule - first try to find possible hosts */
      numpossiblehosts=0;
      minexpect=99e99;
      for (j=0;j<numhosts;j++) { 
	if (erepl[j].ciupper < (1+slack)*ereq.cputime) { 
	  possiblehosts[numpossiblehosts]=j;
	  numpossiblehosts++;
	}
	if (erepl[j].expectedtime < minexpect) { 
	  minexpect=erepl[j].expectedtime;
	  minexpecthost = j;
	}
      }
      
      host = minexpecthost;
      choseminexp=1;
    } else if (predictor==numpreds) {
      // Select Random host
      host = UnsignedRandom()%numhosts;
      erepl[host].cputime=computesec;
      erepl[host].confidence=confidence;
      erepl[host].ciupper=erepl[host].cilower=erepl[host].expectedtime=computesec;
      numpossiblehosts=numhosts;
    } else if (predictor==(numpreds+1)) { 
      // select host with lowest load
      for (j=0;j<numhosts;j++) {
	if (hosts[j]->RequestMeasurement()) { 
	  fprintf(stderr,"Request Measurement Failed for Host %s (%s)\n",
		  hosts[j]->name,prednames[predictor]);
	  goto fail;
	}
      }
      double minload = 99e99;
      double thisload;
      host=0;
      for (j=0;j<numhosts;j++) { 
	if ((thisload=hosts[j]->ReceiveMeasurement())<0) { 
	  fprintf(stderr,"Receive Measurement Failed for Host %s (%s)\n",
		  hosts[j]->name,prednames[predictor]);
	  goto fail;
	}
	if (thisload<minload) { 
	  minload=thisload;
	  host=j;
	}
      }
      erepl[host].cputime=computesec;
      erepl[host].confidence=confidence;
      erepl[host].ciupper=erepl[host].cilower=erepl[host].expectedtime=computesec;
      numpossiblehosts=numhosts;
    }
    
    spinreq.secs=computesec;
    if (hosts[host]->Run(spinreq,spinrepl)) { 
      fprintf(stderr,"Can't run on host %s\n",hosts[host]->name);
      goto fail;
    }

    int deadlinemet = spinrepl.wallsecs < (1+slack)*erepl[host].cputime;
    //    int execinrange = spinrepl.wallsecs <= erepl[host].ciupper && 
    //                  spinrepl.wallsecs >= erepl[host].cilower;

    fprintf(stdout,"%f\t%s\t%s\t%f\t%f\t%d\t%f\t%f\t%f\t%f\t%s\t%s\n",
	    (double)now,hosts[host]->name, prednames[predictor],
	    spinreq.secs, slack, numpossiblehosts, erepl[host].cilower,
	    erepl[host].expectedtime,erepl[host].ciupper,
	    spinrepl.wallsecs, 
	    spinrepl.wallsecs < erepl[host].cilower ? "LOW" :
	      spinrepl.wallsecs > erepl[host].ciupper ? "HIGH" :
	      "in",
	    deadlinemet ? "met" : "MISSED"   );
    fflush(stdout);
	    
  }
   

  return 0;

 fail:

  return -1;
}
  
  
