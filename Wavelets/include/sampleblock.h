#ifndef _sampleblock
#define _sampleblock

#include <deque>
#include <iostream>
#include <cassert>

#include "util.h"

template <class SAMPLETYPE>
class SampleBlock {
protected:
  deque<SAMPLETYPE> samples;
  unsigned blockindex;

public:
  SampleBlock(const unsigned blockindex=0) { this->blockindex = blockindex; };

  SampleBlock(const SampleBlock &rhs) {
    samples = rhs.samples;
    blockindex = rhs.blockindex;
  };

  SampleBlock(const deque<SAMPLETYPE> &input) {
    samples = input;
    blockindex = 0;
  };

  SampleBlock(const deque<SAMPLETYPE> &input, const unsigned blockindex) {
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
      right = rhs[i];
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
      right = rhs[i];
      samples[i] = left+right;
    }
    return *this;
  };
    
  inline void SetSamples(const deque<SAMPLETYPE> &input) {
    samples = input;
  };

  // This routine gets all of the samples
  inline void GetSamples(deque<SAMPLETYPE> &buf) const {
    buf = samples;
  };

  // This routine gets a range of values from the buffer
  void GetSamples(deque<SAMPLETYPE> &buf, 
		  const unsigned first,
		  const unsigned last) const {
    if ((first >= 0) && (first < samples.size()) && (last >= first)) {
      buf.clear();
      for (unsigned i=first; i<last; i++) {
	buf.push_back(samples[i]);
      }
    }
  };

  inline SAMPLETYPE Front() const {
    return samples.front();
  };

  inline void PushSampleFront(const SAMPLETYPE &input) {
    samples.push_front(input);
  };

  inline void PopSampleFront() {
    samples.pop_front();
  };

  inline SAMPLETYPE Back() const {
    return samples.back();
  };

  inline void PushSampleBack(const SAMPLETYPE &input) {
    samples.push_back(input);
  };

  inline void PopSampleBack() {
    samples.pop_back();
  };

  // This is equivalent to random access []
  inline SAMPLETYPE operator[](const unsigned i) const {
    assert(i < samples.size());
    return samples[i];
  };

  inline void SetBlockIndex(const unsigned index) {
    this->blockindex = index;
  };

  inline unsigned GetBlockIndex() const {
    return blockindex;
  };

  void AppendBlockBack(const SampleBlock &block) {
    SAMPLETYPE newsamp;
    for (unsigned i=0; i<block.GetBlockSize(); i++) {
      newsamp = block[i];
      samples.push_back(newsamp);
    }
  };

  void AppendBlockFront(const SampleBlock &block) {
    SAMPLETYPE newsamp;
    for (int i=block.GetBlockSize()-1; i>=0; i--) {
      newsamp = block[i];
      samples.push_front(newsamp);
    }
  };

  void RemoveSamplesFront(const unsigned numsamples) {
    if (numsamples == samples.size()) {
      samples.clear();
    } else {
      for (unsigned i=0; i<numsamples; i++) {
	samples.pop_front();
      }
    }
  };

  void RemoveSamplesBack(const unsigned numsamples) {
    if (numsamples == samples.size()) {
      samples.clear();
    } else {
      for (unsigned i=0; i<numsamples; i++) {
	samples.pop_back();
      }
    }
  };

  inline void ClearBlock() {
    samples.clear();
  };

  inline bool Empty() const {
    return samples.empty();
  };

  inline unsigned GetBlockSize() const {
    return samples.size();
  };

  virtual SampleBlock* clone() const {
    return new SampleBlock(*this);
  };

  virtual ostream & Print(ostream &os) const {
    os << "SampleBlock::" << endl;
    for (unsigned i=0; i<samples.size(); i++) {
      cout << "\t" << i << "\t" << samples[i];
    }
    return os;
  };
  virtual ostream & operator<<(ostream &os) const { return Print(os);};
};

template <class SAMPLETYPE>
inline ostream & operator<<(ostream &os, const SampleBlock<SAMPLETYPE> &rhs) 
{ 
  return rhs.operator<<(os);
};

template <class SAMPLETYPE>
class InputSampleBlock : public SampleBlock<SAMPLETYPE> {
public:
  InputSampleBlock() {};
  InputSampleBlock(const InputSampleBlock &rhs) : 
    SampleBlock<SAMPLETYPE>(rhs) {};
  InputSampleBlock(const deque<SAMPLETYPE> &input) : 
    SampleBlock<SAMPLETYPE>(input) {};
  InputSampleBlock(const deque<SAMPLETYPE> &input, const unsigned index) :
    SampleBlock<SAMPLETYPE>(input,index) {};
  virtual ~InputSampleBlock() {};
};

template <class SAMPLETYPE>
class OutputSampleBlock : public SampleBlock<SAMPLETYPE> {
public:
  OutputSampleBlock() {};
  OutputSampleBlock(const OutputSampleBlock &rhs) : 
    SampleBlock<SAMPLETYPE>(rhs) {};
  OutputSampleBlock(const deque<SAMPLETYPE> &input) : 
    SampleBlock<SAMPLETYPE>(input) {};
  OutputSampleBlock(const deque<SAMPLETYPE> &input, const unsigned index) : 
    SampleBlock<SAMPLETYPE>(input,index) {};
  virtual ~OutputSampleBlock() {};
};

#endif
