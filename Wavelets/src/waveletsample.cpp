#include "waveletsample.h"

/********************************************************************************
 * Wavelet input samples
 *******************************************************************************/
WaveletInputSample::WaveletInputSample() :
  InputSample<double>()
{
}

WaveletInputSample::WaveletInputSample(double value) :
  InputSample<double>(value)
{
}

WaveletInputSample::WaveletInputSample(const WaveletInputSample &rhs) :
  InputSample<double>(rhs)
{
}

WaveletInputSample::~WaveletInputSample()
{
}

WaveletInputSample & WaveletInputSample::operator=(const Sample<double> &rhs) 
{
  if (&rhs != this) {
    // invoke InputSample copy assignment operator
    this->Sample<double>::operator=(rhs);
  }
  return *this;
}

WaveletInputSample & WaveletInputSample::operator=(const double rhs)
{
  this->Sample<double>::operator=(rhs);
  return *this;
}

/********************************************************************************
 * Output samples
 *******************************************************************************/
WaveletOutputSample::WaveletOutputSample() :
  OutputSample<double>()
{
}

WaveletOutputSample::WaveletOutputSample(const WaveletOutputSample &rhs) :
  OutputSample<double>(rhs)
{
}

WaveletOutputSample::WaveletOutputSample(double value, int level, int index) :
  OutputSample<double>(value,level,index)
{
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

WaveletOutputSample & WaveletOutputSample::operator=(const double rhs)
{
  this->Sample<double>::operator=(rhs);
  return *this;
}
