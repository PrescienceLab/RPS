#ifndef _Random
#define _Random
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#if defined(WIN32) // cygwin also does not have these
#define srandom(x) srand(x)
#define random() rand()
#define srand48(x) srand(x)
#define lrand48() rand()
#define mrand48() ( lrand48()>RAND_MAX/2 ? -lrand48() : lrand48())
#define drand48() (((double)lrand48())/((double)RAND_MAX))
#endif

inline void InitRandom() 
{
  srandom(time(0));
  srand48(time(0));
}
  
inline int UnsignedRandom()
{
  return lrand48();
}

inline int SignedRandom()
{
  return mrand48();
}


inline double UniformRandom()
{
  return drand48();
}

inline double UniformRandom(const double lower, const double upper)
{
  return lower + (upper-lower)*UniformRandom();
}

inline double NormalRandom(const double mean, const double sdev)
{
  const int n=12;
  int i;
  double sum;

  sum=0;
  for (i=1;i<=n;i++) {
    sum+=UniformRandom();
  }

  return mean + sdev*((sum-((double)n)/2.0)/sqrt(((double)n)/12.0));
}

inline double ExpRandom(const double mean)
{
  return -mean*log(UniformRandom());
}

#endif
