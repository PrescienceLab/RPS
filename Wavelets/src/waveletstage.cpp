#include "waveletstage.h"

ForwardWaveletStage::ForwardWaveletStage(WaveletType wavetype=DAUB2) :
  rate_l(2), rate_h(2), outlevel_l(-1), outlevel_h(-1),
  downsampler_l(rate_l), downsampler_h(rate_h),
  Stage<WaveletOutputSample, WaveletInputSample, double>(wavetype)
{
}

ForwardWaveletStage::ForwardWaveletStage(const ForwardWaveletStage &rhs) :
  rate_l(rhs.rate_l), rate_h(rhs.rate_h),
  outlevel_l(rhs.outlevel_l), outlevel_h(rhs.outlevel_h),
  downsampler_l(rhs.downsampler_l), downsampler_h(rhs.downsampler_h),
  Stage<WaveletOutputSample, WaveletInputSample, double>(rhs)
{
}

ForwardWaveletStage::ForwardWaveletStage(WaveletType wavetype,
					 unsigned    rate_l,
					 unsigned    rate_h,
					 int         outlevel_l,
					 int         outlevel_h) :
  rate_l(rate_l), rate_h(rate_h), outlevel_l(outlevel_l), outlevel_h(outlevel_h),
  downsampler_l(rate_l), downsampler_h(rate_h),
  Stage<WaveletOutputSample, WaveletInputSample, double>(wavetype)
{
}

ForwardWaveletStage::~ForwardWaveletStage()
{
}

ForwardWaveletStage &
ForwardWaveletStage::operator=(const ForwardWaveletStage &rhs)
{
  // Check that the RTT is actually a ForwardWaveletStage, else
  //  throw exception
  if (typeid(rhs) == typeid(ForwardWaveletStage)) {
    // copy
    rate_l = rhs.rate_l;
    rate_h = rhs.rate_h;
    outlevel_l = rhs.outlevel_l;
    outlevel_h = rhs.outlevel_h;
    downsampler_l = rhs.downsampler_l;
    downsampler_h = rhs.downsampler_h;
    
    if (&rhs != this) {
      // invoke Stage copy assignment operator
      this->Stage<WaveletOutputSample, WaveletInputSample, double>::operator=(rhs);
    }
  } else {
    throw EquivalenceException();
  }
  return *this;
}

void ForwardWaveletStage::SetDownSampleRateLPF(unsigned rate)
{
  downsampler_l.SetDownSampleRate(rate);
}

unsigned ForwardWaveletStage::GetDownSampleRateLPF()
{
  return downsampler_l.GetDownSampleRate();
}

void ForwardWaveletStage::SetDownSampleRateHPF(unsigned rate)
{
  downsampler_h.SetDownSampleRate(rate);
}

unsigned ForwardWaveletStage::GetDownSampleRateHPF()
{
  return downsampler_h.GetDownSampleRate();
}

void ForwardWaveletStage::SetOutputLevelLow(int outlevel)
{
  outlevel_l = outlevel;
}

int ForwardWaveletStage::GetOutputLevelLow()
{
  return outlevel_l;
}

void ForwardWaveletStage::SetOutpueLevelHigh(int outlevel)
{
  outlevel_h = outlevel;
}

int ForwardWaveletStage::GetOutputLevelHigh()
{
  return outlevel_h;
}


bool ForwardWaveletStage::PerformSampleOperation(WaveletOutputSample &output_l,
						 WaveletOutputSample &output_h,
						 WaveletOutputSample &input)
{
}

bool ForwardWaveletStage::PerformSampleOperation(WaveletOutputSample &output_l,
						 WaveletOutputSample &output_h,
						 WaveletInputSample  &input)
{
}

bool ForwardWaveletStage::PerformSampleOperation(WaveletInputSample  &output_l,
						 WaveletInputSample  &output_h,
						 WaveletOutputSample &input)
{
}

bool ForwardWaveletStage::PerformSampleOperation(WaveletInputSample &output_l,
						 WaveletInputSample &output_h,
						 WaveletInputSample  &input)
{
}


// Returns output buffer length (both outputs same length)
unsigned 
ForwardWaveletStage::PerformBlockOperation(OutputSampleBlock<> &output_l,
					   OutputSampleBlock<> &output_h,
					   OutputSampleBlock<> &input)
{
}

unsigned 
ForwardWaveletStage::PerformBlockOperation(OutputSampleBlock<>     &output_l,
					   OutputSampleBlock<>     &output_h,
					   WaveletInputSampleBlock &input)
{
}

unsigned 
ForwardWaveletStage::PerformBlockOperation(WaveletInputSampleBlock &output_l,
					   WaveletInputSampleBlock &output_h,
					   OutputSampleBlock<>     &input)
{
}

unsigned 
ForwardWaveletStage::PerformBlockOperation(WaveletInputSampleBlock &output_l,
					   WaveletInputSampleBlock &output_h,
					   WaveletInputSampleBlock &input)
{
}

ostream & ForwardWaveletStage::Print(ostream &os) const;
