#include "waveletsampleblock.h"

/********************************************************************************
 * Blocks of wavelet input samples
 *******************************************************************************/

WaveletInputSampleBlock::WaveletInputSampleBlock()
{
}

WaveletInputSampleBlock::WaveletInputSampleBlock
(const WaveletInputSampleBlock &rhs) :
  InputSampleBlock<WaveletInputSample>(rhs)
{
}

WaveletInputSampleBlock::WaveletInputSampleBlock
(vector<WaveletInputSample> &rhs) :
  InputSampleBlock<WaveletInputSample>(rhs)
{
}

WaveletInputSampleBlock::~WaveletInputSampleBlock()
{
}

WaveletInputSampleBlock & 
WaveletInputSampleBlock::operator=(const WaveletInputSampleBlock & rhs)
{
  if (&rhs != this) {
    // invoke InputSampleBlock copy assignment operator
    this->InputSampleBlock<WaveletInputSample>::operator=(rhs);
  }
  return *this;
}

/*******************************************************************************
 * Block output samples
 *******************************************************************************/
WaveletOutputSampleBlock::WaveletOutputSampleBlock()
{
}

WaveletOutputSampleBlock::WaveletOutputSampleBlock
(const WaveletOutputSampleBlock &rhs) :
  OutputSampleBlock<WaveletOutputSample>(rhs)
{
}

WaveletOutputSampleBlock::WaveletOutputSampleBlock
(vector<WaveletOutputSample> &outcoefs) :
  OutputSampleBlock<WaveletOutputSample>(outcoefs)
{
}

WaveletOutputSampleBlock::~WaveletOutputSampleBlock()
{
}

WaveletOutputSampleBlock & 
WaveletOutputSampleBlock::operator=(const WaveletOutputSampleBlock &rhs)
{
  if (&rhs != this) {
    // invoke InputSampleBlock copy assignment operator
    this->OutputSampleBlock<WaveletOutputSample>::operator=(rhs);
  }
  return *this;
}
