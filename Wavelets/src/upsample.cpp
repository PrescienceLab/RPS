#include "upsample.h"

UpSample::UpSample(unsigned rate=1)
{
  this->rate = rate;
  samplecount = 0;
}

UpSample::UpSample(const UpSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
}

UpSample::~UpSample()
{
}

UpSample & UpSample::operator=(const UpSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
  return *this;
}

bool UpSample::ZeroSample()
{
  bool zero=true;
  if (samplecount%rate == 0) {
    zero=false;
  }

  samplecount++;
  return zero;
}

void UpSample::UpSampleBuffer(vector<double> &output, vector<double> &input)
{
  output.clear();

  unsigned zeros = GetNumberZeroFill();
  for (unsigned i=0; i<input.size(); i++) {
    output.push_back(input[i]);
    for (unsigned j=0; j<zeros; j++) {
      output.push_back(0);
    }
  } 
}

ostream & UpSample::Print(ostream &os) const
{
  os << "UpSample Information:\n";
  os << "  Current upsample rate: " << rate << endl;
  os << "  Current sample count:  " << samplecount << endl;
  return os;
}
