#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>

#include "PredComp.h"


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
  fprintf(stdout," Rate              : %f /s \n",rate);
  fprintf(stdout," Mean Latency      : %f ms\n",Mean(lat,NUM));
  fprintf(stdout," Std  Latency      : %f ms\n",StandardDeviation(lat,NUM));
  fprintf(stdout," Min  Latency      : %f ms\n",Min(lat,NUM));
  fprintf(stdout," Med  Latency      : %f ms\n",Median(lat,NUM));
  fprintf(stdout," Max  Latency      : %f ms\n",Max(lat,NUM));
  
}

