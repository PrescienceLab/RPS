#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "it.h"

#include "Trace.h"

#include "banner.h"

void usage(const char *n) 
{
  char *b=GetRPSBanner();

  fprintf(stdout,
	  "Determination of Global False Nearest Neighbors\n"
	  "Dimensionality unfolding as per Abarbanel's method\n\n"
	  "usage: %s inputfile T mindim:stepdim:maxdim\n\n"
	  "inputfile  = 1 or 2 column format ascii trace\n"
	  "T          = spacing of samples from file\n"
	  "mindim     = minimum dimension to check\n"
	  "stepdim    = stepsize as dimensions are increased\n"
	  "maxdim     = maximum dimension to check\n\n%s\n",n,b);
  delete [] b;
}


int main(int argc, char *argv[])
{
   char *infile;

   int numsamples;
   double *seq;
   int mindim, stepdim, maxdim;
   int spacing;

   int i;


   if (argc<4) {
      usage(argv[0]);
      exit(-1);
   }

   infile=argv[1];
   spacing=atoi(argv[2]);
   if (sscanf(argv[3],"%d:%d:%d",&mindim,&stepdim,&maxdim)!=3) {
     usage(argv[0]);
     exit(-1);
   }

   numsamples=LoadGenericAsciiTraceFile(infile,&seq);

   double *fractfalse = new double [maxdim+1];

   fprintf(stdout,"# %d samples\n"
	   "# Global False Nearest Neighbors in [dim] [fraction_false] format\n",
	   numsamples);
   fflush(stdout);
   
   GlobalFalseNearestNeighbors(seq,numsamples,spacing,
			       fractfalse, mindim, stepdim, maxdim,
			       15.0);

   for (i=mindim;i<=maxdim;i+=stepdim) {
     fprintf(stdout,"%d\t%f\n",i,fractfalse[(i-mindim)/stepdim]);
   }
  
   
   delete [] seq;

   return 0;

}

