#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "Wavelets.h"
#include "PredComp.h"

#define LOG2(x) (log(x)/log(2.0))


void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Implements StreamQuery from HPDC paper\n\n"
	  "usage: %s frequency source\n\n"
	  "frequency         = peak frequency to resolve in reconstruction\n"
          "up|noup           = whether to upsample to\n"
          "                    rate of underlying signal \n"
 	  "source            = source endpoint\n"
	  "\n%s",n,b);
  delete [] b;
}

const unsigned numcoefs[10] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20};

WaveletRepresentationInfo inputrep;

double   peakfreq;
int      minlevel;
bool     upsample;

typedef WaveletInputSample<double> WISD;
typedef WaveletOutputSample<double> WOSD;

StaticReverseWaveletTransform<double,WISD,WOSD> *xform;
DelayBlock<WOSD> *delay;


int main(int argc, char *argv[]) 
{
  enum {AWAITFIRST, AWAITLOW, AWAITHIGH,NORMAL} state;
  WaveletIndividualSample s;

  if (argc!=4) {
    usage(argv[0]);
    exit(0);
  }

  peakfreq=atof(argv[1]);
  
  EndPoint ep;
  

  if (toupper(argv[2][0]=='U')) { 
    cerr << "Sorry, upsample is not yet implemented\n";
    exit(-1);
  } else {
    upsample=false;
  }

  if (ep.Parse(argv[3])) { 
    fprintf(stderr,"wavelet_streaming_client: Can't parse \"%s\"\n",argv[2]);
    exit(-1);
  }
  
  
  StreamingInputReference<WaveletIndividualSample> source;
  
  if (source.ConnectTo(ep)) {
    fprintf(stderr,"wavelet_streaming_client: Can't connect to \"%s\"\n",argv[2]);
    exit(-1);
  }
  
	     
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
      

      double fs2 = 1.0e6/((double)2.0*(double)inputrep.period_usec);
      if (peakfreq > fs2) {
	cerr << "Can't do query for "<<peakfreq<<" Hz since underlying signal is bandlimited to "<<fs2<<" Hz."<<endl;
	cerr << "Running query at "<<fs2<<" Hz instead."<<endl;
	peakfreq=fs2;
      }
      minlevel = (int) floor(LOG2(fs2/peakfreq));


      // The instantiated transform is just the top numlevels-minlevel
      // of the "actual" reconstruction tree.

      
      if ((inputrep.levels-minlevel)<2) {
	minlevel=inputrep.levels-2;
	peakfreq = fs2 / (pow(2.0,(double)minlevel));
	cerr << "Can't really instantiate less than two levels."<<endl;
	cerr << "Setting a peak frequency of "<<peakfreq<<endl;
	cerr << "and instantiating with 2 levels"<<endl;
      }

      inputrep.levels-=minlevel; 


      cerr << "minlevel="<<minlevel<<" inputrep="<<inputrep<<endl;
      
      xform = new StaticReverseWaveletTransform<double,WISD,WOSD>(inputrep.levels-1,inputrep.wtype,2,2,minlevel);
      int *d = new int[inputrep.levels];
      CalculateWaveletDelayBlock(CQFWaveletCoefficients(inputrep.wtype).GetNumCoefs(),inputrep.levels,d);
      delay=new DelayBlock<WOSD>(inputrep.levels,minlevel,d);
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
	for (int i=s.rinfo.levels-3; i>=minlevel; i--) {
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
      if (s.level<(unsigned)minlevel) { 
	cerr << "Ignoring "<<s<<endl;
	// throw away unneeded samples
	continue;
      }
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
  source.Disconnect();
  return 0;
}
  
