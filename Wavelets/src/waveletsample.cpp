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

WaveletInputSample & WaveletInputSample::operator=(const WaveletInputSample &rhs) 
{
  if (&rhs != this) {
    // invoke InputSample copy assignment operator
    this->InputSample<double>::operator=(rhs);
  }
  return *this;
}

WaveletInputSample & WaveletInputSample::operator=(const double rhs)
{
  this->InputSample<double>::operator=(rhs);
  return *this;
}

WaveletInputSample & WaveletInputSample::operator+=(const WaveletInputSample &rhs)
{
  this->InputSample<double>::operator+=(rhs);
  return *this;
}

WaveletInputSample & WaveletInputSample::operator+=(const double rhs)
{
  this->InputSample<double>::operator+=(rhs);
  return *this;
}

double WaveletInputSample::operator*(const double rhs)
{
  return this->InputSample<double>::operator*(rhs);
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

WaveletOutputSample::WaveletOutputSample(double value, int level) :
  OutputSample<double>(value,level)
{
}

WaveletOutputSample::~WaveletOutputSample()
{
}

WaveletOutputSample & WaveletOutputSample::operator=(const WaveletOutputSample &rhs)
{
  if (&rhs != this) {
    this->OutputSample<double>::operator=(rhs);
  }
  return *this;
}

WaveletOutputSample & WaveletOutputSample::operator=(const double rhs)
{
  this->OutputSample<double>::operator=(rhs);
  return *this;
}

WaveletOutputSample & WaveletOutputSample::operator+=(const WaveletOutputSample &rhs)
{
  this->OutputSample<double>::operator+=(rhs);
  return *this;
}

WaveletOutputSample & WaveletOutputSample::operator+=(const double rhs)
{
  this->OutputSample<double>::operator+=(rhs);
  return *this;
}

double WaveletOutputSample::operator*(const double rhs)
{
  return this->OutputSample<double>::operator*(rhs);
}
