#ifndef _XDRpack
#define _XDRpack

#include <string.h>

#include "socks.h"


#if !defined(IEEE_FLOAT_LSB) && !defined(IEEE_FLOAT_MSB)
#if defined (__alpha) || defined (__alpha__)
#define IEEE_FLOAT_LSB
#elif defined(sun) || defined(sparc)
#define IEEE_FLOAT_MSB
#elif defined(__i386__) || defined(i386) || defined(__i386)
#define IEEE_FLOAT_LSB
#else
#error Can not determine IEEE float byte order
#endif
#endif

#if !defined(IEEE_DOUBLE_LSB) && !defined(IEEE_DOUBLE_MSB)
#if defined (__alpha) || defined (__alpha__)
#define IEEE_DOUBLE_LSB
#elif defined(sun) || defined(sparc)
#define IEEE_DOUBLE_MSB
#elif defined(__i386__) || defined(i386) || defined(__i386)
#define IEEE_DOUBLE_LSB
#else
#error Can not determine IEEE double byte order
#endif
#endif

#if !defined(WIN32) || defined(__CYGWIN__)
#if defined(IEEE_FLOAT_LSB)
#warning This is a IEEE_FLOAT_LSB machine
#endif
#if defined(IEEE_FLOAT_MSB)
#warning This is a IEEE_FLOAT_MSB machine
#endif
#if defined(IEEE_DOUBLE_LSB)
#warning This is a IEEE_DOUBLE_LSB machine
#endif
#if defined(IEEE_DOUBLE_MSB)
#warning This is a IEEE_DOUBLE_MSB machine
#endif
#endif


inline void Swap(const char *in, char *out, int num)
{
  int i;
  for (i=0;i<num;i++) { 
    out[i] = in[num-i-1];
  }
}

inline void htonf(const float x, char out[4]) 
{
  float x2=x;
  char *in = (char *) &x2;
#ifdef IEEE_FLOAT_LSB
  Swap(in,out,4);
#else
  memcpy(out,in,4);
#endif
}


inline void ntohf(const char in[4], float *x) 
{
  char *out = (char *)x;
#ifdef IEEE_FLOAT_LSB
  Swap(in,out,4);
#else
  memcpy(out,in,4);
#endif
}


inline void htond(const double x, char out[8]) 
{
  double x2=x;
  char *in = (char *) &x2;
#ifdef IEEE_DOUBLE_LSB
  Swap(in,out,8);
#else
  memcpy(out,in,8);
#endif
}


inline void ntohd(const char in[8], double *x) 
{
  char *out=(char*)x;
#ifdef IEEE_DOUBLE_LSB
  Swap(in,out,8);
#else
  memcpy(out,in,8);
#endif
}

#if 0
template <class T> 
inline void Pack(char *buf, const T x)
{
  memcpy(buf,&x,sizeof(T));
};

template <class T> 
inline void Unpack(const char *buf, T *x)
{
  memcpy(x,buf,sizeof(T));
};
#endif



inline void Pack(char *buf, const char x)
{
  buf[0]=x;
}

inline void Unpack(const char *buf, char *x)
{
  *x=buf[0];
}

inline void Pack(char *buf, const unsigned char x) 
{ 
  Pack(buf,(const char) x);
}

inline void Unpack(const char *buf, unsigned char *x) 
{ 
  Unpack(buf,(char *) x);
}

inline void Pack(char *buf, const bool x)
{
  Pack(buf,(const char)x);
}

inline void Unpack(const char *buf, bool *x) 
{
  Unpack(buf,(char *)x);
}

inline void Pack(char *buf, const short x)
{
  short y=htons(x);
  memcpy(buf,&y,sizeof(short));
}

inline void Unpack(const char *buf, short *x)
{
  memcpy(x,buf,sizeof(short));
  *x=ntohs(*x);
}

inline void Pack(char *buf, const unsigned short x) 
{ 
  Pack(buf,(const short) x);
}

inline void Unpack(const char *buf, unsigned short *x) 
{ 
  Unpack(buf,(short *) x);
}

inline void Pack(char *buf, const int x)
{
  int y = htonl(x);
  memcpy(buf,&y,sizeof(int));
}

inline void Unpack(const char *buf, int *x)
{
  memcpy(x,buf,sizeof(int));
  *x=ntohl(*x);
}

inline void Pack(char *buf, const unsigned int x) 
{ 
  Pack(buf,(const int) x);
}


inline void Unpack(const char *buf, unsigned int *x)
{
  Unpack(buf,(int *)x);
}


// the follow assume 32 bit longs
inline void Pack(char *buf, const long x)
{
  assert(sizeof(long)==sizeof(int));
  Pack(buf,(const int)x);
}

inline void Unpack(const char *buf, long *x)
{
  assert(sizeof(long)==sizeof(int));
  Unpack(buf,(int*)x);
}

inline void Pack(char *buf, const unsigned  long x) 
{ 
  Pack(buf,(const long) x);
}


inline void Unpack(const char *buf, unsigned long *x)
{
  Unpack(buf,(long *)x);
}


// Floating point
inline void Pack(char *buf, const double x)
{
  char out[8];
  htond(x,out);
  memcpy(buf,out,8);
}

inline void Unpack(const char *buf, double *x)
{
  char in[8];
  memcpy(in,buf,8);
  ntohd(in,x);
}

inline void Pack(char *buf, const float x)
{
  char out[4];
  htonf(x,out);
  memcpy(buf,out,4);
}

inline void Unpack(const char *buf, float *x)
{
  char in[4];
  memcpy(in,buf,4);
  ntohf(in,x);
}


#endif
