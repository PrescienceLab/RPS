#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <math.h>


#include "PredComp.h"

void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Server for one-off predictions\n\n"
	  "usage: %s server+\n\n"
	  "server          = one or more server endpoints\n"
	  "\n%s",n,b);
  delete [] b;
}



// Generic right now
class PredictOnce {
public:
  static int Compute(const PredictionRequest &req, PredictionResponse &pred) {
    pred.tag = req.tag;
    pred.datatimestamp = req.datatimestamp;
    struct timeval tv;
    gettimeofday(&tv,0);
    pred.predtimestamp=TimeStamp(&tv);
    pred.modelinfo=req.modelinfo;
    pred.period_usec=req.period_usec;
    pred.numsteps = req.numsteps;
    pred.Resize(pred.numsteps);

    Model *model;
    Predictor *predictor;
    
    model = FitThis(req.series,req.serlen,req.modelinfo);

    if (!model) {
      pred.flags|=PREDFLAG_FAIL;
      goto done;
    }

    model->Dump(stderr);
    
    predictor = model->MakePredictor();
    
    if (!predictor) {
      pred.flags|=PREDFLAG_FAIL;
      delete model;
      goto done;
    }

    predictor->ComputeVariances(pred.numsteps,pred.errs);


    int i;

    predictor->Begin();
    for (i=0;i<req.serlen;i++) {
      predictor->Step(req.series[i]);
    }
    predictor->Predict(pred.numsteps,pred.preds);
    delete predictor;
    delete model;
    
    pred.flags|=PREDFLAG_OK;
  done:
    return 0;
  }
};

typedef  SerializeableRequestResponseMirror<PredictionRequest,PredictOnce,PredictionResponse> PredictOnceEngine;



class PredictionSink : public LocalTarget {
public:
  void ProcessData(Buffer &buf) {
    PredictionResponse p;
    p.Unserialize(buf);
    p.Print();
  }
};



int main(int argc, char *argv[]) 
{
  int i;
  EndPoint *ep;

  if (argc<2) {
    usage(argv[0]);
    exit(0);
  }


  PredictOnceEngine pred;
  PredictionSink sink;

  //pred.AddLocalTarget(&sink);

  for (i=1;i<argc;i++) { 
    ep = new EndPoint;
    if (ep->Parse(argv[i])) { 
      fprintf(stderr,"Can't parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    if (pred.AddEndPoint(*ep)) { 
      fprintf(stderr,"Can't add \"%s\"\n",argv[i]);
      exit(-1);
    }
  }

  pred.Run();
  return 0;
}

