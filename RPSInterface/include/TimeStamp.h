#ifndef _TimeStamp
#define _TimeStamp

#include <stdio.h>
#include <iostream>
#include "Timers.h"
#include "Serializable.h"

using namespace std;

struct TimeStamp : public timeval, public SerializeableInfo {
  TimeStamp(const unsigned sec, const unsigned usec);  
  TimeStamp(const struct timeval *tv=0);
  virtual ~TimeStamp();
  TimeStamp & operator = (const TimeStamp &right) ;

  bool operator == (const TimeStamp &right);
  bool operator != (const TimeStamp &right);
  bool operator < (const TimeStamp &right);
  bool operator > (const TimeStamp &right);
  bool operator >= (const TimeStamp &right);
  bool operator <= (const TimeStamp &right);


  int GetPackedSize() const ;
  int GetMaxPackedSize() const ;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;

  operator double() const ;

  void Print(FILE *out=stdout) const;
  ostream & Print(ostream &os) const;
  ostream & operator<<(ostream &os) const;

};

inline ostream & operator<<(ostream &os, const TimeStamp &rhs) { return rhs.operator<<(os);}

#endif
