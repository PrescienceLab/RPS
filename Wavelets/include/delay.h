#ifndef _delay
#define _delay

#include <vector>
#include <deque>
#include <iostream>
#include <cassert>

#include "waveletinfo.h"
#include "waveletsampleblock.h"
#include "filter.h"
#include "sample.h"
#include "sampleblock.h"
#include "util.h"

bool CalculateWaveletDelayBlock(const unsigned numcoefs, 
				const unsigned numlevels, 
				int* delay_vals)
{
  if (!delay_vals) {
    return false;
  }

  switch (numlevels) {
  case 0:
  case 1:
    return false;

  case 2:
    delay_vals[numlevels-1] = 0;
    delay_vals[numlevels-2] = 0;
    break;

  default:
    delay_vals[numlevels-1] = 0;
    delay_vals[numlevels-2] = 0;
    delay_vals[numlevels-3] = numcoefs;
    for (int i=numlevels-4; i>=0; i--) {
      delay_vals[i] = numcoefs + 2*delay_vals[i+1] - 2;
    }
  }
  return true;
};

template <class SAMPLE>
class DelayBlock {
private:
  unsigned numlevels;
  int lowest_level;
  int* delay_vals;

  vector<deque<SAMPLE>* > dbanks;

  unsigned StreamBlock(WaveletOutputSampleBlock<SAMPLE> &out,
		       const WaveletOutputSampleBlock<SAMPLE> &in);

public:
  DelayBlock(const unsigned numlevels=2,
	     const int lowest_level=0,
	     int* delay_vals=0);
  DelayBlock(const DelayBlock &rhs);
  virtual ~DelayBlock();

  DelayBlock & operator=(const DelayBlock &rhs);

  inline unsigned GetNumberLevels() const;

  inline int GetLowestLevel() const;
  inline void SetLowestLevel(const int lowest_level);

  inline unsigned GetDelayValueOfLevel(const int level) const;
  bool SetDelayValueOfLevel(const int level, const unsigned delay);

  bool ChangeDelayConfig(const unsigned numlevels,
			 const int lowest_level,
			 int* delay_vals);

  bool ClearLevelDelayLine(const int level);
  void ClearAllDelayLines();

  bool StreamingSampleOperation(vector<SAMPLE> &out, const vector<SAMPLE> &in);

  unsigned StreamingBlockOperation
    (vector<WaveletOutputSampleBlock<SAMPLE> > &outblock,
     const vector<WaveletOutputSampleBlock<SAMPLE> > &inblock);

  ostream & Print(ostream &os) const;
  ostream & operator<<(ostream &os) const;
};

template <class SAMPLE>
inline ostream & operator<<(ostream &os, const DelayBlock<SAMPLE> &rhs) 
{ 
  return rhs.operator<<(os);
};

template<class SAMPLE>
DelayBlock<SAMPLE>::
DelayBlock(const unsigned numlevels, const int lowest_level, int* delay_vals)
{
  DEBUG_PRINT("DelayBlock<SAMPLE>::DelayBlock(const unsigned numlevels,"
	      <<"const int lowest_level, int* delay_vals)");

  if ((numlevels == 0) || (numlevels > MAX_STAGES+1)) {
    // Need at least one level
    this->numlevels = 1;
  } else {
    this->numlevels = numlevels;
  }
  this->lowest_level = lowest_level;

  // Initialize the delay value vector
  unsigned i;
  this->delay_vals = new int[numlevels];

  if (delay_vals == 0) {
    for (i=0; i<numlevels; i++) {
      this->delay_vals[i] = 0;
    }
  } else {
    for (i=0; i<numlevels; i++) {
      this->delay_vals[i] = delay_vals[i];
    }
  }

  // Initialize the delay deques
  for (i=0; i<numlevels; i++) {
    int delay_sz = (this->delay_vals[i] == 0) ? 0 : this->delay_vals[i]-1;
    deque<SAMPLE>* pdis = new deque<SAMPLE>(delay_sz);
    dbanks.push_back(pdis);
  }
}

template<class SAMPLE>
DelayBlock<SAMPLE>::
DelayBlock(const DelayBlock &rhs) : 
  numlevels(rhs.numlevels), lowest_level(rhs.lowest_level)
{
  DEBUG_PRINT("DelayBlock<SAMPLE>::DelayBlock(const DelayBlock &rhs)");

  if ((rhs.numlevels == 0) || (rhs.numlevels > MAX_STAGES+1)) {
    // Need at least one level
    this->numlevels = 1;
    dbanks.clear();

    deque<SAMPLE>* pdis = new deque<SAMPLE>(0);
    dbanks.push_back(pdis);

    this->delay_vals = new int[1];
    this->delay_vals[0] = 0;
  } else {
    // Initialize the delay value vector and the delay deques
    this->delay_vals = new int[rhs.numlevels];
    for (unsigned i=0; i<rhs.numlevels; i++) {
      this->delay_vals[i] = rhs.delay_vals[i];
      int delay_sz = (this->delay_vals[i] == 0) ? 0 : this->delay_vals[i]-1;
      deque<SAMPLE>* pdis = new deque<SAMPLE>(delay_sz);
      dbanks.push_back(pdis);
    }
  }
}

template<class SAMPLE>
DelayBlock<SAMPLE>::
~DelayBlock()
{
  DEBUG_PRINT("DelayBlock<SAMPLE>::~DelayBlock()");

  if (delay_vals != 0) {
    delete[] delay_vals;
    delay_vals=0;
  }

  for (unsigned i=0; i<numlevels; i++) {
    CHK_DEL(dbanks[i]);
  }
  dbanks.clear();
}

template<class SAMPLE>
DelayBlock<SAMPLE> &
DelayBlock<SAMPLE>::
operator=(const DelayBlock &rhs)
{
  DEBUG_PRINT("DelayBlock<SAMPLE>::operator=(const DelayBlock &rhs)");

  this->~DelayBlock();
  return *(new (this) DelayBlock(rhs));
}

template<class SAMPLE>
unsigned DelayBlock<SAMPLE>::
GetNumberLevels() const
{
  return numlevels;
}

template<class SAMPLE>
int DelayBlock<SAMPLE>::
GetLowestLevel() const
{
  return lowest_level;
}

template<class SAMPLE>
void DelayBlock<SAMPLE>::
SetLowestLevel(const int lowest_level)
{
  this->lowest_level = lowest_level;
}


template<class SAMPLE>
unsigned DelayBlock<SAMPLE>::
GetDelayValueOfLevel(const int level) const
{
  int level_indx = level - lowest_level;
  unsigned delay=0;

  if ((level_indx >= 0) && (level_indx <= (int) numlevels)) {
    delay = delay_vals[level];
  }
  return delay;
}

template<class SAMPLE>
bool DelayBlock<SAMPLE>::
SetDelayValueOfLevel(const int level, const unsigned delay)
{
  int level_indx = level - lowest_level;
  if ((level_indx < 0) || (level_indx > (int) numlevels)) {
    return false;
  }

  delay_vals[level_indx] = delay;
  CHK_DEL(dbanks[level_indx]);

  int delay_sz = (delay == 0) ? 0 : delay-1;
  dbanks[level_indx] = new deque<SAMPLE>(delay_sz);
  return true;
}

template<class SAMPLE>
bool DelayBlock<SAMPLE>::
ChangeDelayConfig(const unsigned numlevels,
		  const int lowest_level,
		  int* delay_vals)
{
  if ((numlevels == 0) || (numlevels > MAXSTAGES + 1) || (delay_vals == 0)) {
    return false;
  }

  unsigned i;
  if (this->numlevels == numlevels) {
    for (i=0; i<numlevels; i++) {
      this->delay_vals[i] = delay_vals[i];      
      SetDelayValueOfLevel(i, this->delay_vals[i]);
    }
  } else {
    if (this->delay_vals != 0) {
      delete[] this->delay_vals;
      this->delay_vals=0;
    }

    for (i=0; i<this->numlevels; i++) {
      CHK_DEL(dbanks[i]);
    }
    dbanks.clear();
 
    this->numlevels = numlevels;

    // Initialize the delay value vector
    this->delay_vals = new int[numlevels];
    for (i=0; i<numlevels; i++) {
      this->delay_vals[i] = delay_vals[i];
    }

    // Initialize the delay deques
    for (i=0; i<numlevels; i++) {
      int delay_sz = (this->delay_vals[i] == 0) ? 0 : this->delay_vals[i]-1;
      deque<SAMPLE>* pdis = new deque<SAMPLE>(delay_sz);
      dbanks.push_back(pdis);
    }
  }
  this->lowest_level = lowest_level;
  return true;
}

template<class SAMPLE>
bool DelayBlock<SAMPLE>::
ClearLevelDelayLine(const int level)
{
  int level_indx = level - lowest_level;
  if ((level_indx < 0) || (level_indx > (int) numlevels)) {
    return false;
  } else {
    SAMPLE zero;
    for (unsigned i=0; i<dbanks[level_indx]->size(); i++) {
      dbanks[level_indx]->operator[](i) = zero;
    }
  }
  return true;
}

template<class SAMPLE>
void DelayBlock<SAMPLE>::
ClearAllDelayLines()
{
  for (unsigned i=0; i<numlevels; i++) {
    ClearLevelDelayLine(i);
  }
}

template<class SAMPLE>
bool DelayBlock<SAMPLE>::
StreamingSampleOperation(vector<SAMPLE> &out, const vector<SAMPLE> &in)
{
  unsigned i, sampleindex;
  int samplelevel;

  SAMPLE outsamp;

  // Clear the output vector to signal new output to the calling routine
  out.clear();

  // Check input buffer for new inputs
  int level_indx;
  for (i=0; i<in.size(); i++) {
    samplelevel = in[i].GetSampleLevel();
    level_indx = samplelevel - lowest_level;
    assert((level_indx >= 0) || (level_indx <= (int) numlevels));

    sampleindex = in[i].GetSampleIndex();

    dbanks[level_indx]->push_front(in[i]);
    outsamp = dbanks[level_indx]->back();
    dbanks[level_indx]->pop_back();
    outsamp.SetSampleLevel(samplelevel);
    outsamp.SetSampleIndex(sampleindex);
    out.push_back(outsamp);
  }
  return out.size();
}

template<class SAMPLE>
unsigned DelayBlock<SAMPLE>::
StreamingBlockOperation(vector<WaveletOutputSampleBlock<SAMPLE> > &outblock,
			const vector<WaveletOutputSampleBlock<SAMPLE> > &inblock)
{
  unsigned i;

  // Clear the output block to signal new samples to the calling routine
  outblock.clear();

  WaveletOutputSampleBlock<SAMPLE> block;
  for (i=0; i<inblock.size(); i++) {
    if (StreamBlock(block, inblock[i])) {
      outblock.push_back(block);
    }
  }
  return outblock.size();
}

template<class SAMPLE>
ostream & DelayBlock<SAMPLE>::
Print(ostream &os) const
{
  unsigned i;
  os << "Number of levels: " << numlevels << endl;
  os << "The delay values:" << endl;
  for (i=0; i<numlevels; i++) {
    os << "\t" << delay_vals[i] << endl;
  }

  os << "The size of the dbanks:" << endl;
  for (i=0; i<numlevels; i++) {
    os << "\t" << dbanks[i]->size() << endl;
  }

  return os;
}

template<class SAMPLE>
ostream & DelayBlock<SAMPLE>::
operator<<(ostream &os) const
{
  return Print(os);
}


// Private functions
template<class SAMPLE>
unsigned DelayBlock<SAMPLE>::
StreamBlock(WaveletOutputSampleBlock<SAMPLE> &out, 
	    const WaveletOutputSampleBlock<SAMPLE> &in)
{
  unsigned sampleindex;
  SAMPLE outsamp;

  unsigned block_indx = in.GetBlockIndex();
  int level_indx = in.GetBlockLevel() - lowest_level;

  assert((level_indx >= 0) || (level_indx <= (int) numlevels));

  for (unsigned i=0; i<in.GetBlockSize(); i++) {
    sampleindex = in[i].GetSampleIndex();
    dbanks[level_indx]->push_front(in[i]);
    outsamp = dbanks[level_indx]->back();
    dbanks[level_indx]->pop_back();
    outsamp.SetSampleIndex(sampleindex);
    out.PushSampleBack(outsamp);
  }
  out.SetBlockIndex(block_indx);
  return out.GetBlockSize();
}

#endif
