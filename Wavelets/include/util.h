#ifndef _util
#define _util

#include <iostream>

using namespace std;

#define DEBUG 0

#ifndef CHK_DEL
#define CHK_DEL(x) if ((x)!=0) { delete (x); (x)=0;}
#endif

#if DEBUG==1
#ifndef DEBUG_PRINT
#define DEBUG_PRINT(x) cout << x << endl;
#endif
#else // DEBUG==0
#ifndef DEBUG_PRINT
#define DEBUG_PRINT(X)
#endif
#endif // end DEBUG

//#include "ioutil.h"
// Avoid using this -PAD

#ifndef MINMAX
#define MINMAX
template <typename T>
inline T MAX(const T &lhs, const T &rhs)
{
  return (lhs>rhs) ? lhs : rhs;
};

template <typename T>
inline T MIN(const T &lhs, const T &rhs)
{
  return (lhs<rhs) ? lhs : rhs;
};
#endif

#endif
