#include "sampleblock.h"

SampleBlock::SampleBlock()
{
  samples.clear();
}

SampleBlock::SampleBlock(const SampleBlock &rhs)
{
  samples = rhs.samples;
}

SampleBlock::SampleBlock(vector<Sample> &input)
{
  samples = input;
}

SampleBlock::~SampleBlock()
{
  samples.clear();
}

SampleBlock & SampleBlock::operator=(const SampleBlock &rhs)
{
  samples = rhs.samples;
  return *this;
}

void SampleBlock::AddSamples(vector<Sample> &input)
{
  samples = input;
}

ostream & SampleBlock::Print(ostream &os) const
{
  os << "SampleBlock values: \n";
  for (unsigned i=0; i<samples.size(); i++) {
    os << "\t" << samples[i];
  }
  return os;
}
