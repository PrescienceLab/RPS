#ifndef _util
#define _util

#ifndef CHK_DEL
#define CHK_DEL(x) if ((x)!=0) { delete (x); (x)=0;}
#endif
#ifndef CHK_DEL_MAT
#define CHK_DEL_MAT(x) if ((x)!=0) { delete [] (x); (x)=0;}
#endif
#define ABS(x) ((x)> 0 ? (x) : (-(x)))
#define SQUARE(x) ((x)*(x))
#define LOG2(x) (log(x)/log(2.0))


#if 0
#ifndef MAX
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#endif
#endif

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

#define DEBUG 0


#if DEBUG==1
#ifndef DEBUG_PRINT
#define DEBUG_PRINT(x) cout << x << endl;
#endif
#else // DEBUG==0
#ifndef DEBUG_PRINT
#define DEBUG_PRINT(X)
#endif
#endif // end DEBUG

#endif
