#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/select.h>

#include "PredComp.h"

#define PRINT 1

void usage()
{
  fprintf(stderr,"usage: predclient netspec\n");
}



int main(int argc, char *argv[]) 
{
  PredictionResponse pred;

  if (argc!=2) {
    usage();
    exit(0);
  }

  EndPoint ep;

  if (ep.Parse(argv[1])) { 
    fprintf(stderr,"predclient: Can't parse \"%s\"\n",argv[1]);
    exit(-1);
  }

  
  StreamingInputReference<PredictionResponse> source;

  if (source.ConnectTo(ep)) {
    fprintf(stderr,"predclient: Can't connect to \"%s\"\n",argv[1]);
    exit(-1);
  }

  while (1) {
    if (source.GetNextItem(pred)) { 
      fprintf(stderr,"predclient: Connection failed.\n");
      break;
    }
#if PRINT
    pred.Print();
#endif
  }
  source.Disconnect();
  return 0;
}
  
