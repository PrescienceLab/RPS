#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "fit.h"
#include "bestmean.h"
#include "tools.h"
#include "evaluate_core.h"
#include "random.h"
#include "util.h"


void usage() 
{
   fprintf(stderr,
      "crossval_generic [inputfile] [tag] [numahead] [minfitint] [maxfitint] [mintestint] [maxtestint] [numpasses] [bmlimit] [REFIT interval] \n"
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
  const int first_model=10;
   char *infile;
   int numahead;
   int bmlimit;
   int minfitint, maxfitint, mintestint, maxtestint, numint;

   FILE *inp;
   int numsamples;
   double junk;
   double *seq;
   double *predictions;
   double *variances;
   double *bmpredictions;
   double *bmvariances;

   int i;
   int p,d,q;
   

   Model *model=0;
   Model *bmmodel=0;
   Predictor *pred=0;
   Predictor *bmpred=0;
   Evaluator eval;
   Evaluator bmeval;

   if (argc<first_model+1) {
      usage();
      exit(-1);
   }

   infile=argv[1];
   char *tag=argv[2];
   numahead=atoi(argv[3]);
   minfitint = atoi(argv[4]);
   maxfitint = atoi(argv[5]);
   mintestint = atoi(argv[6]);
   maxtestint = atoi(argv[7]);
   numint = atoi(argv[8]);
   bmlimit = atoi(argv[9]);

   predictions = new double [numahead];
   bmpredictions = new double [numahead];
   variances = new double [numahead];
   bmvariances = new double [numahead];

   if (predictions==0 || variances==0 || bmpredictions==0 || bmvariances==0) {
     fprintf(stderr,"Insufficient memory\n");
     exit(-1);
   }

   ModelTemplate *mt = ParseModel(argc-first_model,&(argv[first_model]));
 
   ((PDQParameterSet*)(mt->ps))->Get(p,d,q);

   if (mt==0) { 
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

   int step, fitfirst,fitnum,testfirst,testnum;

   double *maxerr = new double [numahead];
   double *minerr = new double [numahead];
   double *msqerr = new double [numahead];
   double *meanabserr= new double [numahead];
   double *meanerr = new double [numahead];
   double *bmmaxerr = new double [numahead];
   double *bmminerr = new double [numahead];
   double *bmmsqerr = new double [numahead];
   double *bmmeanabserr= new double [numahead];
   double *bmmeanerr = new double [numahead];

   double *meantestvar = new double [numahead];
   double *meanmsqerr = new double [numahead];
   double *meanbmmsqerr = new double [numahead];
   double *meanimproveoversig = new double [numahead];
   double *meanimproveoverbm = new double [numahead];

   InitRandom();

   for (i=0;i<numahead;i++) {
     meantestvar[i]=0;
     meanmsqerr[i]=0;
     meanbmmsqerr[i]=0;
   }

   if (maxerr==0 || minerr==0 || msqerr==0 || meanabserr==0 || meanerr==0 || 
       bmmaxerr==0 || bmminerr==0 || bmmsqerr==0 || bmmeanabserr==0 || bmmeanerr==0) {
     fprintf(stderr,"insufficient memory\n");
     exit(-1);
   }
   

   printf("#%s %s %s %s %s %s %s %s %s %s %s %s\n",
	  "tag",
	  "testcase",
	  "lead",
	  "fitnum",
	  "testnum",
	  "p",
	  "d",
	  "q",
	  "bmlimit",
	  "testvar",
	  "msqerr",
	  "bmmsqerr");

   for (step=0;step<numint;step++) {
     // Choose random consecutive
     do {
       testnum = mintestint + rand()%(maxtestint-mintestint+1);
       testfirst=rand()%(numsamples-testnum);
       fitnum=minfitint + rand()%(maxfitint-minfitint+1);
       fitfirst=testfirst-fitnum;
     } while (fitfirst<0 || testfirst<0);
     
     fprintf(stderr,"Fit to %d..%d and test on %d..%d\n",
	     fitfirst,fitfirst+fitnum-1,testfirst,testfirst+testnum-1);

     double fitvar=Variance(&(seq[fitfirst]),fitnum);
     double fitmean=Mean(&(seq[fitfirst]),fitnum);
     double testvar=Variance(&(seq[testfirst]),testnum);
     double testmean=Mean(&(seq[testfirst]),testnum);

     fprintf(stderr,"training sequence mean is %f and variance is %f\n",fitmean,fitvar);
     fprintf(stderr,"testing  sequence mean is %f and variance is %f\n",testmean,testvar);

     CHK_DEL(model);
     model=FitThis(&(seq[fitfirst]),fitnum,*mt);

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
     
     // prime predictor
     pred->Begin();
     for (i=fitfirst;i<fitfirst+fitnum;i++) {
       pred->Step(seq[i]);
     }

     pred->Dump(stderr);


     bmmodel=FitThis(BESTMEAN,&(seq[fitfirst]),fitnum,bmlimit,0,0);

     if (bmmodel==0) {
       fprintf(stderr,"bm model fit failed\n");
       exit(-1);
     }


     bmmodel->Dump(stderr);

     CHK_DEL(bmpred);
     bmpred = bmmodel->MakePredictor();

     if (bmpred==0) {
       fprintf(stderr,"couldn't make bm predictor from model\n");
       exit(-1);
     }

     // prime predictor
     bmpred->Begin();
     for (i=fitfirst;i<fitfirst+fitnum;i++) {
       bmpred->Step(seq[i]);
     }

     bmpred->Dump(stderr);

     // for the linear models, the variances will not change, so we will
     // compute them here, out of loop
     pred->ComputeVariances(numahead,variances);
     bmpred->ComputeVariances(numahead,bmvariances);
     fprintf(stderr,"Lead\tVariance\tImprove\tBMVar\tBMImprove\n");
     for (i=0;i<numahead;i++) {
       fprintf(stderr,"+%d\t%f\t%f%%\t%f\t%f%%\n",i+1,variances[i],
	       100.0*(fitvar-variances[i])/fitvar,
	       bmvariances[i],
	       100.0*(fitvar-bmvariances[i])/fitvar);
     }

     if (eval.Initialize(numahead)) {
       fprintf(stderr,"Out of memory for evaluator\n");
       exit(-1);
     }
     if (bmeval.Initialize(numahead)) {
       fprintf(stderr,"Out of memory for bm evaluator\n");
       exit(-1);
     }
     for (i=testfirst;i<testfirst+testnum;i++) {
       pred->Step(seq[i]);
       pred->Predict(numahead,predictions);
       eval.Step(seq[i],predictions);
       bmpred->Step(seq[i]);
       bmpred->Predict(numahead,bmpredictions);
       bmeval.Step(seq[i],bmpredictions);
     }
     eval.Drain();
     PredictionStats *teststats = eval.GetStats();
     bmeval.Drain();
     PredictionStats *bmstats = bmeval.GetStats();
     for (i=0;i<numahead;i++) {
       printf("%s %d +%d %d %d %d %d %d %d %f %f %f\n",
	      tag,
	      step,
	      i+1,
	      fitnum,
	      testnum,
	      p,
	      d,
	      q,
	      bmlimit,
	      testvar,
	      teststats->GetMeanSquaredError(i+1),
	      bmstats->GetMeanSquaredError(i+1));
       /*       printf("eval:\t+%d\t%f\t%f\t%f%%\t%f\t%f%%\t%f%%\n", 
	      i+1, 
	      testvar, 
	      msqerr[i],
	      testvar==0 ? 0 : 100.0*(testvar-msqerr[i])/testvar,
	      bmmsqerr[i],
	      testvar==0 ? 0 : 100.0*(testvar-bmmsqerr[i])/testvar,
	      100.0*(bmmsqerr[i]-msqerr[i])/bmmsqerr[i]);
	      */
       meantestvar[i] += testvar;
       meanmsqerr[i] += teststats->GetMeanSquaredError(i+1);
       meanbmmsqerr[i] += bmstats->GetMeanSquaredError(i+1);
       meanimproveoversig[i] += (testvar-teststats->GetMeanSquaredError(i+1));
       meanimproveoverbm[i] +=
	 (bmstats->GetMeanSquaredError(i+1)-teststats->GetMeanSquaredError(i+1));

     }
     delete bmstats;
     delete teststats;
     /*     printf("Predictor\tMinErr\tMaxErr\tMeanAbsErr\tMeanSquareError\tMeanErr\n");
     for (i=0;i<numahead;i++) { 
       printf(" t+%-3u\t%10.3le\t%10.3le\t%10.3le\t%10.3le\t%10.3le\n",
	      i+1,minerr[i],maxerr[i],meanabserr[i],msqerr[i],meanerr[i]);
     }
     */
	      
   }
#if 0
   printf("lead\tmeansigvar\tmeanmsg\tmeanbmmsq\tmeanimproveoversig\tmeanimproveoverbm\n");
   for (i=0;i<numahead;i++) {
     meantestvar[i]/=numint;
     meanmsqerr[i]/=numint;
     meanbmmsqerr[i]/=numint;
     meanimproveoversig[i]/=numint;
     meanimproveoverbm[i]/=numint;
     printf("+%d\t%f\t%f\t%f\t%f(%f%%)\t%f(%f%%)\n",
	    i+1,
	    meantestvar[i],
	    meanmsqerr[i],
	    meanbmmsqerr[i],
	    meanimproveoversig[i],
	    100.0*meanimproveoversig[i]/meantestvar[i],
	    meanimproveoverbm[i],
	    100.0*meanimproveoverbm[i]/meanbmmsqerr[i]);
     }
#endif
}

