#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
extern "C" {
#include "pvm3.h"
#include "pvmtev.h"
	   }
#include "crossval_core.h"
#include "crossval_pvm.h"
#include <time.h>
#include "random.h"


// input file format is
//
// numtests numahead  bmlimit pmin pmax qmin qmax dmin dmax minfit maxfit mintest maxtest  
// ...
// dmin=-1 -> arfima model
//
// numslaves = 0 => run locally

void usage() 
{
   fprintf(stderr,"crossval_master [numslaves] [cmdfile] [binarytracefile] [tag] [old|new]\n");
}


const int xpvmtrace=0;
void SetupForTrace();

void main(int argc, char *argv[])
{
  int i;

  if (argc<5){
    usage();
    exit(-1);
  }
  
  int numslaves = atoi(argv[1]);
  char *cmdfile=argv[2];
  char *tracefile=argv[3];
  char *tag=argv[4];
  
  int outputformat=0;
  if (argc==6) {
    if (!strcmp(argv[5],"old")) {
      outputformat=1;
    } else {
      outputformat=0;
    }
  }

  FILE *cmd = fopen(cmdfile,"r");
  if (cmd==0) {
    fprintf(stderr,"Can't open command file %s\n",cmdfile);
    exit(-1);
  }

  int numsamples;
  double *seq = LoadBinaryTraceFile(tracefile,&numsamples);
  if (seq==0) {
    fprintf(stderr,"Can't load trace file %s\n",tracefile);
    exit(-1);
  }
  //fprintf(stderr,"Read %d samples from %s\n",numsamples,tracefile);

  InitRandom();

  OutputHeader(stdout,outputformat);
  
  Command *command;

  if (numslaves==0) {  // execute locally
    int testnum;
    while ((command=GetCommand(cmd))!=0) {
      TestcaseGenerator testcasegen(command,tag,numsamples);
      testnum=0;
      while (testnum<command->numtests) {
	Testcase *testcase = testcasegen.GenerateRandomTestcase();
	IntervalStats fitstats, teststats;
	PredictionStats *testmodelstats, *bmmodelstats;
	int bmp;
	
	Simulate(seq,
		 testcase,
	         &fitstats,
		 &teststats,	
		 &bmp, 
		 &testmodelstats, &bmmodelstats);

	if (testmodelstats->valid && bmmodelstats->valid) {
	  OutputTestcaseResults(stdout,
				testcase,
				&fitstats,	
				&teststats,	
				bmp,
				testmodelstats,
				bmmodelstats,
				outputformat);	
	  ++testnum;
	} else {
	  fprintf(stderr,"ignoring invalid prediction results (%d,%d)\n",	
		  bmmodelstats->usertags,testmodelstats->usertags);
	}

	delete testmodelstats;
	delete bmmodelstats;
	delete testcase;
      }
      delete command;
    }
  } else {
    // Initialize
    int mytid = pvm_mytid();
    if (mytid<0) {
      fprintf(stderr,"pvm startup failed\n");
      pvm_exit();	
    }

    // option setup
    //pvm_advise(PvmRouteDirect);
    pvm_setopt(PvmRoute,PvmRouteDirect);

    if (xpvmtrace) {	
      SetupForTrace();
    }

    //spawn slaves
    int *tids = new int [numslaves];
    int numt = pvm_spawn("crossval_slave",0,
			 PvmTaskDefault + (xpvmtrace ? PvmTaskTrace : 0),
			 0,numslaves,tids);
    if (numt!=numslaves) {
      int i;
      fprintf(stderr,"failed to start up all slaves...error codes/shutdowns follow\n");
      for (i=0;i<numslaves;i++) {
	fprintf(stderr,"task %d: %d ",i,tids[i]);
	if (tids[i]>0) {
	  pvm_initsend(PvmDataDefault);
	  pvm_send(tids[i],DONE);
	  fprintf(stderr,"sent DONE\n");
	} 
      }
      pvm_exit();
      exit(-1);
    }

    pvm_notify(PvmTaskExit,TASKFAIL,numt,tids);
    

    // send the load trace
    pvm_initsend(PvmDataDefault);
    pvm_pkint(&numsamples,1,1);
    pvm_pkdouble(seq,numsamples,1);
    pvm_mcast(tids,numt,LOADTRACE);


    int issuedtests, completedtests;

    Command *command;

    while ((command=GetCommand(cmd))!=0) {
      issuedtests=completedtests=0;
      TestcaseGenerator testcasegen(command,tag,numsamples);
      while (completedtests<command->numtests) {
	/* receive a message from some slave */
	int bufid = pvm_recv(-1,-1);
	if (bufid<0) {
	  fprintf(stderr,"Failed to recv message from slave\n");
	  pvm_exit();
	  exit(-1);
	}
	int bytes, msgtag, tid;
	pvm_bufinfo(bufid,&bytes,&msgtag,&tid);
	switch (msgtag) {
	case READY:   {                              // ready
	  Testcase *testcase = testcasegen.GenerateRandomTestcase();

	  pvm_initsend(PvmDataDefault);
	
	  testcase->Pack(PvmPackInts,PvmPackDoubles,PvmPackString);
	
	  pvm_send(tid,JOB);

	  delete testcase;
	  //fprintf(stderr,"Sent JOB to %d\n",tid);
	  ++issuedtests;
	}
	break;
	case RESULT: {
	  //fprintf(stderr,"Received RESULT from %d\n",tid);
	  Testcase testcase;
	  IntervalStats fitstats,teststats;
	  int bmp;
 	  PredictionStats testmodelstats, bmmodelstats;

	  testcase.Unpack(PvmUnpackInts,PvmUnpackDoubles,PvmUnpackString);
	
	  fitstats.Unpack(PvmUnpackInts,PvmUnpackDoubles);
	  teststats.Unpack(PvmUnpackInts,PvmUnpackDoubles);
	
	  pvm_upkint(&bmp,1,1);
	  testmodelstats.Unpack(PvmUnpackDoubles,PvmUnpackInts);
	  bmmodelstats.Unpack(PvmUnpackDoubles,PvmUnpackInts);

	  if (testmodelstats.valid && bmmodelstats.valid) {
	    OutputTestcaseResults(stdout,
				  &testcase,
				  &fitstats,	
				  &teststats,	
				  bmp,
				  &testmodelstats,
				  &bmmodelstats,
				  outputformat);	
	    ++completedtests;
	  } else {
	    fprintf(stderr,"ignoring invalid prediction results (%d,%d)\n",	
		    bmmodelstats.usertags,testmodelstats.usertags);
	  }
	  //fprintf(stderr,"Unpacked RESULT from %d\n",tid);
	}
	break;
	case TASKFAIL: {
	  int thetid, i;
	  pvm_upkint(&thetid,1,1);
	  for (i=0;i<numt;i++) {
	    if (tids[i]==thetid) {
	      break;
	    }
	  }
	  fprintf(stderr,"slave %d died, respawning...\n",i);
	  while (pvm_spawn("crossval_slave",0,
			   PvmTaskDefault + (xpvmtrace ? PvmTaskTrace : 0),
			   0,1,&(tids[i]))!=1) {
	  }
	  pvm_notify(PvmTaskExit,TASKFAIL,1,&(tids[i]));
	  fprintf(stderr,"slave %d respawned...\n",i);
	  // send the load trace
	  pvm_initsend(PvmDataDefault);
	  pvm_pkint(&numsamples,1,1);
	  pvm_pkdouble(seq,numsamples,1);
	  pvm_send(tids[i],LOADTRACE);
	  fprintf(stderr,"load trace resent to slave %d\n",i);
	}
	break;
	default:
	  fprintf(stderr,"Unknown tag %d received\n",msgtag);
	  pvm_exit();
	  exit(-1);
	}
      }
      delete command;
    }
    pvm_initsend(PvmDataDefault);
    pvm_mcast(tids,numt,DONE);
    pvm_exit();
  }
  exit(0);

    
}


void SetupForTrace()
{
    int xpvm_tid,i;  
    Pvmtmask trace_mask;
    if ( (xpvm_tid = pvm_gettid( "xpvm", 0 )) > 0 ) {
      /* Set Self Trace & Output Destinations & Message Codes */ 
      pvm_setopt( PvmSelfTraceTid, xpvm_tid ); 
      pvm_setopt( PvmSelfTraceCode, 666 ); 
      pvm_setopt( PvmSelfOutputTid, xpvm_tid ); 
      pvm_setopt( PvmSelfOutputCode, 667 ); 
      /* Set Future Children's Trace & Output Dests & Codes */ 
      /* (optional) */ 
      pvm_setopt( PvmTraceTid, xpvm_tid ); 
      pvm_setopt( PvmTraceCode, 666 ); 
      pvm_setopt( PvmOutputTid, xpvm_tid ); 
      pvm_setopt( PvmOutputCode, 667 ); 
      /* Generate Default Trace Mask */ 
      TEV_INIT_MASK( trace_mask ); 
#if 1
	for (i=TEV_FIRST;i<=TEV_MAX;i++) 
	  TEV_SET_MASK(trace_mask,i);
#else
	TEV_SET_MASK( trace_mask, TEV_MCAST0 ); 
	TEV_SET_MASK( trace_mask, TEV_SEND0 ); 
	TEV_SET_MASK( trace_mask, TEV_RECV0 ); 
	TEV_SET_MASK( trace_mask, TEV_NRECV0 ); 
	TEV_SET_MASK( trace_mask, TEV_BCAST0);
	TEV_SET_MASK( trace_mask, TEV_INITSEND0);
	TEV_SET_MASK( trace_mask, TEV_PRECV0 ); 
	TEV_SET_MASK( trace_mask, TEV_PRECV1 ); 
	TEV_SET_MASK( trace_mask, TEV_PSEND0 ); 
	TEV_SET_MASK( trace_mask, TEV_PSEND1 ); 
#endif
	/* Add Other Desired Events Here */ 
	/* Set Self Trace Mask */ 
	pvm_settmask( PvmTaskSelf, trace_mask ); 
	/* Set Future Children's Trace Mask */ 
	/* (optional) */ 
	pvm_settmask( PvmTaskChild, trace_mask ); 
    }
}
