#ifndef _sample
#define _sample

#include "util.h"

template <class sampleType>
class InputSample {
protected:
  sampleType value;

public:
  InputSample() {};
  inline InputSample(const InputSample &rhs) { value = rhs.value;};
  virtual ~InputSample() {};

  InputSample & operator=(const InputSample &rhs) {
    value = rhs.value;
    return *this;
  };

  virtual void SetSampleValue(sampleType sample)=0;
  virtual sampleType GetSampleValue()=0;

  virtual ostream & Print(ostream &os) const = 0;
};


#endif
