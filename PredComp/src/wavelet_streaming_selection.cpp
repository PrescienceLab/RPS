#include <stdlib.h>

#include "PredComp.h"

unsigned levellow, levelhigh;

class SelectionMirrorInputHandler : public MirrorHandler {
public:
  SelectionMirrorInputHandler(AbstractMirror *m) : MirrorHandler(m) {
    SetHandlesRead();
  }
  Handler *Clone() const {
    return new SelectionMirrorInputHandler(*this);
  }
  int HandleRead(const int fd, Selector &sel) {
    // Unserialize a wavelet input sample, see 
    // check to see if it is one we will let through
    // and call forward if it is.
    WaveletIndividualSample s;
    s.Unserialize(fd);
    cerr << s << ":";
    if (s.rinfo.rtype==WAVELET_DOMAIN_DETAIL) { 
      if (s.level>=levellow && s.level<=levelhigh) { 
	Buffer b;
	s.Serialize(b);
	mymirror->Forward(b);
	cerr << "forwarded"<<endl;
      } else {
	cerr << "DISCARDED"<<endl;
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


typedef Mirror<SelectionMirrorInputHandler,GenericMirrorNewConnectionHandler,NullHandler,NullHandler> SelectionMirror;


void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Streaming Wavelet Transform Selector\n\n"
	  "usage: %s levellow levelhigh source target+\n\n"
	  "levellow        = lowest level to let pass\n"
	  "levelhigh       = highest level to let pass\n"
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
  
  levellow=atoi(argv[1]);
  levelhigh=atoi(argv[2]);

  SelectionMirror mirror;

  for (i=3;i<argc;i++) { 
    ep = new EndPoint;
    if (ep->Parse(argv[i])) { 
      fprintf(stderr,"wavelet_streaming_selection: Can't parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    if (mirror.AddEndPoint(*ep)) { 
      fprintf(stderr,"wavelet_streaming_selection: Can't add \"%s\"\n",argv[i]);
      exit(-1);
    }
  }

  fprintf(stderr,"wavelet_streaming_selection running.\n");
  mirror.Run();
  return 0;
}



