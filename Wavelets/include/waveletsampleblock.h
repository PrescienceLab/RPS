#ifndef _waveletsampleblock
#define _waveletsampleblock

#include <deque>

#include "sampleblock.h"
#include "waveletsample.h"
#include "util.h"

class WaveletInputSampleBlock: public InputSampleBlock<WaveletInputSample> {
public:
  WaveletInputSampleBlock();
  WaveletInputSampleBlock(const WaveletInputSampleBlock &rhs);
  WaveletInputSampleBlock(const deque<WaveletInputSample> &input);
  WaveletInputSampleBlock(const deque<WaveletInputSample> &input, 
			  const unsigned index);
  virtual ~WaveletInputSampleBlock();
  virtual WaveletInputSampleBlock* clone();
};

class WaveletOutputSampleBlock : public OutputSampleBlock<WaveletOutputSample> {
public:
  WaveletOutputSampleBlock();
  WaveletOutputSampleBlock(const WaveletOutputSampleBlock &rhs);
  virtual ~WaveletOutputSampleBlock();

  virtual WaveletOutputSampleBlock* clone();
  virtual void SetBlockLevel(int level);
  virtual int GetBlockLevel();
};

#endif
