#ifndef _sampleblockout
#define _sampleblockout

#include <iterator>
#include <vector>
#include <iostream>

#include "sampleout.h"
#include "util.h"

class SampleBlockOut {
private:
  vector<SampleOut> coefs;

public:
  SampleBlockOut();
  SampleBlockOut(const SampleBlockOut &rhs);
  SampleBlockOut(vector<SampleOut> &coefs);
  virtual ~SampleBlockOut();

  SampleBlockOut & operator=(const SampleBlockOut &rhs);

  void AddCoefs(vector<SampleOut> &coefs);
  const unsigned GetBlockSize();

  ostream & Print(ostream &os) const;
};

#endif
