#include <stdio.h>

#include "Spin.h"


void usage()
{
  fprintf(stderr,"usage: spinclient sourcenetspec reqsecs\n");
}


int main(int argc, char *argv[])
{
  EndPoint ep;
  SpinRequest req;
  SpinReply   resp;

  if (argc!=3) {
    usage();
    exit(0);
  }

  if (ep.Parse(argv[1])) {
    fprintf(stderr,"Failed to parse \"%s\"\n",argv[1]);
    exit(-1);
  }

  if (ep.atype!=EndPoint::EP_SOURCE) {
    fprintf(stderr,"Must supply source netspec\n");
    exit(-1);
  }

  req.tag=0;
  req.secs=atof(argv[2]);

  SpinServerRef ssr;

  if (ssr.ConnectTo(ep)) { 
    fprintf(stderr,"Can't Connect to %s\n",argv[1]);
    exit(-1);
  }

  ssr.Call(req,resp);
  
  resp.Print();
}
