#ifndef _wavesample
#define _wavesample

#include <vector>

#include "util.h"
#include "sample.h"
#include "sampleblock.h"

class WaveletInputSample : public InputSample<double> {
public:
  inline WaveletInputSample(double value=0.0);
  WaveletInputSample(const WaveletInputSample &rhs);
  virtual ~WaveletInputSample();

  WaveletInputSample & operator=(const WaveletInputSample &rhs);

  inline virtual void SetSampleValue(double sample);
  inline virtual double GetSampleValue();

  virtual ostream & Print(ostream &os) const;
};

class WaveletInputSampleBlock : public InputSampleBlock<WaveletInputSample> {
public:
  WaveletInputSampleBlock();
  WaveletInputSampleBlock(const WaveletInputSampleBlock &rhs);
  WaveletInputSampleBlock(const vector<WaveletInputSample> &rhs);
  virtual ~WaveletInputSampleBlock();

  WaveletInputSampleBlock & operator=(const WaveletInputSampleBlock &rhs);

  
  inline virtual void SetSamples(const vector<WaveletInputSample> &input);
  inline virtual void GetSamples(vector<WaveletInputSample> &outbuf);
  inline virtual void SetSample(WaveletInputSample &input);
  virtual void GetSample(WaveletInputSample *samp, unsigned i);
  inline virtual unsigned GetBlockSize();

  virtual ostream & Print(ostream &os) const;
};

#endif
