#ifndef _sample
#define _sample

#include "util.h"

template <sampleType>
class InputSample {
public:
  Sample();
  Sample(const Sample &rhs);
  virtual ~Sample();

  virtual Sample & operator=(const Sample &rhs)=0;

  virtual void SetSampleValue(sampleType sample)=0;
  virtual sampleType GetSampleValue()=0;

  virtual ostream & Print(ostream &os) const = 0;
};

#endif
