#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Mirror.h"
#include "Spin.h"
#include "GetLoadAvg.h"

#define DEBUG_TAU 1
#define EPSILON  0.05


double EstimateTau(double sample_period, double sample_interval)
{
  double initload;
  double prevload;
  double curload;
  TimeValue inittime;
  TimeValue prevtime;
  TimeValue curtime;
  IntervalValue inittocur,inittoprev;
  int i;
  double compare;
  int numsamples = (int) (sample_period/sample_interval);

#if DEBUG_TAU
  fprintf(stderr,"Pumping up load average now (%f seconds)\n",sample_period);  
#endif
  ApplyLoad(1.0,sample_period);
  inittime.GetCurrentTime();
  RPSgetloadavg(&initload,1);
  prevload=initload;
  prevtime=inittime;
  compare=(1/exp(1.0))*initload;
#if DEBUG_TAU
  fprintf(stderr,"Observing decay from %f to %f now\n",initload,compare);
#endif
  for (i=1;i<numsamples;i++) {
    Sleep(sample_interval);
    curtime.GetCurrentTime();
    RPSgetloadavg(&curload,1);
    if (curload <= compare) {
      double timetoprev,timetocur,frac,est;
      
      TimeValue::Diff(inittime,prevtime,&inittoprev);
      TimeValue::Diff(inittime,curtime,&inittocur);
      timetoprev=inittoprev.GetSeconds();
      timetocur=inittocur.GetSeconds();
      /* Linear interpolation */
      frac = ((1/exp(1.0))*initload - prevload)/(curload-prevload);
      est = timetoprev+frac*(timetocur-timetoprev);
#if DEBUG_TAU
      fprintf(stderr,"Load at step %d is %f - est=%f \n",i,curload,est);
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

int main(int argc, char *argv[])
{
  double sum, sum2;
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
    fprintf(stderr,"Estimate %d is %f\n",i,tauest);
    sum+=tauest;
    sum2+=tauest*tauest;
  }

  fprintf(stderr,"sum2=%f\n",sum2);
  printf("Tau is estimated to be %f ",sum/numsamples);
  if (numsamples<2) {
    printf("\n");
  } else {
    printf("+/- %f seconds with 95%% confidence\n",
	   CI95MEANHALF((double)numsamples,sum,sum2));
  }
  
}

  
    
  
    
