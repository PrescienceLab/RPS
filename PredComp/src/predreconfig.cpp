#include "PredComp.h"

void usage(const char *n)
{
  char *b=GetRPSBanner();
  char *m=GetAvailableModels();

  fprintf(stdout, 
	  "Reconfigure a running predserver and force it to refit\n\n"
	  "usage: %s ctrl numfit numpred MODEL\n\n"
	  "ctrl            = client endpoint for predserver's config\n"
	  "numfit          = number of samples to fit the model to\n"
	  "numpred         = number of steps ahead to predict\n"
	  "MODEL           = a model (see below)\n\n%s\n%s",n,m,b);
  delete [] b;
  delete [] m;
}

#define DEFAULT_PERIOD 1000000

int main(int argc, char *argv[])
{
  EndPoint ep;
  int numfit;
  int numpred;
  ModelInfo mi;
  int first_model=4;

  if (argc<5) { 
    usage(argv[0]);
    exit(-1);
  }

  if (ep.Parse(argv[1])) { 
    fprintf(stderr,"Can't parse %s\n",argv[1]);
    exit(-1);
  }

  numfit = atoi(argv[2]);
  numpred = atoi(argv[3]);
  
  ModelTemplate *mt = ParseModel(argc-first_model,&(argv[first_model]));
  if (mt==0) { 
    usage(argv[0]);
    return 0;
  }
  mi  =  ModelInfo(*mt);

  double *temp = new double [numfit];
  TimeStamp now;
  PredictionReconfigurationRequest req(mi,now,numfit,temp,DEFAULT_PERIOD,numpred);
  PredictionReconfigurationResponse resp;

  Reference<PredictionReconfigurationRequest,PredictionReconfigurationResponse> ref;

  if (ref.ConnectTo(ep)) {
    fprintf(stderr,"Can't connect to server\n");
    exit(-1);
  }

  if (ref.Call(req,resp)) { 
    exit(-1);
  }
    
  fprintf(stdout,"Call succeeded.  Response follows...\n");
  resp.Print();

  delete [] temp;

  return 0;
}
