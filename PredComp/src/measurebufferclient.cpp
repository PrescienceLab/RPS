#include <stdlib.h>

#include "PredComp.h"


void usage()
{
  fprintf(stderr,"usage: measurebufferclient numitems [sourcenetspec] \n");
}



int main(int argc, char *argv[]) 
{
  EndPoint ep;
  int i;

  if (argc!=3) {
    usage();
    exit(0);
  }

  int num=atoi(argv[1]);


  if (ep.Parse(argv[2]) || ep.atype!=EndPoint::EP_SOURCE ) {
    fprintf(stderr,"measurebufferclient: Failed to parse \"%s\"\n",argv[2]);
    exit(-1);
  }

  Reference<BufferDataRequest,BufferDataReply<Measurement> > ref;

  BufferDataRequest req;
  BufferDataReply<Measurement> repl;

  if (ref.ConnectTo(ep)) {
    fprintf(stderr,"measurebufferclient: Can't Connect to \"%s\"\n",argv[2]);
    exit(-1);
  }

  req.num=num;

  if (ref.Call(req,repl)) { 
    fprintf(stderr,"measurebufferclient: Call to \"%s\" failed\n",argv[2]);
  } else {
    for (i=0;i<repl.num;i++) { 
      repl.data[i].Print();
    }
  }

  ref.Disconnect();
  
  return 0;
}
