#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "timing.h"
#include "getloadavg.h"
#include "apply.h"

#define DEBUG 1
#define EPSILON  0.05


double EstimateTau(double sample_period, double sample_interval)
{
  double initload;
  double prevload;
  double curload;
  TimeValue inittime;
  TimeValue prevtime;
  TimeValue curtime;
  TimeInterval inittocur,inittoprev;
  int i;
  double compare;
  int numsamples = (int) (sample_period/sample_interval);

#if DEBUG
  fprintf(stderr,"Pumping up load average now (%lf seconds)\n",sample_period);  
#endif
  ApplyLoad(1.0,sample_period);
  GetCurrentTime(&inittime);
  RPSgetloadavg(&initload,1);
  prevload=initload;
  prevtime=inittime;
  compare=(1/exp(1))*initload;
#if DEBUG
  fprintf(stderr,"Observing decay from %lf to %lf now\n",initload,compare);
#endif
  for (i=1;i<numsamples;i++) {
    Sleep(sample_interval);
    GetCurrentTime(&curtime);
    RPSgetloadavg(&curload,1);
    if (curload <= compare) {
      double timetoprev,timetocur,frac,est;
      
      DiffTimes(&inittime,&prevtime,&inittoprev);
      DiffTimes(&inittime,&curtime,&inittocur);
      timetoprev=TimeIntervalToSeconds(&inittoprev);
      timetocur=TimeIntervalToSeconds(&inittocur);
      /* Linear interpolation */
      frac = ((1/exp(1))*initload - prevload)/(curload-prevload);
      est = timetoprev+frac*(timetocur-timetoprev);
#if DEBUG
      fprintf(stderr,"Load at step %d is %lf - est=%lf \n",i,curload,est);
#endif
      return est;
    } else {
      prevload=curload;
      prevtime=curtime;
    }
  }
  fprintf(stderr,"LOAD DID NOT DECLINE SUFFICIENTLY\n");
  return 99e99;
}


void usage()
{
  fprintf(stderr,"find_tau sample_period sample_interval numsamples\n");
}

void main(int argc, char *argv[])
{
  double sum, sum2, mn, std;
  double tauest;
  double sample_period, sample_interval;
  int i;
  int numsamples;

  if (argc<4) {
    usage();
    exit(0);
  }
  sample_period=atof(argv[1]);
  sample_interval=atof(argv[2]);
  numsamples = atoi(argv[3]);

  CalibrateTiming(EPSILON);
  CalibrateLoop(EPSILON);

  sum=sum2=0;
  for (i=0;i<numsamples;i++) {
    tauest=EstimateTau(sample_period,sample_interval);
    fprintf(stderr,"Estimate %d is %lf\n",i,tauest);
    sum+=tauest;
    sum2+=tauest*tauest;
  }

  fprintf(stderr,"sum2=%lf\n",sum2);
  printf("Tau is estimated to be %lf ",sum/numsamples);
  if (numsamples<2) {
    printf("\n");
  } else {
    printf("+/- %lf seconds with 95%% confidence\n",
	   CI95MEANHALF(numsamples,sum,sum2));
  }
  
}

  
    
  
    
