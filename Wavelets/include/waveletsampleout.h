#ifndef _waveletsampleout
#define _waveletsampleout

#include <vector>

#include "util.h"
#include "sampleout.h"
#include "sampleblockout.h"

class WaveletOutputSample : public OutputSample<double> { 
public:
  WaveletOutputSample();
  WaveletOutputSample(const WaveletOutputSample &rhs);
  WaveletOutputSample(double coef, int level);
  virtual ~WaveletOutputSample();

  WaveletOutputSample & operator=(const WaveletOutputSample &rhs);

  inline virtual void SetOutputSampleValue(double coef);
  inline virtual double GetOutputSampleValue();
  inline virtual void SetOutputSampleLevel(int level);
  inline virtual int GetOutputSampleLevel();

  virtual ostream & Print(ostream &os) const;
};

class WaveletOutputSampleVector : public OutputSampleBlock<WaveletOutputSample> {
private:
  vector<WaveletOutputSample> coefs;

public:
  WaveletOutputSampleVector();
  WaveletOutputSampleVector(const WaveletOutputSampleVector &rhs);
  WaveletOutputSampleVector(vector<WaveletOutputSample> &outcoefs);
  virtual ~WaveletOutputSampleVector();

  WaveletOutputSampleVector & operator=(const WaveletOutputSampleVector &rhs);

  inline virtual void SetWaveletCoef(WaveletOutputSample &coef);
  inline virtual void GetWaveletCoef(WaveletOutputSample *coef, unsigned i);
  inline virtual void ClearBlock();
  inline virtual unsigned GetBlockSize() const;

  virtual ostream & Print(ostream &os) const;
};

#endif
