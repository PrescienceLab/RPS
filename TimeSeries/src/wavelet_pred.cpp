#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "tools.h"
#include "evaluate_core.h"
#include "random.h"
#include "util.h"
#include "fit.h"


//
// SKIP TO "INTERESTING" to avoid all the extra glarp and see how this works
//
//




void usage() 
{
   fprintf(stderr,
      "wavelet_pred [inputfile]  [REFIT interval] \n"
      "                                                 MEAN\n"
      "                                               | LAST\n"
      "                                               | AR [p]\n"
      "                                               | MA [q]\n"
      "                                               | ARMA [p] [q]\n"
      "                                               | ARIMA [p] [d] [q]\n"
      "                                               | ARFIMA [p] [d] [q]\n"
      "                                               | BM [p] \n"   );
}



  

//
// This is non-functional code
// 

int main(int argc, char *argv[])
{
   const int first_model=2;
   char *infile;
   int numahead;
   int fitfirst, fitnum, testnum;
   FILE *inp;
   int numsamples;
   double junk;
   double *seq;
   int i,j;


   // Models are fitted by Modellers - see "FitThis" above
   Model *model=0;
   // Models produce predictors, which contain prediction state
   Predictor *pred=0;

   // Evaluators evaluate predictions using a variety of statistics
   Evaluator eval;

   if (argc<first_model+1) {
      usage();
      exit(-1);
   }

   infile=argv[1];
   numahead=1;


   // Load the data
   // file format is rows of doubles

   inp = fopen(infile,"r");
   if (inp==0) {
     fprintf(stderr,"%s not found.\n",infile);
     exit(-1);
   }
   
   numsamples=0;
   while ((fscanf(inp,"%lf\n",&junk)==1)) {
     ++numsamples;
   }
   rewind(inp);
   
   seq = new double [numsamples];
   if (seq==0) {
     fprintf(stderr,"insufficient memory to read %s\n",infile);
     exit(-1);
   }


   for (i=0;i<numsamples;i++) { 
     fscanf(inp,"%lf\n",&(seq[i]));
   }

   fclose(inp);

   fitfirst = 0;
   fitnum = numsamples/2;
   testnum = fitnum+(numsamples%2);

   ModelTemplate *mt = ParseModel(argc-first_model,&(argv[first_model]));

   if (mt==0) { 
     usage();
     exit(-1);
   }

   model=FitThis(&(seq[fitfirst]),fitnum,*mt);

   if (model==0) {
     fprintf(stderr,"model fit failed\n");
     exit(-1);
   }

   model->Dump(stderr);

   // Make a predictor from the fitted model
   pred = model->MakePredictor();

   if (pred==0) {
     fprintf(stderr,"couldn't make predictor from model\n");
     exit(-1);
   }
   
   pred->Begin();

   pred->Dump(stderr);


   double curpred=0;
   double nextpred=0;

   for (i=0;i<numsamples;i++) {
     // Step the new observation into the predictor - this
     // returns the current one step ahead prediction, but
     // we're just ignoring it here.
     nextpred=pred->Step(seq[i]);
     fprintf(stdout,"%lf\t%lf\n",seq[i],curpred);
     curpred=nextpred;
   }


   // Cleanup nicely
   CHK_DEL(model);
   CHK_DEL(pred);
   CHK_DEL_MAT(seq);
   
   return 0;
}

