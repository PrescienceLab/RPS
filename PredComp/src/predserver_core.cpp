#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <signal.h>

#include "PredComp.h"
#include "predserver_core.h"

#define DEBUG 0

Model *FitThis(PredictionReconfigurationRequest &req)
{
  return FitThis(req.series,req.serlen,req.modelinfo);
}


Predictor *MakeUpToDatePredictor(Model &model, PredictionReconfigurationRequest &req)
{
  int i;

  Predictor *pred = model.MakePredictor();

  for (i=0;i<req.serlen;i++) {
    pred->Step(req.series[i]);
  }
  return pred;
}




void DieGracefully(int sig) 
{
  fprintf(stderr, "predserver_core caught signal %d and is dying gracefully.\n",sig);
  exit(-1);
}




//         stdin                            stdout
// PredictionReconfigurationRequest -> PredictionReconfigurationResponse
// Measurement                      -> PredictionResponse
// repeat
//
// This code will be forked off by predserver
// 
// 

int main(int argc, char *argv[])
{
  PredictionReconfigurationRequest curconfig;
  PredictionReconfigurationResponse curconfigresp;
  PredictionResponse curpredresp;
  PredServerCoreCommand curcmd;

  Model *model=0;
  Predictor *pred=0;
  Measurement measure;
  int i;

  //AttachDebuggerHere(argv[0]);

  fprintf(stderr,"predserver_core is starting up\n");
#if 1
#ifndef WIN32
  SetSignalHandler(SIGHUP,  &DieGracefully);
  SetSignalHandler(SIGINT,  &DieGracefully);
  SetSignalHandler(SIGQUIT, &DieGracefully);
  SetSignalHandler(SIGILL,  &DieGracefully);
  SetSignalHandler(SIGTRAP, &DieGracefully);
  SetSignalHandler(SIGABRT, &DieGracefully);
  SetSignalHandler(SIGBUS,  &DieGracefully);
  SetSignalHandler(SIGFPE,  &DieGracefully);
  SetSignalHandler(SIGUSR1, &DieGracefully);
  SetSignalHandler(SIGSEGV, &DieGracefully);
  SetSignalHandler(SIGUSR2, &DieGracefully);
  SetSignalHandler(SIGPIPE, &DieGracefully);
  SetSignalHandler(SIGALRM, &DieGracefully);
  SetSignalHandler(SIGTERM, &DieGracefully);
#endif
#endif

  bool firsttime=true;

  //BreakHere();
  while (true) {
    curcmd.Unserialize(0);
    if (firsttime) { 
      assert(curcmd.cmd == NEW_CONFIG);
      firsttime=false;
    }
    switch (curcmd.cmd) { 
    case NEW_MEASURE:
#if DEBUG
      fprintf(stderr,"COMMAND: NEW_MEASURE\n");
#endif
      // Incorporate the measurement
      measure.Unserialize(0);
#if DEBUG
      measure.Print(stderr);
#endif
      for (i=0;i<measure.serlen;i++) {
	pred->Step(measure.series[i]);
      }

      curpredresp.tag=measure.tag;
      curpredresp.flags=PREDFLAG_OK;
      curpredresp.datatimestamp = measure.timestamp;
      // It has already been appropriately sized and the other fields have been filled in
      
      pred->Predict(curpredresp.numsteps,curpredresp.preds);
      
      curpredresp.predtimestamp = TimeStamp();
      curpredresp.period_usec=measure.period_usec;
      curpredresp.Serialize(1);
      break;
    case NEW_CONFIG:
#if DEBUG
      fprintf(stderr,"COMMAND: NEW_CONFIG\n");
#endif
      curconfig.Unserialize(0);
#if DEBUG
      curconfig.Print(stderr);
#endif
      // Now refit the model and bring the Predictor up to date
      CHK_DEL(model);
      CHK_DEL(pred);
      model = FitThis(curconfig);
      pred = MakeUpToDatePredictor(*model,curconfig);

      // reconfigure the part of the predresponse that is static
      curpredresp.Resize(curconfig.numsteps);
      pred->ComputeVariances(curpredresp.numsteps,curpredresp.errs,SUM_VARIANCES);
      curpredresp.modelinfo = curconfig.modelinfo;
      curpredresp.period_usec = curconfig.period_usec;
      // Fire off a configuration response .. also sets timestamps
      curconfigresp.MakeMatchingResponse(curconfig);
      curconfigresp.Serialize(1);
      break;
    default:
      assert(0);
      break;
    }
  }

  CHK_DEL(model);
  CHK_DEL(pred);
}

