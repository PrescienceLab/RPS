#ifndef _sampleblockout
#define _sampleblockout

#include <iterator>
#include <vector>
#include <iostream>

#include "sampleout.h"
#include "util.h"

template <class structType>
class OutputSampleBlock {
private:
  structType coefs;

public:
  OutputSampleBlock() : coefs() {};
  OutputSampleBlock(const OutputSampleBlock &rhs) {
    coefs = rhs.coefs;
  };
  OutputSampleBlock(structType &coefs) {
    this->coefs = coefs;
  };
  virtual ~OutputSampleBlock() {};

  OutputSampleBlock & operator=(const OutputSampleBlock &rhs) {
    coefs = rhs.coefs;
  };

  virtual void SetCoefs(structType &coefs)=0;
  virtual void GetCoefs(structType &buf)=0;
  const unsigned GetBlockSize();

  ostream & Print(ostream &os) const;
};

#endif
