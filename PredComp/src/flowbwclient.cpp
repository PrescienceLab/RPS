#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "PredComp.h"

void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Get flow bandwidth measurements as a stream\n\n"
	  "usage: %s source\n\n"
	  "source            = source endpoint\n"
	  "\n%s",n,b);
  delete [] b;
}

#define PRINT 1

int main(int argc, char *argv[]) 
{
  FlowBWMeasurement measure;

  if (argc!=2) {
    usage(argv[0]);
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
  
