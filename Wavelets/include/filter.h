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
  FIRFilter(unsigned numcoefs=0) {
    this->numcoefs = numcoefs;
      
    coefs.clear();
    delayline.clear();
    for (unsigned i=0; i<numcoefs; i++) {
      coefs.push_back(0);
      delayline.push_back(0);
    }
  };

  FIRFilter(const FIRFilter &rhs) {
    coefs = rhs.coefs;
    delayline = rhs.delayline;
    numcoefs = rhs.numcoefs;
  };

  FIRFilter(unsigned numcoefs, vector<double> &coefs) {
    this->numcoefs = numcoefs;
    this->coefs = coefs;
    
    delayline.clear();
    for (unsigned i=0; i<numcoefs; i++) {
      delayline.push_back(0);
    }
  };

  virtual ~FIRFilter() {};

  FIRFilter & operator=(const FIRFilter &rhs) {
    coefs = rhs.coefs;
    delayline = rhs.delayline;
    numcoefs = rhs.numcoefs;
    return *this;
  };

  void   SetFilterCoefs(vector<double> &coefs) {
    this->numcoefs = coefs.size();
    this->coefs = coefs;
 
    delayline.clear();
    for (unsigned i=0; i<numcoefs; i++) {
      delayline.push_back(0);
    }
  };

  void   GetFilterCoefs(vector<double> &coefs) {
    coefs = this->coefs;
  };

  void ClearDelayLine() {
    for (unsigned i=0; i<numcoefs; i++) {
      delayline[i] = 0;
    }
  };

  void GetFilterOutput(OutputSample<inputType> &out, InputSample<inputType> &in) {
    delayline.push_front(in.GetSampleValue()); // insert newest element
    delayline.pop_back();                      // remove oldest element

    inputType output = 0;
    for (unsigned i=0; i<numcoefs; i++) {
      output += coefs[i]*delayline[i];
    }
    out.SetOutputSampleValue(output);
  };

  void TestTypes(OutputSampleBlock<outSample> &out,
		 InputSampleBlock<inSample> &in) {
    out.ClearBlock();
    in.GetBlockSize();
  };

  void GetFilterBufferOutput(OutputSampleBlock<outSample> &out,
			     InputSampleBlock<inSample> &in) {
    out.ClearBlock();
    in.GetBlockSize();
    for (unsigned i=0; i<in.GetBlockSize(); i++) {
      outSample newout;
      inSample  newin;
      in.GetSample(&newin,i);
      GetFilterOutput(newout,newin);
      out.SetWaveletCoef(newout);
    }
  };

  ostream & Print(ostream &os) const {
    os << "FIRFilter information:\n";
    os << "Coefficients,\tDelay Line" << endl;
    for (unsigned i=0; i<numcoefs; i++) {
      os << "  " << coefs[i] << ",\t\t" << delayline[i] << endl;
    }
    return os;
  };

};

#endif
