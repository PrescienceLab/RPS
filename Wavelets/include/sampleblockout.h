#ifndef _sampleblockout
#define _sampleblockout

#include <vector>
#include <iostream>

#include "util.h"

template <class reprType>
class OutputSampleBlock {
public:
  OutputSampleBlock() {};
  virtual ~OutputSampleBlock() {};

  virtual OutputSampleBlock & operator=(const OutputSampleBlock &rhs) {};

  virtual void SetWaveletCoef(reprType &coef)=0;
  virtual void GetWaveletCoef(reprType *coef, unsigned i)=0;
  virtual void ClearBlock()=0;
  virtual unsigned GetBlockSize() const=0;

  virtual ostream & Print(ostream &os) const=0;
};

#endif
