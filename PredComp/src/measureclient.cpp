#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


#include "PredComp.h"

#define PRINT 1


void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Get generic measurements as a stream\n\n"
	  "usage: %s source\n\n"
	  "source            = source endpoint\n"
	  "\n%s",n,b);
  delete [] b;
}




int main(int argc, char *argv[]) 
{
  Measurement measure;

  if (argc!=2) {
    usage(argv[0]);
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
    fflush(stdout);
#endif
  }
  source.Disconnect();
  return 0;
}
  
