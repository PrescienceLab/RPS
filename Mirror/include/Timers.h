#ifndef _timers
#define _timers

// Generic timers
#if defined(WIN32) 
#if!defined(WIN32_GETTIMEOFDAY)
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

typedef class TimeValue IntervalValue;

#define uint32 unsigned

class TimeValue {
private:
  unsigned sec;
  unsigned usec;
public:
  inline TimeValue(unsigned sec=0, unsigned usec=0) {
    this->sec=sec; this->usec=usec;
  }
  inline TimeValue(double secs) {
    sec = (int)secs;
    usec = (int)(1e6*(secs-(int)secs));
  }
  inline void GetCurrentTime() {  
    struct timeval t;
    gettimeofday(&t,0);
    sec=t.tv_sec;
    usec=t.tv_usec;
  };
  inline double GetSeconds() const {
    return sec + usec/1e6;
  };

  static inline void Diff(TimeValue &earlier, 
                          TimeValue &later, 
                          IntervalValue *destination) {
    if (later.usec>=earlier.usec) {
      destination->usec=later.usec-earlier.usec;
      destination->sec=later.sec-earlier.sec;
    } else {
      destination->usec=(later.usec+1000000)-earlier.usec;
      destination->sec=later.sec-earlier.sec-1;
    }
  };
  bool operator < (const TimeValue &right) const {
    return ((sec<right.sec) || ((sec==right.sec) && (usec<right.usec)));
  }
  bool operator == (const TimeValue &right) const {
    return (sec==right.sec && usec==right.usec);
  }
  bool operator != (const TimeValue &right) const {
    return !(*this==right);
  }
  bool operator > (const TimeValue & right) const {
    return !(*this <= right);
  }
  bool operator <= (const TimeValue & right) const {
    return (*this < right) || (*this == right);
  }
  bool operator >= (const TimeValue & right) const {
    return (*this > right) || (*this == right);
  }
  TimeValue & operator = (const TimeValue &right) {
    this->sec=right.sec; this->usec=right.usec;
    return *this;
  }
  void operator += (const TimeValue &right) {
    if (usec+right.usec < 1000000) {
      usec+=right.usec;
      sec+=right.sec;
    } else {
      usec+=right.usec;
      usec-=1000000;
      sec+=right.sec;
      sec+=1;
    }
  }
  void operator -= (const TimeValue &right) {
    if (usec-right.usec < usec) {
      usec-=right.usec;
      sec-=right.sec;
    } else {
      usec+=1000000;
      usec-=right.usec;
      sec-=right.sec;
      sec-=1;
    }
  }
  void MakeTimeval(struct timeval *tv) const {
    tv->tv_sec=sec; tv->tv_usec=usec;
  }

};

const TimeValue MAXTIME(0xffffffff,999999);
const TimeValue MINTIME(0,0);

#endif

