#include "waveletsample.h"

// Individual samples
WaveletInputSample::WaveletInputSample(double value)
{
  this->value = value;
}

WaveletInputSample::WaveletInputSample(const WaveletInputSample &rhs) :
  InputSample<double>(rhs)
{
}

WaveletInputSample::~WaveletInputSample()
{
}

WaveletInputSample & WaveletInputSample::operator=(const WaveletInputSample &rhs) 
{
  if (&rhs != this) {
    // invoke InputSample copy assignment operator
    this->InputSample<double>::operator=(rhs);
  }
  return *this;
}

void WaveletInputSample::SetSampleValue(double sample) 
{
  value = sample;
}

double WaveletInputSample::GetSampleValue() 
{
  return value;
}

ostream & WaveletInputSample::Print(ostream &os) const
{
  os << value;
  return os;
}

// Blocks of wavelet input samples
WaveletInputSampleBlock::WaveletInputSampleBlock()
{
}

WaveletInputSampleBlock::WaveletInputSampleBlock
(const WaveletInputSampleBlock &rhs) :
  InputSampleBlock<WaveletInputSample>(rhs)
{
}

WaveletInputSampleBlock::WaveletInputSampleBlock
(const vector<WaveletInputSample> &rhs) :
  InputSampleBlock<WaveletInputSample>(rhs)
{
}

WaveletInputSampleBlock::~WaveletInputSampleBlock()
{
}

WaveletInputSampleBlock & 
WaveletInputSampleBlock::operator=(const WaveletInputSampleBlock & rhs)
{
  if (&rhs != this) {
    // invoke InputSampleBlock copy assignment operator
    this->InputSampleBlock<WaveletInputSample>::operator=(rhs);
  }
  return *this;
}

void WaveletInputSampleBlock::SetSamples(const vector<WaveletInputSample> &input)
{
  samples = input;
}

void WaveletInputSampleBlock::GetSamples(vector<WaveletInputSample> &outbuf)
{
  outbuf = samples;
}

void WaveletInputSampleBlock::SetSample(WaveletInputSample &input)
{
  samples.push_back(input);
}

void WaveletInputSampleBlock::GetSample(WaveletInputSample *samp, unsigned i)
{
  if (i<samples.size())
    *samp = samples[i];
}

void WaveletInputSampleBlock::ClearBlock()
{
  samples.clear();
}

unsigned WaveletInputSampleBlock::GetBlockSize()
{
  return samples.size();
}

ostream & WaveletInputSampleBlock::Print(ostream &os) const
{
  os << "WaveletInputSampleBlock::" << endl;
  for (unsigned i=0; i<samples.size(); i++) {
    cout << "\t" << i << "\t" << samples[i];
  }
  return os;
}
