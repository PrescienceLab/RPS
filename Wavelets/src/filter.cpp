#include "filter.h"

FIRFilter::FIRFilter(unsigned numcoefs=0)
{
  this->numcoefs = numcoefs;

  coefs.clear();
  delayline.clear();
  for (unsigned i=0; i<numcoefs; i++) {
    coefs.push_back(0);
    delayline.push_back(0);
  }
}

FIRFilter::FIRFilter(const FIRFilter &rhs)
{
  coefs = rhs.coefs;
  delayline = rhs.delayline;
  numcoefs = rhs.numcoefs;
}

FIRFilter::FIRFilter(unsigned numcoefs, vector<double> &coefs)
{
  this->numcoefs = numcoefs;
  this->coefs = coefs;

  delayline.clear();
  for (unsigned i=0; i<numcoefs; i++) {
    delayline.push_back(0);
  }
}

FIRFilter::~FIRFilter()
{
  coefs.clear();
  delayline.clear();
}

FIRFilter & FIRFilter::operator=(const FIRFilter &rhs)
{
  coefs = rhs.coefs;
  delayline = rhs.delayline;
  numcoefs = rhs.numcoefs;
  return *this;
}

void FIRFilter::SetFilterCoefs(vector<double> &coefs)
{
  this->numcoefs = coefs.size();
  this->coefs = coefs;
 
  delayline.clear();
  for (unsigned i=0; i<numcoefs; i++) {
    delayline.push_back(0);
  }
}

void FIRFilter::GetFilterCoefs(vector<double> &coefs)
{
  coefs = this->coefs;
}

void FIRFilter::ClearDelayLine()
{
  for (unsigned i=0; i<numcoefs; i++) {
    delayline[i] = 0;
  }
}

// A streaming operation sample by sample
double FIRFilter::GetFilterOutput(double input)
{
  delayline.push_front(input); // insert newest element
  delayline.pop_back();        // remove oldest element

  double output = 0;
  for (unsigned i=0; i<numcoefs; i++) {
    output += coefs[i]*delayline[i];
  }
  return output;
}

// A buffering operation buffer by buffer ** NOTE: Uses delayline **
void FIRFilter::GetFilterBufferOutput(vector<double> &output,
				      vector<double> &input)
{
  output.clear();

  unsigned outsize = numcoefs + input.size() - 1;
  unsigned i;
  for (i=0; i<input.size(); i++) {
    output.push_back(GetFilterOutput(input[i]));
  }
  for (;i<outsize; i++) {
    output.push_back(GetFilterOutput(0));
  }
}

ostream & FIRFilter::Print(ostream &os) const
{
  os << "FIRFilter information:\n";
  os << "Coefficients,\tDelay Line" << endl;
  for (unsigned i=0; i<numcoefs; i++) {
    os << "  " << coefs[i] << ",\t\t" << delayline[i] << endl;
  }
  return os;
}
