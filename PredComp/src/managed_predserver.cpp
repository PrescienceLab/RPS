#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <signal.h>
#include <math.h>

#include "PredComp.h"


void usage(const char *n)
{
  char *b=GetRPSBanner();
  char *m=GetAvailableModels();

  fprintf(stdout, 
	  "Streaming Prediction Server for Managed models\n\n"
	  "usage: %s measuresource control target+ numpred MANAGED MODEL\n\n"
	  "measuresource   = source endpoint for measurements\n"
	  "control         = server endpoint for control\n"
	  "target+         = one or more target or connect endpoints for predictions\n"
	  "numpred         = number of steps ahead to predict\n"
	  "MANAGED MODEL   = a managed model or some other model\n"
          "                  that does not require startup (see below)\n\n%s\n%s",n,m,b);
  delete [] b;
  delete [] m;
}


int numpred;
ModelTemplate *mt;
Model *model;
Predictor *predictor;

class Prediction {
public:
  static int Compute(Measurement &measure, PredictionResponse &pred) {
    
    for (int i=0;i<measure.serlen;i++) {
      predictor->Step(measure.series[i]);
    }
    pred.tag=measure.tag;
    pred.flags=PREDFLAG_OK;
    pred.datatimestamp=measure.timestamp;
    pred.period_usec=measure.period_usec;
    pred.modelinfo=ModelInfo(*mt);
    pred.Resize(numpred,false);
    predictor->Predict(pred.numsteps,pred.preds);
    predictor->ComputeVariances(pred.numsteps,pred.errs,SUM_VARIANCES);
    pred.predtimestamp=TimeStamp();

    return 0;
  }
};

class Reconfiguration {
public:
  static int Compute(PredictionReconfigurationRequest &req, PredictionReconfigurationResponse &resp) {
    if (req.modelinfo.ps->GetType()==ManagedPDQ) {
      delete model;
      delete predictor;
      delete mt;
      mt=req.modelinfo.Clone();
      model=FitThis((const double*)0,0,*mt);
      predictor=model->MakePredictor();
      numpred=req.numsteps;
      resp.flags=PREDFLAG_OK;
    } else {
      resp.flags=PREDFLAG_FAIL;
    }
    resp.tag=req.tag;
    resp.modelinfo=ModelInfo(*mt);
    resp.requesttimestamp=req.datatimestamp;
    resp.reconfigdonetimestamp=TimeStamp();
    resp.period_usec=req.period_usec;
    resp.numsteps=numpred;
    return 0;
  }
};

typedef FilterWithControl<
  Measurement,
  Prediction,
  PredictionResponse,
  PredictionReconfigurationRequest,
  Reconfiguration,
  PredictionReconfigurationResponse
> PredictionMirror;



int main(int argc, char *argv[]) 
{
  EndPoint sourceep;
  EndPoint reconfigserverep;
  EndPoint *ep;
  int managed;

  if (argc<7) {
    usage(argv[0]);
    exit(0);
  }

  // look for the prediction horizon.  it must be immediately before the model.

  for (managed=4;managed<argc;managed++) {
    if (isdigit(argv[managed][0])) { 
      break;
    }
  }
  if (managed==argc) {
    fprintf(stderr, "Could not find where the model starts!\n");
    exit(-1);
  }
  managed++;

  if (strcasecmp(argv[managed],"MANAGED") && strcasecmp(argv[managed],"AWAIT")) {
    fprintf(stderr,"Warning: model does not have MANAGED or AWAIT modifier.\n"
                   "Going ahead anyway, but this could cause managed_predserver to fail.\n");
  }

  PredictionMirror mirror;

  if (sourceep.Parse(argv[1]) || sourceep.atype!=EndPoint::EP_SOURCE) { 
    fprintf(stderr,"Can't parse \"%s\"\n",argv[1]);
    exit(-1);
  }

  if (mirror.AddEndPoint(sourceep)) { 
    fprintf(stderr,"Can't add \"%s\"\n",argv[1]);
    exit(-1);
  }
  
  if (reconfigserverep.Parse(argv[2]) || reconfigserverep.atype!=EndPoint::EP_SERVER) {
    fprintf(stderr,"Can't parse \"%s\"\n",argv[2]);
    exit(-1);
  }
  if (mirror.AddEndPoint(reconfigserverep)) { 
    fprintf(stderr,"Can't add \"%s\"\n",argv[3]);
    exit(-1);
  }

  int i;
  for (i=3;i<managed-1;i++) { 
    ep = new EndPoint;
    if (ep->Parse(argv[i])) { 
      fprintf(stderr,"Can't parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    if (mirror.AddEndPoint(*ep)) { 
      fprintf(stderr,"Can't add \"%s\"\n",argv[i]);
      exit(-1);
    }
  }
  
  numpred=atoi(argv[managed-1]);

  mt = ParseModel(argc-managed,&(argv[managed]));
  
  if (!mt) {
    fprintf(stderr, "Can't parse model information\n");
    exit(-1);
  }

  model = FitThis((const double*)0,0,*mt);

  if (!model) {
    fprintf(stderr, "Can't fit model\n");
    exit(-1);
  }

  predictor= model->MakePredictor();

  if (!predictor) {
    fprintf(stderr, "Can't make predictor\n");
    exit(-1);
  }

  
  mirror.Run();

  return 0;
}

