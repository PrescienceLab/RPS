#ifndef _delay
#define _delay

#include <vector>
#include <deque>
#include <iostream>

#include "filter.h"
#include "sample.h"
#include "sampleblock.h"
#include "indexmanager.h"
#include "jitterhelper.h"
#include "util.h"

void CalculateWaveletDelayBlock(unsigned numcoefs, unsigned numlevels, int *delay_vals) {
  delay_vals[numlevels-1] = 0;
  delay_vals[numlevels-2] = 0;
  delay_vals[numlevels-3] = numcoefs;
  for (int i=numlevels-4; i>=0; i--) {
    delay_vals[i] = numcoefs + 2*delay_vals[i+1] - 2;
  } 
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class DelayBlock {
private:
  unsigned numlevels;
  int      *delay_vals;

  vector<FIRFilter<SAMPLETYPE,OUTSAMPLE,INSAMPLE> *> dbanks;
  vector<IndexManager *> indexmgrs;

  JitterHelper     jitterhelp;
  vector<INSAMPLE> jitter_buffer;

public:
  DelayBlock(unsigned numlevels=1, 
	     int      *delay_vals=0, 
	     unsigned backlog=5);
  DelayBlock(const DelayBlock &rhs);
  virtual ~DelayBlock();

  DelayBlock & operator=(const DelayBlock &rhs);

  inline unsigned GetNumberLevels() const;
  inline unsigned GetDelayValueOfLevel(unsigned level) const;

  void SetDelayValueOfLevel(unsigned level, unsigned delay);

  bool ChangeDelayConfig(unsigned numlevels,
			 int      *delay_vals, 
			 unsigned backlog);

  void ClearLevelDelayLine(unsigned level);
  void ClearAllDelayLines();

  bool StreamingSampleOperation(vector<OUTSAMPLE> &out, vector<INSAMPLE> &in);

  unsigned StreamingBlockOperation(vector<SampleBlock<OUTSAMPLE> *> &outblock,
				   vector<SampleBlock<INSAMPLE> *> &inblock);

  ostream & Print(ostream &os) const;
};

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DelayBlock(unsigned numlevels=1, int *delay_vals=0, unsigned backlog=5) :
  jitterhelp(backlog)
{
  if (numlevels == 0) {
    // Need at least one level
    this->numlevels = 1;
  } else {
    this->numlevels = numlevels;
  }

  // Initialize the delay value vector
  unsigned i;
  this->delay_vals = new int[numlevels];
  for (i=0; i<numlevels; i++) {
    this->delay_vals[i] = delay_vals[i];
  }

  // Initialize the delay filters
  vector<double> coefs;
  for (i=0; i<numlevels; i++) {
    coefs.clear();
    if (delay_vals[i] == 0) {
      coefs.push_back(1.0);
    } else {
      vector<double> tcoefs(delay_vals[i]-1, 0);
      tcoefs.push_back(1.0);
      coefs = tcoefs;
    } 
    
    FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfir =
      new FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(coefs.size(), coefs);

    dbanks.push_back(pfir);
  }

  // Initialize the index manager
  for (i=0; i<numlevels; i++) {
    IndexManager* pim = new IndexManager();
    indexmgrs.push_back(pim);
  } 
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DelayBlock(const DelayBlock &rhs) : 
  numlevels(rhs.numlevels), jitterhelp(rhs.jitterhelp),
  dbanks(rhs.dbanks), indexmgrs(rhs.indexmgrs)
{
  // Initialize the delay value vector
  this->delay_vals = new int[numlevels];
  for (int i=0; i<numlevels; i++) {
    this->delay_vals[i] = delay_vals[i];
  }
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~DelayBlock()
{
  delete[] delay_vals;

  FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfir;
  IndexManager* pim;
  for (unsigned i=0; i<numlevels; i++) {
    pfir = dbanks[i]; delete pfir;
    pim = indexmgrs[i]; delete pim;
  }
  dbanks.clear();
  indexmgrs.clear();
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
  jitterhelp = rhs.jitterhelp;
  dbanks = rhs.dbanks;
  indexmgrs = rhs.indexmgrs;
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
ChangeDelayConfig(unsigned numlevels, int *delay_vals, unsigned backlog)
{
  if (numlevels == 0) {
    return false;
  }

  // Clear the current backlog
  jitterhelp.ClearCurrentBacklog();
  jitterhelp.SetBacklogThreshold(backlog);

  unsigned i;
  if (this->numlevels == numlevels) {
    for (i=0; i<numlevels; i++) {
      this->delay_vals[i] = delay_vals[i];      

      vector<double> coefs;
      if (delay_vals[i] == 0) {
	coefs.push_back(1.0);
      } else {
	vector<double> tcoefs(delay_vals[i]-1, 0);
	tcoefs.push_back(1.0);
	coefs = tcoefs;
      }
      dbanks[i]->SetFilterCoefs(coefs);
      indexmgrs[i]->ClearIndexSetFlag();
    }
  } else {
    delete[] this->delay_vals;

    for (i=0; i<this->numlevels; i++) {
      delete dbanks[i];
      delete indexmgrs[i];
    }
    dbanks.clear();
    indexmgrs.clear();

    if (numlevels == 0) {
      // Need at least one level
      this->numlevels = 1;
    } else {
      this->numlevels = numlevels;
    }

    // Initialize the delay value vector
    this->delay_vals = new int[numlevels];
    for (i=0; i<numlevels; i++) {
      this->delay_vals[i] = delay_vals[i];
    }

    // Initialize the delay filters
    vector<double> coefs;
    for (i=0; i<numlevels; i++) {
      coefs.clear();
      if (delay_vals[i] == 0) {
	coefs.push_back(1.0);
      } else {
	vector<double> tcoefs(delay_vals[i]-1, 0);
	tcoefs.push_back(1.0);
	coefs = tcoefs;
      } 
    
      FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfir =
	new FIRFilter<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(coefs.size(), coefs);
      IndexManager* pim = new IndexManager();

      dbanks.push_back(pfir);
      indexmgrs.push_back(pim);
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

//--------------------------------------------------------------------
// Assumption here is that a sample time or multiple sample times
//  worth of data is provided (i.e. not disjoint between sample
//  times).
//--------------------------------------------------------------------
template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingSampleOperation(vector<OUTSAMPLE> &out, vector<INSAMPLE> &in)
{
  bool result=false;
  bool jitter=true;
  int samplelevel, sampleindex;
  OUTSAMPLE outsamp;

  // THERE IS PROBABLY A BUG HERE, WHERE NEW SAMPLES COME IN AND ARE ADDED
  //  TO THE BACKLOG, BUT WE NEVER CATCH UP BECAUSE OF THE ORDERING OF THE
  //  BACKLOG.  NOT SURE YET, BUT KEEP IN MIND.

  // Check jitter buffer for ordered samples
  vector<INSAMPLE>::iterator viter;
  for (viter=jitter_buffer.begin(); viter!=jitter_buffer.end(); viter++) {
    samplelevel = viter->GetSampleLevel();
    sampleindex = viter->GetSampleIndex();

    if (indexmgrs[samplelevel]->InOrder(sampleindex)) {
      dbanks[samplelevel]->GetFilterOutput(outsamp, *viter);
      outsamp.SetSampleLevel(samplelevel);
      outsamp.SetSampleIndex(sampleindex);
      out.push_back(outsamp);

      jitter_buffer.erase(viter);
      jitter=false;
      result=true;
    }
  }
  if (!jitter) {
    jitterhelp.DecCurrentBacklog();
  }


  // Check input buffer for new outputs
  unsigned i;
  jitter=false;
  for (i=0; i<in.size(); i++) {
    samplelevel = in[i].GetSampleLevel();
    sampleindex = in[i].GetSampleIndex();

    if (!indexmgrs[samplelevel]->Initialized()) {
      indexmgrs[samplelevel]->SetIndex(sampleindex);
      dbanks[samplelevel]->GetFilterOutput(outsamp, in[i]);
      outsamp.SetSampleLevel(samplelevel);
      outsamp.SetSampleIndex(sampleindex);
      out.push_back(outsamp);
      result = true;

    } else if (!indexmgrs[samplelevel]->InOrder(sampleindex)) {
      jitter_buffer.push_back(in[i]);
      jitter = true;
      result=false;

    } else {
      dbanks[samplelevel]->GetFilterOutput(outsamp, in[i]);
      outsamp.SetSampleLevel(samplelevel);
      outsamp.SetSampleIndex(sampleindex);
      out.push_back(outsamp);
      result=true;

    }
  }
  if (jitter) {
    jitterhelp.IncCurrentBacklog();
    if (jitterhelp.ThresholdExceeded()) {
      ClearAllDelayLines();
      jitterhelp.ClearCurrentBacklog();
      for (unsigned j=0; j<indexmgrs.size(); j++) {
	indexmgrs[j]->ClearIndexSetFlag();
      }
      result=false;
    }
  }
  return result;
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingBlockOperation(vector<SampleBlock<OUTSAMPLE> *> &outblock,
			vector<SampleBlock<INSAMPLE> *> &inblock)
{
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
  os << endl << "The samples in jitter buffer: " << endl;
  for (i=0; i<jitter_buffer.size(); i++) {
    os << "\t" << jitter_buffer[i] << endl;
  }
  os << endl << "The index managers: " << endl;
  vector<IndexManager *>::iterator vim_iter=indexmgrs.begin();
  for (vim_iter=indexmgrs.begin(); vim_iter!=indexmgrs.end(); vim_iter++) {
    os << "\t" << *vim_iter << endl;
  }
  return os;
}

#endif
