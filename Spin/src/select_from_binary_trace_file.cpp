#include <stdlib.h>
#include <stdio.h>
#include "LoadTrace.h"

void usage()
{
  fprintf(stderr,"usage: select_from_binary_trace_file start:end binarytracefile outbinarytracefile\n");
}


int main(int argc, char *argv[])
{
  char *infile;
  char *outfile;
  double *timestamps;
  double *vals;
  int start,end;
  int numin, numout;

  if (argc!=4) {
    usage();
    exit(-1);
  }
  
  if (sscanf(argv[1],"%d:%d",&start,&end)!=2) { 
    fprintf(stderr,"Can't parse %s\n",argv[1]);
    exit(-1);
  }
  infile=argv[2];
  outfile=argv[3];

  numin  = LoadRawBinaryTraceFile(infile,&timestamps,&vals);

  if (start<0 || start>(numin-1) || end<0 || end>(numin-1) || end<start) {
    fprintf(stderr, "Invalid range %d:%d - file has only %d samples\n", start,end,numin);
    exit(-1);
  }

  numout = StoreRawBinaryTraceFile(outfile,&(timestamps[start]),&(vals[start]),end-start+1);

  fprintf(stderr,"%d samples in, %d samples out\n",numin,numout);

  delete [] timestamps;
  delete [] vals;

  return 0;
}
