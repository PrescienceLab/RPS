#include <stdlib.h>
#include <stdio.h>
#include "LoadTrace.h"
#include "banner.h"


void usage(const char *n)
{
  char *b=GetRPSBanner();
  fprintf(stdout,
	  "Convert from 2 column (timestamp, value) ascii trace file to\n"
	  "nework byte-order binary format trace file \n\n"
	  "usage: %s inasciitracefile outnetworkbinarytracefile\n\n%s",n,b);
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

  numin  = LoadAsciiTraceFile(infile,&timestamps,&vals);
  numout = StoreNetworkBinaryTraceFile(outfile,timestamps,vals,numin);

  fprintf(stderr,"%d samples in, %d samples out\n",numin,numout);

  delete [] timestamps;
  delete [] vals;
}
