#ifndef _sampleblock
#define _sampleblock

#include <vector>
#include <iostream>

#include "waveletsample.h"
#include "sample.h"
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
  
  SampleBlock & operator=(const SampleBlock &rhs) {
    samples = rhs.samples;
    return *this;
  };

  virtual void SetSamples(const vector<SAMPLETYPE> &input)=0;
  virtual void GetSamples(vector<SAMPLETYPE> &buf)=0;

  virtual void SetSample(SAMPLETYPE &input)=0;
  virtual void GetSample(SAMPLETYPE *samp, unsigned i)=0;

  virtual void ClearBlock()=0;
  virtual unsigned GetBlockSize()=0;
  
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
  
  InputSampleBlock & operator=(const InputSampleBlock &rhs) {
    if (&rhs != this) {
      // invoke SampleBlock copy assignment operator
      this->SampleBlock<SAMPLETYPE>::operator=(rhs);
    }
    return *this;
  };

  inline virtual void SetSamples(const vector<SAMPLETYPE> &input) {
    samples = input;
  };

  inline virtual void GetSamples(vector<SAMPLETYPE> &buf) {
    buf = samples;
  };

  inline virtual void SetSample(SAMPLETYPE &input) {
    samples.push_back(input);
  };

  inline virtual void GetSample(SAMPLETYPE *samp, unsigned i) {
    if (i < samples.size())
      *samp = samples[i];
  };

  inline virtual void ClearBlock() {
    samples.clear();
  };

  inline virtual unsigned GetBlockSize() {
    return samples.size();
  };
  
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
  
  OutputSampleBlock & operator=(const OutputSampleBlock &rhs) {
    if (&rhs != this) {
      // invoke SampleBlock copy assignment operator
      this->SampleBlock<SAMPLETYPE>::operator=(rhs);
    }
    return *this;
  };

  inline virtual void SetSamples(const vector<SAMPLETYPE> &input) {
    samples = input;
  };

  inline virtual void GetSamples(vector<SAMPLETYPE> &buf) {
    buf = samples;
  };

  inline virtual void SetSample(SAMPLETYPE &input) {
    samples.push_back(input);
  };

  inline virtual void GetSample(SAMPLETYPE *samp, unsigned i) {
    if (i < samples.size())
      *samp = samples[i];
  };

  inline virtual void ClearBlock() {
    samples.clear();
  };

  inline virtual unsigned GetBlockSize() {
    return samples.size();
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
