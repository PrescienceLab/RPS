#ifndef _waveletsample
#define _waveletsample

#include "util.h"
#include "sample.h"

class WaveletInputSample : public InputSample<double> {
public:
  WaveletInputSample();
  WaveletInputSample(const WaveletInputSample &rhs);
  WaveletInputSample(double value);
  virtual ~WaveletInputSample();

  WaveletInputSample & operator=(const WaveletInputSample &rhs);
  WaveletInputSample & operator=(const double rhs);
  WaveletInputSample & operator+=(const WaveletInputSample &rhs);
  WaveletInputSample & operator+=(const double rhs);
  inline double operator*(const double rhs);
};

class WaveletOutputSample : public OutputSample<double> { 
public:
  WaveletOutputSample();
  WaveletOutputSample(const WaveletOutputSample &rhs);
  WaveletOutputSample(double value, int level);
  virtual ~WaveletOutputSample();

  WaveletOutputSample & operator=(const WaveletOutputSample &rhs);
  WaveletOutputSample & operator=(const double rhs);
  WaveletOutputSample & operator+=(const WaveletOutputSample &rhs);
  WaveletOutputSample & operator+=(const double rhs);
  inline double operator*(const double rhs);
};

#endif
