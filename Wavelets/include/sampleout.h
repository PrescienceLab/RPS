#ifndef _sampleout
#define _sampleout

#include <iostream>
#include "util.h"

class SampleOut {
private:
  double coef;
  int    level;

public:
  SampleOut();
  SampleOut(const SampleOut &rhs);
  SampleOut(double coef, int level);
  virtual ~SampleOut();

  SampleOut & operator=(const SampleOut &rhs);

  inline void SetSampleOutValue(double coef);
  double GetSampleOutValue();
  inline void SetSampleOutLevel(int level);
  inline int GetSampleOutLevel();

  ostream & Print(ostream &os) const;
};

#endif
