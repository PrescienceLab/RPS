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
  deque<Sample<SAMPLETYPE> >* delayline;
  unsigned numcoefs;

public:
  FIRFilter(const unsigned numcoefs=0);
  FIRFilter(const FIRFilter &rhs);
  FIRFilter(const unsigned numcoefs, const vector<double> &coefs);
  virtual ~FIRFilter();

  FIRFilter & operator=(const FIRFilter &rhs);

  void SetFilterCoefs(const vector<double> &coefs);
  void GetFilterCoefs(vector<double> &coefs) const;

  void SetNumCoefs(const unsigned numcoefs);
  inline unsigned GetNumCoefs() const;

  void ClearDelayLine();

  void GetFilterOutput(Sample<SAMPLETYPE> &out,
		       const Sample<SAMPLETYPE> &in);

  void GetFilterBufferOutput(SampleBlock<OUTSAMPLE> &out,
			     const SampleBlock<INSAMPLE> &in);

  ostream & Print(ostream &os) const;
  ostream & operator<<(ostream &os) const;
};

template <class SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
inline ostream & operator<<(ostream &os, 
			    const FIRFilter<SAMPLETYPE,OUTSAMPLE,INSAMPLE> &rhs) {
  return rhs.operator<<(os);
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
FIRFilter(const unsigned numcoefs)
{
  this->numcoefs = numcoefs;
  coefs.clear();
  delayline = new deque<Sample<SAMPLETYPE> >(numcoefs);
  for (unsigned i=0; i<numcoefs; i++) {
    coefs.push_back(0.0);
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
FIRFilter(const FIRFilter &rhs)
{
  coefs = rhs.coefs;
  delayline = new deque<Sample<SAMPLETYPE> >(rhs.GetNumCoefs());
  numcoefs = rhs.numcoefs;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
FIRFilter(const unsigned numcoefs, const vector<double> &coefs)
{
  this->numcoefs = numcoefs;
  this->coefs = coefs;
  delayline = new deque<Sample<SAMPLETYPE> >(numcoefs);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~FIRFilter()
{
  CHK_DEL(delayline);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE> & 
FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator=(const FIRFilter &rhs)
{
  this->~FIRFilter();
  return *(new (this) FIRFilter(rhs));
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetFilterCoefs(const vector<double> &coefs)
{
  this->numcoefs = coefs.size();
  this->coefs = coefs;

  CHK_DEL(delayline);
  delayline = new deque<Sample<SAMPLETYPE> >(numcoefs);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetFilterCoefs(vector<double> &coefs) const
{
  coefs = this->coefs;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetNumCoefs(const unsigned numcoefs)
{
  this->numcoefs = numcoefs;

  coefs.clear();
  for (unsigned i=0; i<numcoefs; i++) {
    coefs.push_back(0.0);
  }

  CHK_DEL(delayline);
  delayline = new deque<Sample<SAMPLETYPE> >(numcoefs);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetNumCoefs() const
{
  return this->numcoefs;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearDelayLine()
{
  CHK_DEL(delayline);
  delayline = new deque<Sample<SAMPLETYPE> >(numcoefs);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetFilterOutput(Sample<SAMPLETYPE> &out, const Sample<SAMPLETYPE> &in)
{
  delayline->push_front(in); // insert newest element
  delayline->pop_back();     // remove oldest element

  Sample<SAMPLETYPE> output;
  for (unsigned i=0; i<numcoefs; i++) {
    output += delayline->operator[](i)*coefs[i];
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
    os << "  " << coefs[i] << ",\t\t" << delayline->operator[](i);
  }
  return os;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator<<(ostream &os) const 
{ return Print(os);}

#endif
