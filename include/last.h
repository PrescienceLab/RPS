#ifndef _last
#define _last

#include <stdio.h>
#include "abstract.h"
#include "linear.h"
#include "etathetapred.h"
#include "bestmean.h"

// last value is next value

class LastModel : public BestMeanModel {
 public:
  LastModel(const BestMeanModel &right);
  void   Dump(FILE *out=stdout);
  Predictor *MakePredictor();
};
  

class LastPredictor : public BestMeanPredictor {
public:
  void Dump(FILE *out=stdout);
};

// No State
class LastModeler : public BestMeanModeler {
public:
  static LastModel *Fit(double *sequence, int len);
  static Model *Fit(double *sequence,int len, const ParameterSet &ps);
};


#endif
