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


void usage() 
{
  fprintf(stderr,"flowbw2measure sourcenetspec netspec+\n");
}


int main(int argc, char *argv[]) 
{
  int i;
  EndPoint *ep;

  if (argc<3) {
    usage();
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
}



