#include <stdlib.h>

#include "Wavelets.h"
#include "PredComp.h"


WaveletRepresentationInfo outputrep;

typedef WaveletInputSample<double> WISD;
typedef WaveletOutputSample<double> WOSD;

StaticForwardWaveletTransform<double,WOSD,WISD> xform;
bool firsttime=true;
unsigned curindex=0;

class StreamingWaveletServerMirrorInputHandler : public MirrorHandler {
public:
  StreamingWaveletServerMirrorInputHandler(AbstractMirror *m) : MirrorHandler(m) {
    SetHandlesRead();
  }
  Handler *Clone() const {
    return new StreamingWaveletServerMirrorInputHandler(*this);
  }
  int HandleRead(const int fd, Selector &s) {
    if (firsttime) { 
      xform=StaticForwardWaveletTransform<double,WOSD,WISD>(outputrep.levels-1,outputrep.wtype,2,2,0);
      firsttime=false;
    }

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

    vector<WOSD> output;

    for (int i=0; i<m.serlen; i++) {
      switch (outputrep.rtype) {
      case WAVELET_DOMAIN_DETAIL:
	xform.StreamingDetailSampleOperation(output, WISD(m.series[i],curindex));
	break;
      case WAVELET_DOMAIN_APPROX:
	xform.StreamingApproxSampleOperation(output, WISD(m.series[i],curindex));
	break;
      case WAVELET_DOMAIN_TRANSFORM:
	xform.StreamingTransformSampleOperation(output, WISD(m.series[i],curindex));
	break;
      default:
      assert(0);
      break;
      }
      
      for (unsigned j=0;j<output.size();j++) {
	cerr << output[j]<<endl;
	WaveletIndividualSample w(m.tag,TimeStamp(0),outputrep,0,0,0);
	w.GetFromWaveletOutputSample(output[j]);
	Buffer b;
	w.Serialize(b);
	mymirror->Forward(b);
      }
      curindex++;
      output.clear();
    }
    return 0;
  }
  int HandleException(const int fd, Selector &s) {
    assert(0);
    return -1;
  }  int HandleWrite(const int fd, Selector &s) {
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
	  "usage: %s wavelettype maxlevels details|approx|transform source target+\n\n"
	  "wavelettype     = wavelet type to use\n"
	  "maxlevels       = maximum number of levels\n"
          "details|approx| = details or approximations\n"
          " transform        or details + highest approx\n"
	  "source          = source endpoint for measurements\n"
	  "target+         = one or more target or connect endpoints\n\n%s",n,b);
  delete [] b;
}


WaveletType GetWaveletType(const char *x) 
{
   if (!strcasecmp(x,"DAUB2")) {
     return DAUB2;
   } else if (!strcasecmp(x,"DAUB4")) { 
     return DAUB4;
   } else if (!strcasecmp(x,"DAUB6")) { 
     return DAUB6;
   } else if (!strcasecmp(x,"DAUB8")) { 
     return DAUB8;
   } else if (!strcasecmp(x,"DAUB10")) { 
     return DAUB10;
   } else if (!strcasecmp(x,"DAUB12")) { 
     return DAUB12;
   } else if (!strcasecmp(x,"DAUB14")) { 
     return DAUB14;
   } else if (!strcasecmp(x,"DAUB16")) { 
     return DAUB16;
   } else if (!strcasecmp(x,"DAUB18")) { 
     return DAUB18;
   } else if (!strcasecmp(x,"DAUB20")) { 
     return DAUB20;
   } else {
     fprintf(stderr,"wavelet_reqresp_client: Unknown wavelet type\n");
     exit(-1);
   }
}


int main(int argc, char *argv[]) 
{
  int i;
  EndPoint *ep;
  WaveletType wt;
  WaveletRepresentationType rt;
  int numlevels;
  

  if (argc<6) {
    usage(argv[0]);
    exit(0);
  }

  wt=GetWaveletType(argv[1]);
  numlevels=atoi(argv[2]);
   if (toupper(argv[3][0])=='D') {
     rt=WAVELET_DOMAIN_DETAIL;
   } else if (toupper(argv[3][0])=='A') {
     rt=WAVELET_DOMAIN_APPROX;
   } else if (toupper(argv[3][0])=='T') {
     rt=WAVELET_DOMAIN_TRANSFORM;
   } else {
     usage(argv[0]);
     exit(-1);
   }

   outputrep=WaveletRepresentationInfo(rt,wt,numlevels,0);
   curindex=0;
   
  
  // Ignoring wavelettype, maxlevels, etc. 
  // that should go here.

  WaveletServerMirror mirror;

  for (i=4;i<argc;i++) { 
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



