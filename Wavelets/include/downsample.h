#ifndef _downsample
#define _downsample

#include <vector>
#include <iostream>

#include "util.h"
#include "sampleblock.h"

template <class OUTSAMPLE, class INSAMPLE>
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
  void DownSampleBuffer(SampleBlock<OUTSAMPLE> &output,
			SampleBlock<INSAMPLE>  &input);
  void DownSampleBuffer(SampleBlock<OUTSAMPLE> &output,
			SampleBlock<OUTSAMPLE> &input);


  ostream & Print(ostream &os) const;
};

template <class OUTSAMPLE, class INSAMPLE>
DownSample<OUTSAMPLE, INSAMPLE>::DownSample(unsigned rate=1)
{
  this->rate = rate;
  samplecount = 0;
}

template <class OUTSAMPLE, class INSAMPLE>
DownSample<OUTSAMPLE, INSAMPLE>::DownSample(const DownSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
}

template <class OUTSAMPLE, class INSAMPLE>
DownSample<OUTSAMPLE, INSAMPLE>::~DownSample()
{
}

template <class OUTSAMPLE, class INSAMPLE>
DownSample<OUTSAMPLE, INSAMPLE> & DownSample<OUTSAMPLE, INSAMPLE>::operator=
(const DownSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
  return *this;
}

template <class OUTSAMPLE, class INSAMPLE>
bool DownSample<OUTSAMPLE, INSAMPLE>::KeepSample()
{
  bool keep=false;
  if (samplecount%rate == 0) {
    keep = true;
  }

  samplecount++;
  return keep;
}

template <class OUTSAMPLE, class INSAMPLE>
void DownSample<OUTSAMPLE, INSAMPLE>::DownSampleBuffer
(SampleBlock<OUTSAMPLE> &output, SampleBlock<INSAMPLE> &input)
{
  output.ClearBlock();

  for (unsigned i=0; i<input.GetBlockSize(); i++) {
    if (KeepSample()) {
      INSAMPLE newin;
      input.GetSample(&newin,i);
      output.SetSample(newin);
    }
  }
}

template <class OUTSAMPLE, class INSAMPLE>
void DownSample<OUTSAMPLE, INSAMPLE>::DownSampleBuffer
(SampleBlock<OUTSAMPLE> &output, SampleBlock<OUTSAMPLE> &input)
{
  output.ClearBlock();

  for (unsigned i=0; i<input.GetBlockSize(); i++) {
    if (KeepSample()) {
      OUTSAMPLE newin;
      input.GetSample(&newin,i);
      output.SetSample(newin);
    }
  }
}

template <class OUTSAMPLE, class INSAMPLE>
ostream & DownSample<OUTSAMPLE, INSAMPLE>::Print(ostream &os) const
{
  os << "DownSample information:\n";
  os << "  Current downsample rate: " << rate << endl;
  os << "  Current sample count (state): " << samplecount << endl;
  return os;
}

#endif
