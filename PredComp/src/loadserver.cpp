#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <math.h>

#include "PredComp.h"

#if defined(__osf__)
extern "C" int usleep(int);  // FREAKS
#endif


void usage()
{
  fprintf(stderr,"usage: loadserver period_us [ctrlnetspace] [targetnetspec]+\n");
}





void DeconvolveLoadTrace(double tau, 
                         double *tracein,  
                         int numsamples,  
                         double *traceout) 
{ 
  int i; 
  double Beta; 
 
  Beta = exp(-1/tau); 
 
  for (i=0;i<numsamples-1;i++) { 
    traceout[i] = (tracein[i+1] - Beta*tracein[i])/(1-Beta); 
  } 
} 


class LoadSource : public LocalSource {
private:
  int period_usec;
  double prev;
public:
  LoadSource(int period_usec) { this->period_usec=period_usec; prev=0.0;}

  virtual void GetData(Buffer &buf) {
    LoadMeasurement measure;
    GetData(measure);
    measure.Serialize(buf);
  }
  virtual void GetData(LoadMeasurement &measure) {
    measure.ipaddress=GetMyIPAddress();
    if (gettimeofday(&(measure.timestamp),0)) {
      perror("loadserver can't get time");
      exit(-1);
    }
    if (RPSgetloadavg(measure.avgs,3)!=3) {
      fprintf(stderr,"loadserver can't read load\n");
      exit(-1);
    }
    measure.period_usec=period_usec;
    LoadMeasurement::SetSmoothingType(measure);
    {
      double tau,Beta;
      switch (measure.smoothingtype) {
      case SMOOTH_MACH:
	tau=5.0;
	break;
      case SMOOTH_UNIX:
	tau=60.0;
	break;
      default:
	tau=60.0;
      }
      Beta=exp(-1/tau);
      measure.unsmoothed=(measure.avgs[0] - Beta*prev)/(1.0-Beta);
      prev=measure.avgs[0];
    }
  }
  virtual void GetInterval(TimeValue &tv) {
    tv = TimeValue(period_usec/1000000,period_usec%1000000);
  }
};


class LoadSink : public LocalTarget {
public:
  void ProcessData(Buffer &buf) {
    LoadMeasurement m;
    m.Unserialize(buf);
    m.Print();
  }
};


class RateControl;

typedef FilterWithControl<LoadMeasurement,
                          NullCompute<LoadMeasurement,LoadMeasurement>,
                          LoadMeasurement,
                          LoadMeasurementConfigurationRequest,
                          RateControl,
                          LoadMeasurementConfigurationReply> LoadServer;

LoadSource *curloadsource;
LoadServer *server;


class RateControl {
public:
  static int Compute(LoadMeasurementConfigurationRequest &req,
		     LoadMeasurementConfigurationReply &repl) {
    
    server->DeleteLocalSource(curloadsource);
    delete curloadsource;
    curloadsource = new LoadSource(req.period_usec);
    server->AddLocalSource(curloadsource);
    repl.reqtimestamp=req.timestamp;
    repl.changetimestamp=TimeStamp(0);
    repl.period_usec=req.period_usec;
    return 0;
  }
};




#define FORK_VERSION 0



int main(int argc, char *argv[]) 
{
  EndPoint *ep;
  int i;

  if (argc<4) {
    usage();
    exit(0);
  }

  unsigned period_usec=atoi(argv[1]);

  curloadsource = new LoadSource(period_usec);

  server = new LoadServer;

  server->AddLocalSource(curloadsource);

  for (i=2;i<argc;i++) {
   ep = new EndPoint;
    if (ep->Parse(argv[i])) {
      fprintf(stderr,"Failed to parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    server->AddEndPoint(*ep);
  }
  fprintf(stderr,"loadserver running.\n");
  server->Run();
}

