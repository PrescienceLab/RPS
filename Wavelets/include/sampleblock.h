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

  virtual SampleBlock & operator+(const SampleBlock &rhs) {
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

  virtual SampleBlock & operator+=(const SampleBlock &rhs) {
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
    
  virtual void SetSamples(const vector<SAMPLETYPE> &input) {
    samples = input;
  };

  virtual void GetSamples(vector<SAMPLETYPE> &buf) const {
    buf = samples;
  };

  virtual void SetSample(SAMPLETYPE &input) {
    samples.push_back(input);
  };

  virtual void GetSample(SAMPLETYPE *samp, unsigned i) const {
    if (i < samples.size())
      *samp = samples[i];
  };

  virtual void ClearBlock() {
    samples.clear();
  };

  virtual unsigned GetBlockSize() const {
    return samples.size();
  };

  virtual SampleBlock* clone()=0;
  virtual void SetBlockLevel(int level)=0;
  virtual int GetBlockLevel()=0;

  virtual ostream & Print(ostream &os) const=0;
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

  virtual InputSampleBlock* clone() {
    return new InputSampleBlock(*this);
  };
  
  virtual void SetBlockLevel(int level) {};
  virtual int GetBlockLevel() { return -1;};

  virtual ostream & Print(ostream &os) const {
    os << "InputSampleBlock::" << endl;
    for (unsigned i=0; i<samples.size(); i++) {
      cout << "\t" << i << "\t" << samples[i];
    }
    return os;
  };
};

template <class SAMPLETYPE>
class OutputSampleBlock : public SampleBlock<SAMPLETYPE> {
public:
  OutputSampleBlock() {};

  OutputSampleBlock(const OutputSampleBlock &rhs) :
    SampleBlock<SAMPLETYPE>(rhs) {};

  OutputSampleBlock(const vector<SAMPLETYPE> &output) :
    SampleBlock<SAMPLETYPE>(output) {};

  virtual ~OutputSampleBlock() {};

  virtual OutputSampleBlock* clone() {
    return new OutputSampleBlock(*this);
  };

  virtual void SetBlockLevel(int level) {
    if (!samples.empty()) {
      for (unsigned i=0; i<samples.size(); i++) {
	samples[i].SetSampleLevel(level);
      }
    }
  };

  virtual int GetBlockLevel() {
    int tlevel = -1;
    if (!samples.empty()) {
      tlevel = samples[0].GetSampleLevel();
      
      // Force unified level representation in block
      for (unsigned i=1; i<samples.size(); i++) {
	if (tlevel != samples[i].GetSampleLevel()) {
	  samples[i].SetSampleLevel(tlevel);
	}
      }
    }
    return tlevel;
  };

  virtual ostream & Print(ostream &os) const {
    os << "OutputSampleBlock::" << endl;
    for (unsigned i=0; i<samples.size(); i++) {
      cout << "\t" << i << "\t" << samples[i];
    }
    return os;
  };

};

#endif
