#ifndef _transforms
#define _transforms

#include <vector>
#include <deque>
#include <cmath>

#include "waveletinfo.h"
#include "coefficients.h"
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
protected:
  unsigned numstages;
  unsigned numlevels;
  int      lowest_outlvl;
  unsigned index[MAX_STAGES+1];

  vector<ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE> *> stages;
  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* first_stage;

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
  unsigned index;

  vector<deque<INSAMPLE> *> insignals;
  vector<deque<INSAMPLE> *> intersignals;

  vector<ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE> *> stages;
  ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* last_stage;

  // Protected member functions
  inline void ClearAllDelayLines();
  inline bool SamplePairReady(unsigned level1, unsigned level2, bool topstage) const;

  inline bool BlockPairReady(SampleBlock<INSAMPLE>  &block_l,
			     SampleBlock<INSAMPLE>  &block_h) const;

  void AddRemainingBlockToInsignals(SampleBlock<INSAMPLE> &block,
				    unsigned              minsize,
				    unsigned              level);

  void AddRemainingBlockToIntersignals(SampleBlock<INSAMPLE> &block,
				       unsigned              minsize,
				       unsigned              level);

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


template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class DynamicForwardWaveletTransform : public 
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE> {
public:
  DynamicForwardWaveletTransform() :
    StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>() {};
  DynamicForwardWaveletTransform(const DynamicForwardWaveletTransform &rhs) :
    StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(rhs) {};
  virtual ~DynamicForwardWaveletTransform() {};

  bool AddStage();
  bool AddStage(WaveletType wavetype,
		unsigned    rate_l,
		unsigned    rate_h);
		
  bool RemoveStage();
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class DynamicReverseWaveletTransform : public 
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE> {
public:
  DynamicReverseWaveletTransform() {};
  DynamicReverseWaveletTransform(const DynamicReverseWaveletTransform &rhs) :
    StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(rhs) {};
  virtual ~DynamicReverseWaveletTransform() {};

  bool AddStage();
  bool AddStage(WaveletType wavetype,
		unsigned    rate_l,
		unsigned    rate_h);

  bool RemoveStage();
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class ForwardDiscreteWaveletTransform {
private:
  WaveletType         wavetype;
  WaveletCoefficients wavecoefs;

  int      lowest_outlvl;
  unsigned index[MAX_STAGES+1];

public:
  ForwardDiscreteWaveletTransform(WaveletType wavetype=DAUB2, int lowest_outlvl=0);
  ForwardDiscreteWaveletTransform(const ForwardDiscreteWaveletTransform &rhs);
  virtual ~ForwardDiscreteWaveletTransform();

  ForwardDiscreteWaveletTransform & operator=(const ForwardDiscreteWaveletTransform &rhs);

  inline int GetLowestOutputLevel() const;
  inline void SetLowestOutputLevel(int lowest_outlvl);

  inline unsigned GetIndexNumberOfLevel(int level) const;
  inline void SetIndexNumberOfLevel(int level, unsigned newindex);

  inline WaveletType GetWaveletType() const;
  bool ChangeWaveletType(WaveletType wavetype);

  bool DiscreteWaveletTransform(SampleBlock<OUTSAMPLE> &outblock,
				SampleBlock<INSAMPLE>  &inblock);
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class ReverseDiscreteWaveletTransform {
private:
  WaveletType         wavetype;
  WaveletCoefficients wavecoefs;

  unsigned index;

public:
  ReverseDiscreteWaveletTransform(WaveletType wavetype=DAUB2);
  ReverseDiscreteWaveletTransform(const ReverseDiscreteWaveletTransform &rhs);
  virtual ~ReverseDiscreteWaveletTransform();

  ReverseDiscreteWaveletTransform & operator=(const ReverseDiscreteWaveletTransform &rhs);

  inline unsigned GetIndexNumber() const;
  inline void SetIndexNumber(unsigned newindex);

  inline WaveletType GetWaveletType() const;
  bool ChangeWaveletType(WaveletType wavetype);

  bool InverseDiscreteWaveletTransform(SampleBlock<OUTSAMPLE> &outblock,
				       SampleBlock<INSAMPLE>  &inblock);
};

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
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
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

  // The lowest stage converts from INSAMPLES to OUTSAMPLES
  first_stage = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
  first_stage->SetOutputLevelHigh(outlvl++);
  first_stage->SetOutputLevelLow(outlvl);

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws;
  for (i=0; i<this->numstages-1; i++) {
    pfws = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>();
    pfws->SetOutputLevelHigh(outlvl++);
    pfws->SetOutputLevelLow(outlvl);
    stages.push_back(pfws);
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticForwardWaveletTransform(const StaticForwardWaveletTransform &rhs) : 
  numstages(rhs.numstages), numlevels(rhs.numlevels), 
  lowest_outlvl(rhs.lowest_outlvl), index(rhs.index), 
  first_stage(rhs.first_stage), stages(rhs.stages)
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

  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
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

  // The lowest stage converts from INSAMPLES to OUTSAMPLES
  first_stage = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype,
									 rate_l,
									 rate_h,
									 lowest_outlvl+1,
									 outlvl++);

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws;
  for (i=0; i<this->numstages-1; i++) {
    pfws = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>(wavetype, 
								     rate_l, 
								     rate_h,
								     outlvl,
								     outlvl++);
    stages.push_back(pfws);
  }

}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~StaticForwardWaveletTransform()
{
  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws;

  unsigned i;
  CHK_DEL(first_stage);
  for (i=0; i<numstages-1; i++) {
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

  first_stage = rhs.first_stage;
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
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    return false;
  }

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws;  
  unsigned i;

  CHK_DEL(first_stage);
  for (i=0; i<this->numstages-1; i++) {
    pfws = stages[i]; CHK_DEL(pfws);
  }
  stages.clear();

  this->numstages = numstages;
  this->numlevels = numstages + 1;

  for (i=0; i<numlevels; i++) {
    index[i] = 0;
  }

  int outlvl = lowest_outlvl;

  // The lowest stage converts from INSAMPLES to OUTSAMPLES
  first_stage = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
  first_stage->SetOutputLevelHigh(outlvl++);
  first_stage->SetOutputLevelLow(outlvl);

  for (i=0; i<numstages-1; i++) {
    pfws = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>();
    pfws->SetOutputLevelHigh(outlvl++);
    pfws->SetOutputLevelLow(outlvl);
    stages.push_back(pfws);
  }

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
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    return false;
  }

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws;
  unsigned i;

  CHK_DEL(first_stage);
  for (i=0; i<this->numstages-1; i++) {
    pfws = stages[i]; CHK_DEL(pfws);
  }
  stages.clear();

  this->numstages = numstages;
  this->numlevels = numstages + 1;

  for (i=0; i<numlevels; i++) {
    index[i] = 0;
  }

  int outlvl = lowest_outlvl;

  // The lowest stage converts from INSAMPLES to OUTSAMPLES
  first_stage = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype,
									 rate_l,
									 rate_h,
									 lowest_outlvl+1,
									 outlvl++);

  for (i=0; i<numstages-1; i++) {
    pfws = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>(wavetype,
								     rate_l,
								     rate_h,
								     outlvl,
								     outlvl++);
    stages.push_back(pfws);
  }

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
  if (level < MAX_STAGES+1) {
    return index[level];
  } else {
    return 0;
  }
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

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws;
  Sample<SAMPLETYPE> newin;
  OUTSAMPLE out_l, out_h;

  newin = in;

  // Start with the lowest stage
  if (numstages==1) {
    if (first_stage->PerformSampleOperation(out_l, out_h, newin)) {
      out_h.SetSampleIndex(index[0]++);
      out_l.SetSampleIndex(index[1]++);
      out.push_back(out_h);
      out.push_back(out_l);
      result = true;
    }
  } else {
    if (first_stage->PerformSampleOperation(out_l, out_h, newin)) {
      out_h.SetSampleIndex(index[0]++);
      out.push_back(out_h);
      newin.SetSampleValue(out_l.GetSampleValue());
      result = true;

      // Do the rest of the stages
      for (i=0; i<numstages-1; i++) {
	pfws = stages[i];
	if (!pfws->PerformSampleOperation(out_l, out_h, newin)) {
	  break;
	} else {
	  out_h.SetSampleIndex(index[i+1]++);
	  out.push_back(out_h);
	  newin.SetSampleValue(out_l.GetSampleValue());
	  if (i == numstages-2) {
	    out_l.SetSampleIndex(index[i+2]++);
	    out.push_back(out_l);
	  }
	}
      }
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

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws;
  SampleBlock<OUTSAMPLE> out_l, out_h;
  unsigned blocklen=0, i;

  SampleBlock<INSAMPLE> newinput(inblock);

  // Start with the lowest stage
  if (numstages == 1) {
    blocklen = first_stage->PerformBlockOperation(out_l, out_h, newinput);
    out_h.SetBlockIndex(index[0]);
    out_l.SetBlockIndex(index[1]);
    index[0] += blocklen; index[1] += blocklen;
    
    outblock[0]->AppendBlockBack(out_h);
    outblock[1]->AppendBlockBack(out_l);
  } else {
    blocklen = first_stage->PerformBlockOperation(out_l, out_h, newinput);
    if (blocklen) {
      out_h.SetBlockIndex(index[0]);
      index[0] += blocklen;

      outblock[0]->AppendBlockBack(out_h);

      SampleBlock<OUTSAMPLE> nextinput(out_l);

      for (i=0; i<numstages-1; i++) {
	pfws = stages[i];
	out_l.ClearBlock(); out_h.ClearBlock();
	unsigned blocksize = pfws->PerformBlockOperation(out_l, out_h, nextinput);
	if (blocksize) {
	  out_h.SetBlockIndex(index[i+1]);
	  index[i+1] += blocksize;
	  blocklen += blocksize;

	  outblock[i+1]->AppendBlockBack(out_h);

	  if (i == numstages-2) {
	    out_l.SetBlockIndex(index[i+2]);
	    index[i+2] += blocksize;
	    blocklen += blocksize;

	    outblock[i+2]->AppendBlockBack(out_l);
	  }
	} else {
	  break;
	}
      }
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
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    this->numstages = 1;
  } else {
    this->numstages = numstages;
  }
  this->numlevels = this->numstages+1;
  this->index = 0;

  unsigned i;
  for (i=0; i<numlevels; i++) {
    deque<INSAMPLE>* pdis = new deque<INSAMPLE>();
    insignals.push_back(pdis);
  }

  last_stage = new ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();

  for (i=0; i<this->numstages-1; i++) {
    ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>* prws = 
      new ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>();
    stages.push_back(prws);
  }

  for (i=0; i<this->numstages-1; i++) {
    deque<INSAMPLE>* pdis = new deque<INSAMPLE>();
    intersignals.push_back(pdis);    
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticReverseWaveletTransform(const StaticReverseWaveletTransform &rhs) :
  numstages(rhs.numstages), numlevels(rhs.numlevels), index(rhs.index),
  insignals(rhs.insignals), intersignals(rhs.intersignals),
  last_stage(rhs.last_stage), stages(rhs.stages)
{}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticReverseWaveletTransform(unsigned    numstages,
			      WaveletType wavetype,
			      unsigned    rate_l,
			      unsigned    rate_h)
{
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    this->numstages = 1;
  } else {
    this->numstages = numstages;
  }
  this->numlevels = this->numstages+1;
  this->index = 0;

  unsigned i;
  for (i=0; i<numlevels; i++) {
    deque<INSAMPLE>* pdis = new deque<INSAMPLE>();
    insignals.push_back(pdis);
  }

  last_stage = new ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype,
									rate_l,
									rate_h);
  for (i=0; i<this->numstages-1; i++) {
    ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>* prws = 
      new ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>(wavetype,
							      rate_l,
							      rate_h);
    stages.push_back(prws);
  }

  for (i=0; i<this->numstages-1; i++) {
    deque<INSAMPLE>* pdis = new deque<INSAMPLE>();
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

  CHK_DEL(last_stage);
  for (i=0; i<numstages-1; i++) {
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
  index = rhs.index;
  insignals = rhs.insignals;
  intersignals = rhs.intersignals;
  last_stage = rhs.last_stage;
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
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    return false;
  }

  unsigned i;
  for (i=0; i<this->numlevels; i++) {
    CHK_DEL(insignals[i]);
  }
  
  CHK_DEL(last_stage);
  for (i=0; i<this->numstages-1; i++) {
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

  last_stage = new ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
  for (i=0; i<numstages-1; i++) {
    prws = new ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>();
    stages.push_back(prws);
  }

  for (i=0; i<numstages-1; i++) {
    deque<INSAMPLE>* pdis = new deque<INSAMPLE>();
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
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    return false;
  }
  
  unsigned i;
  for (i=0; i<this->numlevels; i++) {
    CHK_DEL(insignals[i]);
  }

  CHK_DEL(last_stage);
  for (i=0; i<this->numstages-1; i++) {
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

  last_stage = ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype,
								    rate_l,
								    rate_h);
  for (i=0; i<numstages-1; i++) {
    prws = new ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>(wavetype,
								   rate_l,
								   rate_h);
    stages.push_back(prws);
  }

  for (i=0; i<numstages-1; i++) {
    deque<INSAMPLE>* pdis = new deque<INSAMPLE>();
    intersignals.push_back(pdis);    
  }

  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearAllDelayLines()
{
  last_stage->ClearFilterDelayLines();
  for (unsigned i=0; i<numstages-1; i++) {
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
    result = (insignals[level1]->empty() || insignals[level2]->empty());
  } else {
    if ((level1 > numlevels-2) || (level2 > numlevels-2)) {
      return false;
    }
    result = (intersignals[level1]->empty() || insignals[level2]->empty());
  }
  return !result;
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
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
AddRemainingBlockToInsignals(SampleBlock<INSAMPLE> &block, 
			     unsigned              minsize,
			     unsigned              level)
{
  if ( (level >= 0) && (level <= numlevels-1) ) {
    // Transfer samples [minsize, blocksize] into insignals
    INSAMPLE samp;
    for (unsigned i=minsize; i<block.GetBlockSize(); i++) {
      block.GetSample(&samp, i);
      insignals[level]->push_front(samp);
    }
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
AddRemainingBlockToIntersignals(SampleBlock<INSAMPLE> &block, 
				unsigned              minsize,
				unsigned              level)
{
  if ( (level >= 0) && (level <= numlevels-1) ) {
    // Transfer samples [minsize, blocksize] into intersignals
    INSAMPLE samp;
    for (unsigned i=minsize; i<block.GetBlockSize(); i++) {
      block.GetSample(&samp, i);
      intersignals[level]->push_front(samp);
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
  Sample<SAMPLETYPE> in_h, in_l;
  vector<INSAMPLE> tempout;

  // Sort the in vector into insignals
  unsigned i;
  for (i=0; i<in.size(); i++) {
    samplelevel = in[i].GetSampleLevel();
    insignals[samplelevel]->push_front(in[i]);
  }

  if (numstages == 1) {
    // Handle the only stage
    if ( SamplePairReady(numstages, numstages-1, true) ) {
      in_l = insignals[numstages]->back();
      insignals[numstages]->pop_back();

      in_h = insignals[numstages-1]->back();
      insignals[numstages-1]->pop_back();

      last_stage->PerformSampleOperation(out, in_l, in_h);
    }
  } else {

    // Perform the topstage using reverse iteration
    vector<ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE> *>::reverse_iterator
      r_iter=stages.rbegin();

    if ( SamplePairReady(numstages, numstages-1, true) ) {
      in_l = insignals[numstages]->back();
      insignals[numstages]->pop_back();

      in_h = insignals[numstages-1]->back();
      insignals[numstages-1]->pop_back();
      
      (*r_iter)->PerformSampleOperation(tempout, in_l, in_h);

      // Store the outputs into the appropriate intermediate outputs
      for (i=0; i<tempout.size(); i++) {
	intersignals[numstages-2]->push_front(tempout[i]);
      }
      tempout.clear();
    }

    // Perform the rest of the stages
    r_iter++;
    for (int j=numstages-2; r_iter != stages.rend(); r_iter++, j--) {

      if ( SamplePairReady(j, j, false) ) {
	in_l = intersignals[j]->back();
	intersignals[j]->pop_back();

	in_h = insignals[j]->back();
	insignals[j]->pop_back();

	(*r_iter)->PerformSampleOperation(tempout, in_l, in_h);

	// Store the outputs into the appropriate intermediate outputs
	for (unsigned k=0; k<tempout.size(); k++) {
	  intersignals[j-1]->push_front(tempout[k]);
	}
	tempout.clear();
      }
    }

    // Handle the bottom stage
    if ( SamplePairReady(0, 0, false) ) {
      in_l = intersignals[0]->back();
      intersignals[0]->pop_back();

      in_h = insignals[0]->back();
      insignals[0]->pop_back();

      last_stage->PerformSampleOperation(out, in_l, in_h);
    }
  }

  // Set the output indexes
  for (i=0; i<out.size(); i++) {
    out[i].SetSampleIndex(index++);
  }

  return out.size();
}

// Some macros for StreamingBlockOperation

#define HANDLE_UNEVEN_BLOCK_LENGTH_2_INSIGNALS(p_stage, out, low, high) \
  unsigned l_blksize = (low).GetBlockSize();                            \
  unsigned h_blksize = (high).GetBlockSize();                           \
  if (h_blksize < l_blksize) {                                          \
    deque<INSAMPLE> newblock;                                           \
    (low).GetSamples(newblock, 0, h_blksize);                           \
    SampleBlock<INSAMPLE> temp_l(newblock);                             \
    newblock.clear();                                                   \
    (high).GetSamples(newblock, 0, h_blksize);                          \
    SampleBlock<INSAMPLE> temp_h(newblock);                             \
    newblock.clear();                                                   \
    (p_stage)->PerformBlockOperation((out), temp_l, temp_h);            \
    AddRemainingBlockToInsignals((low), h_blksize, numstages);          \
  } else {                                                              \
    deque<INSAMPLE> newblock;                                           \
    (low).GetSamples(newblock, 0, l_blksize);                           \
    SampleBlock<INSAMPLE> temp_l(newblock);                             \
    newblock.clear();                                                   \
    (high).GetSamples(newblock, 0, l_blksize);                          \
    SampleBlock<INSAMPLE> temp_h(newblock);                             \
    newblock.clear();                                                   \
    (p_stage)->PerformBlockOperation((out), temp_l, temp_h);            \
    AddRemainingBlockToInsignals((high), l_blksize, numstages-1);       \
  }                                                                     \

#define HANDLE_UNEVEN_BLOCK_LENGTH_2_SIGNALS(p_stage, level, out, low, high) \
  unsigned l_blksize = (low).GetBlockSize();                                 \
  unsigned h_blksize = (high).GetBlockSize();                                \
  if (h_blksize < l_blksize) {                                               \
    deque<INSAMPLE> newblock;                                                \
    (low).GetSamples(newblock, 0, h_blksize);                                \
    SampleBlock<INSAMPLE> temp_l(newblock);                                  \
    newblock.clear();                                                        \
    (high).GetSamples(newblock, 0, h_blksize);                               \
    SampleBlock<INSAMPLE> temp_h(newblock);                                  \
    newblock.clear();                                                        \
    (p_stage)->PerformBlockOperation((out), temp_l, temp_h);                 \
    AddRemainingBlockToIntersignals((low), h_blksize, level);                \
  } else {                                                                   \
    deque<INSAMPLE> newblock;                                                \
    (low).GetSamples(newblock, 0, l_blksize);                                \
    SampleBlock<INSAMPLE> temp_l(newblock);                                  \
    newblock.clear();                                                        \
    (high).GetSamples(newblock, 0, l_blksize);                               \
    SampleBlock<INSAMPLE> temp_h(newblock);                                  \
    newblock.clear();                                                        \
    (p_stage)->PerformBlockOperation((out), temp_l, temp_h);                 \
    AddRemainingBlockToInsignals((high), l_blksize, level);                  \
  }                                                                          \


template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingBlockOperation(SampleBlock<OUTSAMPLE>           &outblock,
			vector<SampleBlock<INSAMPLE> *>  &inblock)
{
  if (inblock.size() != numlevels) {
    return 0;
  }

  outblock.ClearBlock();

  unsigned i, j;

  // Append insignal samples to the front of the input block
  for (i=0; i<numlevels; i++) {
    deque<INSAMPLE>* pdi = insignals[i];
    SampleBlock<INSAMPLE>* psbi = inblock[i];

    for (j=0; j<pdi->size(); j++) {
      psbi->PushSampleFront(pdi->operator[](j));
    }
    pdi->clear();
  }

  SampleBlock<INSAMPLE>* pin_l = inblock[numstages];
  SampleBlock<INSAMPLE>* pin_h = inblock[numstages-1];

  if (numstages == 1) {
    // Handle the only stage
    if (BlockPairReady(*pin_l, *pin_h)) {
      last_stage->PerformBlockOperation(outblock, *pin_l, *pin_h);
    } else {
      // Here we must run in block mode for the minimum of the two blocks, and 
      //  then store the end of the buffer into insignals
      HANDLE_UNEVEN_BLOCK_LENGTH_2_INSIGNALS(last_stage, outblock, *pin_l, *pin_h);
    }
    outblock.SetBlockIndex(index);
    index += outblock.GetBlockSize();

  } else {
    // Perform the topstage and rest using reverse iteration through the stages
    vector<ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE> *>::reverse_iterator
      r_iter=stages.rbegin();

    // A temp output for intermediate results
    SampleBlock<INSAMPLE> interblock;

    if (BlockPairReady(*pin_l, *pin_h)) {
      (*r_iter)->PerformBlockOperation(interblock, *pin_l, *pin_h);
    } else {
      // Here we must run in block mode for the minimum of the two blocks, and 
      //  then store the end of the bigger buffer into the appropriate insignals
      HANDLE_UNEVEN_BLOCK_LENGTH_2_INSIGNALS((*r_iter), interblock, *pin_l, *pin_h);
    }


    // Perform the rest of the stages
    r_iter++;
    for (i=numstages-2; r_iter != stages.rend(); r_iter++, i--) {

      // If there are any samples in intersignals, append them to the front of
      //  interblock
      for (j=0; j<intersignals[i]->size(); j++) {
	interblock.PushSampleFront(intersignals[i]->operator[](j));
      }
      intersignals[i]->clear();

      SampleBlock<INSAMPLE> newin_l(interblock);
      pin_h = inblock[i];
      interblock.ClearBlock();

      if (BlockPairReady(newin_l, *pin_h)) {
	(*r_iter)->PerformBlockOperation(interblock, newin_l, *pin_h);
      } else {
	HANDLE_UNEVEN_BLOCK_LENGTH_2_SIGNALS((*r_iter), i, interblock, newin_l, *pin_h);
      }
    }

    // Handle the bottom stage
      
    // If there are any samples in intersignals, append them to the front of
    //  interblock
    for (i=0; i<intersignals[0]->size(); i++) {
      interblock.PushSampleFront(intersignals[0]->operator[](i));
    }
    intersignals[0]->clear();

    SampleBlock<INSAMPLE> newin_l(interblock);
    pin_h = inblock[0];

    if (BlockPairReady(newin_l, *pin_h)) {
      last_stage->PerformBlockOperation(outblock, newin_l, *pin_h);
    } else {
      HANDLE_UNEVEN_BLOCK_LENGTH_2_SIGNALS(last_stage, 0, outblock, newin_l, *pin_h);
    }
    outblock.SetBlockIndex(index);
    index += outblock.GetBlockSize();
  }

  return outblock.GetBlockSize();
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

/********************************************************************************
 * 
 * Member functions for the DynamicForwardWaveletTransform class
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DynamicForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
AddStage()
{
  if (this->numstages == MAX_STAGES) {
    return false;
  }

  this->numstages++;
  this->numlevels = this->numstages+1;

  // Clear the indices for the two new levels
  index[numstages] = 0;
  index[numstages-1] = 0;

  // Actually have to clone one of the stages in the system here
  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws = 
    (stages.back())->clone();
  pfws->ClearAllState();
  pfws->SetOutputLevelLow(lowest_outlvl+numstages);
  pfws->SetOutputLevelHigh(lowest_outlvl+numstages-1);
  stages.push_back(pfws);

  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DynamicForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
AddStage(WaveletType wavetype,
	 unsigned    rate_l,
	 unsigned    rate_h)
{
  if (this->numstages == MAX_STAGES) {
    return false;
  }

  this->numstages++;
  this->numlevels = this->numstages+1;

  // Clear the indices for the two new levels
  index[numstages] = 0;
  index[numstages-1] = 0;

  // Use passed parameters to instantiate a new FWS
  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws = new
    ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype,
							 rate_l,
							 rate_h,
							 lowest_outlvl+numstages,
							 lowest_outlvl+numstages-1);
  stages.push_back(pfws);

  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DynamicForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
RemoveStage()
{
  if (this->numstages == 1) {
    return false;
  }

  CHK_DEL(stages.back());
  stages.pop_back();

  this->numstages--;
  this->numlevels = this->numstages+1;
  return true;
}

/********************************************************************************
 * 
 * Member functions for the DynamicReverseWaveletTransform class
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DynamicReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
AddStage()
{
  if (this->numstages == MAX_STAGES) {
    return false;
  }

  deque<INSAMPLE>* pdis;

  // Transfer the highest insignal to the newly created intersignal level
  pdis = new deque<INSAMPLE>(*(insignals[numstages]));
  intersignals.push_back(pdis);

  insignals[numstages].clear();

  pdis = new deque<INSAMPLE>();
  insignals.push_back(pdis);

  // Actually have to clone one of the stages in the system here
  ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>* prws = 
    (stages.back())->clone();

  prws->ClearAllState();
  stages.push_back(prws);

  this->numstages++;
  this->numlevels = this->numstages+1;

  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DynamicReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
AddStage(WaveletType wavetype,
	 unsigned    rate_l,
	 unsigned    rate_h)
{
  if (this->numstages == MAX_STAGES) {
    return false;
  }

  deque<INSAMPLE>* pdis;

  // Transfer the highest insignal to the newly created intersignal level
  pdis = new deque<INSAMPLE>(*(insignals[numstages]));
  intersignals.push_back(pdis);

  insignals[numstages].clear();

  pdis = new deque<INSAMPLE>();
  insignals.push_back(pdis);

  // Actually have to clone one of the stages in the system here
  ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>* prws = 
    new ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>(wavetype,
							    rate_l,
							    rate_h);
  stages.push_back(prws);

  this->numstages++;
  this->numlevels = this->numstages+1;

  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DynamicReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
RemoveStage()
{
  if (this->numstages == 1) {
    return false;
  }

  CHK_DEL(intersignals[numstages-2]);
  intersignals.pop_back();

  CHK_DEL(insignals[numstages]);
  insignals.pop_back();

  CHK_DEL(stages.back());
  stages.pop_back();

  this->numstages--;
  this->numlevels = this->numstages+1;

  return true;
}

/********************************************************************************
 * 
 * Member functions for the ForwardDiscreteWaveletTransform class
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ForwardDiscreteWaveletTransform(WaveletType wavetype=DAUB2, int lowest_outlvl=0) :
  wavecoefs(wavetype)
{
  this->wavetype = wavetype;
  this->lowest_outlvl = lowest_outlvl;
  for (unsigned i=0; i<MAX_STAGES+1; i++) {
    index[i] = 0;
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ForwardDiscreteWaveletTransform(const ForwardDiscreteWaveletTransform &rhs) :
  wavecoefs(rhs.wavetype)
{
  this->wavetype = rhs.wavetype;
  this->lowest_outlvl = rhs.lowest_outlvl;
  for (unsigned i=0; i<MAX_STAGES+1; i++) {
    this->index[i] = rhs.index[i];
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~ForwardDiscreteWaveletTransform()
{}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE> &
ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator=(const ForwardDiscreteWaveletTransform &rhs)
{
  this->wavetype = rhs.wavetype;
  this->wavecoefs = rhs.wavecoefs;
  this->lowest_outlvl = rhs.lowest_outlvl;
  for (unsigned i=0; i<MAX_STAGES+1; i++) {
    this->index[i] = rhs.index[i];
  }
  return *this;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
int ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetLowestOutputLevel() const
{
  return lowest_outlvl;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetIndexNumberOfLevel(int level) const
{
  if (level < MAX_STAGES+1) {
    return index[level];
  } else {
    return 0;
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetIndexNumberOfLevel(int level, unsigned newindex)
{
  if (level < MAX_STAGES+1) {
    index[level] = newindex;
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetLowestOutputLevel(int lowest_outlvl)
{
  this->lowest_outlvl = lowest_outlvl;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
WaveletType ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetWaveletType() const
{
  return wavetype;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeWaveletType(WaveletType wavetype)
{
  bool result=false;
  switch (wavetype) {
  case DAUB2:
  case DAUB4:
  case DAUB6:
  case DAUB8:
  case DAUB10:
  case DAUB12:
  case DAUB14:
  case DAUB16:
  case DAUB18:
  case DAUB20:
    this->wavetype = wavetype;
    wavecoefs.Initialize(wavetype);
    result = true;
    break;
  default:
    break;
  }
  return result;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DiscreteWaveletTransform(SampleBlock<OUTSAMPLE> &outblock,
			 SampleBlock<INSAMPLE>  &inblock)
{
  unsigned M=inblock.GetBlockSize();
  unsigned i, L, bitsum=0, bittest=M;

  // Check if the inblock is a power of 2 and find the value of L
  for (i=0; i<sizeof(unsigned)*8; i++) {
    if ((bittest & 0x1) == 1) {
      L=i;
      bitsum++;
    }
    bittest = bittest >> 1;
  }
  if (bitsum != 1) {
    return false;
  }

  // Clear the output storage
  outblock.ClearBlock();

  vector<double> lpfcoefs, hpfcoefs;
  unsigned N=wavecoefs.GetNumCoefs();
  wavecoefs.GetTransformCoefsLPF(lpfcoefs);
  wavecoefs.GetTransformCoefsHPF(hpfcoefs);

  // Transfer the input block samples to working vector
  vector<SAMPLETYPE> work;
  INSAMPLE insamp;
  for (i=0; i<M; i++) {
    inblock.GetSample(&insamp,i);
    work.push_back(insamp.GetSampleValue());
  }

  vector<SAMPLETYPE> lowout, highout, stageinput;
  unsigned stagebound, j, k;
  for (int l=L; l>0; l--) {

    // Stagebound is log base 2 of loop variable l
    stagebound=1;
    for (i=0; i<l-1; i++) {
      stagebound *= 2;
    }

    lowout.clear(); highout.clear();
    for (i=0; i<stagebound; i++) {
      stageinput.clear();
      for (j=0; j<N; j++) {
	unsigned index = 2*i+j;
	while (index > 2*stagebound-1) {
	  index -= 2*stagebound;
	}
	stageinput.push_back(work[index]);
      }
	
      SAMPLETYPE r1=0, r2=0;
      for (k=0; k<N; k++) {
	r1 += lpfcoefs[k]*stageinput[k];
	r2 += hpfcoefs[k]*stageinput[k];
      }
      lowout.push_back(r1);
      highout.push_back(r2);
    }

    for (i=0; i<2*stagebound; i++) {
      if (i < stagebound) {
	work[i] = lowout[i];
      } else {
	work[i] = highout[i];
      }
    }
  }

  // Transfer the results to the output sample block, add index, and level number
  int level, ring;
  for (i=M-1; i>0; i--) {
    // Find log base 2, cropped at highest bit
    bittest = i;
    for (int l=sizeof(unsigned)*8-1; l>=0; l--) {
      if ((bittest & 0x8000) == 1) {
	ring=l;
	break;
      }
      bittest = bittest << 1;
    }

    level = L - 1 - ring;
    OUTSAMPLE out(work[i], level+lowest_outlvl, index[level]++);
    outblock.PushSampleFront(out);
  }

  // Handle highest level sample
  OUTSAMPLE out(work[0], L+lowest_outlvl, index[L]++);
  outblock.PushSampleFront(out);

  return true;
}

/********************************************************************************
 * 
 * Member functions for the ReverseDiscreteWaveletTransform class
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ReverseDiscreteWaveletTransform(WaveletType wavetype=DAUB2) :
  wavecoefs(wavetype)
{
  this->wavetype = wavetype;
  this->index = 0;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ReverseDiscreteWaveletTransform(const ReverseDiscreteWaveletTransform &rhs) :
  wavecoefs(rhs.wavetype)
{
  this->wavetype = wavetype;
  this->index = 0;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~ReverseDiscreteWaveletTransform()
{}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE> &
ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator=(const ReverseDiscreteWaveletTransform &rhs)
{
  this->wavetype = rhs.wavetype;
  this->wavecoefs = rhs.wavecoefs;
  this->index = rhs.index;
  return *this;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetIndexNumber() const
{
  return index;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetIndexNumber(unsigned newindex)
{
  index = newindex;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
WaveletType ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetWaveletType() const
{
  return this->wavetype;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeWaveletType(WaveletType wavetype)
{
  bool result=false;
  switch (wavetype) {
  case DAUB2:
  case DAUB4:
  case DAUB6:
  case DAUB8:
  case DAUB10:
  case DAUB12:
  case DAUB14:
  case DAUB16:
  case DAUB18:
  case DAUB20:
    this->wavetype = wavetype;
    wavecoefs.Initialize(wavetype);
    result = true;
    break;
  default:
    break;
  }
  return result;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
InverseDiscreteWaveletTransform(SampleBlock<OUTSAMPLE> &outblock,
				SampleBlock<INSAMPLE>  &inblock)
{
  // Check if inblock is a power of 2
  unsigned M = inblock.GetBlockSize();
  unsigned i, L, bitsum=0, bittest=M;
  for (i=0; i<sizeof(unsigned)*8; i++) {
    if ((bittest & 0x1) == 1) {
      L=i;
      bitsum++;
    }
    bittest = bittest >> 1;
  }
  if (bitsum != 1) {
    return false;
  }

  // Clear the output storage
  outblock.ClearBlock();

  vector<double> lowcoefs, highcoefs;
  unsigned N=wavecoefs.GetNumCoefs();
  wavecoefs.GetInverseCoefsLPF(lowcoefs);
  wavecoefs.GetInverseCoefsHPF(highcoefs);

  // For efficiency, create a 2 x N/2 coefficient matrix for high and low
  double lpfcoefs[2][N/2], hpfcoefs[2][N/2];
  for (i=0; i<N/2; i++) {
    lpfcoefs[1][i] = lowcoefs[N-2*i-2];
    lpfcoefs[2][i] = lowcoefs[N-2*i-1];

    hpfcoefs[1][i] = highcoefs[N-2*i-2];
    hpfcoefs[2][i] = highcoefs[N-2*i-1];
  }

  // Transfer the input block samples to work
  vector<SAMPLETYPE> work, tempout(M);
  INSAMPLE insamp;
  for (i=0; i<M; i++) {
    inblock.GetSample(&insamp,i);
    work.push_back(insamp.GetSampleValue());
  }

  tempout[0] = work[0];

  vector<SAMPLETYPE> lowout(M), highout(M), inputh, inputl;
  unsigned stagebound, j, k;
  for (unsigned l=0; l<L; l++) {

    // Stagebound is log base 2 of loop variable l
    stagebound = 1;
    for (i=0; i<l; i++) {
      stagebound *= 2;
    }

    for (i=0; i<stagebound; i++) {
      inputl.clear(), inputh.clear();
      for (j=0; j<N/2; j++) {
	unsigned index = stagebound + i + j + 1 - N/2;
	while (index < stagebound) {
	  index += stagebound;
	}
	inputh.push_back(work[index]);
	inputl.push_back(work[index-stagebound]);
      }

      // Matrix multiply COEFS(2 x N/2) * INPUT(N/2 x 1) = OUT(2 x 1)
      SAMPLETYPE r1=0, r2=0;
      for (j=0; j<2; j++) {
	for (k=0; k<N/2; k++) {
	  r1 += lpfcoefs[j][k]*inputl[k];
	  r2 += hpfcoefs[j][k]*inputh[k];
	}
	lowout[2*i+j-2](r1);
	highout[2*i+j-2](r2);
	r1=0; r2=0;
      }
    }

    // Add the two outputs together
  }
  return true;
}


#endif
