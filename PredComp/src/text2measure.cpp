#include <stdlib.h>

#include "RPSInterface.h"
#include "PredComp.h"

unsigned period_usec;

const int N=10240;

class TextHandler : public MirrorHandler {
 public:
  TextHandler(AbstractMirror *m) : MirrorHandler(m) {
    SetHandlesRead();
  }
  Handler *Clone() const { 
    return new TextHandler(*this);
  }
  int HandleRead(const int fd, Selector &s) {
    char buf[N];
    int i;
    int num;

    for (i=0;i<N-1;i++) {
      num=Receive(fd,&(buf[i]),1,false);
      if (num==0) {
	buf[i]=0;
	break;
      } else if (buf[i]=='\n') {
	buf[i+1]=0;
	break;
      }
    }
    buf[N-1]=0;
    
    double first, second;
    num=sscanf(buf,"%lf %lf",&first,&second);
    if (num<1) {
      return 0;
    } else if (num==1) {
      TimeStamp ts;  // now
      Measurement m(ts,1,&first,period_usec);
      Buffer b;
      m.Serialize(b);
      mymirror->Forward(b);
      return 0;
    } else {
      TimeStamp ts((int)first,(int)((first-(int)first)*1e6));
      Measurement m(ts,1,&second,period_usec);
      Buffer b;
      m.Serialize(b);
      mymirror->Forward(b);
      return 0;
    }
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

typedef Mirror<TextHandler,GenericMirrorNewConnectionHandler,NullHandler,NullHandler> Text2Measure;


void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Convert text measurements to generic measurements\n\n"
	  "usage: %s period_usec textsource target+\n\n"
	  "period_usec     = period to stamp outgoing measurements\n"
	  "textsource      = source endpoint for text\n"
	  "target+         = one or more target or connect endpoints\n"
	  "\n%s",n,b);
  delete [] b;
}




int main(int argc, char *argv[]) 
{
  int i;
  EndPoint *ep;

  if (argc<4) {
    usage(argv[0]);
    exit(0);
  }

  period_usec=atoi(argv[1]);

  Text2Measure mirror;

  for (i=2;i<argc;i++) { 
    ep = new EndPoint;
    if (ep->Parse(argv[i])) { 
      fprintf(stderr,"Can't parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    if (mirror.AddEndPoint(*ep)) { 
      fprintf(stderr,"Can't add \"%s\"\n",argv[i]);
      exit(-1);
    }
  }

  fprintf(stderr,"text2measure running.\n");
  mirror.Run();
  return 0;
}



