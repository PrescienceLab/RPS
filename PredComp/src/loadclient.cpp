#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "PredComp.h"

void usage()
{
  fprintf(stderr,"usage: loadclient netspec\n");
}

#define PRINT 1

int main(int argc, char *argv[]) 
{
  LoadMeasurement measure;

  if (argc!=2) {
    usage();
    exit(0);
  }

  EndPoint ep;

  if (ep.Parse(argv[1])) { 
    fprintf(stderr,"loadclient: Can't parse \"%s\"\n",argv[1]);
    exit(-1);
  }

  
  StreamingInputReference<LoadMeasurement> source;

  if (source.ConnectTo(ep)) {
    fprintf(stderr,"loadclient: Can't connect to \"%s\"\n",argv[1]);
    exit(-1);
  }

  while (1) {
    if (source.GetNextItem(measure)) {
       fprintf(stderr,"loadclient: Call failed\n");
       break;
    }
#if PRINT
    measure.Print();
#endif
  }

  source.Disconnect();
  return 0;
}
  
