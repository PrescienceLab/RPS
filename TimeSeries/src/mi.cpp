#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "it.h"
#include "util.h"

void usage() 
{
   fprintf(stderr,
      "mi [inputfile] [minlag:steplag:maxlag]\n");
}


void main(int argc, char *argv[])
{
   char *infile;

   FILE *inp;
   int numsamples;
   double junk;
   double *seq;
   int minlag, steplag, maxlag;

   int i,j;


   if (argc<3) {
      usage();
      exit(-1);
   }

   infile=argv[1];
   if (sscanf(argv[2],"%d:%d:%d",&minlag,&steplag,&maxlag)!=3) {
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

   double *mutinfo = new double [maxlag+1];

   double *vals;
   int *counts;
   int numuniq;
   
   CountUnique(seq,numsamples,&numuniq,&vals,&counts);
   
   delete [] vals;
   delete [] counts;

   fprintf(stdout,"# %d samples with %d unique values (%lf bits)\n"
	   "# Mutual Information Profile  in  [lag] [bits] format\n",
	   numsamples,numuniq,LOG2(numuniq));
   fflush(stdout);
   
   AverageMutualInformationLagged(seq,numsamples,mutinfo,
				  minlag,steplag,maxlag);

   for (i=minlag;i<=maxlag;i+=steplag) {
     fprintf(stdout,"%d\t%lf\n",i,mutinfo[(i-minlag)/steplag]);
   }
  
   
   delete [] seq;


}

