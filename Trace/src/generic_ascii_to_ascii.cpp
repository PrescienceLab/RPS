#include <stdlib.h>
#include <stdio.h>
#include "LoadTrace.h"
#include "banner.h"

void usage(const char *n)
{
  char *b=GetRPSBanner();
  fprintf(stdout,
	  "Convert from single column generic ascii format (values only) to\n"
	  "2 column ascii format (timestamp, value)\n\n"
	  "usage: %s ingenericasciitracefile outasciitracefile\n\n%s",n,b);
  delete [] b;
}



int main(int argc, char *argv[])
{
  char *infile;
  char *outfile;
  double *timestamps;
  double *vals;
  int numin, numout;

  if (argc!=3) {
    usage(argv[0]);
    exit(0);
  }

  infile=argv[1];
  outfile=argv[2];

  numin  = LoadGenericAsciiTraceFile(infile,&vals);
  timestamps = new double[numin];
  for (int i=0;i<numin;i++) { 
    timestamps[i]=i;
  }
  numout = StoreAsciiTraceFile(outfile,timestamps,vals,numin);

  fprintf(stderr,"%d samples in, %d samples out\n",numin,numout);

  delete [] timestamps;
  delete [] vals;
}
