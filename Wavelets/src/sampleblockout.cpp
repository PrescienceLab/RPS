#include "sampleblockout.h"

SampleBlockOut::SampleBlockOut()
{
  coefs.clear();
}

SampleBlockOut::SampleBlockOut(const SampleBlockOut &rhs)
{
  coefs = rhs.coefs;
}

SampleBlockOut::SampleBlockOut(vector<SampleOut> &coefs)
{
  this->coefs = coefs;
}

SampleBlockOut::~SampleBlockOut()
{
  coefs.clear();
}

SampleBlockOut & SampleBlockOut::operator=(const SampleBlockOut &rhs)
{
  coefs = rhs.coefs;
  return *this;
}

void SampleBlockOut::AddCoefs(vector<SampleOut> &coefs)
{
  this->coefs = coefs;
}

const unsigned SampleBlockOut::GetBlockSize()
{
  return coefs.size();
}

ostream & SampleBlockOut::Print(ostream &os) const
{
  os << "SampleBlockOut values:\n";
  os << "  Coefficient:\tLevel:\n";

  for (unsigned i=0; i<coefs.size(); i++) {
    os << "   " << coefs[i];
  }

  return os;
}
