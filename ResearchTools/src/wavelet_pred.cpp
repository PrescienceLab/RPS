#include "TimeSeries.h"
#include "Wavelets.h"

/*
 
 Input File Format:

 # - ignored
 numlevels
 wavelettype (number)
 0 prediction_horizon model   - for level 0
 1 prediction_horizon model   - for level 1
 ...
 numlevels-1 prediction_horizon model  - for level numlevels-1

*/


const int MAXLEVELS=512;
const int MAXBUF=1024;
int numlevels;


WaveletType wavelettype;

struct LevelOp {
  ModelTemplate *mt;
  Model         *model;
  Predictor     *pred;
  int amountofdata;
  int ahead;
};


LevelOp levelop[MAXLEVELS];


void usage()
{
  cerr << "usage: wavelet_pred specfile < datain > dataout"<<endl;
}



/*
 * Note: assumes that the model part of the input is at most 16 words
 */
int ReadSpecFile(const char *file)
{
  FILE *in;
  char buf[MAXBUF];

  if ((in=fopen(file,"r"))==NULL) {
    return -1;
  }
  
  enum {LEVELS,WAVELET,STAGES} state;

  state=LEVELS;
  while (!feof(in) && fgets(buf,MAXBUF,in)) { 
    if (buf[0]=='#') { 
    } else {
      switch (state) {
      case LEVELS:
	sscanf(buf,"%d",&numlevels);
	cerr << "numlevels: "<<numlevels<<endl;
	state=WAVELET;
	break;
      case WAVELET:
	sscanf(buf,"%d",(int*)&wavelettype);
	cerr << "wavelettype: "<<(int)wavelettype<<endl;
	state=STAGES;
	break;
      case STAGES:
	int level;
	int rest;
	int predh;
        int amt;
	if (sscanf(buf,"%d %d %d %n\n",&level,&predh, &amt,&rest)<3) {
	  break;
	}
	levelop[level].mt=ParseModel(&buf[rest]);
	levelop[level].ahead=predh;
	levelop[level].amountofdata=amt;
	cerr << "level "<< level << ": " << *(levelop[level].mt) << " at "<<levelop[level].ahead<< " after "<<levelop[level].amountofdata<<" items"<<endl;
	break;
      }
    }
  }
  return 0;
}
      
      

  


int main(int argc, char *argv[])
{
  int i; 
  if (argc!=2) {
    usage();
    exit(-1);
  }

  ReadSpecFile(argv[1]);


  // Read the data from file into an input vector
  vector<WaveletInputSample> samples;
  double sample;
  while (cin >> sample) {
    WaveletInputSample wavesample;
    wavesample.SetSampleValue(sample);
    samples.push_back(wavesample);
  }

  StaticForwardWaveletTransform<double, WaveletOutputSample, WaveletInputSample>
    sfwt(numlevels-1,wavelettype,2,2,0);

  StaticReverseWaveletTransform<double, WaveletInputSample, WaveletOutputSample>
    srwt(numlevels-1,wavelettype,2,2);
  
  
  vector<WaveletOutputSample> outsamples;
  vector<WaveletOutputSample> delaysamples;
  vector<WaveletInputSample>  finaloutput;
  vector<WaveletInputSample>  outsamp;

  for (i=0; i<samples.size(); i++) {
    sfwt.StreamingSampleOperation(outsamples, samples[i]);

    /*
    if (srwt.StreamingSampleOperation(outsamp, delaysamples)) {
      for (unsigned j=0; j<outsamp.size(); j++) {
	finaloutput.push_back(outsamp[j]);
      }
    }
    */

    cout << "Input Sample: "<<samples[i]<<endl;
    for (int j=0;j<outsamples.size();j++) { 
      cout << "Output Sample: " << outsamples[j] << endl;
    }
    
    outsamples.clear();
  }
  

 
  
}
  



