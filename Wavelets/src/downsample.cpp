#include "downsample.h"

DownSample::DownSample(unsigned rate=1)
{
  this->rate = rate;
  samplecount = 0;
}

DownSample::DownSample(const DownSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
}

DownSample::~DownSample()
{
}

DownSample & DownSample::operator=(const DownSample &rhs)
{
  rate = rhs.rate;
  samplecount = rhs.samplecount;
  return *this;
}

bool DownSample::KeepSample()
{
  bool keep=false;
  if (samplecount%rate == 0) {
    keep = true;
  }

  samplecount++;
  return keep;
}

void DownSample::DownSampleBuffer(vector<double> &output, vector<double> &input)
{
  output.clear();

  for (unsigned i=0; i<input.size(); i++) {
    if (KeepSample()) {
      output.push_back(input[i]);
    }
  }
}

ostream & DownSample::Print(ostream &os) const
{
  os << "DownSample information:\n";
  os << "  Current downsample rate: " << rate << endl;
  os << "  Current sample count (state): " << samplecount << endl;
  return os;
}
