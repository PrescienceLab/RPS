#include <stdio.h>
#include <stdlib.h>

#include "EstimateExecTime.h"
#include "SpinServer.h"

void usage()
{
  fprintf(stderr,"usage: play loadpredspec spinspec\n");
}


int main(int argc, char *argv[])
{

  if (argc!=3) { 
    usage();
    exit(0);
  }

  EndPoint predep, spinep;

  if (predep.Parse(argv[1])) { 
    fprintf(stderr,"Can't parse %s\n",argv[1]);
    exit(-1);
  }
  
  if (spinep.Parse(argv[2])) { 
    fprintf(stderr,"Can't parse %s\n",argv[2]);
    exit(-1);
  }

  PredBufferRef pref;
  SpinServerRef sref;

  if (pref.ConnectTo(predep)) { 
    fprintf(stderr,"Can't connect to %s\n",argv[1]);
    exit(-1);
  }

  if (sref.ConnectTo(spinep)) { 
    fprintf(stderr,"Can't connect to %s\n",argv[2]);
    exit(-1);
  }

  ExecTimeEstimationRequest req;
  ExecTimeEstimationReply repl;

  SpinRequest spinreq;
  SpinReply   spinrepl;

  double tau;

  while (1) { 
    fprintf(stdout, "cputime confidence tau ? ");
    fflush(stdin);
    fflush(stdout);
    if (fscanf(stdin,"%lf %lf %lf",&(req.cputime), &(req.confidence), &tau)!=3) { 
      break;
    }
    if (EstimateExecTime(pref,req,repl,tau)) { 
      fprintf(stderr,"EstimateExecTime FAILED\n");
      break;
    }
    fprintf(stderr,"request = %lf seconds at %lf %% confidence\n",
	    req.cputime, 100.0*req.confidence);
    fprintf(stderr,"predict = [%lf, %lf] seconds @ %lf %% confidence\n"
                   "          cputime=%lf, expectedtime=%lf\n",
	    repl.cilower,repl.ciupper,
	    repl.confidence*100.0, repl.cputime, repl.expectedtime);
    spinreq.secs=req.cputime;
    sref.Call(spinreq,spinrepl);
    fprintf(stderr,"actual  = %lf seconds (%lf usr, %lf sys)\n",
	    spinrepl.wallsecs, spinrepl.usrsecs, spinrepl.syssecs);
    fprintf(stderr,"error   = %lf secs, %lf %%, %s by %lf\n",
	    repl.expectedtime-spinrepl.wallsecs,
	    100.0*(repl.expectedtime-spinrepl.wallsecs)/repl.expectedtime,
	    spinrepl.wallsecs < repl.cilower ? "BELOW CI" :
	    spinrepl.wallsecs > repl.ciupper ? "ABOVE CI" : "IN CI",
	    spinrepl.wallsecs < repl.cilower ? repl.cilower-spinrepl.wallsecs :
	    spinrepl.wallsecs > repl.ciupper ? spinrepl.wallsecs-repl.ciupper :
	    MAX(spinrepl.wallsecs-repl.cilower,repl.ciupper-spinrepl.wallsecs));
  }

  pref.Disconnect();
  exit(0);
}
