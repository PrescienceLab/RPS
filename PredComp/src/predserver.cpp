#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/wait.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
//#include <stropts.h>
#include <signal.h>
#include <math.h>



#include "Mirror.h"
#include "EndPoint.h"
#include "Measurement.h"
#include "PredictionRequestResponse.h"
#include "Buffer.h"
#include "Reference.h"
#include "predserver_core.h"

#include "pdqparamsets.h"

#include "glarp.h"
#include "debug.h"

#if defined(__osf__)
extern "C" int usleep(int);  // FREAKS
#endif

#define DEBUG 0

#define PREDSERVER_CORE "predserver_core"

int childpid;
int tochildfd;  
int fromchildfd;
int toparentfd;
int fromparentfd;

EndPoint measurebufferep;

bool started=false;
bool resendconfig=true;
PredictionReconfigurationRequest   curreq;
PredictionReconfigurationRequest   curconfig;
PredictionReconfigurationResponse  curconfigresp;


void usage()
{
  fprintf(stderr,"usage: predserver sourceflownetspec sourcebuffernetspec reconfigserverspec targetspecs\n");
}


class Prediction {
public:
  static int RestartChild() {
    int to[2], from[2];

    if (started) {
      close(tochildfd); 
      close(fromchildfd);
      kill(childpid,9);
      waitpid(childpid,0,0);
    }

    fprintf(stderr,"Starting predserver_core\n");
    if (pipe(to) || pipe(from)) {
      perror("predserver");
      return -1;
    }
    fromparentfd = to[0];
    tochildfd = to[1];
    fromchildfd = from[0];
    toparentfd = from[1];
    
    childpid=fork();

    if (childpid<0) { 
      perror("predserver failed to fork");
      return -1;
    }

    if (childpid) { // parent
      close(fromparentfd);
      close(toparentfd);
    } else {  // child
      close(tochildfd);
      close(fromchildfd);
      dup2(fromparentfd,fileno(stdin));
      dup2(toparentfd,fileno(stdout));
      execlp(PREDSERVER_CORE,PREDSERVER_CORE,0);
      // shoold never get here
      perror("predserver failed to exec predserver_core");
      exit(-1);
    }
    started=true;
    resendconfig=true;
    return 0;
  }


  static int SendConfigToChild(PredictionReconfigurationRequest &req) {
    PredServerCoreCommand cmd(NEW_CONFIG);
#if DEBUG
    fprintf(stderr,"SendConfigToChild - config is\n");
    req.Print(stderr);
#endif
    if (!started) { 
      goto fail;
    }
    if (cmd.Serialize(tochildfd)) { 
      goto fail;
    }
    if (req.Serialize(tochildfd)) { 
      goto fail;
    }
    if (curconfigresp.Unserialize(fromchildfd)) {
      goto fail;
    }
    return 0;
  fail:
    fprintf(stderr,"Failed to send config to child, config was:\n");
    req.Print(stderr);
    return -1;
  }

  static int UpdateCurConfigFitData() {
    Reference<BufferDataRequest,BufferDataReply<Measurement> > measurementbuffer;
    if (measurementbuffer.ConnectTo(measurebufferep)) {
      fprintf(stderr,"predserver: Can't connect to measurebuffer\n");
      return -1;
    }
    BufferDataRequest req;
    BufferDataReply<Measurement> resp;
    req.num=curreq.serlen;
    if (measurementbuffer.Call(req,resp)) { 
      measurementbuffer.Disconnect();
      fprintf(stderr,"predserver: Can't make call to measurebuffer\n");
      return -1;
    }
    curconfig = curreq;
    curconfig.period_usec = resp.data[0].period_usec;
    int i,j,k;
    for (i=0,k=0;i<resp.num && k<curreq.serlen;i++) {
      for (j=0;j<resp.data[i].serlen && k<curreq.serlen;j++,k++) {
	curconfig.series[k] = resp.data[i].series[j];
      }
    }
    curconfig.Resize(k);
    measurementbuffer.Disconnect();
    return 0;
  }
    

  static int SendCurConfigToChild() {
    return SendConfigToChild(curconfig);
  }


  static int Compute(Measurement &measure, PredictionResponse &pred) {
    PredServerCoreCommand cmd(NEW_MEASURE);
    if (!started) { 
      goto fail;
    }
    if (cmd.Serialize(tochildfd)) {
      goto fail;
    }
    if (measure.Serialize(tochildfd)) {
      goto fail;
    }
    if (pred.Unserialize(fromchildfd)) {
      goto fail;
    }

    return 0;
  fail:
    fprintf(stderr,"restarting dead or missing predserver_core and skipping measurement\n");
    if (RestartChild()) {
      fprintf(stderr,"Unable to restart predserver_core.  Exiting.\n");
      exit(-1);
    }
    if (SendCurConfigToChild()) { 
      fprintf(stderr,"Unable to send working config to predserver_core. Exiting.\n");
      exit(-1);
    }
    return 0;
  }
};

class Reconfiguration {
public:
  static int Compute(PredictionReconfigurationRequest &req, PredictionReconfigurationResponse &resp) {
    PredictionReconfigurationRequest oldreq=curreq;
    PredictionReconfigurationRequest oldconfig=curconfig;
    curreq=req;
    if (Prediction::UpdateCurConfigFitData()) { 
      goto fail;
    }
    if (Prediction::SendCurConfigToChild()) {
      goto fail;
    }
    goto done;
  fail:
    fprintf(stderr,"predserver reconfiguration FAILED - reverting to old config\n");
    curreq=oldreq;
    curconfig=oldconfig;
    if (Prediction::RestartChild()) {
      fprintf(stderr,"Cannot restart child - terminating.\n");
      exit(-1);
    }
    if (Prediction::SendCurConfigToChild()) { 
      fprintf(stderr,"Cannot revert to old config - terminating.\n");
      exit(-1);
    }
  done:
    resp=curconfigresp;
    return 0;
  }
};

typedef FilterWithControl<
  Measurement,
  Prediction,
  PredictionResponse,
  PredictionReconfigurationRequest,
  Reconfiguration,
  PredictionReconfigurationResponse
> PredictionMirror;



#define DEFAULT_SERLEN 300
#define DEFAULT_MODELCLASS AR
#define DEFAULT_P 16
#define DEFAULT_D 0
#define DEFAULT_Q 0
#define DEFAULT_R 0
#define DEFAULT_PERIOD 1000000
#define DEFAULT_NUMSTEPS 30

int main(int argc, char *argv[]) 
{
  EndPoint sourceep;
  EndPoint reconfigserverep;
  EndPoint *ep;

  if (argc<4) {
    usage();
    exit(0);
  }

  PredictionMirror mirror;

  if (sourceep.Parse(argv[1]) || sourceep.atype!=EndPoint::EP_SOURCE) { 
    fprintf(stderr,"Can't parse \"%s\"\n",argv[1]);
    exit(-1);
  }
  if (mirror.AddEndPoint(sourceep)) { 
    fprintf(stderr,"Can't add \"%s\"\n",argv[1]);
    exit(-1);
  }
  
  if (measurebufferep.Parse(argv[2]) || measurebufferep.atype!=EndPoint::EP_SOURCE) {
    fprintf(stderr,"Can't parse \"%s\"\n",argv[2]);
    exit(-1);
  }

  if (reconfigserverep.Parse(argv[3]) || reconfigserverep.atype!=EndPoint::EP_SERVER) {
    fprintf(stderr,"Can't parse \"%s\"\n",argv[3]);
    exit(-1);
  }
  if (mirror.AddEndPoint(reconfigserverep)) { 
    fprintf(stderr,"Can't add \"%s\"\n",argv[3]);
    exit(-1);
  }

  int i;
  for (i=4;i<argc;i++) { 
    ep = new EndPoint;
    if (ep->Parse(argv[i])) { 
      fprintf(stderr,"Can't parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    if (mirror.AddEndPoint(*ep)) { 
      fprintf(stderr,"Can't add \"%s\"\n",argv[i]);
      exit(-1);
    }
  }

 setinitialconfig:
  
  ParameterSet *ps;
  if (DEFAULT_R>0) { 
    ps = new RefittingPDQParameterSet(DEFAULT_P,DEFAULT_D,DEFAULT_Q,DEFAULT_R);
  } else {
    ps = new PDQParameterSet(DEFAULT_P,DEFAULT_D,DEFAULT_Q);
  }

  ModelTemplate mt(DEFAULT_MODELCLASS,*ps);
  ModelInfo mi(mt);

  curreq.modelinfo = mi;
  curreq.datatimestamp=TimeStamp();
  curreq.period_usec=DEFAULT_PERIOD;
  curreq.numsteps=DEFAULT_NUMSTEPS;
  curreq.Resize(DEFAULT_SERLEN,false);
  

  //AttachDebuggerHere(argv[0]);
  //BreakHere();

  do {
    if (Prediction::UpdateCurConfigFitData()) { 
      fprintf(stderr,"Can't get fit data\n");
      exit(-1);
    }
    if (curconfig.serlen==0) {
       // First pass - get idea of periodicity
       while (curconfig.serlen==0) {
          usleep(DEFAULT_PERIOD);
	  if (Prediction::UpdateCurConfigFitData()) { 
	    fprintf(stderr,"Can't get fit data\n");
	  }
      }
    }
    if (curconfig.serlen < DEFAULT_SERLEN) { 
      fprintf(stderr, "Only %d of %d samples are available, waiting for more...",curconfig.serlen,DEFAULT_SERLEN);
      usleep((curconfig.serlen > 0 ? curconfig.period_usec : DEFAULT_PERIOD)*(DEFAULT_SERLEN-curconfig.serlen));
      fprintf(stderr, "Trying again...");
    } else {
      fprintf(stderr, "%d samples acquired, initializing prediction services\n",curconfig.serlen);
    }
  } while (curconfig.serlen < DEFAULT_SERLEN);

  if ( Prediction::RestartChild()) { 
    fprintf(stderr,"Predserver cannot start predserver_core at startup\n");
    exit(-1);
  }
  if (Prediction::SendCurConfigToChild()) {
    fprintf(stderr,"Predserver cannot configure predserver_core at startup - sleeping and retrying\n");
    fprintf(stderr,"cofig was...\n");
    curconfig.Print(stderr);
    sleep(5+rand()%6);
#if 0
    exit(-1);
#else
    goto setinitialconfig;
#endif
  }

  fprintf(stderr,"predserver running.\n");
  mirror.Run();
}

