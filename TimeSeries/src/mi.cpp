#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "it.h"
#include "util.h"

#include "Trace.h"

#include "banner.h"


void usage(const char *n) 
{
  char *b=GetRPSBanner();

  fprintf(stdout,
	  "Mutual information profile of a sequence\n\n"
	  "usage: %s inputfile minlag:steplag:maxlag\n\n"
	  "inputfile = 1 or 2 column ascii trace file\n"
	  "minlag    = smallest lag in range to test\n"
	  "steplag   = lag step of range\n"
	  "maxlag    = largest lag in range to test\n\n%s\n",n,b);
  delete [] b;
}


int main(int argc, char *argv[])
{
   char *infile;

   int numsamples;
   double *seq;
   int minlag, steplag, maxlag;

   int i;


   if (argc<3) {
      usage(argv[0]);
      exit(-1);
   }

   infile=argv[1];
   if (sscanf(argv[2],"%d:%d:%d",&minlag,&steplag,&maxlag)!=3) {
     usage(argv[0]);
     exit(-1);
   }


   numsamples=LoadGenericAsciiTraceFile(infile,&seq);

   double *mutinfo = new double [maxlag+1];

   double *vals;
   int *counts;
   int numuniq;
   
   CountUnique(seq,numsamples,&numuniq,&vals,&counts);
   
   delete [] vals;
   delete [] counts;

   fprintf(stdout,"# %d samples with %d unique values (%f bits)\n"
	   "# Mutual Information Profile  in  [lag] [bits] format\n",
	   numsamples,numuniq,LOG2(numuniq));
   fflush(stdout);
   
   AverageMutualInformationLagged(seq,numsamples,mutinfo,
				  minlag,steplag,maxlag);

   for (i=minlag;i<=maxlag;i+=steplag) {
     fprintf(stdout,"%d\t%f\n",i,mutinfo[(i-minlag)/steplag]);
   }
  
   
   delete [] seq;

   return 0;
}

