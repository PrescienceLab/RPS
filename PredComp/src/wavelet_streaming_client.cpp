#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "PredComp.h"

void usage()
{
  fprintf(stderr,"usage: wavelet_streaming_client netspec\n");
}



int main(int argc, char *argv[]) 
{
  WaveletIndividualSample s;

  if (argc!=2) {
    usage();
    exit(0);
  }

  EndPoint ep;

  if (ep.Parse(argv[1])) { 
    fprintf(stderr,"wavelet_streaming_client: Can't parse \"%s\"\n",argv[1]);
    exit(-1);
  }

  
  StreamingInputReference<WaveletIndividualSample> source;

  if (source.ConnectTo(ep)) {
    fprintf(stderr,"wavelet_streaming_client: Can't connect to \"%s\"\n",argv[1]);
    exit(-1);
  }

  while (1) {
    if (source.GetNextItem(s)) { 
      fprintf(stderr,"wavelet_streaming_client: Connection failed.\n");
      break;
    }
    cout<<s<<endl;
    // DO SOMETHING WITH SAMPLES HERE
  }
  source.Disconnect();
  return 0;
}
  
