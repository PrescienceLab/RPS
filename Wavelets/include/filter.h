#ifndef _filter
#define _filter

#include <vector>
#include <deque>
#include <iostream>

#include "util.h"
#include "sample.h"
#include "sampleblock.h"

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class FIRFilter {
private:
  vector<double> coefs;
  deque<Sample<SAMPLETYPE> > delayline;
  unsigned numcoefs;

public:
  FIRFilter(const unsigned numcoefs=0);
  FIRFilter(const FIRFilter &rhs);
  FIRFilter(const unsigned numcoefs, const vector<double> &coefs);
  virtual ~FIRFilter();

  FIRFilter & operator=(const FIRFilter &rhs);

  void SetFilterCoefs(const vector<double> &coefs);
  void GetFilterCoefs(vector<double> &coefs) const;
  void ClearDelayLine();

  void GetFilterOutput(Sample<SAMPLETYPE> &out,
		       const Sample<SAMPLETYPE> &in);

  void GetFilterBufferOutput(SampleBlock<OUTSAMPLE> &out,
			     const SampleBlock<INSAMPLE> &in);

  ostream & Print(ostream &os) const;
  ostream & operator<<(ostream &os) const;
};

template <class SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
inline ostream & operator<<(ostream &os, const FIRFilter<SAMPLETYPE,OUTSAMPLE,INSAMPLE> &rhs) { return rhs.operator<<(os);};


template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
FIRFilter(const unsigned numcoefs)
{
  this->numcoefs = numcoefs;
  
  Sample<SAMPLETYPE> zero(0);
  coefs.clear();
  delayline.clear();
  for (unsigned i=0; i<numcoefs; i++) {
    coefs.push_back(0);
    delayline.push_back(zero);
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
FIRFilter(const FIRFilter &rhs)
{
  coefs = rhs.coefs;
  delayline = rhs.delayline;
  numcoefs = rhs.numcoefs;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
FIRFilter(const unsigned numcoefs, const vector<double> &coefs)
{
  this->numcoefs = numcoefs;
  this->coefs = coefs;

  Sample<SAMPLETYPE> zero(0);

  delayline.clear();
  for (unsigned i=0; i<numcoefs; i++) {
    delayline.push_back(zero);
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~FIRFilter()
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE> & 
FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator=(const FIRFilter &rhs)
{
  coefs = rhs.coefs;
  delayline = rhs.delayline;
  numcoefs = rhs.numcoefs;
  return *this;

}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetFilterCoefs(const vector<double> &coefs)
{
  this->numcoefs = coefs.size();
  this->coefs = coefs;
  
  Sample<SAMPLETYPE> zero(0);

  delayline.clear();
  for (unsigned i=0; i<numcoefs; i++) {
    delayline.push_back(zero);
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetFilterCoefs(vector<double> &coefs) const
{
  coefs = this->coefs;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearDelayLine()
{
  for (unsigned i=0; i<numcoefs; i++) {
    delayline[i] = 0;
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetFilterOutput(Sample<SAMPLETYPE> &out, const Sample<SAMPLETYPE> &in)
{
  delayline.push_front(in); // insert newest element
  delayline.pop_back();     // remove oldest element

  Sample<SAMPLETYPE> output;
  for (unsigned i=0; i<numcoefs; i++) {
    output += delayline[i]*coefs[i];
  }
  out = output;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetFilterBufferOutput(SampleBlock<OUTSAMPLE> &out, 
		      const SampleBlock<INSAMPLE> &in)
{
  out.ClearBlock();
  for (unsigned i=0; i<in.GetBlockSize(); i++) {
    OUTSAMPLE newout;
    INSAMPLE  newin;
    newin = in[i];
    GetFilterOutput(newout,newin);
    out.PushSampleBack(newout);
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
Print(ostream &os) const
{
  os << "FIRFilter information:\n";
  os << "Coefficients,\tDelay Line" << endl;
  for (unsigned i=0; i<numcoefs; i++) {
    os << "  " << coefs[i] << ",\t\t" << delayline[i] << endl;
  }
  return os;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator<<(ostream &os) const 
{ return Print(os);}

#endif
