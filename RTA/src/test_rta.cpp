#include <iostream>
#include "RTA.h"


int main(int argc, char *argv[])
{
  if (argc!=4) { 
    cerr << "test_rta tnom conf host"<<endl;
    exit(-1);
  }
  double tnom=atof(argv[1]);
  double conf=atof(argv[2]);
  char *host=argv[3];

  RunningTimePredictionRequest req;
  RunningTimePredictionResponse resp;


  req.host=Host(host);
  req.tnom=tnom;
  req.conf=conf;

  if (PredictRunningTime(req,resp)) { 
    cerr <<"Prediction Failed."<<endl;
  } else {
    cout <<tnom<<" second task on "<<((const char*)resp.host.name)<<" at "<<resp.conf<<" Confidence: ["<<resp.tlb<<","<<resp.tub<<"] ("<<resp.texp<<")"<<endl;
  }
  return 0;
}
     
