#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/select.h>


#include "PredComp.h"

#define PRINT 1

void usage()
{
  fprintf(stderr,"usage: measureclient netspec\n");
}



int main(int argc, char *argv[]) 
{
  Measurement measure;

  if (argc!=2) {
    usage();
    exit(0);
  }

  EndPoint ep;

  if (ep.Parse(argv[1])) { 
    fprintf(stderr,"measureclient: Can't parse \"%s\"\n",argv[1]);
    exit(-1);
  }

  
  StreamingInputReference<Measurement> source;

  if (source.ConnectTo(ep)) {
    fprintf(stderr,"measureclient: Can't connect to \"%s\"\n",argv[1]);
    exit(-1);
  }

  while (1) {
    if (source.GetNextItem(measure)) { 
       fprintf(stderr,"measureclient: Call failed.\n");
       break;
    }
#if PRINT
    measure.Print();
#endif
  }
  source.Disconnect();
  return 0;
}
  
