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


void usage() 
{
  fprintf(stderr,"load2measure avgno sourcenetspec netspec+\n");
}


int main(int argc, char *argv[]) 
{
  int i;
  EndPoint *ep;

  if (argc<4) {
    usage();
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
}



