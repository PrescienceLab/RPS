#ifndef _TimeStamp
#define _TimeStamp

#if defined(WIN32) 
#if !defined(WIN32_GETTIMEOFDAY)
#define WIN32_GETTIMEOFDAY
#include <time.h>
#include <winsock.h> // for struct timeval
inline int gettimeofday(struct timeval *tv, void *junk) {
  tv->tv_sec = time(0);
  tv->tv_usec =0;
  return 0;
}
#endif
#else
#include <sys/time.h>
#endif


#include "Serializable.h"

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

};

#endif
