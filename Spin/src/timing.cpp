#include <math.h>

#include "Timers.h"

#include "timing.h"


#define CALIBRATION_ITERS 1000000
/*#define CALIBRATION_ITERS 10000*/

static double TimingOverhead;

double TimeOnce(void (*func)(int),int arg)
{
  TimeValue before,after;
  IntervalValue dur;

  before.GetCurrentTime();
  func(arg);
  after.GetCurrentTime();
  TimeValue::Diff(before,after,&dur);
  return dur.GetSeconds();
}

double TimeOnceDouble(void (*func)(double),double arg)
{
  TimeValue before,after;
  IntervalValue dur;

  before.GetCurrentTime();
  func(arg);
  after.GetCurrentTime();
  TimeValue::Diff(before,after,&dur);
  return dur.GetSeconds();
}


/*
   func(arg) is repeatedly timed until the mean estimate is T+/-epsilon*T with 95% confidence
*/
double TimeIt(void (*func)(int),int arg, double epsilon)
{
  double curfrac;
  double sum;
  double sum2;
  double t;
  int count;
  int i;

  /* prime */
  count=0;
  sum=0;
  sum2=0;
  for (i=0;i<30;i++) {
    t=TimeOnce(func,arg);	
    sum+=t;
    sum2+=t*t;
    count++;
  }

  curfrac = CI95MEANHALF((double)count,sum,sum2)/MEANFROMSUMS((double)count,sum);

  while (curfrac > epsilon) { 
    t=TimeOnce(func,arg);	
    sum+=t;
    sum2+=t*t;
    count++;
    curfrac = CI95MEANHALF((double)count,sum,sum2)/MEANFROMSUMS((double)count,sum);
  } 

  return MEANFROMSUMS(count,sum)-count*TimingOverhead;
}
  

static void (*NullFuncHelper)();

static void NullFunc2()
{
}

static void NullFunc(int arg)
{
  int i;
  for (i=0;i<arg;i++) {
    NullFuncHelper();
  }
}


void CalibrateTiming(double epsilon)
{
  TimingOverhead=0;
  NullFuncHelper=NullFunc2;
  TimingOverhead=TimeIt(&NullFunc,CALIBRATION_ITERS,epsilon)/CALIBRATION_ITERS;
}
  
