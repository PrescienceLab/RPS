#include "sampleout.h"

SampleOut::SampleOut()
{
  coef = 0;
  level = -1;
}

SampleOut::SampleOut(const SampleOut &rhs)
{
  coef = rhs.coef;
  level = rhs.level;
}

SampleOut::SampleOut(double coef, int level)
{
  this->coef = coef;
  this->level = level;
}

SampleOut::~SampleOut()
{
};

SampleOut & SampleOut::operator=(const SampleOut &rhs)
{
  coef = rhs.coef;
  level = rhs.level;
  return *this;
}

inline void SampleOut::SetSampleOutValue(double coef)
{
  this->coef = coef;
}

double SampleOut::GetSampleOutValue()
{
  return coef;
}

inline void SampleOut::SetSampleOutLevel(int level)
{
  this->level = level;
}

inline int SampleOut::GetSampleOutLevel()
{
  return level;
}

ostream & SampleOut::Print(ostream &os) const
{
  os << coef << "\t" << level << endl;
  return os;
}
