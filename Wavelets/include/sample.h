#ifndef _sample
#define _sample

#include <iostream>
#include "util.h"

template <typename SAMPLETYPE>
class Sample {
protected:
  SAMPLETYPE value;

public:
  Sample() : value(0) {};
  inline Sample(const Sample &rhs) { value = rhs.value;};
  inline Sample(const SAMPLETYPE value) { this->value = value;};
  virtual ~Sample() {};

  virtual Sample & operator=(const Sample &rhs) {
    value = rhs.value;
    return *this;
  };

  virtual Sample & operator=(const SAMPLETYPE rhs) {
    value = rhs;
    return *this;
  };

  virtual Sample & operator+(const SAMPLETYPE rhs) {
    value = value + rhs;
    return *this;
  };

  virtual Sample & operator+(const Sample &rhs) {
    value = value + rhs.value;
    return *this;
  } 

  virtual Sample & operator+=(const SAMPLETYPE rhs) {
    value = value + rhs;
    return *this;
  };

  virtual Sample & operator+=(const Sample &rhs) {
    value = value + rhs.value;
    return *this;
  } 

  virtual SAMPLETYPE operator*(const double rhs) {
    return rhs*value;
  };

  virtual inline void SetSampleValue(SAMPLETYPE sample) {
    value = sample;
  };

  virtual inline SAMPLETYPE GetSampleValue() {
    return value;
  };

  virtual ostream & Print(ostream &os) const=0;
};

template <typename SAMPLETYPE>
class InputSample : public Sample<SAMPLETYPE> {
public:
  InputSample() {};
  InputSample(const InputSample &rhs) : Sample<SAMPLETYPE>(rhs) {};  
  InputSample(const SAMPLETYPE value) : Sample<SAMPLETYPE>(value) {};
  virtual ~InputSample() {};

  virtual ostream & Print(ostream &os) const {
    os << value << endl;
    return os;
  };
};

template <typename SAMPLETYPE>
class OutputSample : public Sample<SAMPLETYPE> {
protected:
  int level;
  int index;

public:
  OutputSample() : level(-1), index(-1) {};
  OutputSample(const OutputSample &rhs) : 
    Sample<SAMPLETYPE>(rhs.value), level(rhs.level), index(rhs.index) {};

  OutputSample(SAMPLETYPE value) : 
    Sample<SAMPLETYPE>(value), level(-1), index(-1) {};
  OutputSample(SAMPLETYPE value, int level, int index) : Sample<SAMPLETYPE>(value) {
    this->level = level;
    this->index = index;
  };

  virtual ~OutputSample() {};

  virtual OutputSample & operator=(const OutputSample &rhs) {
    value = rhs.value;
    level = rhs.level;
    index = rhs.index;
    return *this;
  };

  virtual inline void SetSampleLevel(int level) {
    this->level = level;
  };

  virtual inline int GetSampleLevel() {
    return level;
  };

  virtual inline void SetSampleIndex(int index) {
    this->index = index;
  };

  virtual inline int GetSampleIndex() {
    return index;
  };

  virtual ostream & Print(ostream &os) const {
    os << "level: " << level << "\tvalue: " << value << "\tindex: " << index << endl;
    return os;
  };
};

#endif
