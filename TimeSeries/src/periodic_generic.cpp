#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "ar.h"
#include "ma.h"
#include "arma.h"
#include "arima.h"
#include "arfima.h"
#include "tools.h"

enum ModelType {AR, MA, ARMA, ARIMA, ARFIMA};



Model *FitThis(ModelType mclass,
	       double *seq, int numsamples, 
	       int p, double d, int q)
{
   switch (mclass) {
   case AR:
     return ARModeler::Fit(seq,numsamples,p);
     break;
   case MA:
     return MAModeler::Fit(seq,numsamples,q);
     break;
   case ARMA:
     return ARMAModeler::Fit(seq,numsamples,p,q);
     break;
   case ARIMA:
     return ARIMAModeler::Fit(seq,numsamples,p,(int)d,q);
     break;
   case ARFIMA:
     return ARFIMAModeler::Fit(seq,numsamples,p,d,q);
     break;
   }
}


void usage() 
{
   fprintf(stderr,
      "periodic_generic [inputfile] [numahead] [conf/noconf] [refit_interval] [fit_samples]\n"
      "                                                  AR [p]\n"
      "                                               | MA [q]\n"
      "                                               | ARMA [p] [q]\n"
      "                                               | ARIMA [p] [d] [q]\n"
      "                                               | ARFIMA [p] [d] [q]\n");
}



void main(int argc, char *argv[])
{
  const int first_model=6;
   char *infile;
   ModelType mclass;
   int numahead, conf;
   int refit_interval, fit_samples;
   int p, q;
   double d;

   FILE *inp;
   int numsamples;
   double junk;
   double *seq;
   double *predictions;
   double *variances;

   int i,j;

   Model *model=0;
   Predictor *pred=0;


   if (argc<8) {
      usage();
      exit(-1);
   }

   infile=argv[1];
   numahead=atoi(argv[2]);
   conf = !strcmp(argv[3],"conf");
   refit_interval=atoi(argv[4]);
   fit_samples=atoi(argv[5]);

   predictions = new double [numahead];
   variances = new double [numahead];

   if (predictions==0 || variances==0) {
     fprintf(stderr,"Insufficient memory\n");
     exit(-1);
   }

   if (!strcmp(argv[first_model],"AR")) {
      if (argc!=first_model+2) {
         usage();
         exit(-1);
      }
      p=atoi(argv[first_model+1]);
      mclass=AR;
      goto run;
   }
   if (!strcmp(argv[first_model],"MA")) {
      if (argc!=first_model+2) {
         usage();
         exit(-1);
      }
      q=atoi(argv[first_model+1]);
      mclass=MA;
      goto run;
   }
   if (!strcmp(argv[first_model],"ARMA")) {
      if (argc!=first_model+3) {
         usage();
         exit(-1);
      }
      p=atoi(argv[first_model+1]);
      q=atoi(argv[first_model+2]);
      mclass=ARMA;
      goto run;
   }
   if (!strcmp(argv[first_model],"ARIMA")) {
      if (argc!=10) {
         usage();
         exit(-1);
      }
      p=atoi(argv[first_model+1]);
      d=atof(argv[first_model+2]);
      q=atoi(argv[first_model+3]);
      mclass=ARIMA;
      goto run;
   }
   if (!strcmp(argv[first_model],"ARFIMA")) {
      if (argc!=10) {
         usage();
         exit(-1);
      }
      p=atoi(argv[first_model+1]);
      d=atof(argv[first_model+2]);
      q=atoi(argv[first_model+3]);
      mclass=ARFIMA;
      goto run;
   }

run:
 
   inp = fopen(infile,"r");
   if (inp==0) {
      fprintf(stderr,"%s not found.\n",infile);
      exit(-1);
   }

  numsamples=0;
  while ((fscanf(inp,"%lf %lf\n",&junk,&junk)==2)) {
    ++numsamples;
  }
  rewind(inp);

  seq = new double [numsamples];
  if (seq==0) {
     fprintf(stderr,"insufficient memory to read %s\n",infile);
     exit(-1);
  }


   for (i=0;i<numsamples;i++) { 
       fscanf(inp,"%lf %lf\n",&junk,&(seq[i]));
   }

   fclose(inp);

   int firstsample, samplesused;
   for (i=refit_interval;i<numsamples;i++) {
     if (i%refit_interval==0) {
       // refit the model
       int availsamples = MIN(i,fit_samples);
       double inputvar=Variance(&(seq[i-availsamples]),availsamples);
       double inputmean=Mean(&(seq[i-availsamples]),availsamples);	
       fprintf(stderr,"sequence mean is %lf and variance is %lf\n",inputmean,inputvar);
       CHK_DEL(model);
       firstsample=i-availsamples; samplesused=availsamples;
       model=FitThis(mclass,&(seq[i-availsamples]),availsamples,p,d,q);
       if (model==0) {
	 fprintf(stderr,"model fit failed\n");
	 exit(-1);
       }
       model->Dump(stderr);
       CHK_DEL(pred);
       pred = model->MakePredictor();
       if (pred==0) {
	 fprintf(stderr,"couldn't make predictor from model\n");
	 exit(-1);
       }
       // Now prime the model
       pred->Begin();
       for (j=i-availsamples;j<i;j++) {
	 pred->Step(seq[j]);
       }
       pred->Dump(stderr);

   // for the linear models, the variances will not change, so we will
   // compute them here, out of loop
       pred->ComputeVariances(numahead,variances);
       fprintf(stderr,"Lead\tVariance\tImprove\n");
       for (j=0;j<numahead;j++) {
	 fprintf(stderr,"+%d\t%lf\t%lf%%\n",j+1,variances[j],
		 100.0*(inputvar-variances[j])/inputvar);
       }
     }
     pred->Step(seq[i]);
     pred->Predict(numahead,predictions);
     //fprintf(stderr,"Used samples %d to %d in model fitted to %d to %d to predict sample %d to %d\n",
     //     firstsample,i,firstsample,firstsample+samplesused-1,i+1,i+1+numahead);
	     
     fprintf(stdout,"%lf",seq[i]);
     for (j=0;j<numahead;j++) {
       fprintf(stdout," %lf",predictions[j]);
       if (conf) {
	 fprintf(stdout," %lf",1.96*sqrt(variances[j]));  // 95% conf
       }
     }
     fprintf(stdout,"\n");
   }
}

