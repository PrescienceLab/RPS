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

#endif
