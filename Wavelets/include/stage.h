#ifndef _stage
#define _stage

#include <vector>
#include <string>
#include <iostream>

#include "coefficients.h"
#include "filter.h"
#include "downsample.h"
#include "upsample.h"
#include "waveletinfo.h"
#include "util.h"

// AbstractStage is used simply for multi-methods on the operator= member
class AbstractStage {
  virtual void set(const AbstractStage *astage)=0;
  virtual void set(const Stage *stage)=0;
  virtual void set(const TransformStage *tstage)=0;
  virtual void set(const InverseStage *istage)=0;
};

class Stage : public AbstractStage {
protected:
  WaveletType         wavetype;
  WaveletCoefficients wavecoefs;
  FIRFilter           lowpass;
  FIRFilter           highpass;

public:
  Stage(WaveletType wavetype=DAUB2);
  Stage(const Stage &rhs);
  virtual ~Stage();

  virtual Stage & operator=(const AbstractStage &rhs);
  virtual Stage & operator=(const Stage &rhs);
  virtual Stage & operator=(const TransformStage &rhs);
  virtual Stage & operator=(const InverseStage &rhs);

  void     ChangeWaveletType(WaveletType wavetype);
  string   GetWaveletName();

  void     SetFilterCoefsLPF(vector<double> &coefs);
  unsigned GetNumCoefsLPF();
  void     PrintCoefsLPF();

  void     SetFilterCoefsHPF(vector<double> &coefs);
  unsigned GetNumCoefsHPF();
  void     PrintCoefsHPF();

  virtual ostream & Print(ostream &os) const;
};


class TransformStage : public Stage {
private:
  DownSample          downsampler_l;
  DownSample          downsampler_h;
  int                 outlevel_l;
  int                 outlevel_h;

public:
  TransformStage(WaveletType wavetype=DAUB2);
  TransformStage(const TransformStage &rhs);
  TransformStage(WaveletType wavetype,
		 unsigned    rate_l,
		 unsigned    rate_h,
		 int         outlevel_l,
		 int         outlevel_h);
  virtual ~TransformStage();

  virtual TransformStage & operator=(const AbstractStage &rhs);
  virtual TransformStage & operator=(const Stage &rhs);
  virtual TransformStage & operator=(const TransformStage &rhs);
  virtual TransformStage & operator=(const InverseStage &rhs);

  void     SetDownSampleRateLPF(unsigned rate);
  unsigned GetDownSampleRateLPF();

  void     SetDownSampleRateHPF(unsigned rate);
  unsigned GetDownSampleRateHPF();

  void     SetOutputLevelLow(int outlevel);
  int      GetOutputLevelLow();

  void     SetOutputLevelHigh(int outlevel);
  int      GetOutputLevelHigh();

  // Returns true if there is an output sample
  bool PerformSampleOperation(SampleOut &output_l,
			      SampleOut &output_h,
			      Sample    &input);

  // Returns output buffer length
  unsigned PerformBlockOperation(SampleBlockOut &output, 
				 SampleBlock    &input);

  virtual ostream & Print(ostream &os) const;
};

class InverseStage : public Stage {
private:
  UpSample            upsampler_l;
  UpSample            upsampler_h;

public:
  InverseStage(WaveletType wavetype=DAUB2);
  InverseStage(const InverseStage &rhs);
  InverseStage(WaveletType wavetype, unsigned rate_l, unsigned rate_h);
  virtual ~InverseStage();

  virtual InverseStage & operator=(const AbstractStage &rhs);
  virtual InverseStage & operator=(const Stage &rhs);
  virtual InverseStage & operator=(const TransformStage &rhs);
  virtual InverseStage & operator=(const InverseStage &rhs);

  void     SetUpSampleRateLPF(unsigned rate);
  unsigned GetUpSampleRateLPF();

  void     SetUpSampleRateHPF(unsigned rate);
  unsigned GetUpSampleRateHPF();

  // Returns true if a sample is output
  void PerformSampleOperation(Sample    &output
			      SampleOut &input_l,
			      SampleOut &input_h);

  // Returns output buffer length
  unsigned PerformBlockOperation(SampleBlock    &output,
				 SampleBlockOut &input);

  virtual ostream & Print(ostream &os) const;
};

#endif
