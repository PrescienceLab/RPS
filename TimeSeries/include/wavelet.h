
#ifndef _wavelet
#define _wavelet

#include <string>
#include <vector>
#include <deque>
#include <stdio.h>
#include "abstract.h"
#include "fit.h"

#ifdef HAVE_WAVELETS
// Disgusting FIX FIX FIX
#include "../../Wavelets/include/util.h"
#include "Wavelets.h"
#endif

using namespace std;

// Wavelet decomposition, prediction of details, reconstruction
// configured by an external spec file
//
// All predictors MUST be MANAGED PREDICTORS
//
// Note that recursion is NOT ALLOWED

class WaveletDelay {
private:
  int len;
  deque<double> delayline;
public:
  WaveletDelay(const WaveletDelay &rhs) ;
  WaveletDelay();
  WaveletDelay & operator= (const WaveletDelay &rhs);
  WaveletDelay(const int l);
  virtual ~WaveletDelay();
  double PopPush(const double val);

  void   Dump(FILE *out=stdout) const ;
  ostream &operator<<(ostream &os) const;

};

inline ostream & operator<< (ostream &os, const WaveletDelay &p) {
  return p.operator<<(os);
}

struct WaveletLevelOp {
  ModelTemplate *mt;
  Model         *model;
  Predictor     *pred;
  WaveletDelay  *delay;
  int           samples;       
  int           ahead;

  WaveletLevelOp();
  WaveletLevelOp(const WaveletLevelOp &rhs);
  WaveletLevelOp & operator=(const WaveletLevelOp &rhs);
  virtual ~WaveletLevelOp();

  void   Dump(FILE *out=stdout) const ;
  ostream &operator<<(ostream &os) const;

};

inline ostream & operator<< (ostream &os, const WaveletLevelOp &p) {
  return p.operator<<(os);
}

class WaveletModel : public Model {
private:
  string configfile;
public:
  WaveletModel();
  WaveletModel(const char *specfile);
  WaveletModel(const string &specfile);
  WaveletModel(const WaveletModel &rhs);
  virtual ~WaveletModel();

  WaveletModel & operator=(const WaveletModel &rhs);

  Predictor *MakePredictor() const;

  void   Dump(FILE *out=stdout) const ;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const WaveletModel &p) {
  return p.operator<<(os);
}

class WaveletPredictor : public Predictor {
private:
  string          specfile;
  unsigned        curindex;
  double          nextval;
#ifdef HAVE_WAVELETS
  WaveletType     wtype;
  unsigned        numlevels;
  WaveletLevelOp *levelop;

  typedef WaveletInputSample<double> WISD;
  typedef WaveletOutputSample<double> WOSD;
  
  StaticForwardWaveletTransform<double,WOSD,WISD> *xform;
  StaticReverseWaveletTransform<double,WISD,WOSD> *xformrev;

#endif
  
  int ReadSpecFileAndConfigure();

public:
  WaveletPredictor();
  WaveletPredictor(const char *file);
  WaveletPredictor(const string &file);
  WaveletPredictor(const WaveletPredictor &rhs);
  ~WaveletPredictor();

  WaveletPredictor & operator=(const WaveletPredictor &rhs);

  int Begin();
  int StepsToPrime() const;
  double Step(const double obs);
  int Predict(const int maxahead, double *predictions) const;
  virtual int ComputeVariances(const int maxahead, 
			       double *vars, 
			       const VarianceType vtype=POINT_VARIANCES) const;
  void Dump(FILE *out=stdout) const;
  ostream & operator<<(ostream &rhs) const;
};

inline ostream & operator<< (ostream &os, const WaveletPredictor &p) {
  return p.operator<<(os);
}

// No State
class WaveletModeler : public Modeler {
 public:
  WaveletModeler();
  WaveletModeler(const WaveletModeler &rhs);
  virtual ~WaveletModeler();
  
  WaveletModeler & operator=(const WaveletModeler &rhs);

  static WaveletModel *Fit(const double *sequence, const int len, const string &configfile);
  static Model *Fit(const double *sequence, const int len, const ParameterSet &ps);

  void Dump(FILE *out=stdout) const;
  ostream & operator<<(ostream &rhs) const;
};

inline ostream & operator<< (ostream &os, const WaveletModeler &p) {
  return p.operator<<(os);
}


#endif

