#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
//#include <stropts.h>
#include <sys/select.h>
#include <signal.h>
#include <math.h>



#include "Mirror.h"
#include "EndPoint.h"
#include "Measurement.h"
#include "PredictionRequestResponse.h"

#include "glarp.h"


void usage()
{
  fprintf(stderr,"usage: predserver numsteps sourcenetspec destnetspec+\n");
}


// ICK
int  numsteps;
ModelInfo modelinfo;
//Model *model;
//Predictor *pred;

// Generic right now
class Prediction {
public:
  static int Compute(Measurement &measure, PredictionResponse &pred) {
    pred.tag=measure.tag;
    pred.flags=0;
    pred.datatimestamp = measure.timestamp;
    pred.predtimestamp=TimeStamp();
    pred.modelinfo=modelinfo;
    pred.period_usec=measure.period_usec;
    pred.Resize(numsteps);
    int i;
    for (i=0;i<numsteps;i++) {
      pred.preds[i] = measure.series[0];
      pred.errs[i] = -1.0;
    }
    pred.numsteps=numsteps;
    return 0;
  }
};

typedef  GenericSerializeableInputComputeOutputMirror<
                Measurement,
                Prediction,
                PredictionResponse> PredictionMirror;



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

  if (argc<4) {
    usage();
    exit(0);
  }

  numsteps = atoi(argv[1]);
  modelinfo = ModelInfo();

  PredictionMirror mirror;
  // PredictionSink sink;

  for (i=2;i<argc;i++) { 
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
  fprintf(stderr,"predserver running.\n");
  mirror.Run();
}

