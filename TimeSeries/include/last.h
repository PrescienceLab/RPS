#ifndef _last
#define _last

#include <stdio.h>
#include "abstract.h"
#include "etathetapred.h"
#include "bestmean.h"

// last value is next value

class LastModel : public BestMeanModel {
 public:
  LastModel();
  LastModel(const LastModel &rhs);
  LastModel(const BestMeanModel &right);
  ~LastModel();

  LastModel &operator=(const LastModel &rhs);

  Predictor *MakePredictor() const ;

  void   Dump(FILE *out=stdout) const;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const LastModel &p) {
  return p.operator<<(os);
}
  

class LastPredictor : public BestMeanPredictor {
public:
  LastPredictor();
  LastPredictor(const LastPredictor &rhs);
  ~LastPredictor();

  LastPredictor & operator=(const LastPredictor &rhs);

  void Dump(FILE *out=stdout) const;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const LastPredictor &p) {
  return p.operator<<(os);
}

// No State
class LastModeler : public BestMeanModeler {
public:
  LastModeler();
  LastModeler(const LastModeler &rhs);
  ~LastModeler();
  
  LastModeler & operator=(const LastModeler &rhs);

  static LastModel *Fit(const double *sequence, const int len);
  static Model *Fit(const double *sequence, const int len, const ParameterSet &ps);

  void Dump(FILE *out=stdout) const;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const LastModeler &p) {
  return p.operator<<(os);
}


#endif
