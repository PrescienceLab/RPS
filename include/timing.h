#ifndef _TIMING
#define _TIMING

/* Timing code */
/* must define TimeVal, TimeInteval, GetCurrentTime(), and DiffTimes(), and TimeIntervalToSeconds() */

typedef struct {
  int sec,usec;
} TimeValue, TimeInterval;


void   GetCurrentTime(TimeValue *tv);
double TimeIntervalToSeconds(TimeInterval *ti);
void   DiffTimes(TimeValue *earlier, TimeValue *later, TimeInterval *interval);

void   CalibrateTiming(double epsilon);
double TimeOnce(void (*func)(int),int arg);
double TimeOnceDouble(void (*func)(double),double arg);
double TimeIt(void (*func)(int),int arg, double epsilon);


#define MEANFROMSUMS(ct,sum) ((sum)/(ct))
#define STDFROMSUMS(ct,sum,sum2) (sqrt(((sum2)-(sum)*(sum)/(ct))/((ct)-1)))
#define COVFROMSUMS(ct,sum,sum2) (STDFROMSUMS(ct,sum,sum2)/MEANFROMSUMS(ct,sum))
#define CI95MEANHALF(ct,sum,sum2) (1.96*STDFROMSUMS(ct,sum,sum2)/sqrt(ct))
#define CI95MEAN(ct,sum,sum2) (2*CI95MEANHALF(ct,sum,sum2));


#endif
