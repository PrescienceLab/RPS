#ifndef _stage
#define _stage

#include <vector>
#include <string>
#include <iostream>
#include <typeinfo>

#include "coefficients.h"
#include "filter.h"
#include "sampleblock.h"
#include "waveletsampleblock.h"
#include "downsample.h"
#include "upsample.h"
#include "waveletinfo.h"
#include "util.h"

struct EquivalenceException
{};

struct OperationSyncException
{};

struct InvalidStageTypeException
{};

enum StageType {FORWARD, REVERSE};

/********************************************************************************
 *
 * The following class is simply a helper class that contains common components
 *  between a forward wavelet stage and a reverse wavelet stage.
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class WaveletStageHelper {
protected:
  StageType stagetype;
  WaveletType wavetype;
  WaveletCoefficients wavecoefs;

  FIRFilter<SAMPLETYPE,OUTSAMPLE,INSAMPLE> lowpass;
  FIRFilter<SAMPLETYPE,OUTSAMPLE,INSAMPLE> highpass;

public:
  WaveletStageHelper(const WaveletType wavetype=DAUB2, 
		     const StageType stagetype=FORWARD);
  WaveletStageHelper(const WaveletStageHelper &rhs);
  virtual ~WaveletStageHelper();

  WaveletStageHelper & operator=(const WaveletStageHelper &rhs);

  void ChangeWaveletType(const WaveletType wavetype);
  string GetWaveletName() const;

  void SetFilterCoefsLPF(const vector<double> &coefs);
  unsigned GetNumCoefsLPF() const;
  void PrintCoefsLPF() const;

  void SetFilterCoefsHPF(const vector<double> &coefs);
  unsigned GetNumCoefsHPF() const;
  void PrintCoefsHPF() const;

  void ClearLPFDelayLine();
  void LPFSampleOperation(Sample<SAMPLETYPE> &out, const Sample<SAMPLETYPE> &in);
  void LPFBufferOperation(SampleBlock<OUTSAMPLE> &out,
			  const SampleBlock<INSAMPLE>  &in);

  void ClearHPFDelayLine();
  void HPFSampleOperation(Sample<SAMPLETYPE> &out, const Sample<SAMPLETYPE> &in);
  void HPFBufferOperation(SampleBlock<OUTSAMPLE> &out,
			  const SampleBlock<INSAMPLE>  &in);

  ostream & Print(ostream &os) const;
  ostream & operator<<(ostream &os) const;
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
inline ostream & operator<<(ostream &os, const WaveletStageHelper<SAMPLETYPE,OUTSAMPLE,INSAMPLE> &rhs) { return rhs.operator<<(os);};


/********************************************************************************
 *
 * class:   ForwardWaveletStage
 * purpose: The purpose of this class is to model a forward wavelet stage.  A
 *  forward wavelet stage is essentially 2 filters, 2 downsamplers, and
 *  a set of operations for functioning in a streaming or block fashion.
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class ForwardWaveletStage {
protected:
  WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE> stagehelp;

  unsigned rate_l;
  unsigned rate_h;
  int outlevel_l;
  int outlevel_h;

  DownSample<OUTSAMPLE> downsampler_l;
  DownSample<OUTSAMPLE> downsampler_h;

public:
  ForwardWaveletStage(const WaveletType wavetype=DAUB2);
  ForwardWaveletStage(const ForwardWaveletStage &rhs);
  ForwardWaveletStage(const WaveletType wavetype,
		      const unsigned rate_l,
		      const unsigned rate_h,
		      const int outlevel_l,
		      const int outlevel_h);
  virtual ~ForwardWaveletStage();

  ForwardWaveletStage & operator=(const ForwardWaveletStage &rhs);

  ForwardWaveletStage* clone();

  inline void SetDownSampleRateLow(const unsigned rate);
  inline unsigned GetDownSampleRateLow() const;

  inline void SetDownSampleRateHigh(const unsigned rate);
  inline unsigned GetDownSampleRateHigh() const;

  inline void SetOutputLevelLow(const int outlevel);
  inline int GetOutputLevelLow() const;

  inline void SetOutputLevelHigh(const int outlevel);
  inline int GetOutputLevelHigh() const;

  inline void ClearFilterDelayLines();
  inline void ClearAllState();

  // Returns true if there is an output sample
  bool PerformSampleOperation(WaveletOutputSample<SAMPLETYPE> &out_l,
			      WaveletOutputSample<SAMPLETYPE> &out_h,
			      const Sample<SAMPLETYPE> &in);

  // Returns output buffer length (both outputs same length)
  unsigned PerformBlockOperation(WaveletOutputSampleBlock<OUTSAMPLE> &out_l, 
				 WaveletOutputSampleBlock<OUTSAMPLE> &out_h, 
				 const SampleBlock<INSAMPLE> &in);

  ostream & Print(ostream &os) const;
  ostream & operator<<(ostream &os) const;
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
inline ostream & operator<<(ostream &os, const ForwardWaveletStage<SAMPLETYPE,OUTSAMPLE,INSAMPLE> &rhs) { return rhs.operator<<(os);};


/********************************************************************************
 *
 * class:   ReverseWaveletStage
 * purpose: The purpose of this class is to model the inverse operation of the
 *  wavelet transform.  This class models one stage of the inverse wavelet
 *  transform.  It essentially contains 2 filters, 2 upsamplers and operations
 *  for peforming the inverse wavelet transform in a streaming or block fashion.
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class ReverseWaveletStage {
protected:
  WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE> stagehelp;

  unsigned rate_l;
  unsigned rate_h;

  UpSample<INSAMPLE> upsampler_l;
  UpSample<INSAMPLE> upsampler_h;

public:
  ReverseWaveletStage(const WaveletType wavetype=DAUB2);
  ReverseWaveletStage(const ReverseWaveletStage &rhs);
  ReverseWaveletStage(const WaveletType wavetype,
		      const unsigned rate_l,
		      const unsigned rate_h);
  virtual ~ReverseWaveletStage();

  ReverseWaveletStage & operator=(const ReverseWaveletStage &rhs);

  ReverseWaveletStage* clone();

  inline void SetUpSampleRateLow(const unsigned rate);
  inline unsigned GetUpSampleRateLow() const;

  inline void SetUpSampleRateHigh(const unsigned rate);
  inline unsigned GetUpSampleRateHigh() const;

  inline void ClearFilterDelayLines();
  inline void ClearAllState();

  // Takes two inputs and produces a vector of outputs, depending on upsample
  //  rates
  bool PerformSampleOperation(vector<OUTSAMPLE> &out,
			      const Sample<SAMPLETYPE> &in_l,
			      const Sample<SAMPLETYPE> &in_h);

  unsigned PerformBlockOperation(SampleBlock<OUTSAMPLE> &out,
				 const SampleBlock<INSAMPLE> &in_l,
				 const SampleBlock<INSAMPLE> &in_h);

  ostream & Print(ostream &os) const;
  ostream & operator<<(ostream &os) const;
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
inline ostream & operator<<(ostream &os, const ReverseWaveletStage<SAMPLETYPE,OUTSAMPLE,INSAMPLE> &rhs) { return rhs.operator<<(os);};



/********************************************************************************
 *
 * Member functions for the WaveletStageHelper Class
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
WaveletStageHelper(const WaveletType wavetype, 
		   const StageType stagetype) :
  wavecoefs(wavetype)
{
  this->wavetype = wavetype;
  this->stagetype = stagetype;

  vector<double> coefs;

  switch (stagetype) {
  case FORWARD:
    // Set up LPF
    wavecoefs.GetTransformCoefsLPF(coefs);
    lowpass.SetFilterCoefs(coefs);
    coefs.clear();

    // Set up HPF
    wavecoefs.GetTransformCoefsHPF(coefs);
    highpass.SetFilterCoefs(coefs);
    coefs.clear();
    break;
  case REVERSE:
    // Set up LPF
    wavecoefs.GetInverseCoefsLPF(coefs);
    lowpass.SetFilterCoefs(coefs);
    coefs.clear();

    // Set up HPF
    wavecoefs.GetInverseCoefsHPF(coefs);
    highpass.SetFilterCoefs(coefs);
    coefs.clear();
    break;
  default:
    throw InvalidStageTypeException();
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
WaveletStageHelper(const WaveletStageHelper &rhs) : 
  stagetype(rhs.stagetype), wavetype(rhs.wavetype), wavecoefs(rhs.wavecoefs),
  lowpass(rhs.lowpass), highpass(rhs.highpass)
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~WaveletStageHelper()
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE> & 
WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator=(const WaveletStageHelper &rhs)
{
  // Make sure that the RTT of rhs is WaveletStageHelper, otherwise throw an
  //  exception
  if ((typeid(rhs) == typeid(WaveletStageHelper)) && (this != rhs)) {
    // Copy
    wavetype = rhs.wavetype;
    stagetype = rhs.stagetype;
    wavecoefs = rhs.wavecoefs;
    lowpass = rhs.lowpass;
    highpass = rhs.highpass;
  } else {
    throw EquivalenceException();
  }
  return *this;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeWaveletType(const WaveletType wavetype)
{
  vector<double> &coefs;

  this->wavetype = wavetype;
  wavecoefs.Initialize(wavetype);

  switch (stagetype) {
  case FORWARD:
    // Set up LPF
    wavecoefs.GetTransformCoefsLPF(coefs);
    lowpass.SetFilterCoefs(coefs);
    coefs.clear();

    // Set up HPF
    wavecoefs.GetTransformCoefsHPF(coefs);
    highpass.SetFilterCoefs(coefs);
    coefs.clear();
    break;
  case REVERSE:
    // Set up LPF
    wavecoefs.GetInverseCoefsLPF(coefs);
    lowpass.SetFilterCoefs(coefs);
    coefs.clear();

    // Set up HPF
    wavecoefs.GetInverseCoefsHPF(coefs);
    highpass.SetFilterCoefs(coefs);
    coefs.clear();
    break;
  default:
    throw InvalidStageTypeException();
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
string WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetWaveletName() const
{
  return wavecoefs.GetWaveletName();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetFilterCoefsLPF(const vector<double> &coefs)
{
  lowpass.SetFilterCoefs(coefs);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetNumCoefsLPF() const
{
  return lowpass.GetNumCoefs();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
PrintCoefsLPF() const
{
  vector<double> coefs;
  lowpass.GetFilterCoefs(coefs);
  cout << "LPF coefs:" << endl;
  for (unsigned i=0; i<coefs.size(); i++) {
    cout << "  " << coefs[i] << endl;
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetFilterCoefsHPF(const vector<double> &coefs)
{
  highpass.SetFilterCoefs(coefs);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetNumCoefsHPF() const
{
  return highpass.GetNumCoefs();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
PrintCoefsHPF() const
{
  vector<double> coefs;
  highpass.GetFilterCoefs(coefs);
  cout << "HPF coefs:" << endl;
  for (unsigned i=0; i<coefs.size(); i++) {
    cout << "  " << coefs[i] << endl;
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearLPFDelayLine()
{
  lowpass.ClearDelayLine();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
LPFSampleOperation(Sample<SAMPLETYPE> &out, const Sample<SAMPLETYPE> &in)
{
  lowpass.GetFilterOutput(out,in);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
LPFBufferOperation(SampleBlock<OUTSAMPLE> &out, const SampleBlock<INSAMPLE> &in)
{
  lowpass.GetFilterBufferOutput(out,in);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearHPFDelayLine()
{
  highpass.ClearDelayLine();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
HPFSampleOperation(Sample<SAMPLETYPE> &out, const Sample<SAMPLETYPE> &in)
{
  highpass.GetFilterOutput(out,in);
}


template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
HPFBufferOperation(SampleBlock<OUTSAMPLE> &out, const SampleBlock<INSAMPLE> &in)
{
  highpass.GetFilterBufferOutput(out,in);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
Print(ostream &os) const
{
  os << "WaveletStageHelper::" << endl;
  os << wavecoefs << endl;
  os << lowpass << endl;
  os << highpass << endl;
  return os;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & WaveletStageHelper<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator<<(ostream &os) const
{ return Print(os);}

/********************************************************************************
 *
 * Member functions for the ForwardWaveletStage Class
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ForwardWaveletStage(const WaveletType wavetype) : 
  stagehelp(wavetype, FORWARD), rate_l(2), rate_h(2),
  outlevel_l(-1), outlevel_h(-1), downsampler_l(rate_l), downsampler_h(rate_h)
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ForwardWaveletStage(const ForwardWaveletStage &rhs) :
  stagehelp(rhs.stagehelp), rate_l(rhs.rate_l), rate_h(rhs.rate_h),
  outlevel_l(rhs.outlevel_l), outlevel_h(rhs.outlevel_h),
  downsampler_l(rhs.rate_l), downsampler_h(rhs.rate_h)
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ForwardWaveletStage(const WaveletType wavetype,
		    const unsigned rate_l, 
		    const unsigned rate_h, 
		    const int outlevel_l, 
		    const int outlevel_h) : 
  stagehelp(wavetype, FORWARD), rate_l(rate_l), rate_h(rate_h), 
  outlevel_l(outlevel_l), outlevel_h(outlevel_h),
  downsampler_l(rate_l), downsampler_h(rate_h)
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~ForwardWaveletStage()
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE> &
ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator=(const ForwardWaveletStage &rhs)
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


template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>*
ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
clone()
{
  return new ForwardWaveletStage(*this);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetDownSampleRateLow(const unsigned rate)
{
  downsampler_l.SetDownSampleRate(rate);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetDownSampleRateLow() const
{
  return downsampler_l.GetDownSampleRate();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetDownSampleRateHigh(const unsigned rate)
{
  downsampler_h.SetDownSampleRate(rate);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetDownSampleRateHigh() const
{
  return downsampler_h.GetDownSampleRate();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetOutputLevelLow(const int outlevel)
{
  outlevel_l = outlevel;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
int ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetOutputLevelLow() const
{
  return outlevel_l;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetOutputLevelHigh(const int outlevel)
{
  outlevel_h = outlevel;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
int ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetOutputLevelHigh() const
{
  return out_level_h;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearFilterDelayLines()
{
  stagehelp.ClearLPFDelayLine();
  stagehelp.ClearHPFDelayLine();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearAllState()
{
  ClearFilterDelayLines();
  downsampler_l.ResetState();
  downsampler_h.ResetState();
}


template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
PerformSampleOperation(WaveletOutputSample<SAMPLETYPE> &out_l,
		       WaveletOutputSample<SAMPLETYPE> &out_h, 
		       const Sample<SAMPLETYPE> &in)
{
  // Filter the new input sample through the LPF and HPF filters
  stagehelp.LPFSampleOperation(out_l,in);
  stagehelp.HPFSampleOperation(out_h,in);

  // Set the appropriate levels
  out_l.SetSampleLevel(outlevel_l);
  out_h.SetSampleLevel(outlevel_h);

  // Downsample the results
  bool state;
  if ((state = downsampler_l.KeepSample()) != downsampler_h.KeepSample()) {
    // If the downsamplers are unsynchronized, reset the state and throw an
    //  exception.  If this occurs, we may lose the output from the last filter
    //  operation
    downsampler_l.ResetState();
    downsampler_h.ResetState();
    throw OperationSyncException();
  }

  return state;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
PerformBlockOperation(WaveletOutputSampleBlock<OUTSAMPLE> &out_l, 
		      WaveletOutputSampleBlock<OUTSAMPLE> &out_h, 
		      const SampleBlock<INSAMPLE> &in)
{
  // Need a temporary output block for filter operations
  WaveletOutputSampleBlock<OUTSAMPLE> tempblock_l, tempblock_h;

  // Block filter and downsample the new input buffer
  stagehelp.LPFBufferOperation(tempblock_l, in);
  stagehelp.HPFBufferOperation(tempblock_h, in);  

  downsampler_l.DownSampleBuffer(out_l, tempblock_l);
  downsampler_h.DownSampleBuffer(out_h, tempblock_h);

  out_l.SetBlockLevel(outlevel_l);
  out_h.SetBlockLevel(outlevel_h);

  unsigned blocksize;
  if ((blocksize = out_l.GetBlockSize()) != out_h.GetBlockSize()) {
    // If somehow the filter output lengths are different, clear the delay line
    //  and may lose data (might want to refilter)
    stagehelp.ClearLPFDelayLine();
    stagehelp.ClearHPFDelayLine();
    throw OperationSyncException();    
  }
  return blocksize;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
Print(ostream &os) const
{
  os << "ForwardWaveletStage::" << endl;
  os << "  " << stagehelp << endl;
  os << "  " << downsampler_l << endl;
  os << "  " << downsampler_h << endl;
  os << "  Outlevel (l)       : " << outlevel_l << endl;
  os << "  Outlevel (h)       : " << outlevel_h << endl;
  return os;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator<<(ostream &os) const
{ return Print(os);}



/********************************************************************************
 *
 * Member functions for the ReverseWaveletStage Class
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ReverseWaveletStage(const WaveletType wavetype) : 
  stagehelp(wavetype, REVERSE), rate_l(2), rate_h(2), 
  upsampler_l(rate_l), upsampler_h(rate_h)
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ReverseWaveletStage(const ReverseWaveletStage &rhs) : 
  stagehelp(rhs.stagehelp), rate_l(rhs.rate_l), rate_h(rhs.rate_h), 
  upsampler_l(rhs.rate_l), upsampler_h(rhs.rate_h)
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ReverseWaveletStage(const WaveletType wavetype, 
		    const unsigned rate_l, 
		    const unsigned rate_h) : 
  stagehelp(wavetype, REVERSE), rate_l(rate_l), rate_h(rate_h), 
  upsampler_l(rate_l), upsampler_h(rate_h)
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~ReverseWaveletStage()
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE> & 
ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator=(const ReverseWaveletStage &rhs)
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

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>*
ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
clone()
{
  return new ReverseWaveletStage(*this);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetUpSampleRateLow(const unsigned rate)
{
  upsampler_l.SetUpSampleRate(rate);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetUpSampleRateLow() const
{
  return upsampler_l.GetUpSampleRate();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetUpSampleRateHigh(const unsigned rate)
{
  upsampler_h.SetUpSampleRate(rate);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetUpSampleRateHigh() const
{
  return upsampler_h.GetUpSampleRate();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearFilterDelayLines()
{
  stagehelp.ClearLPFDelayLine();
  stagehelp.ClearHPFDelayLine();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearAllState()
{
  ClearFilterDelayLines();
  upsampler_l.ResetState();
  upsampler_h.ResetState();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
PerformSampleOperation(vector<OUTSAMPLE>  &out, 
		       const Sample<SAMPLETYPE> &in_l, 
		       const Sample<SAMPLETYPE> &in_h)
{
  Sample<SAMPLETYPE> zero(0);
  Sample<SAMPLETYPE> tempout(0);
  out.clear();

  bool zerosample;
  OUTSAMPLE outsamp;
  
  // loop condition could be dependent on rate_h as well (they should be equal)
  for (unsigned i=0; i<rate_l; i++) {

    // Upsample the inputs
    if ((zerosample = upsampler_l.ZeroSample()) != upsampler_h.ZeroSample()) {
      upsampler_l.ResetState();
      upsampler_h.ResetState();
      throw OperationSyncException();    
    } else {
      if (zerosample) {
	stagehelp.LPFSampleOperation(tempout,zero);
	outsamp = tempout;
	stagehelp.HPFSampleOperation(tempout,zero);
	outsamp += tempout;
      } else {
	stagehelp.LPFSampleOperation(tempout,in_l);
	outsamp = tempout;
	stagehelp.HPFSampleOperation(tempout,in_h);
	outsamp += tempout;
      }
    }
    out.push_back(outsamp);
  }
  return out.size();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
PerformBlockOperation(SampleBlock<OUTSAMPLE> &out, 
		      const SampleBlock<INSAMPLE> &in_l, 
		      const SampleBlock<INSAMPLE> &in_h)
{
  // Need a temporary output block for filter operations
  SampleBlock<INSAMPLE>* tempin_l = in_l.clone();
  SampleBlock<INSAMPLE>* tempin_h = in_h.clone();

  // Upsample the input blocks
  upsampler_l.UpSampleBuffer(*tempin_l, in_l);
  upsampler_h.UpSampleBuffer(*tempin_h, in_h);

  // Filter the tempblocks
  SampleBlock<OUTSAMPLE>* tempout_l = out.clone();
  SampleBlock<OUTSAMPLE>* tempout_h = out.clone();

  stagehelp.LPFBufferOperation(*tempout_l, *tempin_l);
  stagehelp.HPFBufferOperation(*tempout_h, *tempin_h);

  if (tempout_l->GetBlockSize() != tempout_h->GetBlockSize()) {
    // If somehow the input filter blocks are different length, clear the delay
    // line and may lose data (might want to refilter)
    stagehelp.ClearLPFDelayLine();
    stagehelp.ClearHPFDelayLine();
    throw OperationSyncException();    
  }

  // Add the two outputs of the filters
  out = *tempout_l + *tempout_h;
  return out.GetBlockSize();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
Print(ostream &os) const
{
  os << "ReverseWaveletStage::" << endl;
  os << "  " << stagehelp << endl;
  os << "  " << upsampler_h << endl;
  os << "  " << upsampler_l << endl;
  return os;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator<<(ostream &os) const
{ return Print(os);}


#endif
