#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <math.h>

#include "PredComp.h"


void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Measure the bandwidth of a flow between two hosts\n\n"
	  "usage: %s period_usec fromhost tohost ctrl target+\n\n"
	  "period_usec     = period in usec for measurements\n"
	  "fromhost,tohost = the path to measure\n"
	  "ctrl            = server endpoint for control\n"
	  "target+         = one or more target or connect endpoints\n"
	  "\n%s",n,b);
  delete [] b;
}




//extern "C" int usleep(unsigned); // Freaks

class FlowBWSource : public LocalSource {
private:
  unsigned fromip;
  unsigned toip;
  int period_usec;
public:
  FlowBWSource(int period_usec, unsigned fromip, unsigned toip) {
    this->period_usec=period_usec; this->fromip=fromip, this->toip=toip;
  }

  virtual void GetData(Buffer &buf) {
    FlowBWMeasurement measure;
    GetData(measure);
    measure.Print(stderr);
    measure.Serialize(buf);
  }
  virtual void GetData(FlowBWMeasurement &measure) {
    measure.fromip=fromip;
    measure.toip=toip;
    if (gettimeofday(&(measure.timestamp),0)) {
      perror("loadserver can't get time");
      exit(-1);
    }
    char fromhost[1024], tohost[1024];
    IPToHostname(fromip,fromhost,1024);
    IPToHostname(toip,tohost,1024);
    if (getflowbw(fromhost,tohost,&(measure.bw))) {
      fprintf(stderr,"flowbwserver can't get a bandwidth measurement\n");
      exit(-1);
    }
    measure.period_usec=period_usec;
  }
  virtual void GetInterval(TimeValue &tv) {
    tv = TimeValue(period_usec/1000000,period_usec%1000000);
  }
};


class FlowBWSink : public LocalTarget {
public:
  void ProcessData(Buffer &buf) {
    FlowBWMeasurement m;
    m.Unserialize(buf);
    m.Print();
  }
};


class RateControl;

typedef FilterWithControl<FlowBWMeasurement,
                          NullCompute<FlowBWMeasurement,FlowBWMeasurement>,
                          FlowBWMeasurement,
                          FlowBWMeasurementConfigurationRequest,
                          RateControl,
                          FlowBWMeasurementConfigurationReply> FlowBWServer;

FlowBWSource *curloadsource;
FlowBWServer *server;


class RateControl {
public:
  static int Compute(FlowBWMeasurementConfigurationRequest &req,
		     FlowBWMeasurementConfigurationReply &repl) {
    
    server->DeleteLocalSource(curloadsource);
    delete curloadsource;
    curloadsource = new FlowBWSource(req.period_usec, req.fromip, req.toip);
    server->AddLocalSource(curloadsource);
    repl.reqtimestamp=req.timestamp;
    repl.changetimestamp=TimeStamp(0);
    repl.period_usec=req.period_usec;
    repl.fromip=req.fromip;
    repl.toip=req.toip;
    return 0;
  }
};


int main(int argc, char *argv[]) 
{
  EndPoint *ep;
  int i;

  if (argc<6) {
    usage(argv[0]);
    exit(0);
  }

  unsigned period_usec=atoi(argv[1]);
  unsigned fromip = ToIPAddress(argv[2]);
  unsigned toip = ToIPAddress(argv[3]);
  
  curloadsource = new FlowBWSource(period_usec,fromip,toip);

  server = new FlowBWServer;

  server->AddLocalSource(curloadsource);

  for (i=4;i<argc;i++) {
   ep = new EndPoint;
    if (ep->Parse(argv[i])) {
      fprintf(stderr,"Failed to parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    server->AddEndPoint(*ep);
  }
  fprintf(stderr,"flowbwserver running.\n");
  server->Run();
}

