#include <stdlib.h>

#include "PredComp.h"
#include "Wavelets.h"

void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Get wavelet samples from a buffer\n\n"
	  "usage: %s num client\n\n"
	  "num             = number of values to get\n"
	  "client          = client endpoint\n"
	  "\n%s",n,b);
  delete [] b;
}



int main(int argc, char *argv[]) 
{
  EndPoint ep;
  int i;

  if (argc!=3) {
    usage(argv[0]);
    exit(0);
  }

  int num=atoi(argv[1]);


  if (ep.Parse(argv[2]) || ep.atype!=EndPoint::EP_SOURCE ) {
    fprintf(stderr,"wavelet_bufferclient: Failed to parse \"%s\"\n",argv[2]);
    exit(-1);
  }

  Reference<BufferDataRequest,BufferDataReply<WaveletIndividualSample> > ref;

  BufferDataRequest req;
  BufferDataReply<WaveletIndividualSample> repl;

  if (ref.ConnectTo(ep)) {
    fprintf(stderr,"wavelet_bufferclient: Can't Connect to \"%s\"\n",argv[2]);
    exit(-1);
  }

  req.num=num;

  if (ref.Call(req,repl)) { 
    fprintf(stderr,"wavelet_bufferclient: Call to \"%s\" failed\n",argv[2]);
    exit(-1);
  }

  ref.Disconnect();

  for (i=0;i<repl.num;i++) { 
    repl.data[i].Print(stdout);
  }
  
	
  return 0;
}
