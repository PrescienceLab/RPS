#include "PredComp.h"

void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Reconfigure a loadserver\n\n"
	  "usage: %s client period_usec\n\n"
	  "client          = client endpoint\n"
	  "period_usec     = period in usec for measurements\n"
	  "\n%s",n,b);
  delete [] b;
}



#define DEFAULT_PERIOD 1000000

int main(int argc, char *argv[])
{
  EndPoint ep;
  LoadMeasurementConfigurationRequest req;
  LoadMeasurementConfigurationReply resp;
  Reference<LoadMeasurementConfigurationRequest, 
            LoadMeasurementConfigurationReply>  ref;

  if (argc<3) { 
    usage(argv[0]);
    exit(-1);
  }

  if (ep.Parse(argv[1])) { 
    fprintf(stderr,"Can't parse %s\n",argv[1]);
    exit(-1);
  }

  req.period_usec=atoi(argv[2]);

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
