#include <stdio.h>

#include "Spin.h"

void usage()
{
  fprintf(stderr,"usage: spinserver [+]calibrationfile serverspec\n");
}



int main(int argc, char *argv[])
{
  SpinServer spinserver;
  EndPoint ep;

  if (argc!=3) {
    usage();
    exit(0);
  }

  if (argv[1][0]!='+') { 
    if (the_global_spin.Unserialize(argv[1])) {
      fprintf(stderr,"Can't read calibration file\n");
      exit(-1);
    }
  } else {
    the_global_spin.Calibrate();
    the_global_spin.Serialize(&(argv[1][1]));
  }

  if (ep.Parse(argv[2])) {
    fprintf(stderr,"Failed to parse \"%s\"\n",argv[2]);
    exit(-1);
  }

  spinserver.AddEndPoint(ep);

  fprintf(stderr,"spinserver running\n");

  spinserver.Run();
}
