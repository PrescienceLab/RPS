#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <math.h>


#include "PredComp.h"

#if defined(__osf__)
extern "C" int usleep(int);  // FREAKS
#endif

#define MEASURE_RATE 0

void usage()
{
  fprintf(stderr,"usage: hostloadpred [ctrlnetspace] [connectnetspec] period_us numfit numpred mintestsamples maxtestsamples maxabserr maxerrmissest [ModelParms]\n");
}

class HostLoadPred : public LocalSource {
private:
  ModelTemplate *startuptemplate;
  ModelTemplate *modeltemplate;
  PredictionResponse predresp;
  int numfit;
  int numpred;
  Model *model;
  Predictor *pred;
  Evaluator *eval;
  double *vals;
  double *valtemp;
  int     cur;
  int period_usec;
  int mintestsamples;
  int maxtestsamples;
  double maxabserr;
  double maxerrmissest;
  int curevalnumsamples;
public:

  void RefitNow() {
    CHK_DEL(model);
    CHK_DEL(pred);
    if (cur<numfit) {
      // Not enough samples.  Just fit a LAST
      char *genericmodel[] = { "NONE" };
      startuptemplate = ParseModel(1,genericmodel);
      model = new NoneModel;
      pred = model->MakePredictor();
      CHK_DEL(startuptemplate)
      startuptemplate = ParseModel(1,genericmodel);
      fprintf(stderr,"Fitted initial model\n");
    } else {
      int nextpos = cur%numfit;
      memcpy(valtemp,&(vals[nextpos]),sizeof(double)*(numfit-nextpos));
      memcpy(&(valtemp[numfit-nextpos]),vals,sizeof(double)*(nextpos));
      model = FitThis(valtemp,numfit,*modeltemplate);
      pred = model->MakePredictor();
      pred->Begin();
      for (int i=numfit-pred->StepsToPrime();i<numfit;i++) {
	pred->Step(valtemp[i]);
      }
      fprintf(stderr,"Fitted chosen model\n");
      modeltemplate->Print(stderr);
    }
    predresp.tag=GetMyIPAddress();
    predresp.flags = PREDFLAG_IPADX;
    if (cur<numfit) {
      predresp.modelinfo = *startuptemplate;
    } else {
      predresp.modelinfo = *modeltemplate;
    }
    predresp.period_usec=period_usec;
    predresp.Resize(numpred,true);
    pred->ComputeVariances(predresp.numsteps,predresp.errs,SUM_VARIANCES);
    CHK_DEL(eval);
    eval=new Evaluator;
    eval->Initialize(numpred);
    curevalnumsamples=0;
  }
  
  HostLoadPred(int period_usec, 
	       int numfit,
	       int numpred,
	       int mintestsamples,
	       int maxtestsamples,
	       double maxabserr,
	       double maxerrmissest,
	       int nummodelparms,
	       char *modelparms[]) { 
    this->period_usec=period_usec; 
    this->numfit=numfit;
    this->numpred=numpred;
    this->mintestsamples=mintestsamples;
    this->maxtestsamples=maxtestsamples;
    this->maxabserr=maxabserr;
    this->maxerrmissest=maxerrmissest;
    modeltemplate = ParseModel(nummodelparms,modelparms);
    startuptemplate = 0;
    model=0;
    pred=0;
    eval=0;
    vals=new double [numfit];
    valtemp = new double [numfit];
    cur = 0;
    curevalnumsamples=0;
    RefitNow();
  }

  virtual ~HostLoadPred() {
    CHK_DEL_MAT(vals);
    CHK_DEL_MAT(valtemp);
    CHK_DEL(model);
    CHK_DEL(pred);
    CHK_DEL(eval);
    CHK_DEL(modeltemplate);
    CHK_DEL(startuptemplate);
  }

  virtual void GetData(Buffer &buf) {
    GetData(predresp);
    predresp.Serialize(buf);
  }
  virtual void GetData(PredictionResponse &pr) {
    double measure;
    if (RPSgetloadavg(&measure,1)!=1) {
      fprintf(stderr,"hostloadpred can't read load\n");
      exit(-1);
    }
    
    vals[cur%numfit]=measure;
    cur++;
    
    if (cur==numfit) {
      RefitNow();
    }

    if (gettimeofday(&(pr.datatimestamp),0)) {
      perror("hostloadpred can't get time");
      exit(-1);
    }
    
    pred->Step(measure);
    pred->Predict(pr.numsteps,pr.preds);
    
    if (gettimeofday(&(pr.predtimestamp),0)) {
      perror("hostloadpred can't get time");
      exit(-1);
    }
    
  evaltop:
    eval->Step(measure,pr.preds);
    curevalnumsamples++;

    double error = eval->GetCurrentPlusOneMeanSquareError();
    
    if (curevalnumsamples>maxtestsamples) {
      // Too many samples, drain it and restart
      eval->Drain();
      PredictionStats *stats = eval->GetStats();
      stats->Dump(stderr);
      CHK_DEL(stats);
      CHK_DEL(eval);
      eval=0;
      eval = new Evaluator;
      eval->Initialize(numpred);
      curevalnumsamples=0;
      goto evaltop;
    }
    
    if (curevalnumsamples>mintestsamples) { 
      if (error>maxabserr) { 
	fprintf(stderr,"Measured error (%f) exceeds maximum (%f) - forcing model reconfig\n",
		error,maxabserr);
	CHK_DEL(eval);
	eval=0;
	goto reconfig;
      }
      double relerrmissest = 100.0*fabs(error-pr.errs[0])/(MAX(0.00001,pr.errs[0]));
      if (relerrmissest>maxerrmissest && error>pr.errs[0]) { 
	fprintf(stderr,"Predictor miss-estimates error by %f%% (est=%f,meas=%f) which exceeds maximum of %f%% - forcing model reconfig\n",
		relerrmissest, pr.errs[0], error,maxerrmissest);
	CHK_DEL(eval);
	eval=0;
	goto reconfig;
      }
    }
    return;
  reconfig:
    RefitNow();
  }

  virtual void GetInterval(TimeValue &tv) {
    tv = TimeValue(period_usec/1000000,period_usec%1000000);
  }
};


#define MAXNUMSAMPLES 10000

class HostLoadPredSink : public LocalTarget {
private: 
  int numsamples;
  TimeStamp begin;
public:
  HostLoadPredSink() { numsamples=0; }
  void ProcessData(Buffer &buf) {
#if MEASURE_RATE
    //fprintf(stderr, "Sample %d\n",numsamples);
    if (numsamples==0) { 
      begin=TimeStamp(0);
    }
    if (numsamples==MAXNUMSAMPLES) { 
      TimeStamp end(0);
      double rate = MAXNUMSAMPLES/((double)end-(double)begin);
      fprintf(stderr,"prediction rate is %f Hz (%d samples)\n",
	      rate, MAXNUMSAMPLES);
      exit(0);
    }
#endif
    PredictionResponse pr;
    pr.Unserialize(buf);
#if !MEASURE_RATE
     pr.Print();
#endif
     numsamples++;
  }
};


class RateControl;

typedef FilterWithControl<PredictionResponse,
                          NullCompute<PredictionResponse,PredictionResponse>,
                          PredictionResponse,
                          LoadMeasurementConfigurationRequest,
                          RateControl,
                          LoadMeasurementConfigurationReply> HostLoadPredServer;

HostLoadPred *curpredsource;
HostLoadPredServer *server;


int numfit;
int numpred;
int mintestsamples;
int maxtestsamples;
double maxabserr;
double maxerrmissest;
int nummodelparms;
char **modelparms;

class RateControl {
public:
  static int Compute(LoadMeasurementConfigurationRequest &req,
		     LoadMeasurementConfigurationReply &repl) {
    
    server->DeleteLocalSource(curpredsource);
    delete curpredsource;
    curpredsource = new HostLoadPred(req.period_usec,
				     numfit,
				     numpred,
				     mintestsamples,
				     maxtestsamples,
				     maxabserr,
				     maxerrmissest,
				     nummodelparms,
				     modelparms);
    server->AddLocalSource(curpredsource);
    repl.reqtimestamp=req.timestamp;
    repl.changetimestamp=TimeStamp(0);
    repl.period_usec=req.period_usec;
    return 0;
  }
};




int main(int argc, char *argv[]) 
{
  EndPoint *ep;
  int i;

  if (argc<11) {
    usage();
    exit(0);
  }

  int period_usec=atoi(argv[3]);
  numfit = atoi(argv[4]);
  numpred = atoi(argv[5]);
  mintestsamples = atoi(argv[6]);
  maxtestsamples = atoi(argv[7]);\
  maxabserr = atof(argv[8]);
  maxerrmissest = atof(argv[9]);

  nummodelparms = argc - 9 - 1;
  modelparms=&(argv[10]);
  

  curpredsource = new HostLoadPred(period_usec,
				   numfit,
				   numpred,
				   mintestsamples,
				   maxtestsamples,
				   maxabserr,
				   maxerrmissest,
				   nummodelparms,
				   modelparms);

  server = new HostLoadPredServer;

  server->AddLocalSource(curpredsource);

  for (i=1;i<3;i++) {
   ep = new EndPoint;
    if (ep->Parse(argv[i])) {
      fprintf(stderr,"Failed to parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    server->AddEndPoint(*ep);
  }
  
#if MEASURE_RATE
  HostLoadPredSink *sink = new HostLoadPredSink;
  server->AddLocalTarget(sink);
#endif

  fprintf(stderr,"hostloadpred running.\n");
  server->Run();
}

