#ifndef _sample
#define _sample

#include <iostream>
#include "util.h"

template <typename SAMPLETYPE>
class Sample {
protected:
  SAMPLETYPE value;
  unsigned index;

public:
  Sample(const SAMPLETYPE value=0, const unsigned index=0) { 
    this->value = value;
    this->index = index;
  };

  inline Sample(const Sample &rhs) { value = rhs.value; index = rhs.index; };
  virtual ~Sample() {};

  virtual Sample<SAMPLETYPE> & operator=(const Sample &rhs) {
    value = rhs.value;
    index = rhs.index;
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

  inline void SetSampleValue(const SAMPLETYPE sample) {
    value = sample;
  };

  inline SAMPLETYPE GetSampleValue() const {
    return value;
  };

  virtual inline void SetSampleIndex(const unsigned index) { 
    this->index = index; 
  };

  virtual inline unsigned GetSampleIndex() const {
    return index;
  };

  virtual ostream & Print(ostream &os) const {
    os << "index:" << index << "\tvalue:" << value << endl;
    return os;
  };

  virtual ostream & operator<<(ostream &os) const { return Print(os);}
    
};

template <class SAMPLETYPE>
inline ostream & operator<<(ostream &os, const Sample<SAMPLETYPE> &rhs) { return rhs.operator<<(os);};


template <typename SAMPLETYPE>
class InputSample : public Sample<SAMPLETYPE> {
public:
  InputSample(const SAMPLETYPE value=0, const unsigned index=0) : 
    Sample<SAMPLETYPE>(value,index) {};
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
  OutputSample(const SAMPLETYPE value=0, const unsigned index=0) : 
    Sample<SAMPLETYPE>(value,index) {};
  OutputSample(const OutputSample &rhs) : Sample<SAMPLETYPE>(rhs) {};
  virtual ~OutputSample() {};
};

#endif
