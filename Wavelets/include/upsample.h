#ifndef _upsample
#define _upsample

#include <vector>
#include <iostream>

#include "util.h"
#include "sample.h"
#include "sampleblock.h"

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
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
  inline void     ResetState() { samplecount=0;};

  bool ZeroSample();
  void UpSampleBuffer(SampleBlock<OUTSAMPLE> &output,
		      SampleBlock<INSAMPLE>  &input);

  ostream & Print(ostream &os) const;
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
UpSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
UpSample(unsigned rate=1) : 
  rate(rate), samplecount(0)
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
UpSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
UpSample(const UpSample &rhs) :
  rate(rhs.rate), samplecount(rhs.samplecount)
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
UpSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~UpSample()
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
UpSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE> & 
UpSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator=(const UpSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
  return *this;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool UpSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ZeroSample()
{
  bool zero=true;
  if (samplecount%rate == 0) {
    zero = false;
  }

  samplecount++;
  return zero;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void UpSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
UpSampleBuffer(SampleBlock<OUTSAMPLE> &output, SampleBlock<INSAMPLE> &input)
{
  output.ClearBlock();
  INSAMPLE newin;

  unsigned i;
  for (i=0; i<input.GetBlockSize(); i++) {
    newin=0;
    while (ZeroSample()) {
      output.PushSampleBack(newin);
    }
    input.GetSample(&newin,i);
    output.PushSampleBack(newin);
  }

  // Take care of remaining zero samples
  newin=0;
  for (i=0; i<rate-1; i++) {
    output.PushSampleBack(newin);
  }
}

#if 0
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void UpSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
UpSampleBuffer(SampleBlock<INSAMPLE> &output, SampleBlock<INSAMPLE> &input)
{
  output.ClearBlock();
  INSAMPLE newin;

  unsigned i;
  for (i=0; i<input.GetBlockSize(); i++) {
    newin=0;
    while (ZeroSample()) {
      output.PushSampleBack(newin);
    }
    input.GetSample(&newin,i);
    output.PushSampleBack(newin);
  }

  // Take care of remaining zero samples
  newin=0;
  for (i=0; i<rate-1; i++) {
    output.PushSampleBack(newin);
  }
}
#endif

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & UpSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
Print(ostream &os) const
{
  os << "UpSample information:\n";
  os << "  Current upsample rate: " << rate << endl;
  os << "  Current sample count (state): " << samplecount << endl;
  return os;
}

#endif
