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
  double junk;
  getflowbw("localhost","localhost",&junk);
}

void watchtowerwrapper()
{
  system("wt_usercpu.pl 1 > /dev/null");
}

#define NUM 10000

int main()
{

  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Measuring Sensor Performance\n\n"
	  "%s\n",b);
  delete [] b;

  TimeStamp start, end;
  int i;

  void (*wrapper)();
  char *name;
  double rate;
  double lat[NUM];
  
  // flow bandwidth
  wrapper = loadavgwrapper;
  name = "GetLoadAvg - on this host";

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

  // flow bandwidth
  wrapper = flowbwwrapper;
  name = "GetFlowBW - Note: localhost to localhost, no checks";

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
  
  // watchtower
  wrapper = watchtowerwrapper;
  name = "WatchTower - Note: one counter";

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

