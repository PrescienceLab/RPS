#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <sys/select.h>
#include <unistd.h>

#include "random.h"

#include "timing.h"
#include "apply.h"

#define DEBUG 0
#define CHECKUSAGE 0
#define RANDOMIZED 1

double MAX_SPUTTER_PER_SECOND=.5;

static volatile double x,y;
static int iters_per_second;

#define LB_ITERS 100000
#define UB_ITERS 1000000

#ifdef ALPHA
extern "C" int usleep(int);
#endif

void SpinInternal(int iters)
{
  int i;
#if DEBUG
  fprintf(stderr,"Run %d iters\n",iters);
#endif
  for (i=0;i<iters;i++) {
    y+=x;
  }
}

void Spin(double interval)
{
#if 0

  double dur;
  double wait;
  int iters;


  wait=0;
  while (interval>0) {
#if DEBUG
    fprintf(stderr,"Attempt Spin for %lf secs\n",interval);
#endif
    iters = (int) (interval*iters_per_second);
    dur=TimeOnce(SpinInternal,iters);
    interval-=dur;
    wait+=dur;
  }
#if DEBUG
  fprintf(stderr,"Spun for total of %lf seconds\n",wait);
#endif
#else
#if CHECKUSAGE
  struct rusage ru;
  double initcpu, curcpu;

  getrusage(RUSAGE_SELF,&ru);
  initcpu = ru.ru_utime.tv_sec + 1e-6*ru.ru_utime.tv_usec +
            ru.ru_stime.tv_sec + 1e-6*ru.ru_stime.tv_usec;
  curcpu=initcpu;

  while (curcpu-initcpu < interval) {
#if DEBUG
    fprintf(stderr,"Attempt Spin for %lf secs\n",interval-(curcpu-initcpu));
#endif
    SpinInternal((int)((interval-(curcpu-initcpu))*iters_per_second));
    getrusage(RUSAGE_SELF,&ru);
    curcpu = ru.ru_utime.tv_sec + 1e-6*ru.ru_utime.tv_usec +
             ru.ru_stime.tv_sec + 1e-6*ru.ru_stime.tv_usec; 

  }
#if DEBUG
  fprintf(stderr,"Spun for total of %lf seconds\n",curcpu-initcpu);
#endif

#else 
  SpinInternal((int)(interval*iters_per_second));
#endif
#endif
}  

void PortableSleepInternal(double second) 
{
#if 0
  struct timeval timeout;
    
  timeout.tv_sec = (int)second;
  timeout.tv_usec = (int)(1e6*(second - ((int)second)));

#if DEBUG
  fprintf(stderr,"Sleep for %d secs, %d usecs\n",timeout.tv_sec,timeout.tv_usec);
#endif

  select(FD_SETSIZE, NULL, NULL, NULL, &timeout);
#else
  unsigned usec = (unsigned) (second*1e6);
#if DEBUG
  fprintf(stderr,"Sleep for %d usecs\n",usec);
#endif
  usleep(usec);
#endif

}
  
void Sleep(double interval)
{
#if CHECKUSAGE
  double dur;
  double wait;

  wait=0;
  while (interval>0) {
#if DEBUG
    fprintf(stderr,"Attempt Sleep for %lf secs\n",interval);
#endif
    dur=TimeOnceDouble(PortableSleepInternal,interval);
    interval-=dur;
    wait+=dur;
  }
#if DEBUG
  fprintf(stderr,"Slept for total of %lf seconds\n",wait);
#endif
#else
  PortableSleepInternal(interval);
#endif
}

#if 0
double UniformRandom(double low, double high)
{
  double val=drand48();

  return low+val*(high-low);
}
#endif


double minrez=0.1;
double maxrez=0.3;

double callspersecond=100;



/* level is 0..1 duration is in seconds */ 
void ApplyLoad(double level, double duration)
{
  if (level<1.0 && level>0.0) {
#if RANDOMIZED
    double totalsleepdur, totalcomputedur;
    double sleepdurleft, computedurleft;
    double thiscompute, thissleep;
    double computerandmax,sleeprandmax;

    totalcomputedur = level*duration;
    totalsleepdur = (1.0-level)*duration;
    
    computedurleft = totalcomputedur;
    sleepdurleft = totalsleepdur;

    computerandmax=2*(totalcomputedur/(duration*callspersecond));
    sleeprandmax=2*(totalsleepdur/(duration*callspersecond));
    

#if DEBUG
      fprintf(stderr,"totalcomputedur=%lf, totalsleepdur=%lf\n",
	      totalcomputedur,totalsleepdur);
#endif

    while (sleepdurleft>0 || computedurleft>0) {
      /*      srand48(time(0));*/
      thiscompute = computedurleft < computerandmax ? computedurleft
                     : UniformRandom(0,computerandmax);
      thissleep = sleepdurleft < sleeprandmax ? sleepdurleft
                     : UniformRandom(0,sleeprandmax);
#if 0
      thiscompute = computedurleft < minrez*totalcomputedur ? computedurleft
                     : UniformRandom(minrez*totalcomputedur,
				     MIN(computedurleft,maxrez*totalcomputedur));
      thissleep = sleepdurleft < minrez*totalsleepdur ? sleepdurleft
                     : UniformRandom(minrez*totalsleepdur,
				     MIN(sleepdurleft,maxrez*totalsleepdur));
#endif
#if 0
      thiscompute = computedur < minrez ? computedur 
                     : UniformRandom(minrez,MIN(computedur,maxrez));
      thissleep = sleepdur < minrez ? sleepdur 
                     : UniformRandom(minrez,MIN(sleepdur,maxrez));
#endif
#if DEBUG
      fprintf(stderr,"compute/sleep = %lf/%lf\n",thiscompute,thissleep);
#endif
      if (thiscompute>0) {
	Spin(thiscompute);
      }
      if (thissleep>0) {
	Sleep(thissleep);
      }
      computedurleft-=thiscompute;
      sleepdurleft-=thissleep;
    }
      
#else 
    int    numsteps     = (int) ceil(duration*MAX_SPUTTER_PER_SECOND);
    double sleepdur     = (1.0-level)*duration/numsteps;
    double computedur   = level*duration/numsteps;
    int i;

#if DEBUG
    fprintf(stderr,"numsteps=%d\n",numsteps);
#endif

    for (i=0;i<numsteps;i++) {
#if DEBUG
      fprintf(stderr,"compute/sleep=%lf/%lf\n",computedur,sleepdur);
#endif
      Spin(computedur);
      Sleep(sleepdur);
    }
#endif
  } else if (level<=0.0) {
    /* Sleep */
    Sleep(duration);
  } else {
    /* Apply maximum load */
    Spin(duration);
  }
}  



void CalibrateLoop(double epsilon)
{
  double lbdur, ubdur;
  int lbiters=LB_ITERS;
  int ubiters=UB_ITERS;

  lbdur=TimeIt(SpinInternal,lbiters,epsilon);
  ubdur=TimeIt(SpinInternal,ubiters,epsilon);

  iters_per_second = (int)((ubiters-lbiters)/(ubdur-lbdur));

}
