#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include "PredComp.h"


void usage()
{
  fprintf(stderr,"usage: predlatclient netspec numsamples stats|dump\n");
}



int main(int argc, char *argv[]) 
{
  PredictionResponse pred;
  TimeStamp now;
  int numsamples;
  int i;
 

  if (argc!=4) {
    usage();
    exit(0);
  }

  EndPoint ep;

  if (ep.Parse(argv[1])) { 
    fprintf(stderr,"predlatclient: Can't parse \"%s\"\n",argv[1]);
    exit(-1);
  }

  numsamples=atoi(argv[2]);
  
  StreamingInputReference<PredictionResponse> source;

  if (source.ConnectTo(ep)) {
    fprintf(stderr,"predlatclient: Can't connect to \"%s\"\n",argv[1]);
    exit(-1);
  }

  double *lat = new double [numsamples];
  double *siz = new double [numsamples];


  TimeStamp begin(0);
	
  for (i=0;i<numsamples;i++) { 
    if (source.GetNextItem(pred)) { 
      fprintf(stderr,"predlatclient: Connection failed.\n");
      source.Disconnect();
      exit(-1);
    }
    now = TimeStamp(0);
    lat[i] = (double)(now) - (double)(pred.datatimestamp) ; 
    siz[i] = (double)(pred.GetPackedSize());
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

    double meanpredsize = Mean(siz,numsamples);
    double stdpredsize = StandardDeviation(siz,numsamples);

    printf("numsamples:    %d\n"
           "rate:          %f Hz\n"
	   "mean latency:  %f msec\n"
	   "mean 95%% int:  [ %f , %f ] msec\n"
	   "stddev:        %f msec\n"
	   "min:           %f msec\n"
	   "median:        %f msec\n"
	   "max:           %f msec\n"
           "predmeansize   %f bytes\n"
           "predstdsize    %f bytes\n",
	   numsamples,
	   rate,
	   mean,
	   mean-1.96*std/sqrt((double)numsamples),
	   mean+1.96*std/sqrt((double)numsamples),
	   std,
	   min,
	   median,
	   max,
	   meanpredsize,
           stdpredsize);
  } else {
    for (i=0;i<numsamples;i++) {
      printf("%f\n",lat[i]);
    }
  }
  
  return 0;
}
  
