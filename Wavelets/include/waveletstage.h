#ifndef _waveletstage
#define _waveletstage

#include <typeinfo>

#include "stage.h"
#include "waveletsample.h"
#include "waveletsampleout.h"

class ForwardWaveletStage : 
public Stage<WaveletOutputSample, WaveletInputSample, double> {
private:
  unsigned rate_l;
  unsigned rate_h;
  int      outlevel_l;
  int      outlevel_h;

  DownSample<WaveletOutputSample, WaveletInputSample> downsampler_l;
  DownSample<WaveletOutputSample, WaveletInputSample> downsampler_h;

public:
  ForwardWaveletStage(WaveletType wavetype=DAUB2);
  ForwardWaveletStage(const ForwardWaveletStage &rhs);
  ForwardWaveletStage(WaveletType wavetype,
		      unsigned    rate_l,
		      unsigned    rate_h,
		      int         outlevel_l,
		      int         outlevel_h);
  virtual ~ForwardWaveletStage();

  ForwardWaveletStage & operator=(const ForwardWaveletStage &rhs);

  inline void     SetDownSampleRateLPF(unsigned rate);
  inline unsigned GetDownSampleRateLPF();

  inline void     SetDownSampleRateHPF(unsigned rate);
  inline unsigned GetDownSampleRateHPF();

  inline void     SetOutputLevelLow(int outlevel);
  inline int      GetOutputLevelLow();

  inline void     SetOutputLevelHigh(int outlevel);
  inline int      GetOutputLevelHigh();

  // Returns true if there is an output sample
  bool PerformSampleOperation(WaveletOutputSample &output_l,
			      WaveletOutputSample &output_h,
			      WaveletOutputSample &input);

  bool PerformSampleOperation(WaveletOutputSample &output_l,
			      WaveletOutputSample &output_h,
			      WaveletInputSample  &input);

  bool PerformSampleOperation(WaveletInputSample  &output_l,
			      WaveletInputSample  &output_h,
			      WaveletOutputSample &input);

  bool PerformSampleOperation(WaveletInputSample &output_l,
			      WaveletInputSample &output_h,
			      WaveletInputSample &input);


  // Returns output buffer length (both outputs same length)
  unsigned PerformBlockOperation(OutputSampleBlock &output_l,
				 OutputSampleBlock &output_h,
				 OutputSampleBlock &input);

  // Returns output buffer length (both outputs same length)
  unsigned PerformBlockOperation(OutputSampleBlock &output_l,
				 OutputSampleBlock &output_h,
				 WaveletInputSampleBlock   &input);

  // Returns output buffer length (both outputs same length)
  unsigned PerformBlockOperation(WaveletInputSampleBlock   &output_l,
				 WaveletInputSampleBlock   &output_h,
				 OutputSampleBlock &input);

  // Returns output buffer length (both outputs same length)
  unsigned PerformBlockOperation(WaveletInputSampleBlock &output_l,
				 WaveletInputSampleBlock &output_h,
				 WaveletInputSampleBlock &input);

  virtual ostream & Print(ostream &os) const;
};


#if 0

class InverseStage : public Stage {
private:
  UpSample            upsampler_l;
  UpSample            upsampler_h;

public:
  ReverseWaveletStage(WaveletType wavetype=DAUB2);
  ReverseWaveletStage(const ReverseWaveletStage &rhs);
  ReverseWaveletStage(WaveletType wavetype, unsigned rate_l, unsigned rate_h);
  virtual ~ReverseWaveletStage();

  ReverseWaveletStage & operator=(const ReverseWaveletStage &rhs);

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

  ostream & Print(ostream &os) const;
};

#endif

#endif
