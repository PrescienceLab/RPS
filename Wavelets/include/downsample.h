#ifndef _downsample
#define _downsample

#include <vector>
#include <iostream>

#include "util.h"
#include "sample.h"
#include "sampleout.h"
#include "sampleblock.h"
#include "sampleblockout.h"

template <class outSample, class inSample>
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
  void DownSampleBuffer(OutputSampleBlock<outSample> &output,
			OutputSampleBlock<outSample> &input);
  void DownSampleBuffer(OutputSampleBlock<outSample> &output,
			InputSampleBlock<inSample>   &input);
  void DownSampleBuffer(InputSampleBlock<inSample>   &output,
			OutputSampleBlock<outSample> &input);
  void DownSampleBuffer(InputSampleBlock<inSample>   &output,
			InputSampleBlock<inSample>   &input);

  ostream & Print(ostream &os) const;
};

template <class outSample, class inSample>
DownSample<outSample, inSample>::DownSample(unsigned rate=1)
{
  this->rate = rate;
  samplecount = 0;
}

template <class outSample, class inSample>
DownSample<outSample, inSample>::DownSample(const DownSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
}

template <class outSample, class inSample>
DownSample<outSample, inSample>::~DownSample()
{
}

template <class outSample, class inSample>
DownSample<outSample, inSample> & DownSample<outSample, inSample>::operator=
(const DownSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
  return *this;
}

template <class outSample, class inSample>
bool DownSample<outSample, inSample>::KeepSample()
{
  bool keep=false;
  if (samplecount%rate == 0) {
    keep = true;
  }

  samplecount++;
  return keep;
}

template <class outSample, class inSample>
void DownSample<outSample, inSample>::DownSampleBuffer
(OutputSampleBlock<outSample> &output, OutputSampleBlock<outSample> &input)
{
  output.ClearBlock();

  for (unsigned i=0; i<input.GetBlockSize(); i++) {
    if (KeepSample()) {
      outSample newin;
      input.GetWaveletCoef(&newin,i);
      output.SetWaveletCoef(newin);
    }
  }
}

template <class outSample, class inSample>
void DownSample<outSample, inSample>::DownSampleBuffer
(OutputSampleBlock<outSample> &output, InputSampleBlock<inSample> &input)
{
  output.ClearBlock();

  for (unsigned i=0; i<input.GetBlockSize(); i++) {
    if (KeepSample()) {
      inSample newin;
      input.GetSample(&newin,i);
      output.SetWaveletCoef(newin);
    }
  }
}

template <class outSample, class inSample>
void DownSample<outSample, inSample>::DownSampleBuffer
(InputSampleBlock<inSample> &output, OutputSampleBlock<outSample> &input)
{
  output.ClearBlock();

  for (unsigned i=0; i<input.GetBlockSize(); i++) {
    if (KeepSample()) {
      outSample newin;
      input.GetWaveletCoef(&newin,i);
      output.SetSample(newin);
    }
  }
}

template <class outSample, class inSample>
void DownSample<outSample, inSample>::DownSampleBuffer
(InputSampleBlock<inSample> &output, InputSampleBlock<inSample> &input)
{
  output.ClearBlock();

  for (unsigned i=0; i<input.GetBlockSize(); i++) {
    if (KeepSample()) {
      inSample newin;
      input.GetSample(&newin,i);
      output.SetSample(newin);
    }
  }
}

template <class outSample, class inSample>
ostream & DownSample<outSample, inSample>::Print(ostream &os) const
{
  os << "DownSample information:\n";
  os << "  Current downsample rate: " << rate << endl;
  os << "  Current sample count (state): " << samplecount << endl;
  return os;
}

#endif
