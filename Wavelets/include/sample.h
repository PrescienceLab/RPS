#ifndef _sample
#define _sample

#include <iostream>
#include "util.h"

template <typename SAMPLETYPE>
class Sample {
protected:
  SAMPLETYPE value;

public:
  Sample(const SAMPLETYPE value=0) { this->value = value;};
  inline Sample(const Sample &rhs) { value = rhs.value;};
  virtual ~Sample() {};

  virtual Sample<SAMPLETYPE> & operator=(const Sample &rhs) {
    value = rhs.value;
    return *this;
  };

  Sample<SAMPLETYPE> & operator=(const SAMPLETYPE rhs) {
    value = rhs;
    return *this;
  };

  Sample<SAMPLETYPE> & operator+(const SAMPLETYPE rhs) {
    value = value + rhs;
    return *this;
  };

  Sample<SAMPLETYPE> & operator+(const Sample &rhs) {
    value = value + rhs.value;
    return *this;
  } 

  Sample<SAMPLETYPE> & operator+=(const SAMPLETYPE rhs) {
    value = value + rhs;
    return *this;
  };

  Sample<SAMPLETYPE> & operator+=(const Sample &rhs) {
    value = value + rhs.value;
    return *this;
  } 

  SAMPLETYPE operator*(const double rhs) {
    return rhs*value;
  };

  inline void SetSampleValue(SAMPLETYPE sample) {
    value = sample;
  };

  inline SAMPLETYPE GetSampleValue() {
    return value;
  };

  virtual void SetSampleLevel(int level) {};
  virtual int GetSampleLevel() { return -1; };


  virtual ostream & Print(ostream &os) const {
    os << value << endl;
    return os;
  };
};

template <typename SAMPLETYPE>
class InputSample : public Sample<SAMPLETYPE> {
public:
  InputSample(const SAMPLETYPE value=0) : Sample<SAMPLETYPE>(value) {};
  InputSample(const InputSample &rhs) : Sample<SAMPLETYPE>(rhs) {};
  virtual ~InputSample() {};

  virtual InputSample<SAMPLETYPE> & operator=(const Sample<SAMPLETYPE> &rhs) {
    if (&rhs != this) {
      this->Sample<SAMPLETYPE>::operator=(rhs);
    }
    return *this; 
  };
};

template <typename SAMPLETYPE>
class OutputSample : public Sample<SAMPLETYPE> {
public:
  OutputSample(const SAMPLETYPE value=0) : Sample<SAMPLETYPE>(value) {};
  OutputSample(const OutputSample &rhs) : Sample<SAMPLETYPE>(rhs) {};
  virtual ~OutputSample() {};
};

#endif
