#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
//#include <stropts.h>
#include <sys/select.h>

#include "socks.h"
#include "LoadMeasurement.h"
#include "Reference.h"
#include "EndPoint.h"
#include "PredictionRequestResponse.h"
#include "Measurement.h"

#include "evaluate_core.h"
#include "fit.h"

#include "glarp.h"


void usage()
{
  fprintf(stderr,"usage: evalfit measuresource predsource predreconfigport mintestsamples maxtestsamples maxabserr maxerrmissestpercent numfit numpred MODELSTUFF\n");
}

#define DEFAULT_PERIOD 1000000


int main(int argc, char *argv[]) 
{
  const int first_model=10;
  Measurement        measure;
  PredictionResponse pred;
  Evaluator          *eval=0;
  int numfit;
  int numpred;
  double maxabserr, maxerrmissest;
  int maxtestsamples, mintestsamples;

  if (argc<first_model+1) {
    usage();
    exit(0);
  }

  EndPoint measureep;
  EndPoint predep;
  EndPoint reconfigep;

  if (measureep.Parse(argv[1])) { 
    fprintf(stderr,"Can't parse \"%s\"\n",argv[1]);
    exit(-1);
  }
  if (predep.Parse(argv[2])) { 
    fprintf(stderr,"Can't parse \"%s\"\n",argv[2]);
    exit(-1);
  }
  if (reconfigep.Parse(argv[3])) { 
    fprintf(stderr,"Can't parse \"%s\"\n",argv[3]);
    exit(-1);
  }

  mintestsamples=atoi(argv[4]);
  maxtestsamples=atoi(argv[5]);

  maxabserr=atof(argv[6]);
  maxerrmissest=atof(argv[7]);
 

  numfit=atoi(argv[8]);
  numpred=atoi(argv[9]);
 

  ModelTemplate *mt = ParseModel(argc-first_model,&(argv[first_model]));
  if (mt==0) { 
    usage();
    return 0;
  }
  ModelInfo mi(*mt);
  
  StreamingInputReference<Measurement>        measuresource;
  StreamingInputReference<PredictionResponse> predsource;
  Reference<PredictionReconfigurationRequest, PredictionReconfigurationResponse> reconfig;

  if (measuresource.ConnectTo(measureep)) {
    fprintf(stderr,"Can't connect to \"%s\"\n",argv[1]);
    exit(-1);
  }
  if (predsource.ConnectTo(predep)) {
    fprintf(stderr,"Can't connect to \"%s\"\n",argv[2]);
    exit(-1);
  }

  if (reconfig.ConnectTo(reconfigep)) {
    fprintf(stderr,"Can't connect to \"%s\"\n",argv[3]);
    exit(-1);
  }

  double *temp = new double [numfit];


  // configure it with our model

 reconfig:

  TimeStamp now=TimeStamp(0);
  PredictionReconfigurationRequest req(mi,now,numfit,temp,DEFAULT_PERIOD,numpred);
  PredictionReconfigurationResponse resp;

  reconfig.Call(req,resp);


  int curpredsteps=0;
  int curnumsamples=0;

  while (1) {
    measuresource.GetNextItem(measure);
    predsource.GetNextItem(pred);

    if (measure.tag!=pred.tag) {
      fprintf(stderr,"Measurement and Prediction Streams have different tags\n");
      goto die;
    }

    while (measure.timestamp!=pred.datatimestamp) { 
      while (measure.timestamp<pred.datatimestamp) { 
	fprintf(stderr,"syncwait... measure:%lf vs pred:%lf\n",
		(double)(measure.timestamp), (double)(pred.datatimestamp));
	measuresource.GetNextItem(measure);
	if (measure.tag!=pred.tag) {
	  fprintf(stderr,"Measurement and Prediction Streams have different tags\n");
	  goto die;
	}
      } 
      while (measure.timestamp>pred.datatimestamp) {
	fprintf(stderr,"syncwait... measure:%lf vs pred:%lf\n",
		(double)(measure.timestamp), (double)(pred.datatimestamp));
	predsource.GetNextItem(pred);
	if (measure.tag!=pred.tag) {
	  fprintf(stderr,"Measurement and Prediction Streams have different tags\n");
	  goto die;
	}
      }
    }

    if (eval==0 || pred.numsteps!=curpredsteps) { 
      CHK_DEL(eval);
      eval = new Evaluator;
      eval->Initialize(pred.numsteps);
      curpredsteps=pred.numsteps;
      curnumsamples=0;
    }
    assert(measure.serlen==1);
    eval->Step(measure.series[0],pred.preds);
    curnumsamples++;

    double error = eval->GetCurrentPlusOneMeanSquareError();
    //fprintf(stdout,"%d\t%lf\t%lf\n",curnumsamples,pred.errs[0],error);
    if (curnumsamples>maxtestsamples) {
      // Too many samples, drain it and restart
      eval->Drain();
      PredictionStats *stats = eval->GetStats();
      stats->Dump(stderr);
      CHK_DEL(stats);
      CHK_DEL(eval);
      eval=0;
      continue;
    }
      
    if (curnumsamples>mintestsamples) { 
      if (error>maxabserr) { 
	fprintf(stderr,"Measured error (%lf) exceeds maximum (%lf) - forcing model reconfig\n",
		error,maxabserr);
	CHK_DEL(eval);
	eval=0;
	goto reconfig;
      }
      double relerrmissest = 100.0*fabs(error-pred.errs[0])/(MAX(0.00001,pred.errs[0]));
      if (relerrmissest>maxerrmissest && error>pred.errs[0]) { 
	fprintf(stderr,"Predictor miss-estimates error by %lf%% (est=%lf,meas=%lf) which exceeds maximum of %lf%% - forcing model reconfig\n",
		relerrmissest, pred.errs[0], error,maxerrmissest);
	CHK_DEL(eval);
	eval=0;
	goto reconfig;
      }
    }
  }

    die:

  return 0;
}
  
