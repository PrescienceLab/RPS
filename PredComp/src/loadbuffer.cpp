#include <stdlib.h>
#include "Buffer.h"
#include "EndPoint.h"
#include "LoadMeasurement.h"
#include "glarp.h"


void usage()
{
  fprintf(stderr,"usage: loadbuffer depth [sourcenetspec] [servernetspec] ...\n");
}


typedef BufferingSerializeableMirror<LoadMeasurement> LoadBuffer;


int main(int argc, char *argv[]) 
{
  EndPoint *ep;
  int i;

  if (argc<4) {
    usage();
    exit(0);
  }

  unsigned depth=atoi(argv[1]);

  LoadBuffer mirror(depth);

  for (i=2;i<argc;i++) {
    ep = new EndPoint;
    if (ep->Parse(argv[i])) {
      fprintf(stderr,"Failed to parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    mirror.AddEndPoint(*ep);
  }

  fprintf(stderr,"loadbuffer running.\n");
  mirror.Run();
}
