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
  OutputSample<double>(rhs), level(rhs.level)
{
}

WaveletOutputSample::WaveletOutputSample
(const double value, const int level, const unsigned index) :
  OutputSample<double>(value,index)
{
  this->level = level;
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
