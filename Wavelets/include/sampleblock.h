#ifndef _sampleblock
#define _sampleblock

#include <vector>
#include <iostream>

#include "sample.h"
#include "util.h"

class SampleBlock {
private:
  vector<Sample> samples;

public:
  SampleBlock();
  SampleBlock(const SampleBlock &rhs);
  SampleBlock(vector<Sample> &input);
  virtual ~SampleBlock();
  
  SampleBlock & operator=(const SampleBlock &rhs);

  void AddSamples(vector<Sample> &input);
  
  ostream & Print(ostream &os) const;
};

#endif
