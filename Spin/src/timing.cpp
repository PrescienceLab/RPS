#include <sys/time.h>
#include <math.h>

#include "timing.h"


#define CALIBRATION_ITERS 1000000
/*#define CALIBRATION_ITERS 10000*/

void GetCurrentTime(TimeValue *tv) 
{  
    struct timeval t;
    gettimeofday(&t,0);
    tv->sec=t.tv_sec;
    tv->usec=t.tv_usec;
}

double TimeIntervalToSeconds(TimeInterval *ti) {
    return ti->sec + ti->usec/1e6;
};


void DiffTimes(TimeValue *earlier, TimeValue *later, TimeInterval *interval) 
{
  if (later->usec>=earlier->usec) {
    interval->usec=later->usec-earlier->usec;
    interval->sec=later->sec-earlier->sec;
  } else {
    interval->usec=(later->usec+1000000)-earlier->usec;
    interval->sec=later->sec-earlier->sec-1;
  }
}

static double TimingOverhead;

double TimeOnce(void (*func)(int),int arg)
{
  TimeValue before,after;
  TimeInterval dur;

  GetCurrentTime(&before);
  func(arg);
  GetCurrentTime(&after);
  DiffTimes(&before,&after,&dur);
  return TimeIntervalToSeconds(&dur);
}

double TimeOnceDouble(void (*func)(double),double arg)
{
  TimeValue before,after;
  TimeInterval dur;

  GetCurrentTime(&before);
  func(arg);
  GetCurrentTime(&after);
  DiffTimes(&before,&after,&dur);
  return TimeIntervalToSeconds(&dur);
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

  curfrac = CI95MEANHALF(count,sum,sum2)/MEANFROMSUMS(count,sum);

  while (curfrac > epsilon) { 
    t=TimeOnce(func,arg);	
    sum+=t;
    sum2+=t*t;
    count++;
    curfrac = CI95MEANHALF(count,sum,sum2)/MEANFROMSUMS(count,sum);
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
  
