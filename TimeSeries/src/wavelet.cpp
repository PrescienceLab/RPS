#include <algorithm>
#include <string.h>

// This is currently limited in the following ways:
//
// The prediction horizon is determined by the wavelet configuration file
// predictions at ALL HORIZONS are reported as the same
// all variances are reported as zero for all horizons
//
// These limitations will be fixed at a future date
//



#include "wavelet.h"
#include "util.h"
#include "tools.h"
#include "fileparameterset.h"

#include <strstream>

WaveletDelay::WaveletDelay() : len(0)
{}

WaveletDelay::WaveletDelay(const WaveletDelay &rhs) : len(rhs.len), delayline(rhs.delayline)
{}

WaveletDelay & WaveletDelay::operator=(const WaveletDelay &rhs)
{
  return *(new(this) WaveletDelay(rhs));
}


WaveletDelay::WaveletDelay(const int l)
{
  len=l;
  for (int i=0;i<len;i++) {
    delayline.push_back(0.0);
  }
}

WaveletDelay::~WaveletDelay()
{
  delayline.clear();
}

double WaveletDelay::PopPush(const double val)
{
  if (len==0) { 
    return val;
  } else {
    double temp=delayline.front();
    delayline.pop_front();
    delayline.push_back(val);
    return temp;
  }
}

void WaveletDelay::Dump(FILE *out) const
{
  fprintf(out,"WaveletDelay: len=%d, entries follow\n",len);
  for (deque<double>::const_iterator i=delayline.begin();i!=delayline.end();++i) {
    fprintf(out,"%lf\n",*i);
  }
}

ostream & WaveletDelay::operator<<(ostream &os) const
{
  os << "WaveletDelay(len="<<len<<", entries=(";
  for (deque<double>::const_iterator i=delayline.begin();i!=delayline.end();++i) {
    if (i!=delayline.begin()) { 
      os << ", ";
    }
    os << *i;
  }
  os << "))";
  return os;
}


WaveletLevelOp::WaveletLevelOp() : mt(0), model(0), pred(0), delay(0), samples(0), ahead(0)
{}

WaveletLevelOp::WaveletLevelOp(const WaveletLevelOp &rhs) : mt(rhs.mt), model(rhs.model), pred(rhs.pred), delay(rhs.delay), samples(rhs.samples), ahead(rhs.ahead)
{
  fprintf(stderr,"WaveletLevelOp copy constructor called!\n");
}

WaveletLevelOp & WaveletLevelOp::operator=(const WaveletLevelOp &rhs)
{
  return *(new (this) WaveletLevelOp(rhs));
}

WaveletLevelOp::~WaveletLevelOp()
{
  CHK_DEL(mt);
  CHK_DEL(model);
  CHK_DEL(pred);
  CHK_DEL(delay);
  samples=ahead=0;
}

void   WaveletLevelOp::Dump(FILE *out) const 
{
  fprintf(out,"WaveletLevelOp: samples=%d, ahead=%d, template, model, predictor, and delay follow\n", samples,ahead);
  if (mt) mt->Dump(out);
  if (model) model->Dump(out);
  if (pred) pred->Dump(out);
  if (delay) delay->Dump(out);
}

ostream & WaveletLevelOp::operator<<(ostream &os) const
{
  os<<"WaveletLevelOp(samples="<<samples<<", ahead="<<ahead;
  if (mt) { 
    os <<", mt="<<(*mt);
  }
  if (model) { 
    os <<", model="<<(*model);
  }
  if (pred) { 
    os<<", pred="<<(*pred);
  } 
  if (delay) { 
    os<<", delay="<<(*delay);
  }
  os<<")";
  return os;
}


WaveletModel::WaveletModel() 
{
}

WaveletModel::WaveletModel(const WaveletModel &rhs) : configfile(rhs.configfile)
{
}

WaveletModel::WaveletModel(const string &specfile) : configfile(specfile)
{
}

WaveletModel::WaveletModel(const char *specfile) : configfile(specfile)
{
}

WaveletModel::~WaveletModel()
{
}

WaveletModel & WaveletModel::operator=(const WaveletModel &rhs)
{
  this->~WaveletModel();
  return *(new(this)WaveletModel(rhs));
}

void WaveletModel::Dump(FILE *out) const
{
  fprintf(out,"WaveletModel(configfile=%s)\n",configfile.c_str());
}

ostream & WaveletModel::operator<<(ostream &os) const
{
  os <<"WaveletModel(configfile="<<configfile<<")";
  return os;
}

Predictor * WaveletModel::MakePredictor() const
{
  return new WaveletPredictor(configfile);
}

WaveletPredictor::WaveletPredictor() :  wtype(DAUB2), numlevels(0), levelop(0), xform(0), xformrev(0)
{}

WaveletPredictor::WaveletPredictor(const WaveletPredictor &rhs) : 
  specfile(rhs.specfile)
{
  ReadSpecFileAndConfigure();
}

WaveletPredictor::WaveletPredictor(const string &f) : specfile(f)
{
  ReadSpecFileAndConfigure();
}

WaveletPredictor::WaveletPredictor(const char *f) : specfile(f)
{
  ReadSpecFileAndConfigure();
}

WaveletPredictor::~WaveletPredictor()
{
  CHK_DEL_MAT(levelop);
  CHK_DEL(xform);
  CHK_DEL(xformrev);
}

WaveletPredictor & WaveletPredictor::operator=(const WaveletPredictor &rhs)
{
  this->~WaveletPredictor();
  return *(new(this)WaveletPredictor(rhs));
}



static const int MAXBUF=1024;

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

/*
 * Note: assumes that the model part of the input is at most 16 words
 */
int WaveletPredictor::ReadSpecFileAndConfigure()
{
  CHK_DEL_MAT(levelop);
  CHK_DEL(xform);
  CHK_DEL(xformrev);

  FILE *in;
  char buf[MAXBUF];

  if ((in=fopen(specfile.c_str(),"r"))==NULL) {
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
	//cerr << "numlevels: "<<numlevels<<endl;
	levelop = new WaveletLevelOp [numlevels];
	state=WAVELET;
	break;
      case WAVELET:
	sscanf(buf,"%d",(int*)&wtype);
	// cerr << "wtype: "<<(int)wtype<<endl;
	state=STAGES;
	
	xform = new StaticForwardWaveletTransform<double,WOSD,WISD>(numlevels-1,
								    wtype,
								    2,
								    2,
								    0);
	xformrev = new StaticReverseWaveletTransform<double,WISD,WOSD>(numlevels-1,
								       wtype,
								       2,
								       2,
								       0);

	break;
      case STAGES:
	int level;
	int rest;
	int predh;
	if (sscanf(buf,"%d %d %n\n",&level,&predh, &rest)<2) {
	  break;
	}
	if (level<0 || (unsigned)level >=numlevels) {
	  break;
	}
	levelop[level].ahead=predh;
	levelop[level].samples=0;
	if (predh>0) { 
	  if ((levelop[level].mt=ParseModel(&buf[rest]))==0) { 
	    // cerr << "Unknown model " << (const char *) &buf[rest]<<endl;
	    return -1;
	  }
	  levelop[level].model=FitThis((double*)0,0,*(levelop[level].mt));
	  levelop[level].pred=levelop[level].model->MakePredictor();
	  levelop[level].delay=0;
	  // cerr << "level "<< level << ": " << *(levelop[level].mt) << " at "<<levelop[level].ahead<< endl;
	} else {
	  levelop[level].mt=0;
	  levelop[level].model=0;
	  levelop[level].pred=0;
	  levelop[level].delay = new WaveletDelay(-predh);
	  // cerr << "level "<< level << ": " << "Delay of "<<-predh<<endl;
	}
	break;
      }
    }
  }
  return 0;
}


int WaveletPredictor::Begin()
{
  curindex=0;
  return 0;
}

int WaveletPredictor::StepsToPrime() const
{
  return 0;
}
 

//
// FIX FIX FIX
//
double WaveletPredictor::Step(const double obs)
{
  vector <WOSD> output;
  vector <WOSD> predoutput;
  vector <WISD> finalout;

  xform->StreamingTransformSampleOperation(output,WISD(obs,curindex++));

  for (unsigned i=0;i<output.size();i++) { 
    int level = output[i].GetSampleLevel();
    double value = output[i].GetSampleValue();
    int index = output[i].GetSampleIndex();
    double outval;

    if (levelop[level].ahead<=0) { 
      outval=levelop[level].delay->PopPush(value);
    } else {
      double preds[levelop[level].ahead];
      levelop[level].pred->Step(value);
      levelop[level].pred->Predict(levelop[level].ahead,preds);
      outval=preds[levelop[level].ahead-1];
    }
    predoutput.push_back(WOSD(outval,level,index+levelop[level].ahead));
  }
  
  xformrev->StreamingTransformSampleOperation(finalout,predoutput);

  for (unsigned i=0; i< finalout.size(); i++ ) {
    nextval=finalout[i].GetSampleValue();
    cerr << "i="<<i<<" => "<<nextval;
  }

  return 0.0;
}

//
// Note how this returns all values as the prediction horizon
// of the config file.  This breaks the semantics of Predictor
// and will eventually be fixed.
int WaveletPredictor::Predict(const int maxahead, double *preds) const
{
  for (int i=0;i<maxahead;i++) { 
    preds[i]=nextval;
  }
  return 0;
}


//
// Note that this incorrectly returns all variances as zero
//
int WaveletPredictor::ComputeVariances(const int maxahead, double *vars,
					  const VarianceType vtype) const
{
  int i,j;

  switch (vtype) { 
  case POINT_VARIANCES:
    for (i=0;i<maxahead;i++) { 
      vars[i]=0.0;
    }
    return 0;
    break;
  case SUM_VARIANCES: 
    for (i=0;i<maxahead;i++) {
      vars[i]=0.0;
    }
    return 0;
    break;
  case CO_VARIANCES:
    for (i=0;i<maxahead;i++) {
      for (j=0;j<maxahead;j++) {
	vars[i*maxahead+j] = (i==j) ? 0.0 : 0.0;
      }
    }
    return 0;
    break;
  }
  return -1;
}

void WaveletPredictor::Dump(FILE *out) const
{
  fprintf(out,"WaveletPredictor: specfile=%s, wtype=%d, numlevels=%u, forward and reverse transforms and levelops follow\n",
	  specfile.c_str(), wtype, numlevels);
  strstream s1,s2;
  xform->Print(s1); fprintf(out,"%s\n",s1.str());
  xformrev->Print(s2); fprintf(out,"%s\n",s2.str());
  for (unsigned i=0;i<numlevels;i++) {
    levelop[i].Dump(out);
  }
}

ostream & WaveletPredictor::operator<<(ostream &os) const
{
  os<<"WaveletPredictor(specfile="<<specfile<<", wtype="<<wtype
    <<", numlevels="<<numlevels<<", xform="<<(*xform)
    <<", xformrev="<<(*xformrev)<<", levelops=(";
  for (unsigned i=0;i<numlevels;i++) {
    if (i!=0) { 
      os << ", ";
    }
    os << levelop[i];
  }
  os << "))";
  return os;
}

WaveletModeler::WaveletModeler()
{}

WaveletModeler::WaveletModeler(const WaveletModeler &rhs)
{}

WaveletModeler::~WaveletModeler()
{}

WaveletModeler & WaveletModeler::operator=(const WaveletModeler &rhs)
{
  this->~WaveletModeler();
  return *(new(this)WaveletModeler(rhs));
}



WaveletModel * WaveletModeler::Fit(const double *sequence, const int len, const string &config)
{
  return new WaveletModel(config);
}


Model *WaveletModeler::Fit(const double *seq, const int len, const ParameterSet &ps)
{
  string s;
  ((const FileParameterSet &)ps).Get(s);
  return Fit(seq,len,s);
}


void WaveletModeler::Dump(FILE *out) const
{
  fprintf(out,"WaveletModeler()\n");
}

ostream & WaveletModeler::operator<<(ostream &os) const
{
  os << "WaveletModeler()";
  return os;
}
