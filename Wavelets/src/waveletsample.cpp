#include "waveletsample.h"

/********************************************************************************
 * Wavelet input samples
 *******************************************************************************/

/********************************************************************************
 * Wavelet output samples
 *******************************************************************************/
WaveletOutputSample::WaveletOutputSample() :
  OutputSample<double>()
{
}

WaveletOutputSample::WaveletOutputSample(const WaveletOutputSample &rhs) :
  OutputSample<double>(rhs), level(rhs.level), index(rhs.index)
{
}

WaveletOutputSample::WaveletOutputSample
(const double value, const int level, const int index)
  //  OutputSample<double>(value)
{
  this->value = value;
  this->level = level;
  this->index = index;
}

WaveletOutputSample::~WaveletOutputSample()
{
}

WaveletOutputSample & WaveletOutputSample::operator=(const Sample<double> &rhs)
{
  if (&rhs != this) {
    this->Sample<double>::operator=(rhs);
  }
  return *this;
}

WaveletOutputSample & WaveletOutputSample::operator=(const WaveletOutputSample &rhs)
{
  this->Sample<double>::operator=(rhs);
  this->level = rhs.level;
  this->index = rhs.index;
  return *this;
}

void WaveletOutputSample::SetSampleLevel(int level)
{
  this->level = level;
}

int WaveletOutputSample::GetSampleLevel() const
{
  return level;
}

void WaveletOutputSample::SetSampleIndex(int index)
{
  this->index = index;
}

int WaveletOutputSample::GetSampleIndex() const
{
  return index;
}
