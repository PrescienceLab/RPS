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

  vector<FIRFilter<SAMPLETYPE,OUTSAMPLE,INSAMPLE> *> dbanks;

public:
  DelayBlock(unsigned numlevels=2, 
	     int*     delay_vals=0);
  DelayBlock(const DelayBlock &rhs);
  virtual ~DelayBlock();

  DelayBlock & operator=(const DelayBlock &rhs);

  inline unsigned GetNumberLevels() const;
  inline unsigned GetDelayValueOfLevel(unsigned level) const;

  void SetDelayValueOfLevel(unsigned level, unsigned delay);

  bool ChangeDelayConfig(unsigned numlevels,
			 int*     delay_vals);

  void ClearLevelDelayLine(unsigned level);
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

  // Initialize the delay filters
  vector<double> coefs;
  for (i=0; i<numlevels; i++) {
    coefs.clear();
    if (this->delay_vals[i] == 0) {
      coefs.push_back(1.0);
    } else {
      vector<double> tcoefs(this->delay_vals[i]-1, 0);
      tcoefs.push_back(1.0);
      coefs = tcoefs;
    } 
    
    FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfir =
      new FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(coefs.size(), coefs);

    dbanks.push_back(pfir);
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

  FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfir;
  for (unsigned i=0; i<numlevels; i++) {
    pfir = dbanks[i]; CHK_DEL(pfir);
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
void DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetDelayValueOfLevel(unsigned level, unsigned delay)
{
  delay_vals[level] = delay;
  vector<double> coefs;
  if (delay == 0) {
    coefs.push_back(1.0);
  } else {
    vector<double> tcoefs(delay-1, 0);
    tcoefs.push_back(1.0);
    coefs = tcoefs;
  }
  dbanks[level]->SetFilterCoefs(tcoefs);
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

      vector<double> coefs;
      if (this->delay_vals[i] == 0) {
	coefs.push_back(1.0);
      } else {
	vector<double> tcoefs(this->delay_vals[i]-1, 0);
	tcoefs.push_back(1.0);
	coefs = tcoefs;
      }
      dbanks[i]->SetFilterCoefs(coefs);
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

    // Initialize the delay filters
    vector<double> coefs;
    for (i=0; i<numlevels; i++) {
      coefs.clear();
      if (this->delay_vals[i] == 0) {
	coefs.push_back(1.0);
      } else {
	vector<double> tcoefs(this->delay_vals[i]-1, 0);
	tcoefs.push_back(1.0);
	coefs = tcoefs;
      }
    
      FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfir =
	new FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(coefs.size(), coefs);
 
      dbanks.push_back(pfir);
    }
  }
  return true;
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearLevelDelayLine(unsigned level)
{
  dbanks[level]->ClearDelayLine();
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearAllDelayLines()
{
  for (unsigned i=0; i<numlevels; i++) {
    dbanks[i]->ClearDelayLine();
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

    dbanks[samplelevel]->GetFilterOutput(outsamp, in[i]);
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
  if (inblock.size() != numlevels) {
    return 0;
  }
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
