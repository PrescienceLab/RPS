#ifndef _waveletsample
#define _waveletsample

#include "util.h"
#include "sample.h"

template <class SAMPLETYPE>
class WaveletInputSample : public InputSample<SAMPLETYPE> {
public:
  WaveletInputSample(const SAMPLETYPE value=0, const unsigned index=0);
  WaveletInputSample(const WaveletInputSample &rhs);
  virtual ~WaveletInputSample();

  virtual WaveletInputSample<SAMPLETYPE> & operator=
    (const Sample<SAMPLETYPE> &rhs);
};

template <class SAMPLETYPE>
class WaveletOutputSample : public OutputSample<SAMPLETYPE> { 
protected:
  int level;

public:
  WaveletOutputSample();
  WaveletOutputSample(const WaveletOutputSample &rhs);
  WaveletOutputSample(const SAMPLETYPE value,
		      const int level,
		      const unsigned index);
  virtual ~WaveletOutputSample();

  virtual WaveletOutputSample & operator=(const Sample<SAMPLETYPE> &rhs);
  WaveletOutputSample & operator=(const WaveletOutputSample &rhs);

  inline void SetSampleLevel(const int level);
  inline int GetSampleLevel() const;

  virtual ostream & Print(ostream &os) const {
    os << "index: " << index << "\tvalue: " << value << "\tlevel: " 
       << level << endl;
    return os;
  };
  virtual ostream & operator<<(ostream &os) const { return Print(os);}
};

template <class SAMPLETYPE>
inline ostream & operator<<(ostream &os, const WaveletOutputSample<SAMPLETYPE> &rhs) 
{ 
  return rhs.operator<<(os);
};

/********************************************************************************
 * Wavelet input samples
 *******************************************************************************/
template <class SAMPLETYPE>
WaveletInputSample<SAMPLETYPE>::
WaveletInputSample(const SAMPLETYPE value, const unsigned index) : 
  InputSample<SAMPLETYPE>(value,index)
{}

template <class SAMPLETYPE>
WaveletInputSample<SAMPLETYPE>::
WaveletInputSample(const WaveletInputSample &rhs) : 
  InputSample<SAMPLETYPE>(rhs) 
{}

template <class SAMPLETYPE>
WaveletInputSample<SAMPLETYPE>::
~WaveletInputSample()
{}

template <class SAMPLETYPE>
WaveletInputSample<SAMPLETYPE> & WaveletInputSample<SAMPLETYPE>::
operator=(const Sample<SAMPLETYPE> &rhs)
{
  if (&rhs != this) {
    this->Sample<SAMPLETYPE>::operator=(rhs);
  }
  return *this; 
}

/********************************************************************************
 * Wavelet output samples
 *******************************************************************************/
template <class SAMPLETYPE>
WaveletOutputSample<SAMPLETYPE>::
WaveletOutputSample() :
  OutputSample<SAMPLETYPE>()
{
  this->level = 0;  
}

template <class SAMPLETYPE>
WaveletOutputSample<SAMPLETYPE>::
WaveletOutputSample(const WaveletOutputSample &rhs) :
  OutputSample<SAMPLETYPE>(rhs), level(rhs.level)
{
}

template <class SAMPLETYPE>
WaveletOutputSample<SAMPLETYPE>::
WaveletOutputSample(const SAMPLETYPE value,
		    const int level,
		    const unsigned index) :
  OutputSample<SAMPLETYPE>(value,index)
{
  this->level = level;
}

template <class SAMPLETYPE>
WaveletOutputSample<SAMPLETYPE>::
~WaveletOutputSample()
{
}

template <class SAMPLETYPE>
WaveletOutputSample<SAMPLETYPE> & WaveletOutputSample<SAMPLETYPE>::
operator=(const Sample<SAMPLETYPE> &rhs)
{
  if (&rhs != this) {
    this->Sample<SAMPLETYPE>::operator=(rhs);
  }
  return *this;
}

template <class SAMPLETYPE>
WaveletOutputSample<SAMPLETYPE> & WaveletOutputSample<SAMPLETYPE>::
operator=(const WaveletOutputSample &rhs)
{
  this->Sample<SAMPLETYPE>::operator=(rhs);
  this->level = rhs.level;
  return *this;
}

template <class SAMPLETYPE>
void WaveletOutputSample<SAMPLETYPE>::
SetSampleLevel(const int level)
{
  this->level = level;
}

template <class SAMPLETYPE>
int WaveletOutputSample<SAMPLETYPE>::
GetSampleLevel() const
{
  return level;
}

#endif
