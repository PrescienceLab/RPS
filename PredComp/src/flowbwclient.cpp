#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "PredComp.h"

void usage()
{
  fprintf(stderr,"usage: flowbwclient netspec\n");
}

#define PRINT 1

int main(int argc, char *argv[]) 
{
  FlowBWMeasurement measure;

  if (argc!=2) {
    usage();
    exit(0);
  }

  EndPoint ep;

  if (ep.Parse(argv[1])) { 
    fprintf(stderr,"flowbwclient: Can't parse \"%s\"\n",argv[1]);
    exit(-1);
  }

  
  StreamingInputReference<FlowBWMeasurement> source;

  if (source.ConnectTo(ep)) {
    fprintf(stderr,"flowbwclient: Can't connect to \"%s\"\n",argv[1]);
    exit(-1);
  }

  while (1) {
    if (source.GetNextItem(measure)) {
       fprintf(stderr,"flowbwclient: Call failed\n");
       break;
    }
#if PRINT
    measure.Print();
#endif
  }

  source.Disconnect();
  return 0;
}
  
