#ifndef _sampleout
#define _sampleout

#include <iostream>
#include "util.h"

template <class sampleType>
class OutputSample {
private:
  sampleType coef;
  int        level;

public:
  OutputSample() : coef(0), level(-1) {};
  OutputSample(const OutputSample &rhs) {
    coef = rhs.coef;
    level = rhs.level;
  };
  OutputSample(double coef, int level) {
    this->coef = coef;
    this->level = level;
  };
  virtual ~OutputSample() {};

  OutputSample & operator=(const OutputSample &rhs) {
    coef = rhs.coef;
    level = rhs.level;
    return *this;
  };

  virtual inline void SetOutputSampleValue(sampleType coef)=0;
  virtual inline sampleType GetOutputSampleValue()=0;
  virtual inline void SetOutputSampleLevel(int level)=0;
  virtual inline int GetOutputSampleLevel()=0;

  ostream & Print(ostream &os) const=0;
};

#endif
