#include <unistd.h>
#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>

#include "timing.h"
#include "getloadavg.h"
#include "apply.h"

/* Used internally */
#define EPSILON  0.05


#define  LEVEL 0.5
#define  DUR   60

#define  NUMCHILD 1

extern double MAX_SPUTTER_PER_SECOND;

void main() 
{
  int pid,i;
  double load;

  CalibrateTiming(EPSILON);
  CalibrateLoop(EPSILON);

  sleep(10);
  
  if ((pid=fork())) {
      for (i=0;i<DUR;i++) {
	RPSgetloadavg(&load,1);
	printf("%lf\n",load);
	fflush(stdout);
	sleep(1);
      }
  } else {
    /*
    for (i=0;i<NUMCHILD-1;i++) {
      if (fork()) {
	break;
      }
    }
    */
    sleep(1);
    srand48(time(0));
    for (i=0;i<DUR;i++) {
      ApplyLoad(LEVEL/NUMCHILD,1.0);
    }
  }
}

  
