#ifndef _Measurement
#define _Measurement

#define MEASUREMENT_MAXSERLEN 1000


#include <stdio.h>
#include <iostream>

#include "Serializable.h"
#include "ModelInfo.h"
#include "TimeStamp.h"



struct Measurement : public SerializeableInfo {
  unsigned tag;
  TimeStamp timestamp;
  unsigned  period_usec;
  int       serlen;
  double    *series;


  Measurement(int len=0);
  Measurement(TimeStamp &ts,
	      int      serlen, 
	      double   *series,
	      unsigned period_usec);
  Measurement(const Measurement &right);

  virtual ~Measurement();

  virtual Measurement & Measurement::operator = (const Measurement &right) ;

  int Resize(int len, bool copy=true);
  int SetSeries(double *ser, unsigned len);

  int GetPackedSize() const;
  int GetMaxPackedSize() const;
  int Pack(Buffer &buf) const;
  int Unpack(Buffer &buf);

  void Print(FILE *out=stdout) const;
  ostream &Print(ostream &os) const;
  ostream &operator<<(ostream &os) const;
};  

inline ostream & operator<<(ostream &os, const Measurement &rhs) { return rhs.operator<<(os);}

#endif
