#include <stdlib.h>

#include "PredComp.h"


void usage()
{
  fprintf(stderr,"usage: measurebuffer depth [sourcenetspec] [servernetspec] ...\n");
}


typedef BufferingSerializeableMirror<Measurement> MeasureBuffer;


int main(int argc, char *argv[]) 
{
  EndPoint *ep;
  int i;

  if (argc<4) {
    usage();
    exit(0);
  }

  int depth=atoi(argv[1]);

  MeasureBuffer mirror(depth);

  for (i=2;i<argc;i++) {
    ep = new EndPoint;
    if (ep->Parse(argv[i])) {
      fprintf(stderr,"Failed to parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    mirror.AddEndPoint(*ep);
  }

  fprintf(stderr,"measurebuffer running.\n");
  mirror.Run();
}
