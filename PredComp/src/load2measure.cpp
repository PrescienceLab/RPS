#include <stdlib.h>

#include "PredComp.h"

unsigned whichavg;

class LoadMeasurement2Measurement {
public:
  static int Compute(LoadMeasurement &loadmeas, Measurement &meas) {
      meas.tag=loadmeas.ipaddress;
      meas.timestamp = loadmeas.timestamp;
      meas.period_usec = loadmeas.period_usec;
      meas.Resize(1,false);
      switch (whichavg) { 
      case 1:
	meas.series[0] = loadmeas.avgs[1];
	break;
      case 2:
	meas.series[0] = loadmeas.avgs[2];
	break;
      case 99:
	meas.series[0] = loadmeas.unsmoothed;
	break;
      case 0:
      default:
	meas.series[0] = loadmeas.avgs[0];
	break;
      }
      return 0;
  }
};

typedef GenericSerializeableInputComputeOutputMirror<
            LoadMeasurement,
            LoadMeasurement2Measurement,
            Measurement > Load2Measure;


void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Convert load measurements to generic measurements\n\n"
	  "usage: %s avgno source target+\n\n"
	  "avgno           = which average to use (0,1,2, or 99(unsmoothed)\n"
	  "source          = source endpoint\n"
	  "target          = one or more target or connect endpoints\n"
	  "%s",n,b);
  delete [] b;
}



int main(int argc, char *argv[]) 
{
  int i;
  EndPoint *ep;

  if (argc<4) {
    usage(argv[0]);
    exit(0);
  }

  whichavg = atoi(argv[1]);

  Load2Measure mirror;


  for (i=2;i<argc;i++) { 
    ep = new EndPoint;
    if (ep->Parse(argv[i])) { 
      fprintf(stderr,"Can't parse \"%s\"\n",argv[i]);
      exit(-1);
    }
    if (mirror.AddEndPoint(*ep)) { 
      fprintf(stderr,"Can't add \"%s\"\n",argv[i]);
      exit(-1);
    }
  }

  fprintf(stderr,"load2measure running.\n");
  mirror.Run();
  return 0;
}



