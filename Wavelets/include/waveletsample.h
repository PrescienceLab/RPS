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

  virtual WaveletInputSample & operator=(const Sample<double> &rhs);
  virtual WaveletInputSample & operator=(const double rhs);
};

class WaveletOutputSample : public OutputSample<double> { 
public:
  WaveletOutputSample();
  WaveletOutputSample(const WaveletOutputSample &rhs);
  WaveletOutputSample(double value, int level, int index);
  virtual ~WaveletOutputSample();

  virtual WaveletOutputSample & operator=(const Sample<double> &rhs);
  virtual WaveletOutputSample & operator=(const double rhs);
};

#endif
