#ifndef _upsample
#define _upsample

#include <vector>
#include <iostream>

#include "util.h"
#include "sample.h"
#include "sampleblock.h"

template <class SAMPLE>
class UpSample {
private:
  unsigned rate;
  unsigned samplecount;

public:
  UpSample(const unsigned rate=1);
  UpSample(const UpSample &rhs);
  virtual ~UpSample();

  UpSample & operator=(const UpSample &rhs);

  inline void SetUpSampleRate(const unsigned rate) { this->rate = rate;};
  inline unsigned GetUpSampleRate() const { return rate;};
  inline void ResetState() { samplecount=0;};

  bool ZeroSample();
  void UpSampleBuffer(SampleBlock<SAMPLE> &output,
		      const SampleBlock<SAMPLE> &input);

  ostream & Print(ostream &os) const;
  ostream & operator<<(ostream &os) const;
};

template <class SAMPLE>
inline ostream & operator<<(ostream &os, const UpSample<SAMPLE> &rhs) 
{ 
  return rhs.operator<<(os);
};

template <class SAMPLE>
UpSample<SAMPLE>::
UpSample(const unsigned rate) : 
  rate(rate), samplecount(0)
{
}

template <class SAMPLE>
UpSample<SAMPLE>::
UpSample(const UpSample &rhs) :
  rate(rhs.rate), samplecount(rhs.samplecount)
{
}

template <class SAMPLE>
UpSample<SAMPLE>::
~UpSample()
{
}

template <class SAMPLE>
UpSample<SAMPLE> & 
UpSample<SAMPLE>::
operator=(const UpSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
  return *this;
}

template <class SAMPLE>
bool UpSample<SAMPLE>::
ZeroSample()
{
  bool zero=true;
  if (samplecount%rate == 0) {
    zero = false;
  }

  samplecount++;
  return zero;
}

template <class SAMPLE>
void UpSample<SAMPLE>::
UpSampleBuffer(SampleBlock<SAMPLE> &output, const SampleBlock<SAMPLE> &input)
{
  output.ClearBlock();
  SAMPLE newin;

  unsigned i;
  for (i=0; i<input.GetBlockSize(); i++) {
    newin=0;
    while (ZeroSample()) {
      output.PushSampleBack(newin);
    }
    newin = input[i];
    output.PushSampleBack(newin);
  }

  // Take care of remaining zero samples
  newin=0;
  for (i=0; i<rate-1; i++) {
    output.PushSampleBack(newin);
  }
}

template <class SAMPLE>
ostream & UpSample<SAMPLE>::
Print(ostream &os) const
{
  os << "UpSample information:\n";
  os << "  Current upsample rate: " << rate << endl;
  os << "  Current sample count (state): " << samplecount << endl;
  return os;
}

#endif
