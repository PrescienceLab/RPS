#include <string.h>
#include <stdio.h>
#include "TimeStamp.h"



TimeStamp::TimeStamp(const unsigned sec, const unsigned usec) 
{
  tv_sec = sec;
  tv_usec = usec;
}



TimeStamp::TimeStamp(const struct timeval *tv) 
{
  if (tv) {
    *((timeval *)(this)) = *tv;
  } else {
    gettimeofday((struct timeval *)this,0);
  }
}

TimeStamp::~TimeStamp()
{}


TimeStamp & TimeStamp::operator = (const TimeStamp &right) 
{
  memcpy(this,&right,sizeof(TimeStamp));
  return *this;
}

bool TimeStamp::operator == (const TimeStamp &right) 
{
  return tv_sec==right.tv_sec && tv_usec==right.tv_usec;
}

bool TimeStamp::operator != (const TimeStamp &right) 
{
  return !((*this)==right);
}



bool TimeStamp::operator < (const TimeStamp &right) 
{
  if (tv_sec<right.tv_sec) { 
    return true;
  } else if (tv_sec==right.tv_sec) { 
    return tv_usec<right.tv_usec;
  } else {
    return false;
  }
}

bool TimeStamp::operator > (const TimeStamp &right)
{
  return !( (*this) <= right);
}

bool TimeStamp::operator >= (const TimeStamp &right) 
{
  return ((*this) == right) || ((*this) > right);
}

bool TimeStamp::operator <= (const TimeStamp &right) 
{
  return ((*this) == right) || ((*this) < right);
}



int TimeStamp::GetPackedSize() const 
{
  return 8;
}

int TimeStamp::GetMaxPackedSize() const 
{
  return GetPackedSize();
}

int TimeStamp::Pack(Buffer &buf) const 
{
  buf.Pack(&tv_sec,1);
  buf.Pack(&tv_usec,1);
  return 0;
}

int TimeStamp::Unpack(Buffer &buf) 
{
  buf.Unpack(&tv_sec,1);
  buf.Unpack(&tv_usec,1);
  return 0;
}

TimeStamp::operator double() const 
{
  return (double) ((double)(tv_sec) + ((double)tv_usec/1e6));
}

void TimeStamp::Print(FILE *out) const 
{
  fprintf(out,"TimeStamp: tv_sec=%lu, tv_usec=%lu\n",tv_sec,tv_usec);
}

ostream &TimeStamp::Print(ostream &os) const
{
  os << "TimeStamp(tv_sec="<<tv_sec<<", tv_usec="<<tv_usec<<")";
  return os;
}
