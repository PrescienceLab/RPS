#ifndef _sampleblock
#define _sampleblock

#include <vector>
#include <iostream>

#include "sample.h"
#include "util.h"

template <class sampleType>
class InputSampleBlock {
private:
  vector< InputSample<sampleType> > samples;

public:
  InputSampleBlock() {
    samples.clear();
  };

  InputSampleBlock(const InputSampleBlock &rhs) {
    samples = rhs.samples;
  };

  InputSampleBlock(vector< InputSample<sampleType> > &input) {
    samples = input;
  };
  virtual ~InputSampleBlock() {};
  
  InputSampleBlock & operator=(const InputSampleBlock &rhs) {
    samples = rhs.samples;
    return *this;
  };

  virtual void SetSamples(vector< InputSample<sampleType> > &input)=0;
  virtual void GetSamples(vector< InputSample<sampleType> > &buf)=0;
  
  virtual ostream & Print(ostream &os) const =0;
};

#endif
