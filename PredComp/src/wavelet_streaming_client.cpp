#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "Wavelets.h"
#include "PredComp.h"

void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Get signal as a wavelet-domain stream\n\n"
	  "usage: %s print|reconstruct source\n\n"
	  "print|reconstruct = print raw samples or reconstruction\n"
	  "source            = source endpoint\n"
	  "\n%s",n,b);
  delete [] b;
}

const unsigned numcoefs[10] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20};

WaveletRepresentationInfo inputrep;

typedef WaveletInputSample<double> WISD;
typedef WaveletOutputSample<double> WOSD;

StaticReverseWaveletTransform<double,WISD,WOSD> xform;
DelayBlock<WOSD> delay;


int main(int argc, char *argv[]) 
{
  bool first=true;
  enum {PRINT, RECONST} action;
  WaveletIndividualSample s;

  if (argc!=3) {
    usage(argv[0]);
    exit(0);
  }

  if (toupper(argv[1][0])=='P') {
    action=PRINT;
  } else if (toupper(argv[1][0])=='R') {
    action=RECONST;
  } else {
    usage(argv[0]);
    exit(-1);
  }
  
  EndPoint ep;
  
  if (ep.Parse(argv[2])) { 
    fprintf(stderr,"wavelet_streaming_client: Can't parse \"%s\"\n",argv[2]);
    exit(-1);
  }
  
  
  StreamingInputReference<WaveletIndividualSample> source;
  
  if (source.ConnectTo(ep)) {
    fprintf(stderr,"wavelet_streaming_client: Can't connect to \"%s\"\n",argv[2]);
    exit(-1);
  }
  
	     
  if (action==PRINT) {
    while (1) {
      if (source.GetNextItem(s)) { 
	fprintf(stderr,"wavelet_streaming_client: Connection failed.\n");
	break;
      }
      cout<<s<<endl;
    }
  } else if (action==RECONST) {
    vector<WOSD> delaysamples;
    vector<WOSD> inputsamples;
    vector<WISD> outputsamples;
    while (1) {
      if (source.GetNextItem(s)) { 
	fprintf(stderr,"wavelet_streaming_client: Connection failed.\n");
	break;
      }
      if (first) { 
	inputrep=s.rinfo;
	cerr << "Instantiating Reverse Transform And Delay With inputrep="<<inputrep<<endl;
	xform=StaticReverseWaveletTransform<double,WISD,WOSD>(inputrep.levels-1,inputrep.wtype,2,2,0);
	int *d = new int[inputrep.levels];
	CalculateWaveletDelayBlock(WaveletCoefficients(inputrep.wtype).GetNumCoefs(),inputrep.levels,d);
	delay=DelayBlock<WOSD>(inputrep.levels,0,d);
	delete [] d;
	first=false;
      }
      WOSD w;
      s.PutAsWaveletOutputSample(w);
      inputsamples.push_back(w);
      delay.StreamingSampleOperation(delaysamples, inputsamples);
      if (xform.StreamingTransformSampleOperation(outputsamples, delaysamples)) {
	for (unsigned j=0; j<outputsamples.size(); j++) {
	  cout << outputsamples[j].GetSampleValue()<<endl;
	}
      }
      delaysamples.clear();
      inputsamples.clear();
      outputsamples.clear();
    }
  }
  source.Disconnect();
  return 0;
}
  
