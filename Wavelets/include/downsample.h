#ifndef _downsample
#define _downsample

#include <vector>
#include <iostream>

#include "util.h"
#include "sample.h"
#include "sampleblock.h"

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
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
  inline unsigned GetDownSampleRate() const { return rate;};
  inline void     ResetState() { samplecount=0;};

  bool KeepSample();
  void DownSampleBuffer(SampleBlock<OUTSAMPLE> &output,
			SampleBlock<INSAMPLE>  &input);
  void DownSampleBuffer(SampleBlock<OUTSAMPLE> &output,
			SampleBlock<OUTSAMPLE> &input);


  ostream & Print(ostream &os) const;
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DownSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::DownSample(unsigned rate=1)
{
  this->rate = rate;
  samplecount = 0;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DownSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::DownSample(const DownSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DownSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::~DownSample()
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DownSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE> & 
DownSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::operator=
(const DownSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
  return *this;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DownSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::KeepSample()
{
  bool keep=false;
  if (samplecount%rate == 0) {
    keep = true;
  }

  samplecount++;
  return keep;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void DownSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::DownSampleBuffer
(SampleBlock<OUTSAMPLE> &output, SampleBlock<INSAMPLE> &input)
{
  output.ClearBlock();

  for (unsigned i=0; i<input.GetBlockSize(); i++) {
    if (KeepSample()) {
      Sample<SAMPLETYPE>  newin;
      input.GetSample(&newin,i);
      output.SetSample(newin);
    }
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void DownSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::DownSampleBuffer
(SampleBlock<OUTSAMPLE> &output, SampleBlock<OUTSAMPLE> &input)
{
  output.ClearBlock();

  for (unsigned i=0; i<input.GetBlockSize(); i++) {
    if (KeepSample()) {
      OUTSAMPLE  newin;
      input.GetSample(&newin,i);
      output.SetSample(newin);
    }
  }
}


template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & DownSample<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::Print(ostream &os) const
{
  os << "DownSample information:\n";
  os << "  Current downsample rate: " << rate << endl;
  os << "  Current sample count (state): " << samplecount << endl;
  return os;
}

#endif
