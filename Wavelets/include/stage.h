#ifndef _stage
#define _stage

#include <vector>
#include <string>
#include <iostream>
#include <typeinfo>

#include "coefficients.h"
#include "filter.h"
#include "downsample.h"
#include "upsample.h"
#include "waveletinfo.h"
#include "util.h"

struct EquivalenceException
{};

/********************************************************************************
 *
 * The following class is simply a helper class that contains common components
 *  between a forward wavelet stage and a reverse wavelet stage.
 *
 *******************************************************************************/
template <class OUTSAMPLE, class INSAMPLE>
class WaveletStageHelper {
protected:
  WaveletType         wavetype;
  WaveletCoefficients wavecoefs;

  FIRFilter<OUTSAMPLE, INSAMPLE> lowpass;
  FIRFilter<OUTSAMPLE, INSAMPLE> highpass;

public:
  WaveletStageHelper(WaveletType wavetype=DAUB2);
  WaveletStageHelper(const WaveletStageHelper &rhs);
  virtual ~WaveletStageHelper();

  WaveletStageHelper & operator=(const WaveletStageHelper &rhs);

  void     ChangeWaveletType(const WaveletType wavetype);
  string   GetWaveletName();

  void     SetFilterCoefsLPF(const vector<double> &coefs);
  unsigned GetNumCoefsLPF();
  void     PrintCoefsLPF();

  void     SetFilterCoefsHPF(const vector<double> &coefs);
  unsigned GetNumCoefsHPF();
  void     PrintCoefsHPF();

  ostream & Print(ostream &os) const;
};

/********************************************************************************
 *
 * class:   ForwardWaveletStage
 * purpose: The purpose of this class is to model a forward wavelet stage.  A
 *  forward wavelet stage is essentially 2 filters, 2 downsamplers, and
 *  a set of operations for functioning in a streaming or block fashion.
 *
 *******************************************************************************/
template <class OUTSAMPLE, class INSAMPLE>
class ForwardWaveletStage {
protected:
  WaveletStageHelper<OUTSAMPLE, INSAMPLE> stagehelp;

  unsigned rate_l;
  unsigned rate_h;
  int      outlevel_l;
  int      outlevel_h;

  DownSample<OUTSAMPLE, INSAMPLE> downsampler_l;
  DownSample<OUTSAMPLE, INSAMPLE> downsampler_h;

public:
  ForwardWaveletStage(WaveletType wavetype=DAUB2);
  ForwardWaveletStage(const ForwardWaveletStage &rhs);
  ForwardWaveletStage(WaveletType wavetype,
		      unsigned    rate_l,
		      unsigned    rate_h,
		      int         outlevel_l,
		      int         outlevel_h);
  virtual ~ForwardWaveletStage();

  ForwardWaveletStage & operator=(const ForwardWaveletStage &rhs);

  inline void SetDownSampleRateLow(unsigned rate);
  inline unsigned GetDownSampleRateLow();

  inline void SetDownSampleRateHigh(unsigned rate);
  inline unsigned GetDownSampleRateHigh();

  inline void SetOutputLevelLow(int outlevel);
  inline int GetOutputLevelLow();

  inline void SetOutputLevelHigh(int outlevel);
  inline int GetOutputLevelHigh();

  // Returns true if there is an output sample
  bool PerformSampleOperation(OUTSAMPLE &out_l,
			      OUTSAMPLE &out_h,
			      INSAMPLE  &in);

  // Returns output buffer length (both outputs same length)
  unsigned PerformBlockOperation(SampleBlock<OUTSAMPLE> &out_l, 
				 SampleBlock<OUTSAMPLE> &out_h, 
				 SampleBlock<INSAMPLE>  &in);

  ostream & Print(ostream &os) const;
};


/********************************************************************************
 *
 * class:   ReverseWaveletStage
 * purpose: The purpose of this class is to model the inverse operation of the
 *  wavelet transform.  This class models one stage of the inverse wavelet
 *  transform.  It essentiall contains 2 filters, 2 upsamplers and operations
 *  for peforming the inverse wavelet transform in a streaming or block fashion.
 *
 *******************************************************************************/
template <class OUTSAMPLE, class INSAMPLE>
class ReverseWaveletStage {
protected:
  WaveletStageHelper<OUTSAMPLE, INSAMPLE> stagehelp;

  unsigned rate_l;
  unsigned rate_h;

  UpSample<OUTSAMPLE, INSAMPLE> upsampler_l;
  UpSample<OUTSAMPLE, INSAMPLE> upsampler_h;

public:
  ReverseWaveletStage(WaveletType wavetype=DAUB2);
  ReverseWaveletStage(const ReverseWaveletStage &rhs);
  ReverseWaveletStage(WaveletType wavetype,
		      unsigned    rate_l,
		      unsigned    rate_h);
  virtual ~ReverseWaveletStage();

  ReverseWaveletStage & operator=(const ReverseWaveletStage &rhs);

  inline void SetUpSampleRateLow(unsigned rate);
  inline unsigned GetUpSampleRateLow();

  inline void SetUpSampleRateHigh(unsigned rate);
  inline unsigned GetUpSampleRateHigh();

  // Takes two inputs and produces an output
  void PerformSampleOperation(OUTSAMPLE &out, INSAMPLE &in_l, INSAMPLE &in_h);

  unsigned PerformBlockOperation(SampleBlock<OUTSAMPLE> &out,
				 SampleBlock<INSAMPLE>  &in_l,
				 SampleBlock<INSAMPLE>  &in_h);

  ostream & Print(ostream &os) const;
};


/********************************************************************************
 *
 * Member functions for the WaveletStageHelper Class
 *
 *******************************************************************************/
template <class OUTSAMPLE, class INSAMPLE>
WaveletStageHelper<OUTSAMPLE, INSAMPLE>::WaveletStageHelper
(WaveletType wavetype=DAUB2) :
  wavecoefs(wavetype)
{
  this->wavetype = wavetype;

  vector<double> coefs;

  // Set up LPF
  wavecoefs.GetTransformCoefsLPF(coefs);
  lowpass.SetFilterCoefs(coefs);
  coefs.clear();

  // Set up HPF
  wavecoefs.GetTransformCoefsHPF(coefs);
  highpass.SetFilterCoefs(coefs);
  coefs.clear();
}

template <class OUTSAMPLE, class INSAMPLE>
WaveletStageHelper<OUTSAMPLE, INSAMPLE>::WaveletStageHelper
(const WaveletStageHelper &rhs) : 
  wavetype(rhs.wavetype), wavecoefs(rhs.wavecoefs),
  lowpass(rhs.lowpass), highpass(rhs.highpass)
{
}

template <class OUTSAMPLE, class INSAMPLE>
WaveletStageHelper<OUTSAMPLE, INSAMPLE>::~WaveletStageHelper()
{
}

template <class OUTSAMPLE, class INSAMPLE>
WaveletStageHelper<OUTSAMPLE, INSAMPLE> & 
WaveletStageHelper<OUTSAMPLE, INSAMPLE>::operator=(const WaveletStageHelper &rhs)
{
  // Make sure that the RTT of rhs is WaveletStageHelper, otherwise throw an
  //  exception
  if ((typeid(rhs) == typeid(WaveletStageHelper)) && (this != rhs)) {
    // Copy
    wavetype = rhs.wavetype;
    wavecoefs = rhs.wavecoefs;
    lowpass = rhs.lowpass;
    highpass = rhs.highpass;
  } else {
    throw EquivalenceException();
  }
  return *this;
}

template <class OUTSAMPLE, class INSAMPLE>
void 
WaveletStageHelper<OUTSAMPLE, INSAMPLE>::ChangeWaveletType
(const WaveletType wavetype)
{
  vector<double> &coefs;

  this->wavetype = wavetype;
  wavecoefs.Initialize(wavetype);

  // Set up LPF
  wavecoefs.GetTransformCoefsLPF(coefs);
  lowpass.SetFilterCoefs(coefs);
  coefs.clear();

  // Set up HPF
  wavecoefs.GetTransformCoefsHPF(coefs);
  highpass.SetFilterCoefs(coefs);
  coefs.clear();
}

template <class OUTSAMPLE, class INSAMPLE>
string WaveletStageHelper<OUTSAMPLE, INSAMPLE>::GetWaveletName()
{
  return wavecoefs.GetWaveletName();
}

template <class OUTSAMPLE, class INSAMPLE>
void
WaveletStageHelper<OUTSAMPLE, INSAMPLE>::SetFilterCoefsLPF
(const vector<double> &coefs)
{
  lowpass.SetFilterCoefs(coefs);
}

template <class OUTSAMPLE, class INSAMPLE>
unsigned WaveletStageHelper<OUTSAMPLE, INSAMPLE>::GetNumCoefsLPF()
{
  return lowpass.GetNumCoefs();
}

template <class OUTSAMPLE, class INSAMPLE>
void WaveletStageHelper<OUTSAMPLE, INSAMPLE>::PrintCoefsLPF()
{
  vector<double> &coefs;
  lowpass.GetFilterCoefs(coefs);
  cout << "LPF coefs:" << endl;
  for (unsigned i=0; i<coefs.size(); i++) {
    cout << "  " << coefs[i] << endl;
  }
}

template <class OUTSAMPLE, class INSAMPLE>
void 
WaveletStageHelper<OUTSAMPLE, INSAMPLE>::SetFilterCoefsHPF
(const vector<double> &coefs)
{
  highpass.SetFilterCoefs(coefs);
}

template <class OUTSAMPLE, class INSAMPLE>
unsigned WaveletStageHelper<OUTSAMPLE, INSAMPLE>::GetNumCoefsHPF()
{
  return highpass.GetNumCoefs();
}

template <class OUTSAMPLE, class INSAMPLE>
void WaveletStageHelper<OUTSAMPLE, INSAMPLE>::PrintCoefsHPF()
{
  vector<double> &coefs;
  highpass.GetFilterCoefs(coefs);
  cout << "HPF coefs:" << endl;
  for (unsigned i=0; i<coefs.size(); i++) {
    cout << "  " << coefs[i] << endl;
  }
}

template <class OUTSAMPLE, class INSAMPLE>
ostream & WaveletStageHelper<OUTSAMPLE, INSAMPLE>::Print(ostream &os) const
{
  os << "WaveletStageHelper::" << endl;
  os << wavecoefs << endl;
  os << lowpass << endl;
  os << highpass << endl;
  return os;
}

/********************************************************************************
 *
 * Member functions for the ForwardWaveletStage Class
 *
 *******************************************************************************/

template <class OUTSAMPLE, class INSAMPLE>
ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::ForwardWaveletStage
(WaveletType wavetype) : stagehelp(wavetype), rate_l(2), rate_h(2),
  outlevel_l(-1), outlevel_h(-1), downsampler_l(rate_l), downsampler_h(rate_h)
{
}

template <class OUTSAMPLE, class INSAMPLE>
ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::ForwardWaveletStage
(const ForwardWaveletStage<OUTSAMPLE, INSAMPLE> &rhs) : stagehelp(rhs.stagehelp),
   rate_l(rhs.rate_l), rate_h(rhs.rate_h), outlevel_l(rhs.outlevel_l), 
   outlevel_h(rhs.outlevel_h), downsampler_l(rhs.rate_l), downsampler_h(rhs.rate_h)
{
}

template <class OUTSAMPLE, class INSAMPLE>
ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::ForwardWaveletStage
(WaveletType wavetype, unsigned rate_l, unsigned rate_h, 
 int outlevel_l, int outlevel_h) : stagehelp(wavetype), rate_l(rate_l),
  rate_h(rate_h), outlevel_l(outlevel_l), outlevel_h(outlevel_h),
  downsampler_l(rate_l), downsampler_h(rate_h)
{
}

template <class OUTSAMPLE, class INSAMPLE>
ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::~ForwardWaveletStage()
{
}

template <class OUTSAMPLE, class INSAMPLE>
ForwardWaveletStage<OUTSAMPLE, INSAMPLE> & ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::operator=
(const ForwardWaveletStage &rhs)
{
  // Check that the RTT is actually a ForwardWaveletStage, else
  //  throw exception
  if ((typeid(rhs) == typeid(ForwardWaveletStage)) && 
      (this !=rhs)) {
    // copy
    stagehelp = rhs.stagehelp;
    rate_l = rhs.rate_l;
    rate_h = rhs.rate_h;
    outlevel_l = rhs.outlevel_l;
    outlevel_h = rhs.outlevel_h;
    downsampler_l = rhs.downsampler_l;
    downsampler_h = rhs.downsampler_h;
  } else {
    throw EquivalenceException();
  }
  return *this;
}

template <class OUTSAMPLE, class INSAMPLE>
void ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::SetDownSampleRateLow
(unsigned rate)
{
  downsampler_l.SetDownSampleRate(rate);
}

template <class OUTSAMPLE, class INSAMPLE>
unsigned ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::GetDownSampleRateLow()
{
  return downsampler_l.GetDownSampleRate();
}

template <class OUTSAMPLE, class INSAMPLE>
void ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::SetDownSampleRateHigh
(unsigned rate)
{
  downsampler_h.SetDownSampleRate(rate);
}

template <class OUTSAMPLE, class INSAMPLE>
unsigned ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::GetDownSampleRateHigh()
{
  return downsampler_h.GetDownSampleRate();
}

template <class OUTSAMPLE, class INSAMPLE>
void ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::SetOutputLevelLow(int outlevel)
{
  outlevel_l = outlevel;
}

template <class OUTSAMPLE, class INSAMPLE>
int ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::GetOutputLevelLow()
{
  return outlevel_l;
}

template <class OUTSAMPLE, class INSAMPLE>
void ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::SetOutputLevelHigh(int outlevel)
{
  outlevel_h = outlevel;
}

template <class OUTSAMPLE, class INSAMPLE>
int ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::GetOutputLevelHigh()
{
  return out_level_h;
}

template <class OUTSAMPLE, class INSAMPLE>
bool ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::PerformSampleOperation
(OUTSAMPLE &out_l, OUTSAMPLE &out_h, INSAMPLE &in)
{
  bool result=false;
  return result;
}

template <class OUTSAMPLE, class INSAMPLE>
unsigned ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::PerformBlockOperation
(SampleBlock<OUTSAMPLE> &out_l, SampleBlock<OUTSAMPLE> &out_h, 
 SampleBlock<INSAMPLE>  &in)
{
  unsigned result=0 ;
  return result;
}


template <class OUTSAMPLE, class INSAMPLE>
ostream & ForwardWaveletStage<OUTSAMPLE, INSAMPLE>::Print(ostream &os) const
{
  os << "ForwardWaveletStage::" << endl;
  os << "  " << stagehelp << endl;
  os << "  " << downsampler_l << endl;
  os << "  " << downsampler_h << endl;
  os << "  Outlevel (l)       : " << outlevel_l << endl;
  os << "  Outlevel (h)       : " << outlevel_h << endl;
  return os;
}


/********************************************************************************
 *
 * Member functions for the ReverseWaveletStage Class
 *
 *******************************************************************************/
template <class OUTSAMPLE, class INSAMPLE>
ReverseWaveletStage<OUTSAMPLE, INSAMPLE>::ReverseWaveletStage
(WaveletType wavetype) : stagehelp(wavetype), rate_l(2), rate_h(2),
  upsampler_l(rate_l), upsampler_h(rate_h)
{
}

template <class OUTSAMPLE, class INSAMPLE>
ReverseWaveletStage<OUTSAMPLE, INSAMPLE>::ReverseWaveletStage
(const ReverseWaveletStage &rhs) : stagehelp(rhs.stagehelp), rate_l(rhs.rate_l),
  rate_h(rhs.rate_h), upsampler_l(rhs.rate_l), upsampler_h(rhs.rate_h)
{
}

template <class OUTSAMPLE, class INSAMPLE>
ReverseWaveletStage<OUTSAMPLE, INSAMPLE>::ReverseWaveletStage
(WaveletType wavetype, unsigned rate_l, unsigned rate_h) : stagehelp(wavetype),
  rate_l(rate_l), rate_h(rate_h), upsampler_l(rate_l), upsampler_h(rate_h)
{
}

template <class OUTSAMPLE, class INSAMPLE>
ReverseWaveletStage<OUTSAMPLE, INSAMPLE>::~ReverseWaveletStage()
{
}

template <class OUTSAMPLE, class INSAMPLE>
ReverseWaveletStage<OUTSAMPLE, INSAMPLE> & ReverseWaveletStage<OUTSAMPLE, INSAMPLE>::operator=
(const ReverseWaveletStage &rhs)
{
  // Check that the RTT is actually a ReverseWaveletStage, else
  //  throw exception
  if ((typeid(rhs) == typeid(ReverseWaveletStage)) && (this !=rhs)) {
    // copy
    stagehelp = rhs.stagehelp;
    rate_l = rhs.rate_l;
    rate_h = rhs.rate_h;
    upsampler_l = rhs.upsampler_l;
    upsampler_h = rhs.upsampler_h;
  } else {
    throw EquivalenceException();
  }
  return *this;
}

template <class OUTSAMPLE, class INSAMPLE>
void ReverseWaveletStage<OUTSAMPLE, INSAMPLE>::SetUpSampleRateLow(unsigned rate)
{
  upsampler_l.SetUpSampleRate(rate);
}

template <class OUTSAMPLE, class INSAMPLE>
unsigned ReverseWaveletStage<OUTSAMPLE, INSAMPLE>::GetUpSampleRateLow()
{
  return upsampler_l.GetUpSampleRate();
}

template <class OUTSAMPLE, class INSAMPLE>
void ReverseWaveletStage<OUTSAMPLE, INSAMPLE>::SetUpSampleRateHigh(unsigned rate)
{
  upsampler_h.SetUpSampleRate(rate);
}

template <class OUTSAMPLE, class INSAMPLE>
unsigned ReverseWaveletStage<OUTSAMPLE, INSAMPLE>::GetUpSampleRateHigh()
{
  return upsampler_h.GetUpSampleRate();
}

template <class OUTSAMPLE, class INSAMPLE>
void ReverseWaveletStage<OUTSAMPLE, INSAMPLE>::PerformSampleOperation
(OUTSAMPLE &out, INSAMPLE &in_l, INSAMPLE &in_h)
{
  return;
}

template <class OUTSAMPLE, class INSAMPLE>
unsigned ReverseWaveletStage<OUTSAMPLE, INSAMPLE>::PerformBlockOperation
(SampleBlock<OUTSAMPLE> &out, SampleBlock<INSAMPLE> &in_l, 
 SampleBlock<INSAMPLE> &in_h)
{
  unsigned result=0;
  return result;
}

template <class OUTSAMPLE, class INSAMPLE>
ostream & ReverseWaveletStage<OUTSAMPLE, INSAMPLE>::Print(ostream &os) const
{
  os << "ReverseWaveletStage::" << endl;
  os << "  " << stagehelp << endl;
  os << "  " << upsampler_h << endl;
  os << "  " << upsampler_l << endl;
  return os;
}

#endif
