#ifndef _filter
#define _filter

#include <vector>
#include <deque>
#include <iostream>

#include "util.h"
#include "sample.h"
#include "sampleout.h"
#include "sampleblock.h"
#include "sampleblockout.h"

template <class outSample, class inSample, class inputType>
class FIRFilter {
private:
  vector<double>    coefs;
  deque<inputType>  delayline;
  unsigned          numcoefs;

public:
  FIRFilter(unsigned numcoefs=0);
  FIRFilter(const FIRFilter &rhs);
  FIRFilter(unsigned numcoefs, vector<double> &coefs);
  virtual ~FIRFilter();

  FIRFilter & operator=(const FIRFilter &rhs);

  void   SetFilterCoefs(vector<double> &coefs);
  void   GetFilterCoefs(vector<double> &coefs);
  void   ClearDelayLine();

  void   GetFilterOutput(OutputSample<inputType> &out,
			 OutputSample<inputType> &in);
  void   GetFilterOutput(OutputSample<inputType> &out,
			 InputSample<inputType>  &in);
  void   GetFilterOutput(InputSample<inputType>  &out,
			 OutputSample<inputType> &in);
  void   GetFilterOutput(InputSample<inputType>  &out,
			 InputSample<inputType>  &in);

  void GetFilterBufferOutput(OutputSampleBlock<outSample> &out,
			     OutputSampleBlock<outSample> &in);
  void GetFilterBufferOutput(OutputSampleBlock<outSample> &out,
			     InputSampleBlock<inSample>   &in);
  void GetFilterBufferOutput(InputSampleBlock<inSample>   &out,
			     OutputSampleBlock<outSample> &in);
  void GetFilterBufferOutput(InputSampleBlock<inSample>   &out,
			     InputSampleBlock<inSample>   &in);


  ostream & Print(ostream &os) const;
};

template <class outSample, class inSample, class inputType>
FIRFilter<outSample, inSample, inputType>::FIRFilter(unsigned numcoefs)
{
  this->numcoefs = numcoefs;
  
  coefs.clear();
  delayline.clear();
  for (unsigned i=0; i<numcoefs; i++) {
    coefs.push_back(0);
    delayline.push_back(0);
  }
}

template <class outSample, class inSample, class inputType>
FIRFilter<outSample, inSample, inputType>::FIRFilter(const FIRFilter &rhs)
{
  coefs = rhs.coefs;
  delayline = rhs.delayline;
  numcoefs = rhs.numcoefs;
}

template <class outSample, class inSample, class inputType>
FIRFilter<outSample, inSample, inputType>::FIRFilter
(unsigned numcoefs, vector<double> &coefs)
{
  this->numcoefs = numcoefs;
  this->coefs = coefs;
  
  delayline.clear();
  for (unsigned i=0; i<numcoefs; i++) {
    delayline.push_back(0);
  }
}

template <class outSample, class inSample, class inputType>
FIRFilter<outSample, inSample, inputType>::~FIRFilter()
{
}

template <class outSample, class inSample, class inputType>
FIRFilter<outSample, inSample, inputType> & 
FIRFilter<outSample, inSample, inputType>::operator=
(const FIRFilter &rhs)
{
  coefs = rhs.coefs;
  delayline = rhs.delayline;
  numcoefs = rhs.numcoefs;
  return *this;

}

template <class outSample, class inSample, class inputType>
void FIRFilter<outSample, inSample, inputType>::SetFilterCoefs
(vector<double> &coefs)
{
  this->numcoefs = coefs.size();
  this->coefs = coefs;
  
  delayline.clear();
  for (unsigned i=0; i<numcoefs; i++) {
    delayline.push_back(0);
  }
}

template <class outSample, class inSample, class inputType>
void FIRFilter<outSample, inSample, inputType>::GetFilterCoefs
(vector<double> &coefs)
{
  coefs = this->coefs;
}

template <class outSample, class inSample, class inputType>
void FIRFilter<outSample, inSample, inputType>::ClearDelayLine()
{
  for (unsigned i=0; i<numcoefs; i++) {
    delayline[i] = 0;
  }
}

template <class outSample, class inSample, class inputType>
void FIRFilter<outSample, inSample, inputType>::GetFilterOutput
(OutputSample<inputType> &out, OutputSample<inputType> &in)
{
  delayline.push_front(in.GetOutputSampleValue()); // insert newest element
  delayline.pop_back();                            // remove oldest element

  inputType output = 0;
  for (unsigned i=0; i<numcoefs; i++) {
    output += coefs[i]*delayline[i];
  }
  out.SetOutputSampleValue(output);
}

template <class outSample, class inSample, class inputType>
void FIRFilter<outSample, inSample, inputType>::GetFilterOutput
(OutputSample<inputType> &out, InputSample<inputType> &in)
{
  delayline.push_front(in.GetSampleValue()); // insert newest element
  delayline.pop_back();                      // remove oldest element

  inputType output = 0;
  for (unsigned i=0; i<numcoefs; i++) {
    output += coefs[i]*delayline[i];
  }
  out.SetOutputSampleValue(output);
}

template <class outSample, class inSample, class inputType>
void FIRFilter<outSample, inSample, inputType>::GetFilterOutput
(InputSample<inputType> &out, OutputSample<inputType> &in)
{
  delayline.push_front(in.GetOutputSampleValue()); // insert newest element
  delayline.pop_back();                            // remove oldest element

  inputType output = 0;
  for (unsigned i=0; i<numcoefs; i++) {
    output += coefs[i]*delayline[i];
  }
  out.SetSampleValue(output);
}

template <class outSample, class inSample, class inputType>
void FIRFilter<outSample, inSample, inputType>::GetFilterOutput
(InputSample<inputType> &out, InputSample<inputType> &in)
{
  delayline.push_front(in.GetSampleValue()); // insert newest element
  delayline.pop_back();                      // remove oldest element

  inputType output = 0;
  for (unsigned i=0; i<numcoefs; i++) {
    output += coefs[i]*delayline[i];
  }
  out.SetSampleValue(output);
}

template <class outSample, class inSample, class inputType>
void FIRFilter<outSample, inSample, inputType>::GetFilterBufferOutput
(OutputSampleBlock<outSample> &out, OutputSampleBlock<outSample> &in)
{
  out.ClearBlock();
  in.GetBlockSize();
  for (unsigned i=0; i<in.GetBlockSize(); i++) {
    outSample newout;
    outSample newin;
    in.GetWaveletCoef(&newin,i);
    GetFilterOutput(newout,newin);
    out.SetWaveletCoef(newout);
  }
}


template <class outSample, class inSample, class inputType>
void FIRFilter<outSample, inSample, inputType>::GetFilterBufferOutput
(OutputSampleBlock<outSample> &out, InputSampleBlock<inSample> &in)
{
  out.ClearBlock();
  in.GetBlockSize();
  for (unsigned i=0; i<in.GetBlockSize(); i++) {
    outSample newout;
    inSample  newin;
    in.GetSample(&newin,i);
    GetFilterOutput(newout,newin);
    out.SetWaveletCoef(newout);
  }
}

template <class outSample, class inSample, class inputType>
void FIRFilter<outSample, inSample, inputType>::GetFilterBufferOutput
(InputSampleBlock<inSample> &out, OutputSampleBlock<outSample> &in)
{
  out.ClearBlock();
  in.GetBlockSize();
  for (unsigned i=0; i<in.GetBlockSize(); i++) {
    inSample  newout;
    outSample newin;
    in.GetWaveletCoef(&newin,i);
    GetFilterOutput(newout,newin);
    out.SetSample(newout);
  }
}

template <class outSample, class inSample, class inputType>
void FIRFilter<outSample, inSample, inputType>::GetFilterBufferOutput
(InputSampleBlock<inSample> &out, InputSampleBlock<inSample> &in)
{
  out.ClearBlock();
  in.GetBlockSize();
  for (unsigned i=0; i<in.GetBlockSize(); i++) {
    inSample newout;
    inSample newin;
    in.GetSample(&newin,i);
    GetFilterOutput(newout,newin);
    out.SetSample(newout);
  }
}

template <class outSample, class inSample, class inputType>
ostream & FIRFilter<outSample, inSample, inputType>::Print
(ostream &os) const
{
  os << "FIRFilter information:\n";
  os << "Coefficients,\tDelay Line" << endl;
  for (unsigned i=0; i<numcoefs; i++) {
    os << "  " << coefs[i] << ",\t\t" << delayline[i] << endl;
  }
  return os;
}

#endif
