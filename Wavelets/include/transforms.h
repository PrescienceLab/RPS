#ifndef _transforms
#define _transforms

#include <vector>
#include <deque>

#include "waveletinfo.h"
#include "stage.h"
#include "sample.h"
#include "sampleblock.h"
#include "util.h"

/********************************************************************************
 *
 * This class performs a streaming wavelet transform, either block by block or 
 *  sample by sample.  The number of stages in the transform is statically set
 *  up upon construction, but can be changed.  Changing the number of stages
 *  resets the entire structure.  The stages in this class are all uniform, in
 *  that they all have the same upsample rate, and filter coefficients.
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class StaticForwardWaveletTransform {
private:
  unsigned numstages;
  unsigned numlevels;
  int      lowest_outlvl;
  unsigned index[MAX_STAGES+1];

  vector<ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE> *> stages;

public:
  StaticForwardWaveletTransform(unsigned numstages=1,
				int      lowest_outlvl=0);
  StaticForwardWaveletTransform(const StaticForwardWaveletTransform &rhs);
  StaticForwardWaveletTransform(unsigned    numstages,
				WaveletType wavetype,
				unsigned    rate_l,
				unsigned    rate_h,
				int         lowest_outlvl);

  virtual ~StaticForwardWaveletTransform();

  StaticForwardWaveletTransform & operator=(const StaticForwardWaveletTransform &rhs);

  inline unsigned GetNumberStages() const;
  bool ChangeNumberStages(unsigned    numstages);
  bool ChangeNumberStages(unsigned    numstages,
			  WaveletType wavetype,
			  unsigned    rate_l,
			  unsigned    rate_h,
			  int         lowest_outlvl);


  inline int GetLowestOutputLevel() const;
  inline void SetLowestOutputLevel(int lowest_outlvl);

  inline unsigned GetIndexNumberOfLevel(int level) const;
  inline void SetIndexNumberOfLevel(int level, unsigned newindex);

  bool StreamingSampleOperation(vector<OUTSAMPLE> &out, Sample<SAMPLETYPE> &in);

  unsigned StreamingBlockOperation(vector<SampleBlock<OUTSAMPLE> *> &outblock,
				   SampleBlock<INSAMPLE>            &inblock);

  ostream & Print(ostream &os) const;
};


/********************************************************************************
 *
 * This class is the inverse of the above class.  Once the output
 *  of the forward class is manipulated, this class brings us back to a one
 *  dimensional time series.  The stages in this class are static, but can be 
 *  changed on the fly (the state is lost on changes however).  This class
 *  performs the streaming reverse operation block by block or sample by sample.
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class StaticReverseWaveletTransform {
protected:
  unsigned numstages;
  unsigned numlevels;

  vector<deque<INSAMPLE> *>  insignals;
  vector<deque<OUTSAMPLE> *> intersignals;

  vector<ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE> *> stages;

  // Protected member functions
  inline void ClearAllDelayLines();
  inline bool SamplePairReady(unsigned level1, unsigned level2, bool topstage) const;

  inline bool BlockPairReady(SampleBlock<INSAMPLE>  &block_l,
			     SampleBlock<INSAMPLE>  &block_h) const;
  inline bool BlockPairReady(SampleBlock<OUTSAMPLE> &inter_l,
			     SampleBlock<INSAMPLE>  &block_h) const;

  void AddRemainderToSignals(SampleBlock<INSAMPLE>  &block,
			     unsigned               minsize) const;
  void AddRemainderToSignals(SampleBlock<OUTSAMPLE> &block,
			     unsigned               minsize,
			     unsigned               level) const;

public:
  StaticReverseWaveletTransform(unsigned numstages=1);
  StaticReverseWaveletTransform(const StaticReverseWaveletTransform &rhs);
  StaticReverseWaveletTransform(unsigned    numstages,
				WaveletType wavetype,
				unsigned    rate_l,
				unsigned    rate_h);

  virtual ~StaticReverseWaveletTransform();

  StaticReverseWaveletTransform & operator=(const StaticReverseWaveletTransform &rhs);

  inline unsigned GetNumberStages() const;
  bool ChangeNumberStages(unsigned    numstages);
  bool ChangeNumberStages(unsigned    numstages,
			  WaveletType wavetype,
			  unsigned    rate_l,
			  unsigned    rate_h);

  bool StreamingSampleOperation(vector<OUTSAMPLE> &out, vector<INSAMPLE> &in);

  unsigned StreamingBlockOperation(SampleBlock<OUTSAMPLE>          &outblock,
				   vector<SampleBlock<INSAMPLE> *> &inblock);

  ostream & Print(ostream &os) const;
};


#if 0
class DynamicForwardWaveletTransform {

};

class DynamicReverseWaveletTransform {

};

class ForwardDiscreteWaveletTransform {

};

class ReverseDiscreteWaveletTransform {

};
#endif

/********************************************************************************
 * 
 * Member functions for the StaticForwardWaveletTransform class
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticForwardWaveletTransform(unsigned numstages=1,
			      int      lowest_outlvl=0)
{
  if ( (numstages <= 0) || (numstages > MAX_STAGES) ) {
    this->numstages = 1;
  } else {
    this->numstages = numstages;
  }
  this->numlevels = this->numstages + 1;
  this->lowest_outlvl = lowest_outlvl;

  for (i=0; i<numlevels; i++) {
    index[i] = 0;
  }

  int outlvl = lowest_outlvl;
  unsigned i;
  for (i=0; i<this->numstages-1; i++) {
    ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfws = 
      new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
    pfws->SetOutputLevelHigh(outlvl++);
    pfws->SetOutputLevelLow(-1);
    stages.push_back(pfws);
  }

  // Set the top stage a little differently
  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfws = 
    new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
  pfws->SetOutputLevelHigh(outlvl++);
  pfws->SetOutputLevelLow(outlvl);
  stages.push_back(pfws);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticForwardWaveletTransform(const StaticForwardWaveletTransform &rhs) : 
  numstages(rhs.numstages), numlevels(rhs.numlevels), 
  lowest_outlvl(rhs.lowest_outlvl), index(rhs.index), 
  stages(rhs.stages)
{
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticForwardWaveletTransform(unsigned    numstages,
			      WaveletType wavetype,
			      unsigned    rate_l, 
			      unsigned    rate_h,
			      int         lowest_outlvl)
{
  unsigned i;

  if ( (numstages <= 0) || (numstages > MAX_STAGES) ) {
    this->numstages = 1;
  } else {
    this->numstages = numstages;
  }
  this->numlevels = this->numstages + 1;
  this->lowest_outlvl = lowest_outlvl;
  
  for (i=0; i<numlevels; i++) {
    index[i] = 0;
  }

  int outlvl = lowest_outlvl;
  for (i=0; i<this->numstages-1; i++) {
    ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfws = 
      new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype, 
							       rate_l, 
							       rate_h,
							       -1, 
							       outlvl++);
    stages.push_back(pfws);
  }

  // Set last low frequency stage to highest level (DC)
  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfws = 
    new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype, 
							     rate_l, 
							     rate_h,
							     -1, 
							     outlvl++);
  pfws->SetOutputLevelLow(outlvl);
  stages.push_back(pfws);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~StaticForwardWaveletTransform()
{
  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfws;

  unsigned i;
  for (i=0; i<numstages; i++) {
    pfws = stages[i]; CHK_DEL(pfws);
  }

  stages.clear();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE> &
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator=(const StaticForwardWaveletTransform &rhs)
{
  numstages = rhs.numstages;
  numlevels = rhs.numlevels;
  lowest_outlvl = rhs.lowest_outlvl;

  for (unsigned i=0; i<numlevels; i++) {
    index[i] = rhs.index[i];
  }

  stages = rhs.stages;
  return *this;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetNumberStages() const
{
  return numstages;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeNumberStages(unsigned numstages)
{
  if ( (numstages <= 0) || (numstages > MAX_STAGES) ) {
    return false;
  }
  
  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfws;

  unsigned i;
  for (i=0; i<this->numstages; i++) {
    pfws = stages[i]; CHK_DEL(pfws);
  }

  stages.clear();

  this->numstages = numstages;
  this->numlevels = numstages + 1;

  for (i=0; i<numlevels; i++) {
    index[i] = 0;
  }

  int outlvl = lowest_outlvl;
  for (i=0; i<numstages-1; i++) {
    pfws = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
    pfws->SetOutputLevelLow(-1);
    pfws->SetOutputLevelHigh(lowest_outlvl++);
    stages.push_back(pfws);
  }

  // Set the top stage to highest level (DC)
  pfws = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
  pfws->SetOutputLevelHigh(outlvl++);
  pfws->SetOutputLevelLow(outlvl);
  stages.push_back(pfws);

  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeNumberStages(unsigned    numstages,
		   WaveletType wavetype,
		   unsigned    rate_l,
		   unsigned    rate_h, 
		   int         lowest_outlvl)
{
  if ( (numstages <= 0) || (numstages > MAX_STAGES) ) {
    return false;
  }
  
  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfws;

  unsigned i;
  for (i=0; i<this->numstages; i++) {
    pfws = stages[i]; CHK_DEL(pfws);
  }
  stages.clear();

  this->numstages = numstages;
  this->numlevels = numstages + 1;

  for (i=0; i<numlevels; i++) {
    index[i] = 0;
  }

  int outlvl = lowest_outlvl;
  for (i=0; i<numstages-1; i++) {
    pfws = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype,
								    rate_l,
								    rate_h,
								    -1,
								    outlvl++);
    stages.push_back(pfws);
  }

  // Set last low frequency stage to highest level (DC)
  pfws = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype, 
								  rate_l, 
								  rate_h,
								  -1, 
								  outlvl++);
  pfws->SetOutputLevelLow(outlvl);
  stages.push_back(pfws);

  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
int StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetLowestOutputLevel() const
{
  return lowest_outlvl;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetLowestOutputLevel(int lowest_outlvl)
{
  this->lowest_outlvl = lowest_outlvl;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetIndexNumberOfLevel(int level) const
{
  return index[level];
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetIndexNumberOfLevel(int level, unsigned newindex)
{
  index[level] = newindex;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingSampleOperation(vector<OUTSAMPLE> &out, Sample<SAMPLETYPE> &in)
{
  bool result = false;
  unsigned i;

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfws;
  Sample<SAMPLETYPE> newin;
  OUTSAMPLE out_l, out_h;
 
  // Run stages on input sample
  newin = in;
  for (i=0; i<numstages; i++) {
    pfws = stages[i];
    if (!pfws->PerformSampleOperation(out_l, out_h, newin)) {
      break;
    } else {
      out_h.SetSampleIndex(index[i]++);
      out.push_back(out_h);
      newin.SetSampleValue(out_l.GetSampleValue());
      if (i == numstages-1) {
	out_l.SetSampleIndex(index[i+1]++);
	out.push_back(out_l);
      }
      result = true;
    }
  }

  return result;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingBlockOperation(vector<SampleBlock<OUTSAMPLE> *> &outblock,
			SampleBlock<INSAMPLE>            &inblock)
{
  if (outblock.size() != numlevels) {
    return 0;
  }

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfws;
  SampleBlock<OUTSAMPLE> out_l, out_h;
  SampleBlock<OUTSAMPLE>* psbo;
  unsigned blocklen=0;

  SampleBlock<INSAMPLE> newinput(inblock);
  for (unsigned i=0; i<numstages; i++) {
    pfws = stages[i];
    psbo = outblock[i];
    blocklen += pfws->PerformBlockOperation(out_l, out_h, newinput);
    newinput = out_l;
    psbo->AppendBlockBack(out_h);
    if (i == numstages-1) {
      psbo = outblock[i+1];
      psbo->AppendBlockBack(out_l);
      break;
    }
  }

  return blocklen;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
Print(ostream &os) const
{
  os << "Number of stages: " << numstages << endl;
  os << "Level range:      " << "[" << lowest_outlvl 
     << "," << lowest_outlvl+numstages << "]" << endl;
  os << endl;

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfws;
  for (unsigned i=0; i<numstages; i++) {
    os << "STAGE " << i << ":" << endl;
    pfws = stages[i];
    os << *pfws << endl;
  }
  return os;
}


/********************************************************************************
 * 
 * Member functions for the StaticReverseWaveletTransform class
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticReverseWaveletTransform(unsigned numstages=1)
{
  if ( (numstages <= 0) || (numstages > MAX_STAGES) ) {
    this->numstages = 1;
  } else {
    this->numstages = numstages;
  }
  this->numlevels = this->numstages+1;

  unsigned i;
  for (i=0; i<numlevels; i++) {
    deque<INSAMPLE>* pdis = new deque<INSAMPLE>();
    insignals.push_back(pdis);
  }

  for (i=0; i<this->numstages; i++) {
    ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* prws = 
      new ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
    stages.push_back(prws);

    deque<OUTSAMPLE>* pdis = new deque<OUTSAMPLE>();
    intersignals.push_back(pdis);    
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticReverseWaveletTransform(const StaticReverseWaveletTransform &rhs) :
  numstages(rhs.numstages), numlevels(rhs.numlevels), 
  insignals(rhs.insignals), intersignals(rhs.intersignals),
  stages(rhs.stages)
{}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticReverseWaveletTransform(unsigned    numstages,
			      WaveletType wavetype,
			      unsigned    rate_l,
			      unsigned    rate_h)
{
  if ( (numstages <= 0) || (numstages > MAX_STAGES) ) {
    this->numstages = 1;
  } else {
    this->numstages = numstages;
  }
  
  this->numlevels = this->numstages+1;

  unsigned i;
  for (i=0; i<numlevels; i++) {
    deque<INSAMPLE>* pdis = new deque<INSAMPLE>();
    insignals.push_back(pdis);
  }

  for (i=0; i<this->numstages; i++) {
    ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* prws = 
      new ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype, 
							       rate_l, 
							       rate_h);
    stages.push_back(prws);

    deque<OUTSAMPLE>* pdis = new deque<OUTSAMPLE>();
    intersignals.push_back(pdis);
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~StaticReverseWaveletTransform()
{
  unsigned i;
  for (i=0; i<numlevels; i++) {
    CHK_DEL(insignals[i]);
  }

  for (unsigned i=0; i<numstages; i++) {
    CHK_DEL(stages[i]);
    CHK_DEL(intersignals[i]);
  }
  insignals.clear();
  intersignals.clear();
  stages.clear();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE> & 
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator=(const StaticReverseWaveletTransform &rhs)
{
  numstages = rhs.numstages;
  numlevels = rhs.numlevels;
  insignals = rhs.insignals;
  intersignals = rhs.intersignals;
  stages = rhs.stages;
  return *this;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetNumberStages() const
{
  return numstages;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeNumberStages(unsigned numstages)
{
  if ( (numstages <= 0) || (numstages > MAX_STAGES) ) {
    return false;
  }

  unsigned i;
  for (i=0; i<this->numlevels; i++) {
    CHK_DEL(insignals[i]);
  }
  
  for (i=0; i<this->numstages; i++) {
    CHK_DEL(stages[i]);
    CHK_DEL(interstages[i]);
  }

  insignals.clear();
  intersignals.clear();
  stages.clear();

  this->numstages = numstages;
  this->numlevels = numstages+1;

  for (i=0; i<numlevels; i++) {
    deque<INSAMPLE>* pdis = new deque<INSAMPLE>();
    insignals.push_back(pdis);
  }    

  for (i=0; i<numstages; i++) {
    prws = new ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
    stages.push_back(prws);

    deque<OUTSAMPLE>* pdis = new deque<OUTSAMPLE>();
    intersignals.push_back(pdis);
  }

  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeNumberStages(unsigned    numstages,
		   WaveletType wavetype,
		   unsigned    rate_l,
		   unsigned    rate_h)
{
  if ( (numstages <= 0) || (numstages > MAX_STAGES) ) {
    return false;
  }
  
  unsigned i;
  for (i=0; i<this->numlevels; i++) {
    CHK_DEL(insignals[i]);
    CHK_DEL(indexmgrs[i]);
  }

  for (i=0; i<this->numstages; i++) {
    CHK_DEL(stages[i]);
    CHK_DEL(intersignals[i]);
  }
  insignals.clear();
  intersignals.clear();
  stages.clear();

  this->numstages = numstages;
  this->numlevels = numstages+1;

  for (i=0; i<numlevels; i++) {
    deque<INSAMPLE>* pdis = new deque<INSAMPLE>();
    insignals.push_back(pdis);
  }    

  for (i=0; i<numstages; i++) {
    prws = new ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype,
								    rate_l,
								    rate_h);
    stages.push_back(prws);

    deque<OUTSAMPLE>* pdis = new deque<OUTSAMPLE>();
    intersignals.push_back(pdis);
  }

  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearAllDelayLines()
{
  for (unsigned i=0; i<numstages; i++) {
    stages[i]->ClearFilterDelayLines();
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SamplePairReady(unsigned level1, unsigned level2, bool topstage) const
{
  bool result;
  if ((level1 < 0) || (level2 < 0) || (level1 > numlevels) || (level2 > numlevels)) {
    return false;
  }

  if (topstage) {
    result = !(insignals[level1]->empty() || insignals[level2]->empty());
  } else {
    if ((level1 > numlevels-1) || (level2 > numlevels-1)) {
      return false;
    }
    result = !(intersignals[level1]->empty() || insignals[level2]->empty());
  }
  return result;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
BlockPairReady(SampleBlock<INSAMPLE> &block_l, SampleBlock<INSAMPLE> &block_h) const
{
  bool result=false;
  if (block_l.GetBlockSize() == block_h.GetBlockSize()) {
    result = true;
  }
  return result;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
BlockPairReady(SampleBlock<OUTSAMPLE> &inter_l, SampleBlock<INSAMPLE> &block_h) const
{
  bool result=false;
  if (inter_l.GetBlockSize() == block_h.GetBlockSize()) {
    result = true;
  }
  return result;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
AddRemainderToSignals(SampleBlock<INSAMPLE> &block, 
		      unsigned              minsize) const
{
  int blocklevel = block.GetBlockLevel();
  if ( (blocklevel != -1) && (blocklevel < numlevels) ) {
    // Transfer samples [minsize, blocksize] into insignals
    INSAMPLE samp;
    for (unsigned i=minsize; i<block.GetBlockSize(); i++) {
      block.GetSample(samp, i);
      insignals[blocklevel].push_front(samp);
    }
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
AddRemainderToSignals(SampleBlock<OUTSAMPLE> &block, 
		      unsigned               minsize,
		      unsigned               level) const
{
  if ( (level >= 0) && (level < numlevels) ) {
    // Transfer samples [minsize, blocksize] into intersignals
    OUTSAMPLE samp;
    for (unsigned i=minsize; i<block.GetBlockSize(); i++) {
      block.GetSample(samp, i);
      intersignals[level].push_front(samp);
    }
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingSampleOperation(vector<OUTSAMPLE> &out, vector<INSAMPLE> &in)
{
  int samplelevel;

  // Clear the output vector to signal new output to the calling routine
  out.clear();

  // Perform the operation on matched levels of input signal
  ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* prws;
  INSAMPLE  in_h, in_l;

  // Sort the in vector into insignals
  unsigned i;
  for (i=0; i<in.size(); i++) {
    samplelevel = in[i].GetSampleLevel();
    insignals[samplelevel]->push_front(in[i]);
  }

  bool topstage=true;
  for (int j=numstages-1; j>=0; j--) {
    prws = stages[j];

    if ( SamplePairReady(j+1, j, topstage) ) {

      if (topstage) {
	in_l = insignals[numstages]->back();
	insignals[numstages]->pop_back();

	in_h = insignals[numstages-1]->back();
	insignals[numstages-1]->pop_back();

      } else {
	in_l = intersignals[j+1]->back();
	intersignals[j+1]->pop_back();

	in_h = insignals[j]->back();
	insignals[j]->pop_back();
      }
	
      prws->PerformSampleOperation(out, in_l, in_h);

      // Store the outputs into the appropriate intermediate outputs
      for (unsigned k=0; k<out.size(); k++) {
	intersignals[j]->push_front(out[k]);
      }

      if (j != 0) {
	out.clear();
      }
    }
    topstage=false;
  }

  return out.size();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingBlockOperation(SampleBlock<OUTSAMPLE>           &outblock,
			vector<SampleBlock<INSAMPLE> *>  &inblock)
{
  if (inblock.size() != numlevels) {
    return 0;
  }

  ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* prws;
  SampleBlock<INSAMPLE>* in_h;
  SampleBlock<INSAMPLE>* in_l;
  SampleBlock<OUTSAMPLE> inter_l;
  deque<INSAMPLE> newblock;
  unsigned blocklen=0;

  // Perform the top stage a bit differently
  in_l = inblock[numstages];
  in_h = inblock[numstages-1];

  if ( BlockPairReady(*in_l, *in_h) ) {
    blocklen += stages[numstages-1]->PerformBlockOperation(outblock, *in_l, *in_h);
  } else {
    // Here we must run in block mode for the minimum of the two blocks, and 
    //  then store the end of the buffer into insignals
    unsigned l_blksize = in_l->GetBlockSize();
    unsigned h_blksize = in_h->GetBlockSize();

    SampleBlock<INSAMPLE>* pblock = (h_blksize < l_blksize) ? in_l : in_h;
    unsigned min_blksize = (h_blksize < l_blksize) ? h_blksize :
      l_blksize;

    in_l->GetSamples(newblock, 0, min_blocksize);
    SampleBlock<INSAMPLE> newin_l(newblock);
    newblock.clear();

    in_h->GetSamples(newblock, 0, min_blocksize);
    SampleBlock<INSAMPLE> newin_h(newblock);
    newblock.clear();

    blocklen += stages[numstages-1]->PerformBlockOperation(outblock,
							   newin_l, 
							   newin_h);

    AddRemainderToInsignals(*pblock, min_blocksize);
  }

  for (int j=0; j<numstages-1; j--) {
    prws = stages[j];

    if (topstage) {
      in_l = inblock[numstages];
      in_h = inblock[numstages-1];
    } else {
      in_l = newout;
      in_h  = inblock[j];
    }

    if (BlockPairReady(*in_l, *in_h)) {
      prws->PerformBlockOperation(newout, in_l, in_h);
    }


    topstage=false;
  }
  return blocklen;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
Print(ostream &os) const
{
  os << "Number of stages: " << numstages << endl;
  os << "Number of levels: " << numlevels << endl;

  os << "The samples in the level oriented input buffer: " << endl;
  unsigned i;
  deque<INSAMPLE>* pdis;
  for (i=0; i<numlevels; i++) {
    os << "Level " << i << ":" << endl;

    pdis = insignals[i];
    for (unsigned j=0; j<pdis->size(); j++) {
      os << pdis[j] << endl;
    }
  }

  ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* prws;
       for (i=0; i<numstages; i++) {
    os << "STAGE " << i << ":" << endl;
    prws = stages[i];
    os << *prws << endl;
  }
  return os;
}

#endif
