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
//#include <stropts.h>

#include "socks.h"
#include "EndPoint.h"
#include "Reference.h"
#include "EndPoint.h"
#include "PredictionRequestResponse.h"

#include "glarp.h"

#include "tools.h"

#include "fit.h"

void usage() 
{
   fprintf(stderr,
      "pred_reqresp_client netspec [inputfile] [numahead] [conf/noconf]\n"
      "                                                 MEAN\n"
      "                                               | LAST\n"
      "                                               | BM [p]\n"
      "                                               | AR [p]\n"
      "                                               | MA [q]\n"
      "                                               | ARMA [p] [q]\n"
      "                                               | ARIMA [p] [d] [q]\n"
      "                                               | ARFIMA [p] [d] [q]\n");
}


int main(int argc, char *argv[])
{
   char *infile;
   int conf;
   int i;

   FILE *inp;
   double junk;
   const int first_model=5;

   PredictionRequest req;
   PredictionResponse resp;


   if (argc<6) {
      usage();
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
     usage();
     exit(-1);
   }

   req.modelinfo = ModelInfo(*mt);

   inp = fopen(infile,"r");
   if (inp==0) {
     fprintf(stderr,"pred_reqresp_client: %s not found.\n",infile);
     exit(-1);
   }
   
   int numsamples=0;
   while ((fscanf(inp,"%lf %lf\n",&junk,&junk)==2)) {
    ++numsamples;
   }
   rewind(inp);

   req.Resize(numsamples);

   for (i=0;i<numsamples;i++) { 
     fscanf(inp,"%lf %lf\n",&junk,&(req.series[i]));
   }

   fclose(inp);

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
}
