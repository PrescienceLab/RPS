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

class Stage {
private:
  WaveletType         wavetype;
  WaveletCoefficients wavecoefs;
  FIRFilter           lowpass;
  FIRFilter           highpass;

public:
  Stage(WaveletType wavetype=DAUB2);
  Stage(const Stage &rhs);
  virtual ~Stage();

  virtual Stage & operator=(const Stage &rhs) = 0;

  void     ChangeWaveletType(WaveletType wavetype);
  string   GetWaveletName();

  void     SetFilterCoefsLPF(vector<double> &coefs);
  unsigned GetNumCoefsLPF();
  void     PrintCoefsLPF();

  void     SetFilterCoefsHPF(vector<double> &coefs);
  unsigned GetNumCoefsHPF();
  void     PrintCoefsHPF();

  // Returns true if there is an output sample
  virtual bool PerformSampleOperation(SampleOut &out_in_l,
				      SampleOut &out_in_h,
				      Sample    &in_out) = 0;

  // Returns output buffer length
  virtual unsigned PerformBlockOperation(SampleBlockOut &out_in,
					 SampleBlock    &in_out) = 0;

  virtual ostream & Print(ostream &os) const = 0;
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

  virtual TransformStage & operator=(const TransformStage &rhs);

  void     SetDownSampleRateLPF(unsigned rate);
  unsigned GetDownSampleRateLPF();

  void     SetDownSampleRateHPF(unsigned rate);
  unsigned GetDownSampleRateHPF();

  void     SetOutputLevelLow(int outlevel);
  int      GetOutputLevelLow();

  void     SetOutputLevelHigh(int outlevel);
  int      GetOutputLevelHigh();

  // Returns true if there is an output sample
  virtual bool PerformSampleOperation(SampleOut &output_l,
				      SampleOut &output_h,
				      Sample    &input);

  // Returns output buffer length
  virtual unsigned PerformBlockOperation(SampleBlockOut &output, 
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

  virtual InverseStage & operator=(const InverseStage &rhs);

  void     SetUpSampleRateLPF(unsigned rate);
  unsigned GetUpSampleRateLPF();

  void     SetUpSampleRateHPF(unsigned rate);
  unsigned GetUpSampleRateHPF();

  // Returns true if a sample is output
  virtual void PerformSampleOperation(SampleOut &input_l,
				      SampleOut &input_h,
				      Sample    &output);

  // Returns output buffer length
  virtual unsigned PerformBlockOperation(SampleBlockOut &input,
					 SampleBlock    &output);

  virtual ostream & Print(ostream &os) const;
};

#endif
