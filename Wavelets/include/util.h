#ifndef _util
#define _util

#include <iostream>

#define DEBUG 1

using namespace std;

#ifndef CHK_DEL
#define CHK_DEL(x) if ((x)!=0) { delete (x); (x)=0;}
#endif

template <class T> 
inline ostream & operator<< (ostream &lhs, const T &rhs) { return rhs.Print(lhs);} ;

template <typename T>
T MAX(const T &lhs, const T &rhs)
{
  return (lhs>rhs) ? lhs : rhs;
};

template <typename T>
T MIN(const T &lhs, const T &rhs)
{
  return (lhs<rhs) ? lhs : rhs;
};

#endif
