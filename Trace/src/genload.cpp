#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>

#include "LoadTrace.h"
#include "timing.h"
#include "GetLoadAvg.h"
#include "apply.h"

#ifndef MAX
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#endif

int numworkers;
int *inpipes;
int *outpipes;
int *pids;

#define DEBUG 0

int dfprintf(FILE *where,...)
{
#if DEBUG
  va_list list;
  va_start(list,where);
  return vprintf(where,list);
#else 
  return 0;
#endif
}

/* Used internally */
#define EPSILON  0.05


void usage()
{
  fprintf(stderr,"usage: genload tau feedback networkbinarytracefile\n");
}



double Max(double *seq, int len)
{
  int i;
  double max;

  assert(len>0);

  max=seq[0];
  
  for (i=1;i<len;i++) {
    if (seq[i]>max) {
      max=seq[i];
    }
  }
  return max;
}

void DeconvolveLoadTrace(double tau,
			 double *tracein, 
			 int numsamples, 
			 double *traceout)
{
  int i;
  double Beta;

  Beta = exp(-1/tau);

  for (i=0;i<numsamples-1;i++) {
    traceout[i] = (tracein[i+1] - Beta*tracein[i])/(1-Beta);
  }
}


int main(int argc, char *argv[])
{
  int pipetemp1[2], pipetemp2[2];
  int myinpipe;
  int myoutpipe;
  int myid;
  int i,j;
  int i_am_master;
  int rc;
  double level, dur;
  double *trace;
  double *timestamps;
  int     numsamples;
  double  max;
  double *dtrace;   /* Deconvolved trace */
  double tau, feedbacklevel;
  double beta;
  double sum, sum2;

  if (argc!=4) {
    usage();
    exit(0);
  }

  tau=atof(argv[1]);
  feedbacklevel=atof(argv[2]);
  beta = exp(-1/tau);

  dfprintf(stderr,"Loading Trace %s...",argv[3]);
  numsamples=LoadNetworkBinaryTraceFile(argv[3],&timestamps,&trace);
  if (numsamples<0) {
    fprintf(stderr,"FAILED\n");
    exit(-1);
  }
  max=Max(trace,numsamples);
  dfprintf(stderr,"done (%d samples, max=%f)\n",numsamples,max);

  dfprintf(stderr,"Deconvolving Trace...");
  dtrace = (double*)malloc(sizeof(double)*(numsamples-1));
  DeconvolveLoadTrace(tau,trace,numsamples,dtrace);
  dfprintf(stderr,"done\n");

  dfprintf(stderr,"Calibrating Timing...");
  CalibrateTiming(EPSILON);
  dfprintf(stderr,"done\n");

  dfprintf(stderr,"Calibrating Loop...");
  CalibrateLoop(EPSILON);
  dfprintf(stderr,"done\n");


  numworkers=(int)(Max(dtrace,numsamples-1)+1);
  inpipes = (int *)malloc(sizeof(int)*numworkers);
  outpipes = (int *)malloc(sizeof(int)*numworkers);
  pids = (int *)malloc(sizeof(int)*numworkers);

  dfprintf(stderr,"Firing up %d workers...",numworkers); fflush(stdout);

    for (i=0;i<numworkers;i++) {
      pipe(pipetemp1);
      pipe(pipetemp2);
      if ((pids[i]=fork())!=0) {
	/* master */
	i_am_master=1;
	outpipes[i]= pipetemp1[1];
	close(pipetemp1[0]);
	inpipes[i] = pipetemp2[0];
	close(pipetemp2[1]);
	write(outpipes[i],&i,sizeof(int));
      } else {
	/* slave */
	i_am_master=0;
	myinpipe = pipetemp1[0];
	close(pipetemp1[1]);
	myoutpipe = pipetemp2[1];
	close(pipetemp2[0]);
	read(myinpipe,&myid,sizeof(int));
	break;
      }
    }

#if 0
  sleep(60); /* Wait for startup noise to die down */
#endif

  if (i_am_master) {
    double curloadavg, error;
    double feedback,targetlevel,requestlevel,desiredlevel;
    double prevmeasure;
    double dmeasure;

    sum=sum2=0;

    dfprintf(stderr,"done\n");
    /*printf("%%Applied\tDesired\tMeasured\tError\tPercent\n");*/
    prevmeasure=0;
    feedback=0;
    for (i=0;i<numsamples-1;i++) {
      dur=1.0;
      targetlevel=dtrace[i];
      desiredlevel=trace[i+1];
      requestlevel=targetlevel+feedback;
      dfprintf(stderr,"request=%f=%f+%f\n",requestlevel,targetlevel,feedback);
      requestlevel=MAX(requestlevel,0);
      for (j=0;j<(int)(requestlevel);j++) {
	level=1.0;
	write(outpipes[j],&level,sizeof(double));
	write(outpipes[j],&dur,sizeof(double));
      }
      level=requestlevel - (double)((int)(requestlevel));
      write(outpipes[(int)(requestlevel)],&level,sizeof(double));
      write(outpipes[(int)(requestlevel)],&dur,sizeof(double));
      for (j=0;j<=((int)requestlevel);j++) {
	read(inpipes[j],&rc,1);
      }
      RPSgetloadavg(&curloadavg,1);
      error=curloadavg-desiredlevel;
      sum+=error;
      sum2+=(error*error);
      if (i>0) {
	fprintf(stderr,"%d\t%f\t%f\t%f\t%f\r",i+1,MEANFROMSUMS(i+1,sum),
		STDFROMSUMS(i+1,sum,sum2),sum,sum2 );
      }
      fprintf(stdout,"%f\t%f\t%f\t%f\t%f\n",
	      requestlevel,desiredlevel,curloadavg,error,
	      desiredlevel>0 ? 100.0*(error)/desiredlevel : 0.0);
      fflush(stdout);
      dmeasure = (curloadavg - beta * prevmeasure)/(1-beta);
      prevmeasure = curloadavg;
      feedback = feedbacklevel*(dmeasure-requestlevel);
    }
    level=-99e99;
    dur=-99e99;
    for (i=0;i<numworkers;i++) {
      write(outpipes[i],&level,sizeof(double));
      write(outpipes[i],&dur,sizeof(double));

      waitpid(pids[i],&rc,0);
      dfprintf(stderr,"%d: exit(%d)\n",i,WEXITSTATUS(rc));
    }
  } else {
    /* Until done, do what master says */
    while (1) {
      read(myinpipe,&level,sizeof(double));	
      read(myinpipe,&dur,sizeof(double));
      dfprintf(stderr,"%d: Applying %f load for %f seconds\n",myid,level,dur);
      if (level<-99 && dur<-99) {
	exit(0);
      } else {
	ApplyLoad(level,dur);
	rc=0;
	write(myoutpipe,&rc,1);
      }
    }
  }

  free(dtrace);
  free(trace);
  free(inpipes);
  free(outpipes);
  free(pids);
  exit(0);
}
