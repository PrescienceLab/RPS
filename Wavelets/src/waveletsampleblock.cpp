#include "waveletsampleblock.h"

/********************************************************************************
 * Blocks of wavelet input samples
 *******************************************************************************/
WaveletInputSampleBlock::WaveletInputSampleBlock() : 
  InputSampleBlock<WaveletInputSample>()
{
}

WaveletInputSampleBlock::WaveletInputSampleBlock
(const deque<WaveletInputSample> &input) : 
  InputSampleBlock<WaveletInputSample>(input)
{
}

WaveletInputSampleBlock::WaveletInputSampleBlock
(const deque<WaveletInputSample> &input, const unsigned index) : 
  InputSampleBlock<WaveletInputSample>(input,index)
{
}

WaveletInputSampleBlock::
WaveletInputSampleBlock(const WaveletInputSampleBlock &rhs) :
  InputSampleBlock<WaveletInputSample>(rhs)
{
}

WaveletInputSampleBlock::~WaveletInputSampleBlock()
{
}

WaveletInputSampleBlock* WaveletInputSampleBlock::clone()
{
  return new WaveletInputSampleBlock(*this);
}


/*******************************************************************************
 * Block output samples
 *******************************************************************************/

WaveletOutputSampleBlock::WaveletOutputSampleBlock() :
  OutputSampleBlock<WaveletOutputSample>()
{
}

WaveletOutputSampleBlock::WaveletOutputSampleBlock
(const WaveletOutputSampleBlock &rhs) :
  OutputSampleBlock<WaveletOutputSample>(rhs)
{
}

WaveletOutputSampleBlock::~WaveletOutputSampleBlock()
{
}

WaveletOutputSampleBlock* WaveletOutputSampleBlock::clone()
{
  return new WaveletOutputSampleBlock(*this);
}

void WaveletOutputSampleBlock::SetBlockLevel(int level)
{
  if (!samples.empty()) {
    for (unsigned i=0; i<samples.size(); i++) {
      samples[i].SetSampleLevel(level);
    }
  }
}

int WaveletOutputSampleBlock::GetBlockLevel()
{
  int tlevel = -1;
  if (!samples.empty()) {
    tlevel = samples[0].GetSampleLevel();
    
    // Force unified level representation in block
    for (unsigned i=1; i<samples.size(); i++) {
      if (tlevel != samples[i].GetSampleLevel()) {
	samples[i].SetSampleLevel(tlevel);
      }
    }
  }
  return tlevel;
}
