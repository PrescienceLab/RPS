#include "waveletsampleout.h"

// Output samples
WaveletOutputSample::WaveletOutputSample() :
  OutputSample<double>()
{
}

WaveletOutputSample::WaveletOutputSample(const WaveletOutputSample &rhs) :
  OutputSample<double>(rhs)
{
}

WaveletOutputSample::WaveletOutputSample(double coef, int level) :
  OutputSample<double>(coef,level)
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

void WaveletOutputSample::SetOutputSampleValue(double coef)
{
  this->coef = coef;
}

double WaveletOutputSample::GetOutputSampleValue()
{
  return this->coef;
}

void WaveletOutputSample::SetOutputSampleLevel(int level)
{
  this->level = level;
}

int WaveletOutputSample::GetOutputSampleLevel()
{
  return this->level;
}

ostream & WaveletOutputSample::Print(ostream &os) const
{
  os << "level= " << level << ", coef= " << coef;
  return os;
}

// Block output samples, first incarnation is a vector
WaveletOutputSampleVector::WaveletOutputSampleVector()
{
  coefs.clear();
}

WaveletOutputSampleVector::WaveletOutputSampleVector
(const WaveletOutputSampleVector &rhs)
{
  coefs = rhs.coefs;
}

WaveletOutputSampleVector::WaveletOutputSampleVector
(vector<WaveletOutputSample> &outcoefs)
{
  coefs = outcoefs;
}

WaveletOutputSampleVector::~WaveletOutputSampleVector()
{
}

WaveletOutputSampleVector & 
WaveletOutputSampleVector::operator=(const WaveletOutputSampleVector &rhs)
{
  coefs = rhs.coefs;
  return *this;
}

void WaveletOutputSampleVector::SetWaveletCoef(WaveletOutputSample &coef)
{
  coefs.push_back(coef);
}

void WaveletOutputSampleVector::GetWaveletCoef(WaveletOutputSample *coef, unsigned i)
{
  if (i<coefs.size())
    *coef = coefs[i];
}

void WaveletOutputSampleVector::ClearBlock()
{
  coefs.clear();
}

unsigned WaveletOutputSampleVector::GetBlockSize() const
{
  return coefs.size();
}

ostream & WaveletOutputSampleVector::Print(ostream &os) const
{
  os << "WaveletOutputSampleVector::" << endl;
  for (unsigned i=0; i<GetBlockSize(); i++) {
    cout << coefs[i] << endl;
  }
  return os;
}
