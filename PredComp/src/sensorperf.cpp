#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>

#include "TimeStamp.h"

#include "getloadavg.h"

#include "tools.h"


void loadavgwrapper()
{
  double junk[3];
  RPSgetloadavg(junk,3);
}

void flowbwwrapper()
{
}

#define NUM 10000

int main()
{
  TimeStamp start, end;
  int i;

  void (*wrapper)();
  char *name;
  double rate;
  double lat[NUM];
  
  // flow bandwidth
  wrapper = loadavgwrapper;
  name = "GetLoadAvg";

  start = TimeStamp(0);
  for (i=0;i<NUM;i++) { 
    wrapper();
  }
  end = TimeStamp(0);

  rate = (double)NUM/((double)end - (double)start);

  for (i=0;i<NUM;i++) { 
    start = TimeStamp(0);
    wrapper();
    end = TimeStamp(0);
    lat[i] = NUM*((double)end - (double)start);
  }

  fprintf(stdout,"%s:\n",name);
  fprintf(stdout," Rate              : %lf /s \n",rate);
  fprintf(stdout," Mean Latency      : %lf ms\n",Mean(lat,NUM));
  fprintf(stdout," Std  Latency      : %lf ms\n",StandardDeviation(lat,NUM));
  fprintf(stdout," Min  Latency      : %lf ms\n",Min(lat,NUM));
  fprintf(stdout," Med  Latency      : %lf ms\n",Median(lat,NUM));
  fprintf(stdout," Max  Latency      : %lf ms\n",Max(lat,NUM));
  
}

