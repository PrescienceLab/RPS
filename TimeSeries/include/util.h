#ifndef _util
#define _util

#ifndef CHK_DEL
#define CHK_DEL(x) if ((x)!=0) { delete (x); (x)=0;}
#endif
#ifndef CHK_DEL_MAT
#define CHK_DEL_MAT(x) if ((x)!=0) { delete [] (x); (x)=0;}
#endif
#ifndef MAX
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#endif
#define ABS(x) ((x)> 0 ? (x) : (-(x)))
#define SQUARE(x) ((x)*(x))
#define LOG2(x) (log(x)/log(2))

#endif
