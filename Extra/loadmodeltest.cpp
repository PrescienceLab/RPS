#include <stdlib.h>
#include <math.h>
#include <iostream.h>
#include "Spin.h"
#include "TimeStamp.h"
#include "getloadavg.h"
#include "junk.h"

void usage()
{
  cerr << "loadmodeltest calibrationfile comptime delta\n";
}
int main(int argc, char *argv[])
{
  if (argc<4) { 
    usage();
    exit(0);
  }

  char *calfile = argv[1];
  double comptime = atof(argv[2]);
  double delta = atof(argv[3]);

  Spin s;
  
  if (s.Unserialize(calfile)) { 
    cerr << "Can't read "<< argv[1]<<"\n";
    exit(-1);
  }
  
  double syst, usrt, wallt, totalt; 
  double avgs[3], lastavg;
  double loadavgest;
  double sys, usr, wall;
  double t;
  double deconvload;
  double tau = 5;
  double beta = exp(-1/tau);	
  
  syst=usrt=wallt=totalt=0;
  loadavgest=0;
  lastavg=0;

//  cout << "time\tloadavgest\tactual\n";

  TimeStamp begin(0);

  while (totalt<comptime) {
    TimeStamp start(0);
    RPSgetloadavg(avgs,3);
    s.SpinFor(delta,wall,sys,usr);
    syst+=sys;
    usrt+=usr;
    totalt+=sys+usr;
    wallt+=wall;
    deconvload = (avgs[0] - beta*lastavg)/(1-beta);
    lastavg=avgs[0];
//    loadavgest += wall*(1/(MAX(1.0,avgs[0]))) ;
    loadavgest += wall*(1/(MAX(1.0,deconvload)));

    cout << (double)start- (double)begin << "\t" << loadavgest << "\t" << totalt << "\t" << avgs[0] << "\t" << deconvload <<"\n";
  }
}
