#ifndef _sampleblock
#define _sampleblock

#include <vector>
#include <iostream>

#include "util.h"

template <class SAMPLETYPE>
class SampleBlock {
protected:
  vector<SAMPLETYPE> samples;
  unsigned           blockindex;

public:
  SampleBlock(const unsigned blockindex=0) { this->blockindex = blockindex; };

  SampleBlock(const SampleBlock &rhs) {
    samples = rhs.samples;
    blockindex = rhs.blockindex;
  };

  SampleBlock(const vector<SAMPLETYPE> &input) {
    samples = input;
    blockindex = 0;
  };

  SampleBlock(const vector<SAMPLETYPE> &input, const unsigned blockindex) {
    samples = input;
    this->blockindex = blockindex;
  };

  virtual ~SampleBlock() {};
  
  virtual SampleBlock & operator=(const SampleBlock &rhs) {
    samples = rhs.samples;
    blockindex = rhs.blockindex;
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
    
  inline void SetSamples(const vector<SAMPLETYPE> &input) {
    samples = input;
  };

  inline void GetSamples(vector<SAMPLETYPE> &buf) const {
    buf = samples;
  };

  inline void SetSample(SAMPLETYPE &input) {
    samples.push_back(input);
  };

  inline void GetSample(SAMPLETYPE *samp, unsigned i) const {
    if (i < samples.size())
      *samp = samples[i];
  };

  inline void SetBlockIndex(const unsigned index) {
    this->blockindex = index;
  };

  inline unsigned GetBlockIndex() {
    return blockindex;
  };

  void AppendBlock(SampleBlock &block) {
    SAMPLETYPE newsamp;
    for (unsigned i=0; i<block.GetBlockSize(); i++) {
      block.GetSample(&newsamp,i);
      samples.push_back(newsamp);
    }
  };

  inline void ClearBlock() {
    samples.clear();
  };

  inline unsigned GetBlockSize() const {
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
  InputSampleBlock(const vector<SAMPLETYPE> &input, const unsigned index) :
    SampleBlock<SAMPLETYPE>(input,index) {};
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
  OutputSampleBlock(const vector<SAMPLETYPE> &input, const unsigned index) : 
    SampleBlock<SAMPLETYPE>(input,index) {};
  virtual ~OutputSampleBlock() {};
};

#endif
