#ifndef _downsample
#define _downsample

#include <vector>
#include <iostream>

#include "util.h"

class DownSample {
private:
  unsigned rate;
  unsigned samplecount;

public:
  DownSample(unsigned rate=1);
  DownSample(const DownSample &rhs);
  virtual ~DownSample();

  DownSample & operator=(const DownSample &rhs);

  inline void     SetDownSampleRate(unsigned rate) { this->rate = rate;};
  inline unsigned GetDownSampleRate() { return rate;};
  inline void     ResetState() { samplecount=0;};

  bool KeepSample();
  void DownSampleBuffer(vector<double> &output, vector<double> &input);

  ostream & Print(ostream &os) const;
};

#endif
