#ifndef _downsample
#define _downsample

#include <vector>
#include <iostream>

#include "util.h"
#include "sample.h"
#include "sampleblock.h"

template <class SAMPLE>
class DownSample {
private:
  unsigned rate;
  unsigned samplecount;

public:
  DownSample(const unsigned rate=1);
  DownSample(const DownSample &rhs);
  virtual ~DownSample();

  DownSample & operator=(const DownSample &rhs);

  inline void     SetDownSampleRate(const unsigned rate) { this->rate = rate;};
  inline unsigned GetDownSampleRate() const { return rate;};
  inline void     ResetState() { samplecount=0;};

  bool KeepSample();
  void DownSampleBuffer(SampleBlock<SAMPLE> &output,
			const SampleBlock<SAMPLE> &input);

  ostream & Print(ostream &os) const;
};

template <class SAMPLE>
DownSample<SAMPLE>::
DownSample(const unsigned rate=1)
{
  this->rate = rate;
  samplecount = 0;
}

template <class SAMPLE>
DownSample<SAMPLE>::
DownSample(const DownSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
}

template <class SAMPLE>
DownSample<SAMPLE>::
~DownSample()
{
}

template <class SAMPLE>
DownSample<SAMPLE> & 
DownSample<SAMPLE>::
operator=(const DownSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
  return *this;
}

template <class SAMPLE>
bool DownSample<SAMPLE>::
KeepSample()
{
  bool keep=false;
  if (samplecount%rate == 0) {
    keep = true;
  }

  samplecount++;
  return keep;
}

template <class SAMPLE>
void DownSample<SAMPLE>::
DownSampleBuffer(SampleBlock<SAMPLE> &output,
		 const SampleBlock<SAMPLE> &input)
{
  output.ClearBlock();

  for (unsigned i=0; i<input.GetBlockSize(); i++) {
    if (KeepSample()) {
      SAMPLE newin;
      input.GetSample(&newin,i);
      output.PushSampleBack(newin);
    }
  }
}

template <class SAMPLE>
ostream & DownSample<SAMPLE>::
Print(ostream &os) const
{
  os << "DownSample information:\n";
  os << "  Current downsample rate: " << rate << endl;
  os << "  Current sample count (state): " << samplecount << endl;
  return os;
}

#endif
