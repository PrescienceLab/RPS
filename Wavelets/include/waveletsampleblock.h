#ifndef _waveletsampleblock
#define _waveletsampleblock

#include <vector>

#include "sampleblock.h"
#include "waveletsample.h"
#include "util.h"

class WaveletInputSampleBlock : public InputSampleBlock<WaveletInputSample> {
public:
  WaveletInputSampleBlock();
  WaveletInputSampleBlock(const WaveletInputSampleBlock &rhs);
  WaveletInputSampleBlock(vector<WaveletInputSample> &rhs);
  virtual ~WaveletInputSampleBlock();
};

class WaveletOutputSampleBlock : public OutputSampleBlock<WaveletOutputSample> {
public:
  WaveletOutputSampleBlock();
  WaveletOutputSampleBlock(const WaveletOutputSampleBlock &rhs);
  WaveletOutputSampleBlock(vector<WaveletOutputSample> &outcoefs);
  virtual ~WaveletOutputSampleBlock();
};

#endif
