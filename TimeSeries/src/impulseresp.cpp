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
//
// SKIP TO "INTERESTING" to avoid all the extra glarp and see how this works
//
//




void usage() 
{
   fprintf(stderr,
      "impulseresp [inputfile] [[s]resplen] [REFIT refitint]\n"
      "                                  MEAN\n"
      "                                | LAST\n"
      "                                | BM [p]\n"
      "                                | AR [p]\n"
      "                                | MA [q]\n"
      "                                | ARMA [p] [q]\n"
      "                                | ARIMA [p] [d] [q]\n"
      "                                | ARFIMA [p] [d] [q]\n");
}





int main(int argc, char *argv[])
{
   const int first_model=3;
   char *infile;
   FILE *inp;
   int numsamples;
   double junk;
   double *seq;
   int i;
   int resplen;
   bool stepresp=false;

   Model *model=0;
   Predictor *pred=0;

   if (argc<first_model+1) {
      usage();
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

