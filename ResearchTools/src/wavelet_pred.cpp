#include "TimeSeries.h"
#include "Wavelets.h"

/*
 
 Input File Format:

 # - ignored
 numlevels
 wavelettype (number)
 0 prediction_horizon initdata model   - for level 0
 1 prediction_horizon initdata model   - for level 1
 ...
 numlevels-1 prediction_horizon initdata model  - for level numlevels-1

*/


const int MAXLEVELS=512;
const int MAXBUF=1024;
int numlevels;


WaveletType wavelettype;

struct LevelOp {
  ModelTemplate *mt;
  Model         *model;
  Predictor     *pred;
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
    for (int i=0;i<strlen(buf);i++) { 
      if (buf[i]=='\n') {
	buf[i]=' ';
      }
    }
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
	if (sscanf(buf,"%d %d %n\n",&level,&predh, &rest)<2) {
	  break;
	}
	levelop[level].mt=ParseModel(&buf[rest]);
	levelop[level].model=FitThis((double*)0,0,*(levelop[level].mt));
	levelop[level].pred=levelop[level].model->MakePredictor();
	levelop[level].ahead=predh;
	cerr << "level "<< level << ": " << *(levelop[level].mt) << " at "<<levelop[level].ahead<< endl;
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
  
  
  vector<WaveletOutputSample> forwardout;
  vector<WaveletOutputSample> delayout;
  vector<WaveletOutputSample> predout;
  vector<WaveletInputSample>  finalout;
  vector<WaveletInputSample>  reverseout;

  for (i=0; i<samples.size(); i++) {
    sfwt.StreamingSampleOperation(forwardout, samples[i]);

    /*
    if (srwt.StreamingSampleOperation(outsamp, delaysamples)) {
      for (unsigned j=0; j<outsamp.size(); j++) {
	finaloutput.push_back(outsamp[j]);
      }
    }
    */

    cerr << "Input Sample: "<<samples[i]<<endl;
    for (int j=0;j<forwardout.size();j++) { 
      cerr << "Output Sample: " << forwardout[j] << endl;
    }
    for (int j=0;j<forwardout.size();j++) { 
      int level=forwardout[j].GetSampleLevel();
      levelop[level].pred->Step(forwardout[j].GetSampleValue());
      double preds[levelop[level].ahead];
      levelop[level].pred->Predict(levelop[level].ahead,preds);
      cerr << "Level " << level << " Predictions: " ;
      for (int k=0;k<levelop[level].ahead;k++) {
	cerr << preds[k] << "\t";
      }
      cerr <<endl;
      predout.push_back(WaveletOutputSample(preds[levelop[level].ahead-1],level,forwardout[j].GetSampleIndex()));
     
    }

    srwt.StreamingSampleOperation(reverseout, predout);

    for (unsigned j=0; j<reverseout.size(); j++) {
      finalout.push_back(reverseout[j]);
    }
    forwardout.clear();
    predout.clear();
    reverseout.clear();
  }

  for (i=0; i<finalout.size(); i++) {
    cerr << samples[i] << "\t" << finalout[i] << endl;
  }

  for (i=0; i<finalout.size(); i++) {
    cout << samples[i].GetSampleValue() << "\t" << finalout[i].GetSampleValue() << endl;
  }
  
 
}
  

