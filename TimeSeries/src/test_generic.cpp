#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "fit.h"
#include "tools.h"

void usage() 
{
   fprintf(stderr,
      "test_generic [inputfile] [numahead] [conf/noconf] [REFIT interval]\n"
      "                                                 MEAN\n"
      "                                               | LAST\n"
      "                                               | BESTMEAN [p]\n"
      "                                               | AR [p]\n"
      "                                               | MA [q]\n"
      "                                               | ARMA [p] [q]\n"
      "                                               | ARIMA [p] [d] [q]\n"
      "                                               | ARFIMA [p] [d] [q]\n");
}


int main(int argc, char *argv[])
{
   char *infile;
   int numahead, conf;

   FILE *inp;
   int numsamples;
   double junk;
   double *seq;
   double *predictions;
   double *variances;
   int first_model=4;
   int i,j;

   ModelTemplate *mt;
   Model *model;
   Predictor *pred;


   if (argc<5) {
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

   if ((mt=ParseModel(argc-first_model,&(argv[first_model])))==0) { 
     usage();
     exit(-1);
   }


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

