#ifndef _filter
#define _filter

#include <vector>
#include <deque>
#include <iostream>

#include "util.h"
#include "sample.h"
#include "sampleblock.h"

template <class OUTSAMPLE, class INSAMPLE>
class FIRFilter {
private:
  vector<double>    coefs;
  deque<INSAMPLE>   delayline;
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

  void   GetFilterOutput(OUTSAMPLE &out,
			 INSAMPLE  &in);

  void GetFilterBufferOutput(SampleBlock<OUTSAMPLE> &out,
			     SampleBlock<INSAMPLE>  &in);

  ostream & Print(ostream &os) const;
};

template <class OUTSAMPLE, class INSAMPLE>
FIRFilter<OUTSAMPLE, INSAMPLE>::FIRFilter(unsigned numcoefs)
{
  this->numcoefs = numcoefs;
  
  INSAMPLE zero = 0;
  coefs.clear();
  delayline.clear();
  for (unsigned i=0; i<numcoefs; i++) {
    coefs.push_back(0);
    delayline.push_back(zero);
  }
}

template <class OUTSAMPLE, class INSAMPLE>
FIRFilter<OUTSAMPLE, INSAMPLE>::FIRFilter(const FIRFilter &rhs)
{
  coefs = rhs.coefs;
  delayline = rhs.delayline;
  numcoefs = rhs.numcoefs;
}

template <class OUTSAMPLE, class INSAMPLE>
FIRFilter<OUTSAMPLE, INSAMPLE>::FIRFilter
(unsigned numcoefs, vector<double> &coefs)
{
  this->numcoefs = numcoefs;
  this->coefs = coefs;

  INSAMPLE zero = 0;

  delayline.clear();
  for (unsigned i=0; i<numcoefs; i++) {
    delayline.push_back(zero);
  }
}

template <class OUTSAMPLE, class INSAMPLE>
FIRFilter<OUTSAMPLE, INSAMPLE>::~FIRFilter()
{
}

template <class OUTSAMPLE, class INSAMPLE>
FIRFilter<OUTSAMPLE, INSAMPLE> & 
FIRFilter<OUTSAMPLE, INSAMPLE>::operator=
(const FIRFilter &rhs)
{
  coefs = rhs.coefs;
  delayline = rhs.delayline;
  numcoefs = rhs.numcoefs;
  return *this;

}

template <class OUTSAMPLE, class INSAMPLE>
void FIRFilter<OUTSAMPLE, INSAMPLE>::SetFilterCoefs
(vector<double> &coefs)
{
  this->numcoefs = coefs.size();
  this->coefs = coefs;
  
  INSAMPLE zero = 0;

  delayline.clear();
  for (unsigned i=0; i<numcoefs; i++) {
    delayline.push_back(zero);
  }
}

template <class OUTSAMPLE, class INSAMPLE>
void FIRFilter<OUTSAMPLE, INSAMPLE>::GetFilterCoefs
(vector<double> &coefs)
{
  coefs = this->coefs;
}

template <class OUTSAMPLE, class INSAMPLE>
void FIRFilter<OUTSAMPLE, INSAMPLE>::ClearDelayLine()
{
  for (unsigned i=0; i<numcoefs; i++) {
    delayline[i] = 0;
  }
}

template <class OUTSAMPLE, class INSAMPLE>
void FIRFilter<OUTSAMPLE, INSAMPLE>::GetFilterOutput
(OUTSAMPLE &out, INSAMPLE &in)
{
  delayline.push_front(in); // insert newest element
  delayline.pop_back();     // remove oldest element

  OUTSAMPLE output;
  for (unsigned i=0; i<numcoefs; i++) {
    output += delayline[i]*coefs[i];
  }
  out = output;
}

template <class OUTSAMPLE, class INSAMPLE>
void FIRFilter<OUTSAMPLE, INSAMPLE>::GetFilterBufferOutput
(SampleBlock<OUTSAMPLE> &out, SampleBlock<INSAMPLE> &in)
{
  out.ClearBlock();
  in.GetBlockSize();
  for (unsigned i=0; i<in.GetBlockSize(); i++) {
    OUTSAMPLE newout;
    INSAMPLE newin;
    in.GetSample(&newin,i);
    GetFilterOutput(newout,newin);
    out.SetSample(newout);
  }
}

template <class OUTSAMPLE, class INSAMPLE>
ostream & FIRFilter<OUTSAMPLE, INSAMPLE>::Print
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
