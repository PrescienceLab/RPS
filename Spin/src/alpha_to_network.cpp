#include <stdlib.h>
#include <stdio.h>
#include "LoadTrace.h"

void usage()
{
  fprintf(stderr,"usage: alpha_to_network inalphabinarytracefile outnetworkbinarytracefile\n");
}


int main(int argc, char *argv[])
{
  char *infile;
  char *outfile;
  double *timestamps;
  double *vals;
  int numin, numout;

  if (argc!=3) {
    usage();
    exit(0);
  }

  infile=argv[1];
  outfile=argv[2];

  numin  = LoadAlphaBinaryTraceFile(infile,&timestamps,&vals);
  PervertBinaryTrace(timestamps,vals,numin);
  PervertBinaryTrace(timestamps,vals,numin);

  numout = StoreNetworkBinaryTraceFile(outfile,timestamps,vals,numin);

  fprintf(stderr,"%d samples in, %d samples out\n",numin,numout);

  delete [] timestamps;
  delete [] vals;
}
