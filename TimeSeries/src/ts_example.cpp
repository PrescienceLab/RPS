#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "tools.h"
#include "evaluate_core.h"
#include "random.h"
#include "util.h"
#include "fit.h"
#include "banner.h"

#include "Trace.h"

//
// SKIP TO "INTERESTING" to avoid all the extra glarp and see how this works
//
//




void usage(const char *n) 
{
  char *s=GetAvailableModels();
  char *b=GetRPSBanner();

  fprintf(stdout,
	  "Fit, predict, and evaluate example showing many features of TimeSeries\n\n"
	  "usage: %s inputfile fitfirst fitnum testnum numahead model\n\n"
	  "inputfile = 2 column (timestamp value) ascii file\n"
	  "fitfirst  = offset in rows in inputfile to first value to fit\n"
	  "fitnum    = number of rows in inputfile to which to fit\n"
	  "testnum   = number of rows beyond that to test\n"
	  "numahead  = number of steps ahead for each prediction\n"
          "model     = time series model to use (details below)\n\n"
	  "%s\n%s", n, s, b) ;
  delete [] b;
  delete [] s; 
}


  


int main(int argc, char *argv[])
{
   const int first_model=6;
   char *infile;
   int numahead;
   int fitfirst, fitnum, testnum;
   int numsamples;
   double *seq;
   double *predictions;
   double *variances;
   double *autocovs;
   double *sumvars;
   int i,j;


   // Models are fitted by Modellers - see "FitThis" above
   Model *model=0;
   // Models produce predictors, which contain prediction state
   Predictor *pred=0;

   // Evaluators evaluate predictions using a variety of statistics
   Evaluator eval;

   if (argc<first_model+1) {
      usage(argv[0]);
      exit(-1);
   }

   infile=argv[1];
   fitfirst = atoi(argv[2]);
   fitnum = atoi(argv[3]);
   testnum = atoi(argv[4]);
   numahead= atoi(argv[5]);

   predictions = new double [numahead];
   variances = new double [numahead*numahead];
   autocovs = new double [numahead];
   sumvars = new double [numahead];

   if (predictions==0 || variances==0 || autocovs==0 || sumvars==0) {
     fprintf(stderr,"Insufficient memory\n");
     exit(-1);
   }

   numsamples=LoadGenericAsciiTraceFile(infile,&seq);

   // INTERESTING 
   // THIS IS THE CORE
   // seq contains sequence, fitfirst is the first element to fit to
   // fitnum is the number of items to fit to
   // testnum is the number of items to test on
   // mclass,p,d,q define the model to be used.
   

   double fitvar=Variance(&(seq[fitfirst]),fitnum);
   double fitmean=Mean(&(seq[fitfirst]),fitnum);
   double testvar=Variance(&(seq[fitfirst+fitnum]),testnum);
   double testmean=Mean(&(seq[fitfirst+fitnum]),testnum);

   ComputeAutoCov(&(seq[fitfirst]),fitnum,autocovs,numahead);
   AutoCov2VarianceOfSum(autocovs,numahead,sumvars);

   fprintf(stderr,"training sequence mean is %f and variance is %f\n",fitmean,fitvar);
   fprintf(stderr,"testing  sequence mean is %f and variance is %f\n",testmean,testvar);

   CHK_DEL(model);
   
   // Attempt to fit the model

   ModelTemplate *mt = ParseModel(argc-first_model,&(argv[first_model]));

   if (mt==0) { 
     usage(argv[0]);
     exit(-1);
   }

   if (!getenv("RPS_NEWSTYLEOUTPUT")) { mt->Dump(stderr);} else { cerr<<*mt<<endl;}

   model=FitThis(&(seq[fitfirst]),fitnum,*mt);

   if (model==0) {
     fprintf(stderr,"model fit failed\n");
     exit(-1);
   }

   if (!getenv("RPS_NEWSTYLEOUTPUT")) { model->Dump(stderr); } else { cerr<<*model<<endl;}

   CHK_DEL(pred);

   // Make a predictor from the fitted model
   pred = model->MakePredictor();

   if (pred==0) {
     fprintf(stderr,"couldn't make predictor from model\n");
     exit(-1);
   }
   
   // prime predictor
   // Basically, we "step" the sequence that we fit to into 
   // the predictor so that it's internal state is up to date
   pred->Begin();
   for (i=fitfirst;i<fitfirst+fitnum-1;i++) {
     pred->Step(seq[i]);
   }

   if (!getenv("RPS_NEWSTYLEOUTPUT")) { pred->Dump(stderr); } else { cerr<<*pred<<endl;}


   // now we can ask the predictor what it thinks its prediction
   // errors will be:
   pred->ComputeVariances(numahead,variances,POINT_VARIANCES);
   fprintf(stderr,"Variances of individual estimates\n");
   for (i=0;i<numahead;i++) {
     fprintf(stderr,"+%d\t%f\n",i+1,variances[i]);
   }

   // We can also ask for the variances of the running sum
   pred->ComputeVariances(numahead,variances,SUM_VARIANCES);
   fprintf(stderr,"Variances of running sum of estimates, 95%% CIs for mean over preds and input signal\n");
   for (i=0;i<numahead;i++) {
     fprintf(stderr,"+%d\t%f\t%f\t%f\n",i+1,variances[i],2*1.96*sqrt(variances[i]/(i+1)),2*1.96*sqrt(sumvars[i]/(i+1)));
   }

   // We can also ask for the covariances
   pred->ComputeVariances(numahead,variances,CO_VARIANCES);
   fprintf(stderr,"Covariances of estimates\n");
   for (i=0;i<numahead;i++) {
     for (j=0;j<numahead;j++) { 
       fprintf(stderr,"\t%f",variances[i*numahead+j]);
     }
     fprintf(stderr,"\n");
   }

   // Here is how we would initialize an evaluator
   //
   if (eval.Initialize(numahead)) {
       fprintf(stderr,"Out of memory for evaluator\n");
       exit(-1);
   }


   // Now we can start working on predicting the test interval
   //
   for (i=fitfirst+fitnum-1;i<fitfirst+fitnum+testnum;i++) {
     // Step the new observation into the predictor - this
     // returns the current one step ahead prediction, but
     // we're just ignoring it here.
     pred->Step(seq[i]);
     // Ask for predictions from 1 to numahead steps into the future
     // given the state in the predictor at this point
     pred->Predict(numahead,predictions);
     // We could step the prediction to the evaluator here
     eval.Step(seq[i],predictions);
     fprintf(stdout,"%d\t%f",i,seq[i]);
     for (j=0;j<numahead;j++) {
       fprintf(stdout,"\t%f",predictions[j]);
     }
     fprintf(stdout,"\n");
   }

   // Here is how we would get the stats from the evaluator
   eval.Drain();
   PredictionStats *teststats = eval.GetStats();

   if (!getenv("RPS_NEWSTYLEOUTPUT")) { teststats->Dump(stderr); } else { cerr<<*teststats<<endl;}

   // Cleanup nicely
   CHK_DEL(model);
   CHK_DEL(pred);
   CHK_DEL_MAT(predictions);
   CHK_DEL_MAT(variances);
   CHK_DEL_MAT(seq);
   
   return 0;
}

