#include <stdlib.h>

#include "PredComp.h"
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
    for (unsigned i=0;i<strlen(buf);i++) { 
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



class Prediction {
public:
  static int Compute(WaveletIndividualSample &in, WaveletIndividualSample &out) {
    assert(in.rinfo.rtype==WAVELET_DOMAIN_TRANSFORM || in.rinfo.rtype==WAVELET_DOMAIN_APPROX || in.rinfo.rtype==WAVELET_DOMAIN_DETAIL);
    assert(in.rinfo.levels==(unsigned)numlevels);

    cerr << "Input:" <<in <<endl;
    
    int level=in.level;
    double inval=in.value;
    double outval;
    
    if (levelop[level].ahead<=0) { 
      outval=levelop[level].delay->PopPush(inval);
    } else {
      double *preds = new double[levelop[level].ahead];
      levelop[level].pred->Step(inval);
      levelop[level].pred->Predict(levelop[level].ahead,preds);
      outval=preds[levelop[level].ahead-1];
	  delete [] preds;
    }

    out=in;
    double outtime = ((double) in.timestamp ) + (levelop[level].ahead*in.rinfo.period_usec)/1e6;
    out.timestamp=TimeStamp((int)outtime, (int)((outtime-((int)outtime))*1e6));
    out.index+=levelop[level].ahead;
    out.value=outval;
    
    cerr << "Output:" << out <<endl;
    
    return 0;
  }
};





void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Streaming Wavelet Predictor \n\n"
	  "usage: %s specfile source target+\n\n"
	  "specfile        = specification file (see comment)\n"
	  "source+         = source endpoints for measurements\n"
	  "target+         = one or more target or connect endpoints\n\n%s",n,b);
  delete [] b;
}



typedef GenericSerializeableInputComputeOutputMirror<WaveletIndividualSample, Prediction, WaveletIndividualSample> WaveletPredictionMirror;




int main(int argc, char *argv[]) 
{
  int i;
  

  if (argc<4) {
    usage(argv[0]);
    exit(0);
  }

  ReadSpecFile(argv[1]);

  WaveletPredictionMirror mirror;

  for (i=2;i<argc;i++) { 
    EndPoint *ep = new EndPoint;
    if (ep->Parse(argv[i])) { 
      fprintf(stderr,"wavelet_predict: Can't parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    if (mirror.AddEndPoint(*ep)) { 
      fprintf(stderr,"wavelet_predict: Can't add \"%s\"\n",argv[i]);
      exit(-1);
    }
  }

  fprintf(stderr,"wavelet_predict running.\n");
  mirror.Run();
  return 0;
}



