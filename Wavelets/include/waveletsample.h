#ifndef _waveletsample
#define _waveletsample

#include "util.h"
#include "sample.h"

typedef InputSample<double> WaveletInputSample;

class WaveletOutputSample : public OutputSample<double> { 
protected:
  int level;
  int index;

public:
  WaveletOutputSample();
  WaveletOutputSample(const WaveletOutputSample &rhs);
  WaveletOutputSample(const double value, const int level, const int index);
  virtual ~WaveletOutputSample();

  virtual WaveletOutputSample & operator=(const Sample<double> &rhs);
  WaveletOutputSample & operator=(const WaveletOutputSample &rhs);

  virtual inline void SetSampleLevel(int level);
  virtual inline int GetSampleLevel();
  virtual inline void SetSampleIndex(int index);
  virtual inline int GetSampleIndex();

  virtual ostream & Print(ostream &os) const {
    os << "level: " << level << "\tvalue: " << value << "\tindex: " << index << endl;
    return os;
  };
};

#endif
