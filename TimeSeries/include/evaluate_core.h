#ifndef _evaluate_core
#define _evaluate_core

#include <iostream>
#include <stdio.h>

using namespace std;

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
  int Realloc(const int numresiduals=-1);
public:
  Evaluator();
  Evaluator(const Evaluator &rhs);
  virtual ~Evaluator();

  Evaluator & operator=(const Evaluator &rhs);
  
  int Initialize(const int numpred, const int numresiduals=100000);
  int Step(const double *curandpreds);
  int Step(const double cur, const double *pred);
  double GetCurrentPlusOneMeanSquareError() const;
  int Drain();
  PredictionStats *GetStats(const int maxacflag=100, const double acfconf=0.95) const;

  int Dump(FILE *out=stderr) const;
  ostream & operator<<(ostream &os) const;
};

inline ostream & operator<<(ostream &os, const Evaluator &e) { return e.operator<<(os);}


typedef int (*PackDoubles)( double *buf,  int len);
typedef int (*UnpackDoubles)(double *buf,  int len);
typedef int (*PackInts)( int *buf, int len);
typedef int (*UnpackInts)(int *buf, int len);

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
  PredictionStats(const PredictionStats &rhs);
  virtual ~PredictionStats();

  PredictionStats & operator=(const PredictionStats &rhs);
  
  int    Initialize(int numpred);

  double GetMeanSquaredError(int pred) const;
  double GetMeanAbsError(int pred) const;
  double GetMeanError(int pred) const;
  double GetMinError(int pred) const;
  double GetMaxError(int pred) const;

  int    Dump(FILE *out=stderr) const;
  ostream & operator<<(ostream &os) const;

  // note, this cannot be const-correct due to a PVM interaction
  int    Pack(PackDoubles PD, PackInts PI) ;
  int    Unpack(UnpackDoubles PD, UnpackInts PI);
};

inline ostream & operator<<(ostream &os, const PredictionStats &p) { return p.operator<<(os);}

#endif
