#ifndef _waveletsample
#define _waveletsample

#include "util.h"
#include "sample.h"

typedef InputSample<double> WaveletInputSample;

class WaveletOutputSample : public OutputSample<double> { 
protected:
  int level;

public:
  WaveletOutputSample();
  WaveletOutputSample(const WaveletOutputSample &rhs);
  WaveletOutputSample(const double value, const int level, const unsigned index);
  virtual ~WaveletOutputSample();

  virtual WaveletOutputSample & operator=(const Sample<double> &rhs);
  WaveletOutputSample & operator=(const WaveletOutputSample &rhs);

  virtual inline void SetSampleLevel(int level);
  virtual inline int GetSampleLevel() const;

  virtual ostream & Print(ostream &os) const {
    os << "index: " << index << "\tvalue: " << value << "\tlevel: " << level << endl;
    return os;
  };
};

#endif
