#ifndef _ioutil
#define _ioutil

#include <iostream>

using namespace std;

template <class T> 
inline ostream & operator<<(ostream &os, const T &x) 
{
  x.Print(os);
  return os;
};


#endif

