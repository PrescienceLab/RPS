#ifndef _sampleblock
#define _sampleblock

#include <vector>
#include <iostream>

#include "sample.h"
#include "util.h"

template <class sampleType>
class InputSampleBlock {
protected:
  vector<sampleType> samples;

public:
  InputSampleBlock() {
    samples.clear();
  };

  InputSampleBlock(const InputSampleBlock &rhs) {
    samples = rhs.samples;
  };

  InputSampleBlock(const vector<sampleType> &input) {
    samples = input;
  };
  virtual ~InputSampleBlock() {};
  
  InputSampleBlock & operator=(const InputSampleBlock &rhs) {
    samples = rhs.samples;
    return *this;
  };

  virtual void SetSamples(const vector<sampleType> &input)=0;
  virtual void GetSamples(vector<sampleType> &buf)=0;
  virtual void SetSample(sampleType &input)=0;
  virtual void GetSample(sampleType *samp, unsigned i)=0;
  virtual unsigned GetBlockSize()=0;
  
  virtual ostream & Print(ostream &os) const =0;
};

#endif
