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

int sampletime;


WaveletType wavelettype;


class Delay {
private:
  int len;
  deque<double> delayline;
public:
  Delay(int l) {
    len=l;
    for (int i=0;i<len;i++) {
      delayline.push_back(0.0);
    }
  }
  virtual ~Delay() { 
    delayline.clear();
  }
  double PopPush(double val) {
    if (len==0) { 
      return val;
    } else {
      double temp=delayline.front();
      delayline.pop_front();
      delayline.push_back(val);
      return temp;
    }
  }
};


struct LevelOp {
  ModelTemplate *mt;
  Model         *model;
  Predictor     *pred;
  Delay         *delay;
  int           samples;       
  int           ahead;
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
	if (level<0 || level >=numlevels) {
	  break;
	}
	levelop[level].ahead=predh;
	levelop[level].samples=0;
	if (predh>0) { 
	  if ((levelop[level].mt=ParseModel(&buf[rest]))==0) { 
	    cerr << "Unknown model " << (const char *) &buf[rest]<<endl;
	    return -1;
	  }
	  levelop[level].model=FitThis((double*)0,0,*(levelop[level].mt));
	  levelop[level].pred=levelop[level].model->MakePredictor();
	  levelop[level].delay=0;
	  cerr << "level "<< level << ": " << *(levelop[level].mt) << " at "<<levelop[level].ahead<< endl;
	} else {
	  levelop[level].mt=0;
	  levelop[level].model=0;
	  levelop[level].pred=0;
	  levelop[level].delay = new Delay(-predh);
	  cerr << "level "<< level << ": " << "Delay of "<<-predh<<endl;
	}
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

  sampletime=0;
  for (i=0; i<samples.size(); i++) {
    sfwt.StreamingSampleOperation(forwardout, samples[i]);

    for (int j=0;j<forwardout.size();j++) { 
      int level=forwardout[j].GetSampleLevel();
      double outval;
      if (levelop[level].ahead<=0) { 
	outval=levelop[level].delay->PopPush(forwardout[j].GetSampleValue());
	cerr << sampletime<< "("<<levelop[level].samples<<"@"<< level << "): Sample:" <<forwardout[j].GetSampleValue()<< " Delayed Value: " << outval <<endl;
      } else {
	double preds[levelop[level].ahead];
	levelop[level].pred->Step(forwardout[j].GetSampleValue());
	levelop[level].pred->Predict(levelop[level].ahead,preds);
	outval=preds[levelop[level].ahead-1];
	cerr << sampletime<< "("<<levelop[level].samples<<"@"<< level << "): Sample:" <<forwardout[j].GetSampleValue()<< " Prediction: " << outval <<endl;
      }
      predout.push_back(WaveletOutputSample(outval,level,forwardout[j].GetSampleIndex()));
      levelop[level].samples++;
    }

    srwt.StreamingSampleOperation(reverseout, predout);

    for (unsigned j=0; j<reverseout.size(); j++) {
      finalout.push_back(reverseout[j]);
    }
    forwardout.clear();
    predout.clear();
    delayout.clear();
    reverseout.clear();
    sampletime++;
  }


  int basepred = levelop[0].ahead;
  for (i=0; i<finalout.size(); i++) {
    cout << samples[i].GetSampleValue() << "\t" << finalout[i].GetSampleValue() << "\t" << finalout[i].GetSampleValue() - samples[i].GetSampleValue() << endl;
  }
  
 
}
  

