#include <stdlib.h>
#include <stdio.h>
#include "LoadTrace.h"
#include "banner.h"

void usage(const char *name)
{
  char *b=GetRPSBanner();
  fprintf(stdout,
	  "Convert from DEC alpha binary format trace file to\n"
	  "2 column ascii format (timestamp, value)\n\n"
	  "usage: %s inalphabinarytracefile outasciitracefile\n\n%s",name,b);
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

  numin  = LoadAlphaBinaryTraceFile(infile,&timestamps,&vals);
  numout = StoreAsciiTraceFile(outfile,timestamps,vals,numin);

  fprintf(stderr,"%d samples in, %d samples out\n",numin,numout);

  delete [] timestamps;
  delete [] vals;
}
