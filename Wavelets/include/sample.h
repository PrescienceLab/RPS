#ifndef _sample
#define _sample

#include "util.h"

class Sample {
private:
  double value;

public:
  Sample() { value = 0.0;};
  Sample(const Sample &rhs) { value = rhs.value;};
  Sample(double value) { this->value = value;};
  virtual ~Sample() {};

  Sample & operator=(const Sample &rhs) { value=rhs.value; return *this;};

  inline void SetSampleValue(double sample) { value = sample;};
  inline double GetSampleValue() { return value;};

  ostream & Print(ostream &os) const {
    os << "Sample Value: " << value << endl; return os;};
};

#endif
