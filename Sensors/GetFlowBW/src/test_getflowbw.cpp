#include <stdlib.h>
#include <stdio.h>
#include "getflowbw.h"

void usage() 
{ 
  fprintf(stderr,"usage: test from to num interval\n");
}


int main(int argc, char *argv[])
{
  char *from, *to;
  int num;
  int interval_usec;
  int i;
  double bw;

  if (argc!=5) { 
    usage();
    exit(-1);
  }

  from = argv[1];
  to = argv[2];
  num = atoi(argv[3]);
  interval_usec = (int)  (1e6*atof(argv[4]));
  
  init_getflowbw();

  for (i=0;i<num;i++) {
    if (getflowbw(from,to,&bw)) {
      fprintf(stderr,"getflowbw failed\n");
      exit(-1);
    }
    fprintf(stdout,"%d\t%f\n",i,bw);
    //    usleep(interval_usec);
  }

  deinit_getflowbw();

  return 0;
}
