#include "Spin.h"
#include <sys/resource.h>

SpinRequest::SpinRequest(int tag, double secs)
{
  this->tag=tag;
  this->secs=secs;
}

SpinRequest:: SpinRequest(const SpinRequest &right)
{
  this->tag=right.tag;
  this->secs=right.secs;
}

SpinRequest::~SpinRequest()
{}
  
SpinRequest & SpinRequest::operator = (const SpinRequest &right)
{
  this->tag=right.tag;
  this->secs=right.secs;
  return *this;
}

int SpinRequest::GetPackedSize() const 
{
  return sizeof(int)+sizeof(double);
}

int SpinRequest::GetMaxPackedSize() const 
{
  return GetPackedSize();
}

int SpinRequest::Pack(Buffer &buf) const 
{
  buf.Pack(tag);
  buf.Pack(secs);
  return 0;
}

int SpinRequest::Unpack(Buffer &buf) 
{
  buf.Unpack(tag);
  buf.Unpack(secs);
  return 0;
}


void SpinRequest::Print(FILE *out) const
{
  fprintf(out,"SpinRequest: tag=%d secs=%f\n",tag,secs);
}

ostream & SpinRequest::operator<<(ostream &os) const
{
  return (os<<"SpinRequest(tag="<<tag<<", secs="<<secs<<")");
}

SpinReply::SpinReply(int tag, 
		     double reqsecs, 
		     double wallsecs,
		     double usrsecs,
		     double syssecs) 
{
  this->tag=tag;
  this->reqsecs=reqsecs;
  this->wallsecs=wallsecs;
  this->usrsecs=usrsecs;
  this->syssecs=syssecs;
}

SpinReply:: SpinReply(const SpinReply &right)
{
  this->tag=right.tag;
  this->wallsecs=right.wallsecs;
  this->usrsecs=right.usrsecs;
  this->syssecs=right.syssecs;
}


SpinReply::~SpinReply()
{}
  
SpinReply & SpinReply::operator = (const SpinReply &right)
{
  this->tag=right.tag;
  this->wallsecs=right.wallsecs;
  this->usrsecs=right.usrsecs;
  this->syssecs=right.syssecs;
  return *this;
}

int SpinReply::GetPackedSize() const 
{
  return sizeof(int)+4*sizeof(double);
}

int SpinReply::GetMaxPackedSize() const 
{
  return GetPackedSize();
}

int SpinReply::Pack(Buffer &buf) const 
{
  buf.Pack(tag);
  buf.Pack(reqsecs);
  buf.Pack(wallsecs);
  buf.Pack(usrsecs);
  buf.Pack(syssecs);
  return 0;
}

int SpinReply::Unpack(Buffer &buf) 
{
  buf.Unpack(tag);
  buf.Unpack(reqsecs);
  buf.Unpack(wallsecs);
  buf.Unpack(usrsecs);
  buf.Unpack(syssecs);
  return 0;
}

void SpinReply::Print(FILE *out) const
{
  fprintf(out,"SpinReply: tag=%d reqsecs=%f wallsecs=%f usrsecs=%f syssecs=%f\n",tag,reqsecs,wallsecs,usrsecs,syssecs);
}

ostream & SpinReply::operator<<(ostream &os) const
{
  return (os<<"SpinReply(tag="<<tag<<", reqsecs="<<reqsecs<<", wallsecs="<<wallsecs<<", usrsecs="<<usrsecs<<", syssecs="<<syssecs<<")");
}


int SpinCompute::Compute(const SpinRequest &req, SpinReply &repl)
{
  repl.tag=req.tag;
  repl.reqsecs = req.secs;
  req.Print(stderr);
  the_global_spin.SpinFor(req.secs,repl.wallsecs,repl.syssecs,repl.usrsecs);
  repl.Print(stderr);

  return 0;
}


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Spin.h"
#include "TimeStamp.h"
#include "tools.h"
#include "random.h"
#include "junk.h"

Spin::Spin() 
{
}

Spin::~Spin()
{}


void Spin::GetRusage(int junk, double &systime, double &usrtime)
{
  struct rusage x;

  if (getrusage(RUSAGE_SELF,&x)) {
    perror("getrusage");
    exit(-1);
  }

  if (junk>=0) { 
    systime=(double)(TimeStamp(&(x.ru_stime)));
    usrtime=(double)(TimeStamp(&(x.ru_utime)));
  } else {
    // This is intentional.  We need to force the read+write in
    // order to do the timing correctly
    volatile double syst=(double)(TimeStamp(&(x.ru_stime)));
    volatile double usrt=(double)(TimeStamp(&(x.ru_utime)));
  }
}


void Spin::Null(int junk)
{
}

volatile double SpinGarbageVolatile;
volatile double SpinGarbageVolatile2;

void Spin::SpinInternalRaw(int iters)
{
  int i;
  for (i=0;i<iters;i++) {
    SpinGarbageVolatile+=SpinGarbageVolatile2;
  }
}



void Spin::Time(void (Spin::*call)(int arg,...),
		int    thearg, 
		int    iters,
		double &walltime, 
		double &systime, 
		double &usrtime)
{
  int i;
  double sysbegin, usrbegin, sysend, usrend;

  TimeStamp begin(0);
  GetRusage(0,sysbegin,usrbegin);
  for (i=0;i<iters;i++) {
    (this->*call)(thearg);
  }
  GetRusage(0,sysend,usrend);
  TimeStamp end(0);

  walltime = MAX(0.0, (double)end - (double)begin 
              - 2 * getrusage_overhead -  iters*loop_overhead);
  systime = MAX(0.0,sysend - sysbegin - 2 * getrusage_overhead);
  usrtime = MAX(0.0,usrend - usrbegin - iters * loop_overhead); 
}




#define MIN_RESOLVE_SECS 0.05
#define MAX_RESOLVE_SECS 5

#define LOOP_OVERHEAD_ITERS 1000000
#define GETRUSAGE_OVERHEAD_ITERS 1000000
#define NUMSAMPLES_ITER_CAL 1000
#define NUMSAMPLES_TIME_CAL 100
#define ITERS_MIN 1000
#define ITERS_MAX 100000

void Spin::Calibrate()
{
  iters_per_second = 0;
  getrusage_overhead = 0;
  loop_overhead = 0;

  cerr << "Calibrating...\n";

  double walltime, systime, usrtime;

  cerr << "Estimating loop overhead...";
  // First, measure the overheads of the timing loop
  Time( (void (Spin::*)(int,...)) &Spin::Null,-1,LOOP_OVERHEAD_ITERS,walltime,systime,usrtime);
  loop_overhead = usrtime/LOOP_OVERHEAD_ITERS;
  cerr << loop_overhead*1e6 << " microseconds\n";

  cerr << "Estimating getrusage overhead...";
  // Now, measure overhead of getrusage
  Time((void (Spin::*)(int,...)) &Spin::GetRusage,-1,GETRUSAGE_OVERHEAD_ITERS,walltime,systime,usrtime);
  getrusage_overhead = systime/GETRUSAGE_OVERHEAD_ITERS;
  cerr << getrusage_overhead*1e6 << " microseconds\n";

  // Now, generate a bunch of iters, time pairs to estimate iters_per_second

  double usertime[NUMSAMPLES_ITER_CAL];
  double iters[NUMSAMPLES_ITER_CAL];
  int i;

  InitRandom();

  cerr << "Generating random iteration/time pairs...";
  for (i=0;i<NUMSAMPLES_ITER_CAL;i++) { 
    iters[i] = (double)(ITERS_MIN+UnsignedRandom()%(ITERS_MAX-ITERS_MIN+1));
    Time((void (Spin::*)(int,...)) &Spin::SpinInternalRaw,
	 (int)(iters[i]),1,walltime,systime,usertime[i]);
  }
  cerr << "Done.\n";
  cerr << "Doing Least Squares Fit...";

  double m, b;
  double R2 = LeastSquares(usertime,iters,NUMSAMPLES_ITER_CAL,&m,&b);
  cerr << "Done.\n";
  cerr << "m="<<m<<" b="<<b<<" R2="<<R2<<"\n";

  cerr << "Generating random iteration/time pairs again\n";
  cerr << "using initial fit (this may take some time)...";
  for (i=0;i<NUMSAMPLES_TIME_CAL;i++) { 
    double secs = UniformRandom(MIN_RESOLVE_SECS,MAX_RESOLVE_SECS);
    iters[i] = b + m * secs;
    Time((void (Spin::*)(int,...)) &Spin::SpinInternalRaw,
	 (int)(iters[i]),1,walltime,systime,usertime[i]);
  }
  cerr << "Done.\n";

  cerr << "Doing Least Squares Fit...";
  R2 = LeastSquares(usertime,iters,NUMSAMPLES_TIME_CAL,
			   &iters_per_second,&min_iters);
  cerr << "Done.\n";
  cerr << iters_per_second<<" iters/sec "<< min_iters << " iters min R2=" << R2 << "\n"; 

  cerr << "Calibration finished.\n";

}


void Spin::SpinInternal(int iters, double &walltime, double &systime, double &usrtime)
{
  Time((void (Spin::*)(int,...)) &Spin::SpinInternalRaw,iters,1,walltime,systime,usrtime);
}

#define STEPSIZE 0.1

void Spin::SpinFor(double seconds, double &walltime, double &systime, double &usrtime)
{
#if 1
  double thiswalltime, thissystime, thisusrtime;
  double left = seconds;
  double req;
  int itersthisstep;

  systime=usrtime=0.0;

  TimeStamp start(0);

  while (left>0.0) { 
    if (left<=STEPSIZE) { 
      req=MIN(left,MIN_RESOLVE_SECS);
    } else {
      req=STEPSIZE;
    }
    itersthisstep =(int)MAX(0.0,min_iters+iters_per_second*req);
    SpinInternal(itersthisstep, thiswalltime, thissystime, thisusrtime);
    systime+=thissystime;
    usrtime+=thisusrtime;
    left-=thisusrtime;
  }

  TimeStamp end(0);

  walltime = (double)end - (double)start;

#else
  SpinInternal((int)MAX(0.0,(min_iters+seconds*iters_per_second)), walltime,systime,usrtime);
#endif
}


int Spin::GetPackedSize() const 
{
  return 4*sizeof(double);
}

int Spin::GetMaxPackedSize() const 
{
  return GetPackedSize();
}


int Spin::Pack(Buffer &buf) const 
{
  buf.Pack(min_iters);
  buf.Pack(iters_per_second);
  buf.Pack(getrusage_overhead);
  buf.Pack(loop_overhead);
  return 0;
}


int Spin::Unpack(Buffer &buf)
{
  buf.Unpack(min_iters);
  buf.Unpack(iters_per_second);
  buf.Unpack(getrusage_overhead);
  buf.Unpack(loop_overhead);
  return 0;
}

int Spin::Serialize(char *fn)
{
  int rc;
  int fd = open(fn,O_WRONLY|O_CREAT,0644);
  rc=SerializeableInfo::Serialize(fd);
  close(fd);
  return rc;
}

int Spin::Unserialize(char *fn)
{
  int rc;
  int fd = open(fn,O_RDONLY);
  if (fd<0) { 
    perror("Spin::Unserialize open");
    return -1;
  }
  rc=SerializeableInfo::Unserialize(fd);
  close(fd);
  return rc;
}


ostream & Spin::operator<<(ostream &os) const
{
  return (os<<"Spin(details omitted)");
}



Spin the_global_spin;
