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
