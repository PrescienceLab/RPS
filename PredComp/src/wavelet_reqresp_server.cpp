#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <math.h>

#include "PredComp.h"

void usage()
{
  fprintf(stderr,"usage: wavelet_reqresp_server [targetspec+ | connectspec+]\n");
}


// Generic right now
class WaveletTransformBlock {
public:
  static int Compute(const WaveletTransformBlockRequest &req, 
                           WaveletTransformBlockResponse &resp) {

    fprintf(stderr,"In Compute\n");

    //
    // COMPUTE TRANSFORM HERE AND REPLACE THIS COPY
    //
    resp=req;
    resp.timeout=TimeStamp(0);

    fprintf(stderr,"Response finished.  Leaving Compute\n");

    return 0;
  }
};

typedef  SerializeableRequestResponseMirror<WaveletTransformBlockRequest,
					    WaveletTransformBlock,
					    WaveletTransformBlockResponse> 
WaveletTransformBlockEngine;

class WaveletTransformBlockResponseSink : public LocalTarget {
public:
  void ProcessData(Buffer &buf) {
    WaveletTransformBlockResponse r;
    r.Unserialize(buf);
    r.Print();
  }
};



int main(int argc, char *argv[]) 
{
  int i;
  EndPoint *ep;

  if (argc<2) {
    usage();
    exit(0);
  }


  WaveletTransformBlockEngine wave;
  WaveletTransformBlockResponseSink sink;

  wave.AddLocalTarget(&sink);

  for (i=1;i<argc;i++) { 
    ep = new EndPoint;
    if (ep->Parse(argv[i])) { 
      fprintf(stderr,"wavelet_reqresp_server: Can't parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    if (wave.AddEndPoint(*ep)) { 
      fprintf(stderr,"wavelet_reqresp_server: Can't add \"%s\"\n",argv[i]);
      exit(-1);
    }
  }

  fprintf(stderr,"wavelet_reqresp_server running\n");

  wave.Run();
  return 0;
}

