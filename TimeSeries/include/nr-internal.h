#ifndef _nr_internal
#define _nr_internal

/*
  Bits and pieces of code incorporated from the Numerical Recipes 
  in C book.  Essentially, this contains the powell optimizer and 
  the utilities that it depends on.

  This will eventually go away once we move to a more clever 
  approach to fitting MA and ARMA models, probably innovations.

  We pull this code into RPS to avoid the current dependency on
  NRC to be able to provide these models to users who do not
  have access to NRC.
*/

namespace nrc {
  float *vector(long nl, long nh);
  float **matrix(long nrl, long nrh, long ncl, long nch);
  void free_vector(float *v, long nl, long nh);
  void free_matrix(float **m, long nrl, long nrh, long ncl, long nch);

  void powell(float p[], float **xi, int n, float ftol, int *iter, float *fret, float (*func)(float []));
};


#endif
