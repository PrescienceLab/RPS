#ifndef _upsample
#define _upsample

#include <vector>
#include <iostream>

#include "util.h"

class UpSample {
private:
  unsigned rate;
  unsigned samplecount;

public:
  UpSample(unsigned rate=1);
  UpSample(const UpSample &rhs);
  virtual ~UpSample();

  UpSample & operator=(const UpSample &rhs);

  inline void     SetUpSampleRate(unsigned rate) { this->rate = rate;};
  inline unsigned GetUpSampleRate() { return rate;};
  inline unsigned GetNumberZeroFill() { return rate-1;};
  inline void     ResetState() { samplecount=0;};

  bool ZeroSample();
  void UpSampleBuffer(vector<double> &output, vector<double> &input);

  ostream & Print(ostream &os) const;
};

#endif
