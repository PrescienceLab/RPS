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
          "delay|nodelay     = delay locally or not\n"
	  "source            = source endpoint\n"
	  "\n%s",n,b);
  delete [] b;
}

const unsigned numcoefs[10] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20};

WaveletRepresentationInfo inputrep;

typedef WaveletInputSample<double> WISD;
typedef WaveletOutputSample<double> WOSD;

StaticReverseWaveletTransform<double,WISD,WOSD> *xform;
DelayBlock<WOSD> *delay;


int main(int argc, char *argv[]) 
{
  enum {AWAITFIRST, AWAITLOW, AWAITHIGH,NORMAL} state;
  enum {PRINT, RECONST_DELAY, RECONST_NODELAY} action;
  WaveletIndividualSample s;

  if (argc!=4) {
    usage(argv[0]);
    exit(0);
  }

  if (toupper(argv[1][0])=='P') {
    action=PRINT;
  } else if (toupper(argv[1][0])=='R') {
    action=RECONST_DELAY;
  } else {
    usage(argv[0]);
    exit(-1);
  }


  if (toupper(argv[2][0])=='D') {
    if (action!=RECONST_DELAY) { 
      usage(argv[0]);
      exit(-1);
    }
  } else if (toupper(argv[2][0])=='N') {
    if (action!=RECONST_DELAY) { 
      usage(argv[0]);
      exit(-1);
    }
    action=RECONST_NODELAY;
  } else {
    usage(argv[0]);
    exit(-1);
  }

  
  EndPoint ep;
  
  if (ep.Parse(argv[3])) { 
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
  } else if (action==RECONST_DELAY) {
    vector<WOSD> delaysamples;
    vector<WOSD> inputsamples;
    vector<WISD> outputsamples;
    state=AWAITFIRST;
    while (1) {
      if (source.GetNextItem(s)) { 
	fprintf(stderr,"wavelet_streaming_client: Connection failed.\n");
	break;
      }
      switch (state) {
      case AWAITFIRST: {
	inputrep=s.rinfo;
	state=AWAITLOW;
	cerr << "Instantiating Reverse Transform And Delay With inputrep="<<inputrep<<endl;
	cerr << "Sample is <<"<<s<<endl;
	xform = new StaticReverseWaveletTransform<double,WISD,WOSD>(inputrep.levels-1,inputrep.wtype,2,2,0);
	int *d = new int[inputrep.levels];
	CalculateWaveletDelayBlock(CQFWaveletCoefficients(inputrep.wtype).GetNumCoefs(),inputrep.levels,d);
	delay=new DelayBlock<WOSD>(inputrep.levels,0,d);
	delete [] d;
	}
	// intentional fallthrough
      case AWAITLOW: {
	if (s.level==(s.rinfo.levels-2)) {
	  WOSD w;
	  s.PutAsWaveletOutputSample(w);
	  inputsamples.push_back(w);
	  state=AWAITHIGH;
	}
      }
	break;
      case AWAITHIGH: {
	if (s.level==(s.rinfo.levels-1)) {
	  WOSD w;
	  s.PutAsWaveletOutputSample(w);
	  inputsamples.push_back(w);
	  for (int i=s.rinfo.levels-3; i>=0; i--) {
	    inputsamples.push_back(WOSD(0.0,i,0));
	  }
	  delay->StreamingSampleOperation(delaysamples, inputsamples);
	  if (xform->StreamingTransformSampleOperation(outputsamples, delaysamples)) {
	    for (unsigned j=0; j<outputsamples.size(); j++) {
	      cout <<outputsamples[j].GetSampleValue()<<endl;
	    }
	  }
	  delaysamples.clear();
	  inputsamples.clear();
	  outputsamples.clear();
	  state=NORMAL;
	} else {
	  inputsamples.clear();
	  state=AWAITLOW;
	}
      }
	break;
      case NORMAL: {
	  WOSD w;
	  s.PutAsWaveletOutputSample(w);
	  inputsamples.push_back(w);

	  delay->StreamingSampleOperation(delaysamples, inputsamples);
	  if (xform->StreamingTransformSampleOperation(outputsamples, delaysamples)) {
	    for (unsigned j=0; j<outputsamples.size(); j++) {
	      cout <<outputsamples[j].GetSampleValue()<<endl;
	    }
	  }
	  delaysamples.clear();
	  inputsamples.clear();
	  outputsamples.clear();
      }
      }
    }
  } else if (action==RECONST_NODELAY) {
    vector<WOSD> inputsamples;
    vector<WISD> outputsamples;
    state=AWAITFIRST;
    while (1) {
      if (source.GetNextItem(s)) { 
	fprintf(stderr,"wavelet_streaming_client: Connection failed.\n");
	break;
      }
      switch (state) {
      case AWAITFIRST: {
	inputrep=s.rinfo;
	state=AWAITLOW;
	cerr << "Instantiating Reverse Transform With inputrep="<<inputrep<<endl;
	cerr << "Sample is <<"<<s<<endl;
	xform = new StaticReverseWaveletTransform<double,WISD,WOSD>(inputrep.levels-1,inputrep.wtype,2,2,0);
	}
	// intentional fallthrough
      case AWAITLOW: {
	if (s.level==(s.rinfo.levels-2)) {
	  WOSD w;
	  s.PutAsWaveletOutputSample(w);
	  inputsamples.push_back(w);
	  state=AWAITHIGH;
	}
      }
	break;
      case AWAITHIGH: {
	if (s.level==(s.rinfo.levels-1)) {
	  WOSD w;
	  s.PutAsWaveletOutputSample(w);
	  inputsamples.push_back(w);
	  for (int i=s.rinfo.levels-3; i>=0; i--) {
	    inputsamples.push_back(WOSD(0.0,i,0));
	  }
	  if (xform->StreamingTransformSampleOperation(outputsamples, inputsamples)) {
	    for (unsigned j=0; j<outputsamples.size(); j++) {
	      cout <<outputsamples[j].GetSampleValue()<<endl;
	    }
	  }
	  inputsamples.clear();
	  outputsamples.clear();
	  state=NORMAL;
	} else {
	  inputsamples.clear();
	  state=AWAITLOW;
	}
      }
	break;
      case NORMAL: {
	  WOSD w;
	  s.PutAsWaveletOutputSample(w);
	  inputsamples.push_back(w);

	  if (xform->StreamingTransformSampleOperation(outputsamples, inputsamples)) {
	    for (unsigned j=0; j<outputsamples.size(); j++) {
	      cout <<outputsamples[j].GetSampleValue()<<endl;
	    }
	  }
	  inputsamples.clear();
	  outputsamples.clear();
      }
      }
    }
  }
  source.Disconnect();
  return 0;
}
  
