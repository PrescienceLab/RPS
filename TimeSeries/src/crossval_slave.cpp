#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
extern "C" {
#include "pvm3.h"
	   }
#include "crossval_core.h"
#include "crossval_pvm.h"

#include "banner.h"

void usage(const char *n) 
{
  char *s=GetAvailableModels();
  char *b=GetRPSBanner();

  fprintf(stdout,
	  "Randomized evaluation using parallelism\n\n"
	  "usage: [should not be used directly - crossval_master calls this]\n"
	  "\n\n%s\n",b);
  delete [] b;
  delete [] s; 
}



int main(int argc, char *argv[])
{

  if (argc>1) {
    usage(argv[0]);
    exit(-1);
  }

  int mytid=pvm_mytid();
  int mastertid=pvm_parent();

  if (mastertid==PvmNoParent || mytid<0) { 
    usage(argv[0]);
    exit(-1);
  }

  //pvm_advise(PvmRouteDirect);
  pvm_setopt(PvmRoute,PvmRouteDirect);

  pvm_notify(PvmTaskExit,TASKFAIL,1,&mastertid);

  double *seq=0;
  int numsamples;
  int bmp;


  Testcase testcase;
  IntervalStats fitstats, teststats;

  int done=0;
  while (!done) {
    int bufid=pvm_recv(mastertid,-1);
    int bytes,msgtag,tid;
    pvm_bufinfo(bufid,&bytes,&msgtag,&tid);
    switch (msgtag) {
    case LOADTRACE:
      pvm_upkint(&numsamples,1,1);
      CHK_DEL_MAT(seq); 
      seq=new double [numsamples];
      pvm_upkdouble(seq,numsamples,1);
      pvm_initsend(PvmDataDefault);
      pvm_send(mastertid,READY);
      break;
    case JOB:
      testcase.Unpack(PvmUnpackInts, PvmUnpackDoubles, PvmUnpackString);
      
      PredictionStats *testmodelstats, *bmmodelstats;

      Simulate(seq,
	       &testcase,
	       &fitstats,
	       &teststats,	
	       &bmp, 
	       &testmodelstats, &bmmodelstats);


      pvm_initsend(PvmDataDefault);

      testcase.Pack(PvmPackInts,PvmPackDoubles,PvmPackString);

      fitstats.Pack(PvmPackInts, PvmPackDoubles);
      teststats.Pack(PvmPackInts, PvmPackDoubles);

      pvm_pkint(&bmp,1,1);
      testmodelstats->Pack(PvmPackDoubles,PvmPackInts);
      bmmodelstats->Pack(PvmPackDoubles,PvmPackInts);

      fprintf(stdout,"Send RESULT to master (%d)\n",mastertid); fflush(stdout);
      pvm_send(mastertid,RESULT);

      pvm_initsend(PvmDataDefault);
      fprintf(stdout,"Send READY to master (%d)\n",mastertid); fflush(stdout);
      pvm_send(mastertid,READY);

      delete testmodelstats;	
      delete bmmodelstats;
      
      break;
    case TASKFAIL:
    case DONE:
      done=1;
      break;
    default:
      fprintf(stderr,"Recieved message with unknown tag %d\n",msgtag);
      pvm_exit();
      exit(-1);
    }
  }
  pvm_exit();
  exit(0);
}



