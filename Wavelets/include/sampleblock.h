#ifndef _sampleblock
#define _sampleblock

#include <vector>
#include <iostream>

#include "util.h"

template <class SAMPLETYPE>
class SampleBlock {
protected:
  vector<SAMPLETYPE> samples;

public:
  SampleBlock() {};

  SampleBlock(const SampleBlock &rhs) {
    samples = rhs.samples;
  };

  SampleBlock(const vector<SAMPLETYPE> &input) {
    samples = input;
  };

  virtual ~SampleBlock() {};
  
  virtual SampleBlock & operator=(const SampleBlock &rhs) {
    samples = rhs.samples;
    return *this;
  };

  SampleBlock & operator+(const SampleBlock &rhs) {
    unsigned minsize = (GetBlockSize() <= rhs.GetBlockSize()) ?
      GetBlockSize() : rhs.GetBlockSize();

    for (unsigned i=0; i<minsize; i++) {
      SAMPLETYPE left, right;
      left = samples[i];
      rhs.GetSample(&right, i);
      samples[i] = left+right;
    }
    return *this;
  };

  SampleBlock & operator+=(const SampleBlock &rhs) {
    unsigned minsize = (GetBlockSize() <= rhs.GetBlockSize()) ?
      GetBlockSize() : rhs.GetBlockSize();

    for (unsigned i=0; i<minsize; i++) {
      SAMPLETYPE left, right;
      left = samples[i];
      rhs.GetSample(&right, i);
      samples[i] = left+right;
    }
    return *this;
  };
    
  void SetSamples(const vector<SAMPLETYPE> &input) {
    samples = input;
  };

  void GetSamples(vector<SAMPLETYPE> &buf) const {
    buf = samples;
  };

  void SetSample(SAMPLETYPE &input) {
    samples.push_back(input);
  };

  void GetSample(SAMPLETYPE *samp, unsigned i) const {
    if (i < samples.size())
      *samp = samples[i];
  };

  void AppendBlock(SampleBlock &block) {
    SAMPLETYPE newsamp;
    for (unsigned i=0; i<block.GetBlockSize(); i++) {
      block.GetSample(&newsamp,i);
      samples.push_back(newsamp);
    }
  };

  void ClearBlock() {
    samples.clear();
  };

  unsigned GetBlockSize() const {
    return samples.size();
  };

  virtual SampleBlock* clone() {
    return new SampleBlock(*this);
  };

  virtual ostream & Print(ostream &os) const {
    os << "SampleBlock::" << endl;
    for (unsigned i=0; i<samples.size(); i++) {
      cout << "\t" << i << "\t" << samples[i];
    }
    return os;
  };

  virtual void SetBlockLevel(int level) {};
  virtual int GetBlockLevel() const { return -1;};
};

template <class SAMPLETYPE>
class InputSampleBlock : public SampleBlock<SAMPLETYPE> {
public:
  InputSampleBlock() {};
  InputSampleBlock(const InputSampleBlock &rhs) : 
    SampleBlock<SAMPLETYPE>(rhs) {};
  InputSampleBlock(const vector<SAMPLETYPE> &input) : 
    SampleBlock<SAMPLETYPE>(input) {};
  virtual ~InputSampleBlock() {};
};

template <class SAMPLETYPE>
class OutputSampleBlock : public SampleBlock<SAMPLETYPE> {
public:
  OutputSampleBlock() {};
  OutputSampleBlock(const OutputSampleBlock &rhs) : 
    SampleBlock<SAMPLETYPE>(rhs) {};
  OutputSampleBlock(const vector<SAMPLETYPE> &input) : 
    SampleBlock<SAMPLETYPE>(input) {};
  virtual ~OutputSampleBlock() {};
};

#endif
