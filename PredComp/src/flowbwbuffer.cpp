#include <stdlib.h>
#include "PredComp.h"


void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Buffer flow bandwidth measurements\n\n"
	  "usage: %s depth source server target*\n\n"
	  "depth           = number to buffer\n"
	  "source          = source endpoint\n"
	  "server          = server endpoint (for requests)\n"
	  "target*         = zero or more target or connect endpoints\n\n"
	  "\n%s",n,b);
  delete [] b;
}


typedef BufferingSerializeableMirror<FlowBWMeasurement> FlowBWBuffer;


int main(int argc, char *argv[]) 
{
  EndPoint *ep;
  int i;

  if (argc<4) {
    usage(argv[0]);
    exit(0);
  }

  unsigned depth=atoi(argv[1]);

  FlowBWBuffer mirror(depth);

  for (i=2;i<argc;i++) {
    ep = new EndPoint;
    if (ep->Parse(argv[i])) {
      fprintf(stderr,"Failed to parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    mirror.AddEndPoint(*ep);
  }

  fprintf(stderr,"flowbuffer running.\n");
  mirror.Run();
  return 0;
}
