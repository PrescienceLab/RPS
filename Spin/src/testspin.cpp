#include <stdlib.h>
#include <iostream>
#include "Spin.h"

void usage()
{
  cerr << "testspin calibrationfile\n";
}
int main(int argc, char *argv[])
{
  if (argc<2) { 
    usage();
    exit(0);
  }

  if (the_global_spin.Unserialize(argv[1])) { 
    cerr << "Can't read "<< ((const char*)(argv[1]))<<"\n";
    exit(-1);
  }

  double sys, usr, wall;
  double t;

  cout << "req\twall\tsys\tusr\ttotal\n";
  for (t=0.0001;t<=10.0;t*=2) {
    the_global_spin.SpinFor(t,wall,sys,usr);
    cout << t << "\t" << wall << "\t" << sys << "\t" 
	 << usr << "\t" << sys+usr << "\n";
  }
}
