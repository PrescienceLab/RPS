#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "PredComp.h"

#define PRINT 1

void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Get predictons as a stream\n\n"
	  "usage: %s source\n\n"
	  "source            = source endpoint\n"
	  "\n%s",n,b);
  delete [] b;
}



int main(int argc, char *argv[]) 
{
  PredictionResponse pred;

  if (argc!=2) {
    usage(argv[0]);
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
  
