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

void usage() 
{
   fprintf(stderr,
      "test_generic [inputfile] [numahead] [conf/noconf]\n"
      "                                                  AR [p]\n"
      "                                               | MA [q]\n"
      "                                               | ARMA [p] [q]\n"
      "                                               | ARIMA [p] [d] [q]\n"
      "                                               | ARFIMA [p] [d] [q]\n");
}


void main(int argc, char *argv[])
{
   char *infile;
   enum {AR, MA, ARMA, ARIMA, ARFIMA} mclass;
   int numahead, conf;
   int p, q;
   double d;

   FILE *inp;
   int numsamples;
   double junk;
   double *seq;
   double *predictions;
   double *variances;

   int i,j;

   Model *model;
   Predictor *pred;


   if (argc<6) {
      usage();
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

   if (!strcmp(argv[4],"AR")) {
      if (argc!=6) {
         usage();
         exit(-1);
      }
      p=atoi(argv[5]);
      mclass=AR;
      goto run;
   }
   if (!strcmp(argv[4],"MA")) {
      if (argc!=6) {
         usage();
         exit(-1);
      }
      q=atoi(argv[5]);
      mclass=MA;
      goto run;
   }
   if (!strcmp(argv[4],"ARMA")) {
      if (argc!=7) {
         usage();
         exit(-1);
      }
      p=atoi(argv[5]);
      q=atoi(argv[6]);
      mclass=ARMA;
      goto run;
   }
   if (!strcmp(argv[4],"ARIMA")) {
      if (argc!=8) {
         usage();
         exit(-1);
      }
      p=atoi(argv[5]);
      d=atof(argv[6]);
      q=atoi(argv[7]);
      mclass=ARIMA;
      goto run;
   }
   if (!strcmp(argv[4],"ARFIMA")) {
      if (argc!=8) {
         usage();
         exit(-1);
      }
      p=atoi(argv[5]);
      d=atof(argv[6]);
      q=atoi(argv[7]);
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
  while ((fscanf(inp,"%f %f\n",&junk,&junk)==2)) {
    ++numsamples;
  }
  rewind(inp);

  seq = new double [numsamples];
  if (seq==0) {
     fprintf(stderr,"insufficient memory to read %s\n",infile);
     exit(-1);
  }


   for (i=0;i<numsamples;i++) { 
       fscanf(inp,"%f %f\n",&junk,&(seq[i]));
   }

   fclose(inp);

   double inputvar=Variance(seq,numsamples);
   double inputmean=Mean(seq,numsamples);
   
   fprintf(stderr,"sequence mean is %f and variance is %f\n",inputmean,inputvar);

   switch (mclass) {
   case AR:
     model=ARModeler::Fit(seq,numsamples,p);
     break;
   case MA:
     model=MAModeler::Fit(seq,numsamples,q);
     break;
   case ARMA:
     model=ARMAModeler::Fit(seq,numsamples,p,q);
     break;
   case ARIMA:
     model=ARIMAModeler::Fit(seq,numsamples,p,(int)d,q);
     break;
   case ARFIMA:
     model=ARFIMAModeler::Fit(seq,numsamples,p,d,q);
     break;
   }

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
}

