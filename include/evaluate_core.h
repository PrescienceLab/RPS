#ifndef _evaluate_core
#define _evaluate_core
#include <stdio.h>

struct PredictionStats;

class Evaluator {
private:
  int numpred;
  int numsamples;
  int seensamples;
  double **row;
  double *maxerr;
  double *minerr;
  double *msqerr;
  double *meanabserr;
  double *meanerr;
  double *resids;  // +1 prediction errors
  int    residbufsize;			
  int Clear();
  int Realloc(int numresiduals=-1);
public:
  Evaluator();
  virtual ~Evaluator();
  
  int Initialize(int numpred, int numresiduals=100000);
  int Step(double *curandpreds);
  int Step(double cur, double *pred);
  double GetCurrentPlusOneMeanSquareError();
  int Drain();
  PredictionStats *GetStats(int maxacflag=100, double acfconf=0.95);
  int Dump(FILE *out=stderr);

};


typedef int (*PackDoubles)(double *buf,int len);
typedef int (*UnpackDoubles)(double *buf,int len);
typedef int (*PackInts)(int *buf,int len);
typedef int (*UnpackInts)(int *buf,int len);

struct PredictionStats {
  int    valid;         // validity
  int    usertags;      // set to zero - for user tagging of structure
  int    numsamples;    // number of samples     
  int    numpred;       // number of predictors +1..+numpred
  int    maxlag;        // maximum acf lag observered
  double acfconf;       // confidence level for acf tests
  double *msqerr;       // mean squared error for all predictors
  double *meanabserr;   // mean absolute error 
  double *meanerr;      // mean error
  double *minerr;       // minimum error
  double *maxerr;       // maximum error
  double medianresid;   // median of residuals (+1 errors)
  double sigacffrac;    // fraction of residuals acf coeffs that are significant 
  double tpfrac;        // fraction of 3 point pairs that are turning points
  double scfrac;        // fraction of 2 point pairs that are sign changes
  double portmanteauQ;  // portmanteau Q statistic (power in acfs)
  double r2normfit;     // R^2 value for linear fit of resids vs N(0,1) qqplot

  PredictionStats();
  virtual ~PredictionStats();
  
  int    Initialize(int numpred);

  double GetMeanSquaredError(int pred);
  double GetMeanAbsError(int pred);
  double GetMeanError(int pred);
  double GetMinError(int pred);
  double GetMaxError(int pred);

  int    Dump(FILE *out=stderr);

  int    Pack(PackDoubles PD, PackInts PI);
  int    Unpack(UnpackDoubles PD, PackInts PI);
};

#endif
