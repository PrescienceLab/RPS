#ifndef _sampleout
#define _sampleout

#include <iostream>
#include <vector>

#include "util.h"

template <class sampleType>
class OutputSample {
protected:
  sampleType coef;
  int        level;

public:
  OutputSample() : coef(0), level(-1) {};
  OutputSample(const OutputSample &rhs) {
    coef = rhs.coef;
    level = rhs.level;
  };
  OutputSample(sampleType coef, int level) {
    this->coef = coef;
    this->level = level;
  };
  virtual ~OutputSample() {};

  OutputSample & operator=(const OutputSample &rhs) {
    coef = rhs.coef;
    level = rhs.level;
    return *this;
  };

  virtual void SetOutputSampleValue(sampleType coef)=0;
  virtual sampleType GetOutputSampleValue()=0;
  virtual void SetOutputSampleLevel(int level)=0;
  virtual int GetOutputSampleLevel()=0;

  virtual ostream & Print(ostream &os) const=0;
};

#endif
