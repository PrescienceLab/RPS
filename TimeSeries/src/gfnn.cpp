#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "it.h"


void usage() 
{
   fprintf(stderr,
      "gfnn [inputfile] [T] [mindim:stepdim:maxdim]\n");
}


int main(int argc, char *argv[])
{
   char *infile;

   FILE *inp;
   int numsamples;
   double junk;
   double *seq;
   int mindim, stepdim, maxdim;
   int spacing;

   int i;


   if (argc<4) {
      usage();
      exit(-1);
   }

   infile=argv[1];
   spacing=atoi(argv[2]);
   if (sscanf(argv[3],"%d:%d:%d",&mindim,&stepdim,&maxdim)!=3) {
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

