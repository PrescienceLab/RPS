#ifndef _junk
#define _junk

#ifdef MAX
#undef MAX
#endif
#ifdef MIN
#undef MIN
#endif

#ifndef CHK_DEL
#define CHK_DEL(x) if (x) { delete (x); x=0;} 
#endif

#ifndef CHK_DEL_MAT
#define CHK_DEL_MAT(x) if (x) { delete [] (x); x=0;} 
#endif

template <class T> 
void SWAP(T& left, T&right)
{
   T temp = left;
   left=right;
   right=temp;
};

#ifndef MINMAX
#define MINMAX
template <class T> 
inline T  MIN (T left, T right) 
{
  if (left<right) { 
    return left;
  } else {
    return right;
  }
};

template <class T> 
inline T  MAX (T left, T right) 
{
  if (left>right) { 
    return left;
  } else {
    return right;
  }
};
#endif

#endif
