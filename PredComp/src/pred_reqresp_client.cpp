#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "PredComp.h"
#include "Trace.h"

void usage(const char *n)
{
  char *b=GetRPSBanner();
  char *m=GetAvailableModels();

  fprintf(stdout, 
	  "Client for one-off predictions\n\n"
	  "usage: %s client file numahead conf|noconf MODEL\n\n"
	  "client          = client endpoint for server\n"
	  "file            = input datafile (one or two column)\n"
	  "conf|noconf     = whether to show confidence intervals\n"
	  "MODEL           = a model (see below)\n\n%s\n%s",n,m,b);
  delete [] b;
  delete [] m;
}


int main(int argc, char *argv[])
{
   char *infile;
   int conf;

   const int first_model=5;

   PredictionRequest req;
   PredictionResponse resp;


   if (argc<6) {
      usage(argv[0]);
      exit(-1);
   }

   EndPoint ep;
   
   if (ep.Parse(argv[1])) { 
     fprintf(stderr,"pred_reqresp_client: Can't parse \"%s\"\n",argv[1]);
     exit(-1);
   }
   if (ep.atype != EndPoint::EP_SOURCE || ep.ctype !=EndPoint::COMM_TCP) {
     fprintf(stderr,"pred_reqresp_client: Invalid netspec\n");
     exit(-1);
   }
     
   Reference<PredictionRequest,PredictionResponse> ref;

   infile=argv[2];
   req.numsteps=atoi(argv[3]);
   req.period_usec=1000000;  // hack
   conf = !strcmp(argv[4],"conf");


   ModelTemplate *mt = ParseModel(argc-first_model,&(argv[first_model]));
   if (mt==0) { 
     usage(argv[0]);
     exit(-1);
   }

   req.modelinfo = ModelInfo(*mt);

   double *temp;

   int numsamples=LoadGenericAsciiTraceFile(infile,&temp);

   req.Resize(numsamples);
   memcpy(req.series,temp,numsamples*sizeof(double));
   delete [] temp;


   double inputvar=Variance(req.series,req.serlen);
   double inputmean=Mean(req.series,req.serlen);
   
   fprintf(stderr,"pred_reqresp_client: sequence mean is %f and variance is %f\n",inputmean,inputvar);

   if (ref.ConnectTo(ep)) {
     fprintf(stderr,"pred_reqresp_client: Can't Connect to %s\n",argv[1]);
     exit(-1);
   }

   if (ref.Call(req,resp)) {
     fprintf(stderr,"pred_reqresp_client: Call failed\n");
   } else {
     resp.Print();
   }

   ref.Disconnect();
   return 0;
}
