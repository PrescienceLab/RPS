#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "PredComp.h"

void usage()
{
  fprintf(stderr,"usage: loadlatclient netspec numsamples stats|dump\n");
}



int main(int argc, char *argv[]) 
{
  LoadMeasurement measure;
  TimeStamp now;
  int numsamples;
  int i;

  if (argc!=4) {
    usage();
    exit(0);
  }

  EndPoint ep;

  if (ep.Parse(argv[1])) { 
    fprintf(stderr,"loadlatclient: Can't parse \"%s\"\n",argv[1]);
    exit(-1);
  }
  
  numsamples=atoi(argv[2]);
  
  StreamingInputReference<LoadMeasurement> source;

  if (source.ConnectTo(ep)) {
    fprintf(stderr,"loadlatclient: Can't connect to \"%s\"\n",argv[1]);
    exit(-1);
  }

  double *lat = new double [numsamples];

  TimeStamp begin(0);

  for (i=0;i<numsamples;i++) { 
    if (source.GetNextItem(measure)) { 
       fprintf(stderr,"loadlatclient: Call failed\n");
       source.Disconnect();
       exit(-1);
    } else {
       now = TimeStamp(0);
       lat[i] = (double)(now) - (double)(measure.timestamp) ; 
       //    fprintf(stderr,"%f\t%f\n",(double)(now),(double)(measure.timestamp));
    }
  }

  TimeStamp end(0);

  source.Disconnect();

  if (argv[3][0]=='s' || argv[3][0]=='S') { 

    double rate = numsamples/((double)end - (double)begin);
    double mean = 1000.0*Mean(lat,numsamples);
    double std = 1000.0*StandardDeviation(lat,numsamples);
    double min = 1000.0*Min(lat,numsamples);
    double median = 1000.0*Median(lat,numsamples);
    double max = 1000.0*Max(lat,numsamples);

    printf("numsamples:    %d\n"
           "rate:          %f Hz\n"
	   "mean latency:  %f msec\n"
	   "mean 95%% int:  [ %f , %f ] msec\n"
	   "stddev:        %f msec\n"
	   "min:           %f msec\n"
	   "median:        %f msec\n"
	   "max:           %f msec\n"
           "sizeofloadmes  %d bytes\n",
	   numsamples,
	   rate,
	   mean,
	   mean-1.96*std/sqrt(numsamples),
	   mean+1.96*std/sqrt(numsamples),
	   std,
	   min,
	   median,
	   max,
	   measure.GetPackedSize());
  } else {
    for (i=0;i<numsamples;i++) {
      printf("%f\n",lat[i]);
    }
  }
  
  return 0;
}
  
