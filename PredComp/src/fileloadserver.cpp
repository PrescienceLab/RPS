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
  fprintf(stderr,"usage: loadfileserver period_us filename [ctrlnetspace] [targetnetspec]+\n");
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

int LoadAsciiTraceFile(char *name,
                       double **timestamps,
                       double **vals)
{
  double junk;
  int numrecs,i;
  FILE *inp;


  inp = fopen(name,"r");
  numrecs=0;
  while (fscanf(inp,"%lf %lf",&junk,&junk)==2) {
    numrecs++;
  }
  fclose(inp);

  *timestamps = new double [numrecs];
  *vals = new double [numrecs];

  inp = fopen(name,"r");
  for (i=0;i<numrecs;i++) {
    fscanf(inp,"%lf %lf",&((*timestamps)[i]),&((*vals)[i]));
  }
  fclose(inp);

  return numrecs;
}

class FileLoadSource : public LocalSource {
private:
  double *vals;
  int numsamples;
  int cur;
  int period_usec;
  double prev;
public:
  FileLoadSource(int period_usec, char *fn) { 
    this->period_usec=period_usec; 
    prev=0.0;
    double *junk;
    numsamples = LoadAsciiTraceFile(fn,&junk,&vals);
    delete [] junk;
    cur=0;
  }
  virtual ~FileLoadSource() { delete [] vals; }

  virtual void GetData(Buffer &buf) {
    LoadMeasurement measure;
    GetData(measure);
    measure.Serialize(buf);
  }
  virtual void GetData(LoadMeasurement &measure) {
    measure.ipaddress=GetMyIPAddress();
    if (gettimeofday(&(measure.timestamp),0)) {
      perror("fileloadserver can't get time");
      exit(-1);
    }
    
    measure.avgs[0] = measure.avgs[1] = measure.avgs[2] = vals[cur];
    cur = (cur+1) % numsamples;

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

char *fn;
FileLoadSource *curloadsource;
LoadServer *server;


class RateControl {
public:
  static int Compute(LoadMeasurementConfigurationRequest &req,
		     LoadMeasurementConfigurationReply &repl) {
    
    server->DeleteLocalSource(curloadsource);
    delete curloadsource;
    curloadsource = new FileLoadSource(req.period_usec,fn);
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

  if (argc<5) {
    usage();
    exit(0);
  }

  unsigned period_usec=atoi(argv[1]);
  fn = argv[2];

  curloadsource = new FileLoadSource(period_usec,fn);

  server = new LoadServer;

  server->AddLocalSource(curloadsource);

  for (i=3;i<argc;i++) {
   ep = new EndPoint;
    if (ep->Parse(argv[i])) {
      fprintf(stderr,"Failed to parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    server->AddEndPoint(*ep);
  }
  fprintf(stderr,"fileloadserver running.\n");
  server->Run();
  return 0;
}

