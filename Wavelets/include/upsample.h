#ifndef _upsample
#define _upsample

#include <vector>
#include <iostream>

#include "util.h"
#include "sampleblock.h"

template <class OUTSAMPLE, class INSAMPLE>
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

template <class OUTSAMPLE, class INSAMPLE>
UpSample<OUTSAMPLE, INSAMPLE>::UpSample(unsigned rate=1)
{
  this->rate = rate;
  samplecount = 0;
}

template <class OUTSAMPLE, class INSAMPLE>
UpSample<OUTSAMPLE, INSAMPLE>::UpSample(const UpSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
}

template <class OUTSAMPLE, class INSAMPLE>
UpSample<OUTSAMPLE, INSAMPLE>::~UpSample()
{
}

template <class OUTSAMPLE, class INSAMPLE>
UpSample<OUTSAMPLE, INSAMPLE> & UpSample<OUTSAMPLE, INSAMPLE>::operator=
(const UpSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
  return *this;
}

template <class OUTSAMPLE, class INSAMPLE>
bool UpSample<OUTSAMPLE, INSAMPLE>::ZeroSample()
{
  bool zero=true;
  if (samplecount%rate == 0) {
    zero = false;
  }

  samplecount++;
  return zero;
}

template <class OUTSAMPLE, class INSAMPLE>
void UpSample<OUTSAMPLE, INSAMPLE>::UpSampleBuffer
(SampleBlock<OUTSAMPLE> &output, SampleBlock<INSAMPLE> &input)
{
  output.ClearBlock();
  INSAMPLE newin;

  for (unsigned i=0; i<input.GetBlockSize(); i++) {
    newin=0;
    if (!ZeroSample()) {
      input.GetSample(&newin,i);
    }
    output.SetSample(newin);
  }
}

template <class OUTSAMPLE, class INSAMPLE>
ostream & UpSample<OUTSAMPLE, INSAMPLE>::Print(ostream &os) const
{
  os << "UpSample information:\n";
  os << "  Current upsample rate: " << rate << endl;
  os << "  Current sample count (state): " << samplecount << endl;
  return os;
}

#endif
