#include <stdlib.h>
#include "PredComp.h"


class FlowBWMeasurement2Measurement {
public:
  static int Compute(FlowBWMeasurement &flowbwmeas, Measurement &meas) {
      meas.tag=flowbwmeas.fromip;
      meas.timestamp = flowbwmeas.timestamp;
      meas.period_usec = flowbwmeas.period_usec;
      meas.Resize(1,false);
      meas.series[0] = flowbwmeas.bw;
      return 0;
  }
};

typedef GenericSerializeableInputComputeOutputMirror<
            FlowBWMeasurement,
            FlowBWMeasurement2Measurement,
            Measurement > FlowBW2Measure;


void usage(const char *n)
{
  char *b=GetRPSBanner();

  fprintf(stdout, 
	  "Convert flow bandwidth measurements to generic measurements\n\n"
	  "usage: %s source target+\n\n"
	  "source          = source endpoint\n"
	  "target          = one or more target or connect endpoints\n"
	  "\n%s",n,b);
  delete [] b;
}


int main(int argc, char *argv[]) 
{
  int i;
  EndPoint *ep;

  if (argc<3) {
    usage(argv[0]);
    exit(0);
  }

  FlowBW2Measure mirror;

  for (i=1;i<argc;i++) { 
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

  fprintf(stderr,"flowbw2measure running.\n");
  mirror.Run();
  return 0;
}



