#ifndef _filter
#define _filter

#include <vector>
#include <deque>
#include <iostream>

#include "util.h"

class FIRFilter {
private:
  vector<double> coefs;
  deque<double>  delayline;
  unsigned       numcoefs;

public:
  FIRFilter(unsigned numcoefs=0);
  FIRFilter(const FIRFilter &rhs);
  FIRFilter(unsigned numcoefs, vector<double> &coefs);
  virtual ~FIRFilter();

  FIRFilter & operator=(const FIRFilter &rhs);

  void   SetFilterCoefs(vector<double> &coefs);
  void   GetFilterCoefs(vector<double> &coefs);
  void   ClearDelayLine();
  double GetFilterOutput(double input);
  void   GetFilterBufferOutput(vector<double> &output, vector<double> &input);

  ostream & Print(ostream &os) const;
};

#endif
