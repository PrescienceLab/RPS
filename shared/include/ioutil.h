#ifndef _ioutil
#define _ioutil

template <class T> 
ostream & operator<<(ostream &os, const T &x) 
{
  x.Print(os);
  return os;
};

#endif

