#ifndef _waveletsampleblock
#define _waveletsampleblock

#include <deque>

#include "waveletinfo.h"
#include "sampleblock.h"
#include "waveletsample.h"
#include "util.h"

template <class SAMPLETYPE>
class WaveletInputSampleBlock: public InputSampleBlock<SAMPLETYPE> {
public:
  WaveletInputSampleBlock() {};
  WaveletInputSampleBlock(const WaveletInputSampleBlock &rhs) : 
    InputSampleBlock<SAMPLETYPE>(rhs) {};
  WaveletInputSampleBlock(const deque<SAMPLETYPE> &input) : 
    InputSampleBlock<SAMPLETYPE>(input) {};
  WaveletInputSampleBlock(const deque<SAMPLETYPE> &input, const unsigned index) :
    InputSampleBlock<SAMPLETYPE>(input,index) {};
  virtual ~WaveletInputSampleBlock() {};
};

template <class SAMPLETYPE>
class WaveletOutputSampleBlock : public OutputSampleBlock<SAMPLETYPE> {
protected:
  int level;

public:
  WaveletOutputSampleBlock(const int level=0);
  WaveletOutputSampleBlock(const WaveletOutputSampleBlock &rhs);
  WaveletOutputSampleBlock(const deque<SAMPLETYPE> &input, const unsigned index) :
    OutputSampleBlock<SAMPLETYPE>(input,index) {};

  virtual ~WaveletOutputSampleBlock();

  WaveletOutputSampleBlock & operator=(const WaveletOutputSampleBlock &rhs);
  virtual WaveletOutputSampleBlock* clone() const;
  inline void SetBlockLevel(const int level);
  inline int GetBlockLevel() const;

  void SetSamples(const double* series, const int serlen);

  bool AllSamplesLevelCorrect();
  void SetAllSamplesToCorrectLevel();
};

template <class SAMPLETYPE>
class WaveletRandomOutputSampleBlock : public OutputSampleBlock<SAMPLETYPE> {
public:
  WaveletRandomOutputSampleBlock();
  WaveletRandomOutputSampleBlock(const WaveletRandomOutputSampleBlock &rhs);
  virtual ~WaveletRandomOutputSampleBlock();

  virtual WaveletRandomOutputSampleBlock* clone() const;
  inline void SetBlockLevelOfSample(const unsigned index, const int level);
  inline int GetBlockLevelOfSample(const unsigned index) const;
};

template <class SAMPLETYPE>
class DiscreteWaveletOutputSampleBlock : public OutputSampleBlock<SAMPLETYPE> {
private:
  int lowest_level;
  unsigned numlevels;
  TransformType tt;

public:
  DiscreteWaveletOutputSampleBlock(const unsigned numlevels=2,
				   const int lowest_level=0,
				   const TransformType tt=TRANSFORM);
  DiscreteWaveletOutputSampleBlock(const DiscreteWaveletOutputSampleBlock &rhs);
  virtual ~DiscreteWaveletOutputSampleBlock();

  virtual DiscreteWaveletOutputSampleBlock* clone() const;

  inline int GetLowestLevel() const;
  inline void SetLowestLevel(const int lowest_level);

  inline unsigned GetNumberLevels() const;
  inline void SetNumberLevels(const unsigned numlevels);

  inline TransformType GetTransformType() const;
  inline void SetTransformType(const TransformType tt);

  bool SetSamplesAtLevel(const deque<SAMPLETYPE> &samps, const int level);
  bool GetSamplesAtLevel(deque<SAMPLETYPE> &out, const int level) const;
};

/*******************************************************************************
 * WaveletOutputSampleBlock member functions
 *******************************************************************************/
template <class SAMPLETYPE>
WaveletOutputSampleBlock<SAMPLETYPE>::
WaveletOutputSampleBlock(const int level) :
  OutputSampleBlock<SAMPLETYPE>()
{
  this->level = level;
}

template <class SAMPLETYPE>
WaveletOutputSampleBlock<SAMPLETYPE>::
WaveletOutputSampleBlock(const WaveletOutputSampleBlock &rhs) :
  OutputSampleBlock<SAMPLETYPE>(rhs)
{
  this->level = rhs.level;
}

template <class SAMPLETYPE>
WaveletOutputSampleBlock<SAMPLETYPE>::
~WaveletOutputSampleBlock()
{
}

template <class SAMPLETYPE>
WaveletOutputSampleBlock<SAMPLETYPE> & WaveletOutputSampleBlock<SAMPLETYPE>::
operator=(const WaveletOutputSampleBlock &rhs)
{
  this->samples = rhs.samples;
  this->blockindex = rhs.blockindex;
  this->level = rhs.level;
  return *this;
}

template <class SAMPLETYPE>
WaveletOutputSampleBlock<SAMPLETYPE>* WaveletOutputSampleBlock<SAMPLETYPE>::
clone() const
{
  return new WaveletOutputSampleBlock<SAMPLETYPE>(*this);
}

template <class SAMPLETYPE>
void WaveletOutputSampleBlock<SAMPLETYPE>::
SetBlockLevel(const int level)
{
  this->level = level;
  SetAllSamplesToCorrectLevel();
}

template <class SAMPLETYPE>
int WaveletOutputSampleBlock<SAMPLETYPE>::
GetBlockLevel() const
{
  return level;
}

template <class SAMPLETYPE>
void WaveletOutputSampleBlock<SAMPLETYPE>::
SetSamples(const double* series, const int serlen)
{
  int i;
  for (i=0; i<serlen; i++) {
    SAMPLETYPE samp(series[i], this->level, blockindex+i);
    samples.push_back(samp);
  }
}

template <class SAMPLETYPE>
bool WaveletOutputSampleBlock<SAMPLETYPE>::
AllSamplesLevelCorrect()
{
  bool result=true;
  for (unsigned i=0; i<samples.size(); i++) {
    if (level != samples[i].GetSampleLevel()) {
      result=false;
    }
  }
  return result;
}

template <class SAMPLETYPE>
void WaveletOutputSampleBlock<SAMPLETYPE>::
SetAllSamplesToCorrectLevel()
{
  if (!samples.empty()) {
    for (unsigned i=0; i<samples.size(); i++) {
      samples[i].SetSampleLevel(level);
    }
  }
}

/*******************************************************************************
 * WaveletRandomOutputSampleBlock member functions
 *******************************************************************************/
template <class SAMPLETYPE>
WaveletRandomOutputSampleBlock<SAMPLETYPE>::
WaveletRandomOutputSampleBlock() : 
  OutputSampleBlock<SAMPLETYPE>()
{
}

template <class SAMPLETYPE>
WaveletRandomOutputSampleBlock<SAMPLETYPE>::
WaveletRandomOutputSampleBlock(const WaveletRandomOutputSampleBlock &rhs) :
  OutputSampleBlock<SAMPLETYPE>(rhs)
{
}

template <class SAMPLETYPE>
WaveletRandomOutputSampleBlock<SAMPLETYPE>::~WaveletRandomOutputSampleBlock()
{
}

template <class SAMPLETYPE>
WaveletRandomOutputSampleBlock<SAMPLETYPE>*
WaveletRandomOutputSampleBlock<SAMPLETYPE>::
clone() const
{
  return new WaveletRandomOutputSampleBlock<SAMPLETYPE>(*this);
}

template <class SAMPLETYPE>
void WaveletRandomOutputSampleBlock<SAMPLETYPE>::
SetBlockLevelOfSample(const unsigned index, const int level)
{
  if (index < samples.size()) {
    samples[index].SetSampleLevel(level);
  }
}

template <class SAMPLETYPE>
int WaveletRandomOutputSampleBlock<SAMPLETYPE>::
GetBlockLevelOfSample(const unsigned index) const
{
  int level=-1;
  if (index < samples.size()) {
    level = samples[index].GetSampleLevel();
  }
  return level;
}

/*******************************************************************************
 * DiscreteWaveletOutputSampleBlock member functions
 *******************************************************************************/
template <class SAMPLETYPE>
DiscreteWaveletOutputSampleBlock<SAMPLETYPE>::
DiscreteWaveletOutputSampleBlock(const unsigned numlevels, 
				 const int lowest_level,
				 const TransformType tt=TRANSFORM) :
  OutputSampleBlock<SAMPLETYPE>()
{
  this->numlevels = numlevels;
  this->lowest_level = lowest_level;
  this->tt = tt;
}

template <class SAMPLETYPE>
DiscreteWaveletOutputSampleBlock<SAMPLETYPE>::
DiscreteWaveletOutputSampleBlock(const DiscreteWaveletOutputSampleBlock &rhs) :
  OutputSampleBlock<SAMPLETYPE>(rhs)
{
  this->numlevels = rhs.numlevels;
  this->lowest_level = rhs.lowest_level;
  this->tt = rhs.tt;
}

template <class SAMPLETYPE>
DiscreteWaveletOutputSampleBlock<SAMPLETYPE>::
~DiscreteWaveletOutputSampleBlock()
{
}

template <class SAMPLETYPE>
DiscreteWaveletOutputSampleBlock<SAMPLETYPE>* 
DiscreteWaveletOutputSampleBlock<SAMPLETYPE>::
clone() const
{
  return new DiscreteWaveletOutputSampleBlock<SAMPLETYPE>(*this);
}

template <class SAMPLETYPE>
int DiscreteWaveletOutputSampleBlock<SAMPLETYPE>::
GetLowestLevel() const
{
  return this->lowest_level;
}

template <class SAMPLETYPE>
void DiscreteWaveletOutputSampleBlock<SAMPLETYPE>::
SetLowestLevel(const int lowest_level)
{
  this->lowest_level = lowest_level;
}

template <class SAMPLETYPE>
unsigned DiscreteWaveletOutputSampleBlock<SAMPLETYPE>::
GetNumberLevels() const
{
  return this->numlevels;
}

template <class SAMPLETYPE>
void DiscreteWaveletOutputSampleBlock<SAMPLETYPE>::
SetNumberLevels(const unsigned numlevels)
{
  this->numlevels = numlevels;
}

template <class SAMPLETYPE>
TransformType DiscreteWaveletOutputSampleBlock<SAMPLETYPE>::
GetTransformType() const
{
  return this->tt;
}

template <class SAMPLETYPE>
void DiscreteWaveletOutputSampleBlock<SAMPLETYPE>::
SetTransformType(const TransformType tt)
{
  this->tt = tt;
}

template <class SAMPLETYPE>
bool DiscreteWaveletOutputSampleBlock<SAMPLETYPE>::
SetSamplesAtLevel(const deque<SAMPLETYPE> &samps, const int level)
{
  int level_indx = level - lowest_level;
  if ((level_indx < 0) || (level_indx > (int)numlevels)) {
    return false;
  }

  // Find the right indice based on the level
  // Find the size of the level
  unsigned M=numlevels-1;
  unsigned index=0, size=1;
  if ((unsigned)level_indx != M) {
    size = index = 0x1 << (M-level_indx-1);
  }

  for (unsigned i=0; i<size; i++, index++) {
    samples[index] = samps[i];
  }
  return true;
}

template <class SAMPLETYPE>
bool DiscreteWaveletOutputSampleBlock<SAMPLETYPE>::
GetSamplesAtLevel(deque<SAMPLETYPE> &out, const int level) const
{
  int level_indx = level - lowest_level;
  if ((level_indx < 0) || (level_indx > (int)numlevels)) {
    return false;
  }

  out.clear();

  // Find the right indice based on the level
  // Find the size of the level
  // These are dependent on transform type
  unsigned M=numlevels-1;
  unsigned index=0, size=1;
  if ((unsigned)level_indx != M) {
    if (tt==TRANSFORM) {
      size = index = 0x1 << (M-level_indx-1);
    } else {
      size = 0x1 << (M-level_indx);
      index = (0x1 << (M-level_indx)) - 1;
    }
  }

  for (unsigned i=0; i<size; i++, index++) {
    out.push_back(samples[index]);
  }
  return true;
}

#endif
