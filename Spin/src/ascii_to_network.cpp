#include <stdlib.h>
#include <stdio.h>
#include "LoadTrace.h"

void usage()
{
  fprintf(stderr,"usage: ascii_to_network inasciitracefile outnetworkbinarytracefile\n");
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

  numin  = LoadAsciiTraceFile(infile,&timestamps,&vals);
  numout = StoreNetworkBinaryTraceFile(outfile,timestamps,vals,numin);

  fprintf(stderr,"%d samples in, %d samples out\n",numin,numout);

  delete [] timestamps;
  delete [] vals;
}
