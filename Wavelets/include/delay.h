#ifndef _delay
#define _delay

#include <vector>
#include <deque>
#include <iostream>

#include "waveletinfo.h"
#include "filter.h"
#include "sample.h"
#include "sampleblock.h"
#include "util.h"

bool CalculateWaveletDelayBlock(unsigned numcoefs, unsigned numlevels, int* delay_vals) {
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

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class DelayBlock {
private:
  unsigned numlevels;
  int*     delay_vals;

  vector<deque<INSAMPLE> *> dbanks;

public:
  DelayBlock(unsigned numlevels=2, 
	     int*     delay_vals=0);
  DelayBlock(const DelayBlock &rhs);
  virtual ~DelayBlock();

  DelayBlock & operator=(const DelayBlock &rhs);

  inline unsigned GetNumberLevels() const;
  inline unsigned GetDelayValueOfLevel(unsigned level) const;

  bool SetDelayValueOfLevel(unsigned level, unsigned delay);

  bool ChangeDelayConfig(unsigned numlevels,
			 int*     delay_vals);

  bool ClearLevelDelayLine(unsigned level);
  void ClearAllDelayLines();

  bool StreamingSampleOperation(vector<OUTSAMPLE> &out, vector<INSAMPLE> &in);

  unsigned StreamingBlockOperation(vector<SampleBlock<OUTSAMPLE> *> &outblock,
				   vector<SampleBlock<INSAMPLE> *>  &inblock);

  ostream & Print(ostream &os) const;
};

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DelayBlock(unsigned numlevels=2, int* delay_vals=0)
{
  if ((numlevels == 0) || (numlevels > MAX_STAGES+1)) {
    // Need at least two level
    this->numlevels = 2;
  } else {
    this->numlevels = numlevels;
  }

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
    deque<INSAMPLE>* pdis = new deque<INSAMPLE>(delay_sz);
    dbanks.push_back(pdis);
  }

}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DelayBlock(const DelayBlock &rhs) : 
  numlevels(rhs.numlevels), dbanks(rhs.dbanks)
{
  // Initialize the delay value vector
  this->delay_vals = new int[numlevels];
  for (int i=0; i<numlevels; i++) {
    this->delay_vals[i] = rhs.delay_vals[i];
  }
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~DelayBlock()
{
  if (delay_vals != 0) {
    delete[] delay_vals;
    delay_vals=0;
  }

  for (unsigned i=0; i<numlevels; i++) {
    CHK_DEL(dbanks[i]);
  }
  dbanks.clear();
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE> &
DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator=(const DelayBlock &rhs)
{
  unsigned cnum = (numlevels <= rhs.numlevels) ? numlevels : rhs.numlevels; 
  for (int i=0; i<cnum; i++) {
    this->delay_vals[i] = rhs.delay_vals[i];
  }
  dbanks = rhs.dbanks;
  return *this;
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetNumberLevels() const
{
  return numlevels;
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetDelayValueOfLevel(unsigned level) const
{
  return delay_vals[level];
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetDelayValueOfLevel(unsigned level, unsigned delay)
{
  if (level > numlevels) {
    return false;
  }

  delay_vals[level] = delay;
  CHK_DEL(dbanks[level]);

  int delay_sz = (delay == 0) ? 0 : delay-1;
  dbanks[level] = new deque<INSAMPLE>(delay_sz);
  return true;
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeDelayConfig(unsigned numlevels, int* delay_vals)
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
      deque<INSAMPLE>* pdis = new deque<INSAMPLE>(delay_sz);
      dbanks.push_back(pdis);
    }
  }
  return true;
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearLevelDelayLine(unsigned level)
{
  if (level > numlevels) {
    return false;
  } else {
    INSAMPLE zero;
    for (unsigned i=0; i<dbanks[level]->size(); i++) {
      dbanks[level]->operator[](i) = zero;
    }
  }
  return true;
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearAllDelayLines()
{
  for (unsigned i=0; i<numlevels; i++) {
    ClearLevelDelayLine(i);
  }
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingSampleOperation(vector<OUTSAMPLE> &out, vector<INSAMPLE> &in)
{
  unsigned i, sampleindex;
  int samplelevel;

  OUTSAMPLE outsamp;

  // Clear the output vector to signal new output to the calling routine
  out.clear();

  // Check input buffer for new inputs
  for (i=0; i<in.size(); i++) {
    samplelevel = in[i].GetSampleLevel();
    sampleindex = in[i].GetSampleIndex();

    dbanks[samplelevel]->push_front(in[i]);
    outsamp = dbanks[samplelevel]->back();
    dbanks[samplelevel]->pop_back();
    outsamp.SetSampleLevel(samplelevel);
    outsamp.SetSampleIndex(sampleindex);
    out.push_back(outsamp);
  }

  return out.size();
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingBlockOperation(vector<SampleBlock<OUTSAMPLE> *> &outblock,
			vector<SampleBlock<INSAMPLE> *>  &inblock)
{
#if 0
  if ((inblock.size() != numlevels) || (outblock.size() != numlevels)) {
    return 0;
  }

  unsigned i;
  unsigned blocklen=0;
  SampleBlock<OUTSAMPLE>* psbo;
  SampleBlock<INSAMPLE>*  psbi;

  // Clear the output block to signal new samples to the calling routine
  for (i=0; i<outblock.size(); i++) {
    psbo = outblock[i];
    psbo->ClearBlock();
  }

  for (i=0; i<outblock.size(); i++) {
    psbo = outblock[i];
    psbi = inblock[i];

    dbanks[i]->GetFilterBufferOutput(*psbo, *psbi);
    blocklen += psbo->GetBlockSize();
  }
#endif
  return 5;
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
Print(ostream &os) const
{
  unsigned i;
  os << "Number of levels: " << numlevels << endl;
  os << "The delay values:" << endl;
  for (i=0; i<numlevels; i++) {
    os << "\t" << delay_vals[i] << endl;
  }

  return os;
}

#endif
