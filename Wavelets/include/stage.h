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

template <class outSample, class inSample, class inputType>
class WaveletStageHelper {
protected:
  WaveletType         wavetype;
  WaveletCoefficients wavecoefs;

  FIRFilter<outSample, inSample, inputType> lowpass;
  FIRFilter<outSample, inSample, inputType> highpass;

public:
  Stage(WaveletType wavetype=DAUB2);
  Stage(const Stage &rhs);
  virtual ~Stage();

  Stage & operator=(const Stage &rhs);

  void     ChangeWaveletType(WaveletType wavetype);
  string   GetWaveletName();

  void     SetFilterCoefsLPF(vector<double> &coefs);
  unsigned GetNumCoefsLPF();
  void     PrintCoefsLPF();

  void     SetFilterCoefsHPF(vector<double> &coefs);
  unsigned GetNumCoefsHPF();
  void     PrintCoefsHPF();

  ostream & Print(ostream &os) const;
};


template <class outSample, class inSample, class inputType>
Stage<outSample, inSample, inputType>::Stage(WaveletType wavetype=DAUB2) :
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

template <class outSample, class inSample, class inputType>
Stage<outSample, inSample, inputType>::Stage(const Stage &rhs) :
  wavetype(rhs.wavetype), wavecoefs(rhs.wavecoefs),
  lowpass(rhs.lowpass), highpass(rhs.highpass)
{
}

template <class outSample, class inSample, class inputType>
Stage<outSample, inSample, inputType>::~Stage()
{
}

template <class outSample, class inSample, class inputType>
Stage<outSample, inSample, inputType> & 
Stage<outSample, inSample, inputType>::operator=(const Stage &rhs)
{
  // Make sure that the RTT of rhs is Stage, otherwise throw an
  //  exception
  if (typeid(rhs) == typeid(Stage)) {
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

template <class outSample, class inSample, class inputType>
void 
Stage<outSample, inSample, inputType>::ChangeWaveletType(WaveletType wavetype)
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

template <class outSample, class inSample, class inputType>
string Stage<outSample, inSample, inputType>::GetWaveletName()
{
  return wavecoefs.GetWaveletName();
}

template <class outSample, class inSample, class inputType>
void
Stage<outSample, inSample, inputType>::SetFilterCoefsLPF(vector<double> &coefs)
{
  lowpass.SetFilterCoefs(coefs);
}

template <class outSample, class inSample, class inputType>
unsigned Stage<outSample, inSample, inputType>::GetNumCoefsLPF()
{
  return lowpass.GetNumCoefs();
}

template <class outSample, class inSample, class inputType>
void Stage<outSample, inSample, inputType>::PrintCoefsLPF()
{
  vector<double> &coefs;
  lowpass.GetFilterCoefs(coefs);
  cout << "LPF coefs:" << endl;
  for (unsigned i=0; i<coefs.size(); i++) {
    cout << "  " << coefs[i] << endl;
  }
}

template <class outSample, class inSample, class inputType>
void 
Stage<outSample, inSample, inputType>::SetFilterCoefsHPF(vector<double> &coefs)
{
  highpass.SetFilterCoefs(coefs);
}

template <class outSample, class inSample, class inputType>
unsigned Stage<outSample, inSample, inputType>::GetNumCoefsHPF()
{
  return highpass.GetNumCoefs();
}

template <class outSample, class inSample, class inputType>
void Stage<outSample, inSample, inputType>::PrintCoefsHPF()
{
  vector<double> &coefs;
  highpass.GetFilterCoefs(coefs);
  cout << "HPF coefs:" << endl;
  for (unsigned i=0; i<coefs.size(); i++) {
    cout << "  " << coefs[i] << endl;
  }
}

template <class outSample, class inSample, class inputType>
ostream & Stage<outSample, inSample, inputType>::Print(ostream &os) const
{
  os << "Stage::" << endl;
  os << wavecoefs << endl;
  os << lowpass << endl;
  os << highpass << endl;
  return os;
}

#endif
