#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "fit.h"
#include "tools.h"

#include "banner.h"

#include "Trace.h"

void usage(const char *n) 
{
  char *s=GetAvailableModels();
  char *b=GetRPSBanner();
  fprintf(stdout,
	  "Fit a model to data and show how good the fit is\n\n"
	  "usage: %s inputfile numahead conf|noconf model\n\n"
	  "inputfile   = 1 or 2 column ascii input file\n"
	  "numahead    = steps into the \"future\" to predict\n"
	  "conf|noconf = show 95%% confidence intervals or not\n"
	  "model       = predictive model (see below)\n\n%s\n%s\n",n,s,b);
  delete [] s;
  delete [] b;
}


int main(int argc, char *argv[])
{
   char *infile;
   int numahead, conf;

   int numsamples;
   double *seq;
   double *predictions;
   double *variances;
   int first_model=4;
   int i,j;

   ModelTemplate *mt;
   Model *model;
   Predictor *pred;


   if (argc<5) {
      usage(argv[0]);
      exit(-1);
   }

   infile=argv[1];
   numahead=atoi(argv[2]);
   conf = !strcmp(argv[3],"conf");

   predictions = new double [numahead];
   variances = new double [numahead];

   if (predictions==0 || variances==0) {
     fprintf(stderr,"Insufficient memory\n");
     exit(-1);
   }

   if ((mt=ParseModel(argc-first_model,&(argv[first_model])))==0) { 
     usage(argv[0]);
     exit(-1);
   }


   numsamples=LoadGenericAsciiTraceFile(infile,&seq);

   double inputvar=Variance(seq,numsamples);
   double inputmean=Mean(seq,numsamples);
   
   fprintf(stderr,"sequence mean is %f and variance is %f\n",inputmean,inputvar);

   model=FitThis(seq,numsamples,*mt);

   if (model==0) {
     fprintf(stderr,"model fit failed\n");
     exit(-1);
   }

   model->Dump(stderr);

   pred = model->MakePredictor();

   if (pred==0) {
     fprintf(stderr,"couldn't make predictor from model\n");
     exit(-1);
   }

   pred->Dump(stderr);

   // for the linear models, the variances will not change, so we will
   // compute them here, out of loop
   pred->ComputeVariances(numahead,variances);
   fprintf(stderr,"Lead\tVariance\tImprove\n");
   for (i=0;i<numahead;i++) {
     fprintf(stderr,"+%d\t%f\t%f%%\n",i+1,variances[i],
	     100.0*(inputvar-variances[i])/inputvar);
   }

   pred->Begin();

   for (i=0;i<numsamples;i++) {
     pred->Step(seq[i]);
     pred->Predict(numahead,predictions);
     fprintf(stdout,"%f",seq[i]);
     for (j=0;j<numahead;j++) {
       fprintf(stdout," %f",predictions[j]);
       if (conf) {
         fprintf(stdout," %f",1.96*sqrt(variances[j]));  // 95% conf
       }
     }
     fprintf(stdout,"\n");
   }
   return 0;
}

