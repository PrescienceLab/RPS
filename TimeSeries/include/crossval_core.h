#ifndef _crossval_core
#define _crossval_core
#include "ar.h"
#include "ma.h"
#include "arma.h"
#include "arima.h"
#include "arfima.h"
#include "bestmean.h"
#include "tools.h"
#include "evaluate_core.h"
#include "random.h"
#include "fit.h"
#include <string.h>

#define DEFAULT_MAXLAG  100
#define DEFAULT_ACFCONF 0.95

//#define CHECK_FOR_BLOWUP

// Tags
#define BLOWUP_TAG    1

typedef int (*PackDoubles)( double *buf, int len);
typedef int (*UnpackDoubles)(double *buf,  int len);
typedef int (*PackInts)( int *buf, int len);
typedef int (*UnpackInts)(int *buf,  int len);
typedef int (*PackString)( char *buf,  int len);
typedef int (*UnpackString)(char *buf,  int len);

#define TAGLEN 40
typedef char Tag[TAGLEN];

struct Command {
  int numtests, numahead;
  int bmlimit;
  int pmin, pmax, dmin, dmax, qmin, qmax;
  int minfit, maxfit;
  int mintest, maxtest;
};

struct Testcase {
  Tag tag;
  int number;  // used to uniqueify
  int numahead;
  int testfirst, testnum;
  int fitfirst, fitnum;
  int p,d,q;
  int bmlimit;
  int maxlag;
  double acfconf;
  int Pack(PackInts, PackDoubles, PackString);
  int Unpack(UnpackInts, UnpackDoubles, UnpackString);
};


struct IntervalStats
{
  int maxlag;
  double acfconf;
  double mean, var, median, min, max, Q, r2norm, sigacffrac, tpfrac, scfrac;
  int Pack(PackInts, PackDoubles);
  int Unpack(UnpackInts, UnpackDoubles);
};


class TestcaseGenerator {
private:
  int numsamples;
  Tag tag;
  Command cmd;
  int    maxlag;
  double acfconf;
public:  
  TestcaseGenerator(Command *c, Tag tg, int numsamples, 
		    int maxlag=DEFAULT_MAXLAG, double acfconf=DEFAULT_ACFCONF) { 	
    cmd = *c; strncpy(tag,tg,TAGLEN);  this->numsamples=numsamples;
    this->maxlag=maxlag; this->acfconf=acfconf;
  }
  Testcase *GenerateRandomTestcase();
};

int Simulate(double          *seq,
	     Testcase        *testcase,
	     IntervalStats   *fit,
	     IntervalStats   *test,
	     int             *bmp, 
	     PredictionStats **testmodelstats,
	     PredictionStats **bmmodelstats);

void OutputHeader(FILE *out, int outputformat=0);
void OutputTestcaseResults(FILE *out, 
			   Testcase *testcase,
			   IntervalStats *fit,	
			   IntervalStats *test,
			   int              bmp,
			   PredictionStats *testmodel,
			   PredictionStats *bmmodel,
			   int outputformat=0);



Command *GetCommand(FILE *cmd);

double *LoadBinaryTraceFile(char *name, int *num, int perversion=0);
double *LoadAsciiTraceFile(char *name, int *num);

#endif
