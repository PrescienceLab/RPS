#include "PredComp.h"

void usage()
{
  fprintf(stderr,"usage: predreconfig controlspec numfit numpred modelclass modeldata*\n");
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
    usage();
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
    usage();
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
