#include "PredComp.h"

void usage()
{
  fprintf(stderr,"usage: flowbwreconfig controlspec period_usec fromhost tohost\n");
}


#define DEFAULT_PERIOD 1000000

int main(int argc, char *argv[])
{
  EndPoint ep;
  FlowBWMeasurementConfigurationRequest req;
  FlowBWMeasurementConfigurationReply resp;
  Reference<FlowBWMeasurementConfigurationRequest, 
            FlowBWMeasurementConfigurationReply>  ref;

  if (argc<5) { 
    usage();
    exit(-1);
  }

  if (ep.Parse(argv[1])) { 
    fprintf(stderr,"Can't parse %s\n",argv[1]);
    exit(-1);
  }

  req.period_usec=atoi(argv[2]);
  req.fromip = ToIPAddress(argv[3]);
  req.toip = ToIPAddress(argv[4]);
  
  if (ref.ConnectTo(ep)) {
    fprintf(stderr,"Can't connect to server\n");
    exit(-1);
  }

  if (ref.Call(req,resp)) { 
    fprintf(stderr,"Call FAILED Response follows...\n");
    resp.Print();
    exit(-1);
  }
    
  fprintf(stdout,"Call succeeded.  Response follows...\n");
  resp.Print();

  return 0;
}
