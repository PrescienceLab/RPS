#include <iostream.h>
#include "RTSA.h"


int main(int argc, char *argv[])
{
  if (argc<5) { 
    cerr << "test_rtsa tnom conf sf host1 host2 ..."<<endl;
    exit(-1);
  }
  RTSARequest req;
  RTSAResponse resp;

  req.tnom=atof(argv[1]);
  req.conf=atof(argv[2]);
  req.sf=atof(argv[3]);
  req.numhosts=argc-4;
  
  req.hosts = new Host * [req.numhosts];
  for (int i=0;i<req.numhosts;i++) {
    req.hosts[i] = new Host(argv[4+i]);
  }

  if (RTSAAdviseTask(req,resp)) {
    cerr << "RTSAAdviseTask failed.\n";
    exit(-1);
  } else {
    cout <<resp.tnom<<" second task with sf="<<resp.sf<<" (deadline "<<resp.tnom*(1+resp.sf)<<") and confidence "<<resp.conf<<" advised to go to host "<<((const char*)resp.host.name)<<" with running time ["<<resp.runningtime.tlb<<","<<resp.runningtime.tub<<"] ("<<resp.runningtime.texp<<")"<<endl;
  }
  return 0;
}
     
