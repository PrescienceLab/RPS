#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <math.h>

#include "fit.h"
#include "bestmean.h"
#include "ar.h"
#include "ma.h"
#include "arma.h"
#include "arima.h"
#include "arfima.h"
#include "tools.h"

#include "Mirror.h"
#include "glarp.h"
#include "debug.h"
#include "EndPoint.h"
#include "PredictionRequestResponse.h"


void usage()
{
  fprintf(stderr,"usage: pred_reqresp_server [targetspec+ | connectspec+]\n");
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
    int p, di, q;
    double d;
    
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
    usage();
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
}

