#ifndef _sample
#define _sample

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

public:
  OutputSample() : level(-1) {};
  OutputSample(const OutputSample &rhs) : 
    Sample<SAMPLETYPE>(rhs.value), level(rhs.level) {};

  OutputSample(SAMPLETYPE value) : Sample<SAMPLETYPE>(value), level(-1) {};
  OutputSample(SAMPLETYPE value, int level) : Sample<SAMPLETYPE>(value) {
    this->level = level;
  };

  virtual ~OutputSample() {};

  virtual OutputSample & operator=(const OutputSample &rhs) {
    value = rhs.value;
    level = rhs.level;
    return *this;
  };

  virtual inline void SetSampleLevel(int level) {
    this->level = level;
  };

  virtual inline int GetSampleLevel() {
    return level;
  };

  virtual ostream & Print(ostream &os) const {
    os << "level: " << level << "\tvalue: " << value << endl;
    return os;
  };
};

#endif