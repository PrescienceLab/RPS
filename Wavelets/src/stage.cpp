#include "stage.h"

Stage::Stage(WaveletType wavetype=DAUB2)
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

Stage::Stage(const Stage &rhs)
{
  wavetype = rhs.wavetype;
  wavecoefs = rhs.wavecoefs;
  lowpass = rhs.lowpass;
  highpass = rhs.highpass;
}

Stage::~Stage()
{
}

Stage & Stage::operator=(const Stage &rhs)
{
  wavetype = rhs.wavetype;
  wavecoefs = rhs.wavecoefs;
  lowpass = rhs.lowpass;
  highpass = rhs.highpass;
  return *this;
}

void Stage::ChangeWaveletType(WaveletType wavetype)
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

string Stage::GetWaveletName()
{
  return wavecoefs.GetWaveletName();
}

void Stage::SetFilterCoefsLPF(vector<double> &coefs)
{
  lowpass.SetFilterCoefs(coefs);
}

unsigned Stage::GetNumCoefsLPF()
{
  return lowpass.GetNumCoefs();
}

void Stage::PrintCoefsLPF()
{
  vector<double> &coefs;
  lowpass.GetFilterCoefs(coefs);
  cout << "LPF coefs:" << endl;
  for (unsigned i=0; i<coefs.size(); i++) {
    cout << "  " << coefs[i] << endl;
  }
}

void Stage::SetFilterCoefsHPF(vector<double> &coefs)
{
  highpass.SetFilterCoefs(coefs);
}

unsigned Stage::GetNumCoefsHPF()
{
  return highpass.GetNumCoefs();
}

void Stage::PrintCoefsHPF()
{
  vector<double> &coefs;
  highpass.GetFilterCoefs(coefs);
  cout << "HPF coefs:" << endl;
  for (unsigned i=0; i<coefs.size(); i++) {
    cout << "  " << coefs[i] << endl;
  }
}

