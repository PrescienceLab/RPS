#include <stdlib.h>
#include "Buffer.h"
#include "EndPoint.h"
#include "Reference.h"
#include "FlowBWMeasurement.h"
#include "glarp.h"

void usage()
{
  fprintf(stderr,"usage: flowbufferclient numitems [sourcenetspec] \n");
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
    fprintf(stderr,"flowbufferclient: Failed to parse \"%s\"\n",argv[2]);
    exit(-1);
  }

  Reference<BufferDataRequest,BufferDataReply<FlowBWMeasurement> > ref;

  BufferDataRequest req;
  BufferDataReply<FlowBWMeasurement> repl;

  if (ref.ConnectTo(ep)) {
    fprintf(stderr,"flowbufferclient: Can't Connect to \"%s\"\n",argv[2]);
    exit(-1);
  }

  req.num=num;

  if (ref.Call(req,repl)) { 
    fprintf(stderr,"flowbufferclient: Call to \"%s\" failed\n",argv[2]);
    ref.Disconnect();
    exit(-1);
  }

  ref.Disconnect();

  for (i=0;i<repl.num;i++) { 
    repl.data[i].Print(stderr);
  }
  
  return 0;
}
