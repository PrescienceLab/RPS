#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "fit.h"
#include "tools.h"
#include "evaluate_core.h"
#include "random.h"
#include "util.h"

#include "Trace.h"

#include "banner.h"

//
// SKIP TO "INTERESTING" to avoid all the extra glarp and see how this works
//
//




void usage(const char *n) 
{
  char *s=GetAvailableModels();
  char *b=GetRPSBanner();

  fprintf(stdout,
	  "Output the impulse or step response of a model fitted to given data\n\n"
	  "usage: %s inputfile resplen model\n\n"
	  "inputfile = 1 or 2 column ascii trace file\n"
	  "resplen   = length of response to compute - prepend with 's' for step response\n"
	  "model     = model, as below\n\n%s\n%s\n",n,s,b);
  delete [] b;
  delete [] s; 
}







int main(int argc, char *argv[])
{
   const int first_model=3;
   char *infile;
   int numsamples;
   double *seq;
   int i;
   int resplen;
   bool stepresp=false;

   Model *model=0;
   Predictor *pred=0;

   if (argc<first_model+1) {
      usage(argv[0]);
      exit(-1);
   }

   infile=argv[1];
   if (!isdigit(argv[2][0])) { 
     stepresp=true;
     resplen = atoi(&(argv[2][1]));
   } else {
     stepresp=false;
     resplen = atoi(&(argv[2][0]));
   }

   ModelTemplate *mt = ParseModel(argc-first_model,&(argv[first_model]));
   if (mt==0) { 
     usage(argv[0]);
     exit(-1);
   }
 
   numsamples=LoadGenericAsciiTraceFile(infile,&seq);

   model=FitThis(seq,numsamples,*mt);

   if (model==0) {
     fprintf(stderr,"model fit failed\n");
     exit(-1);
   }

   //   model->Dump(stderr);

   // Make a predictor from the fitted model
   pred = model->MakePredictor();

   if (pred==0) {
     fprintf(stderr,"couldn't make predictor from model\n");
     exit(-1);
   }
   
   // step in tons of zeros
   for (i=0;i<resplen*10;i++) { 
     pred->Step(0.0);
   }

   //pred->Dump(stderr);

   //Now the impulse
   double input,output;
   for (i=0;i<resplen;i++) {
     if (stepresp) { 
       input = 1.0;
     } else {
       input = i==0 ? 1.0 : 0.0;
     }
     output = pred->Step(input);
     fprintf(stdout,"%d\t%f\t%f\n",i,input,output);
   }
   //pred->Dump(stderr);
   // Cleanup nicely
   CHK_DEL(model);
   CHK_DEL(pred);
   CHK_DEL_MAT(seq);

   return 0;
   
}

