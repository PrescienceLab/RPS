#include <stdlib.h>

#include "PredComp.h"

double absmin;
bool discard=true;

class DenoiseMirrorInputHandler : public MirrorHandler {
public:
  DenoiseMirrorInputHandler(AbstractMirror *m) : MirrorHandler(m) {
    SetHandlesRead();
  }
  Handler *Clone() const {
    return new DenoiseMirrorInputHandler(*this);
  }
  int HandleRead(const int fd, Selector &sel) {
    // Unserialize a wavelet input sample, see 
    // check to see if it is one we will let through
    // and call forward if it is.
    WaveletIndividualSample s;
    s.Unserialize(fd);
    //    cerr << s << ":";
    if (s.rinfo.rtype==WAVELET_DOMAIN_TRANSFORM) { 
      bool send=false;
      if (fabs(s.value)>absmin) { 
	send=true;
      } else {
	if (!discard) { 
	  s.value=0;
	  send=true;
	}
      }
      
      if (send) { 
	Buffer b;
	s.Serialize(b);
	mymirror->Forward(b);
	cerr << "forwarded"<<endl;
      }

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


typedef Mirror<DenoiseMirrorInputHandler,GenericMirrorNewConnectionHandler,NullHandler,NullHandler> DenoiseMirror;


void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Streaming Wavelet Transform Denoiser\n\n"
	  "usage: %s absmin discard|zero source target+\n\n"
	  "absmin          = minimum value to let through\n"
	  "discard|zero    = send a zero value or send nothing\n"
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
  
  absmin=atof(argv[1]);
  if (toupper(argv[2][0])=='D') { 
    discard=true;
  } else {
    discard=false;
  }

  DenoiseMirror mirror;

  for (i=3;i<argc;i++) { 
    ep = new EndPoint;
    if (ep->Parse(argv[i])) { 
      fprintf(stderr,"wavelet_streaming_denoise: Can't parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    if (mirror.AddEndPoint(*ep)) { 
      fprintf(stderr,"wavelet_streaming_denoise: Can't add \"%s\"\n",argv[i]);
      exit(-1);
    }
  }

  fprintf(stderr,"wavelet_streaming_denoise running.\n");
  mirror.Run();
  return 0;
}



