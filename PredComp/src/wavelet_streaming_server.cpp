#include <stdlib.h>

#include "PredComp.h"

WaveletRepresentationInfo outputrep;
unsigned curindex,curlevel;


class StreamingWaveletServerMirrorInputHandler : public MirrorHandler {
public:
  StreamingWaveletServerMirrorInputHandler(AbstractMirror *m) : MirrorHandler(m) {
    SetHandlesRead();
  }
  Handler *Clone() const {
    return new StreamingWaveletServerMirrorInputHandler(*this);
  }
  int HandleRead(const int fd, Selector &s) {
    // The expectation is that the input is a measurement.
    // The output is 0 or more WaveletIndividualSamples
    // Since this is currently doing nothing, wavelet-wise, 
    // and I want to test to be sure that multiple forwards
    // are possible, each input generates 1-10 outputs
    //
    // Unserialize a measurement, 
    // generate 1-10 WaveletIndividualSamples from it
    // and send them

    Measurement m;
    m.Unserialize(fd);
    cerr << m << endl;
    WaveletIndividualSample w(tag,TimeStamp(0),outputrep,curindex,curlevel,m.serlen>0 ? m.series[0] : 0);
    curindex++;
    curlevel=(unsigned)LOG2((double)curindex);
    unsigned num=UnsignedRandom()%10;
    cerr << "Forwarding "<<num<<" copies of "<<w<<endl;
    Buffer b;
    w.Serialize(b);
    for (unsigned i=0;i<num;i++) {
      mymirror->Forward(b);
    }
    return 0;
  }
  int HandleException(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleWrite(const int fd, Selector &s) {
    assert(0);
    return -1;
  }
  int HandleTimeout(Selector &s) {
    assert(0);
    return -1;
  }
};


typedef Mirror<StreamingWaveletServerMirrorInputHandler,GenericMirrorNewConnectionHandler,NullHandler,NullHandler> WaveletServerMirror;


void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Streaming Wavelet Transform Server\n\n"
	  "usage: %s wavelettype maxlevels source target+\n\n"
	  "wavelettype     = wavelet type to use\n"
	  "maxlevels       = maximum number of levels\n"
	  "source          = source endpoint for measurements\n"
	  "target+         = one or more target or connect endpoints\n\n%s",n,b);
  delete [] b;
}


int main(int argc, char *argv[]) 
{
  int i;
  EndPoint *ep;

  if (argc<5) {
    usage(argv[0]);
    exit(0);
  }
  
  // Ignoring wavelettype, maxlevels, etc. 
  // that should go here.

  WaveletServerMirror mirror;

  for (i=3;i<argc;i++) { 
    ep = new EndPoint;
    if (ep->Parse(argv[i])) { 
      fprintf(stderr,"wavelet_streaming_server: Can't parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    if (mirror.AddEndPoint(*ep)) { 
      fprintf(stderr,"wavelet_streaming_server: Can't add \"%s\"\n",argv[i]);
      exit(-1);
    }
  }

  fprintf(stderr,"wavelet_streaming_server running.\n");
  mirror.Run();
  return 0;
}



