#include <stdlib.h>
#include <iostream.h>
#include "Spin.h"


void usage()
{
  cerr << "calibrate outputfile\n";
}

int main(int argc, char *argv[])
{
  if (argc<2) { 
    usage();
    exit(0);
  }

  
  double sys, usr, wall;
  double t;

  the_global_spin.Calibrate();

  cout << "req\twall\tsys\tusr\ttotal\n";
  for (t=0.0001;t<=10.0;t*=2) {
    the_global_spin.SpinFor(t,wall,sys,usr);
    cout << t << "\t" << wall << "\t" << sys << "\t" 
	 << usr << "\t" << sys+usr << "\n";
  }

  the_global_spin.Serialize(argv[1]);

  cout << "Calibration data written to "<<argv[1]<<"\n";
  
  return 0;
}
