#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>


#include "LoadTrace.h"
#include "banner.h"


/* Used internally */
#define EPSILON  0.05


void usage(const char *n)
{
  char *b=GetRPSBanner();
  fprintf(stdout,
	  "Remove exponential smoothing from a network byte order trace file\n\n"
	  "usage: %s tau innetworkbinarytracefile outnetworkbinarytracefile\n"
	  "tau                       = time constant of smoothing filter\n"
	  "innetworkbinarytracefile  = input trace in network byte order\n"
	  "outnetworkbinarytracefile = output trace in network byte order\n\n%s",n,b);
  delete [] b;
}





void DeconvolveLoadTrace(double tau,
			 double *timestamps,
			 double *tracein, 
			 int numsamples, 
			 double *traceout)
{
  int i;
  double Beta;
  double interval;


  
  for (i=0;i<numsamples-1;i++) {
    interval = timestamps[i+1] - timestamps[i];
    Beta = exp(-interval/tau);
    traceout[i] = (tracein[i+1] - Beta*tracein[i])/(1-Beta);
  }
}


int main(int argc, char *argv[])
{
  int numsamples;
  double *vals;
  double *timestamps;
  double *dtrace;
  double tau;
  
  if (argc!=4) {
    usage(argv[0]);
    exit(0);
  }
  
  tau=atof(argv[1]);
  
  numsamples = LoadNetworkBinaryTraceFile(argv[2],&timestamps,&vals);
  
  if (numsamples<0) {
    fprintf(stderr,"FAILED\n");
    exit(-1);
  }
  
  dtrace = new double [numsamples-1];
  
  DeconvolveLoadTrace(tau,timestamps,vals,numsamples,dtrace);
  
  int numout = StoreNetworkBinaryTraceFile(argv[3],timestamps,dtrace,numsamples-1);

  fprintf(stderr, "%d in, %d out\n",numsamples,numout);

  delete [] dtrace;
  delete [] timestamps;
  delete [] vals;

  return 0;
}
