#ifndef _transforms
#define _transforms

#include <vector>
#include <deque>

#include "stage.h"
#include "sample.h"
#include "sampleblock.h"
#include "util.h"

// This is to limit static transforms to 20 stages
const unsigned MAX_STAGES = 20;

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

  vector<deque<OUTSAMPLE> *>      outsamples;
  vector<SampleBlock<OUTSAMPLE> *> outblocks;

  vector<ForwardWaveletStage<SAMPLETYPE,OUTSAMPLE,INSAMPLE> *> stages;

public:
  StaticForwardWaveletTransform(unsigned numstages=1, int lowest_outlvl=0);
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

  inline unsigned GetIndexNumberOfLevel(int level);
  inline void SetIndexNumberOfLevel(int level, unsigned newindex);

  bool StreamInSample(INSAMPLE &in);
  bool StreamingSampleOperation(vector<OUTSAMPLE> &out, INSAMPLE &in);

  unsigned StreamInBlock(SampleBlock<INSAMPLE> &inblock);
  unsigned StreamingBlockOperation(vector<SampleBlock<OUTSAMPLE> *> &outblock,
				   SampleBlock<INSAMPLE>            &inblock);

  void GetOutputSamples(vector<OUTSAMPLE> &samplebuf);
  void GetOutputBlocks(vector<SampleBlock<OUTSAMPLE> *> &outblock);

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
private:
  unsigned       numstages;

  vector<OUTSAMPLE> outsignal;
  vector<ReverseWaveletStage<SAMPLETYPE,OUTSAMPLE,INSAMPLE> *> stages;

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

  bool StreamInSamples(vector<INSAMPLE> &in);
  bool StreamingSampleOperation(OUTSAMPLE &out, vector<INSAMPLE> &in);

  unsigned StreamInBlock(vector<SampleBlock<INSAMPLE> *> &inblock);
  unsigned StreamingBlockOperation(SampleBlock<OUTSAMPLE>          &outblock,
				   vector<SampleBlock<INSAMPLE> *> &inblock);

  void GetOutputSamples(vector<OUTSAMPLE> &samplebuf);
  void GetOutputBlocks(SampleBlock<OUTSAMPLE> &outblock);

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
StaticForwardWaveletTransform(unsigned numstages=1, int lowest_outlvl=0)
{
  if ( (numstages <= 0) || (numstages > MAX_STAGES) ) {
    this->numstages = 1;
  } else {
    this->numstages = numstages;
  }
  this->numlevels = numstages + 1;
  this->lowest_outlvl = lowest_outlvl;
  
  int outlvl = lowest_outlvl;
  unsigned i;
  for (i=0; i<numstages-1; i++) {
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

  for (i=0; i<numlevels; i++) {
    deque<OUTSAMPLE>* pdos = new deque<OUTSAMPLE>();
    SampleBlock<OUTSAMPLE>* psbo = new SampleBlock<OUTSAMPLE>();
    outsamples.push_back(pvos);
    outblocks.push_back(psbo);
    index[i] = 0;
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticForwardWaveletTransform(const StaticForwardWaveletTransform &rhs) : 
  numstages(rhs.numstages), numlevels(rhs.numlevels), 
  lowest_outlvl(rhs.lowest_outlvl), outsamples(rhs.outsamples), 
  outblocks(rhs.outblocks), stages(rhs.stages)
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
  if ( (numstages <= 0) || (numstages > MAX_STAGES) ) {
    this->numstages = 1;
  } else {
    this->numstages = numstages;
  }
  this->numlevels = numstages + 1;
  this->lowest_outlvl = lowest_outlvl;
  
  int outlvl = lowest_outlvl;
  unsigned i;
  for (i=0; i<numstages-1; i++) {
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

  for (i=0; i<numlevels; i++) {
    deque<OUTSAMPLE>* pdos = new deque<OUTSAMPLE>();
    SampleBlock<OUTSAMPLE>* psbo = new SampleBlock<OUTSAMPLE>();
    outsamples.push_back(pdos);
    outblocks.push_back(psbo);
    index[i] = 0;
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~StaticForwardWaveletTransform()
{
  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfws;
  deque<OUTSAMPLE>* pdos;
  SampleBlock<OUTSAMPLE>* psbo;

  unsigned i;
  for (i=0; i<numstages; i++) {
    pfws = stages[i]; delete pfws;
  }
  for (i=0; i<numlevels; i++) {
    pdos = outsamples[i]; delete pdos;
    psbo = outblocks[i]; delete psbo;
  }
  stages.clear();
  outsamples.clear();
  outblocks.clear();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE> &
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator=(const StaticForwardWaveletTransform &rhs)
{
  numstages = rhs.numstages;
  numlevels = rhs.numlevels;
  lowest_outlvl = rhs.lowest_outlvl;
  stages = rhs.stages;
  outsamples = rhs.outsamples;
  outblocks = rhs.outblocks;

  for (unsigned i=0; i<numlevels; i++) {
    index[i] = rhs.index[i];
  }
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
  deque<OUTSAMPLE>* pdos;
  SampleBlock<OUTSAMPLE>* psbo;

  unsigned i;
  for (i=0; i<this->numstages; i++) {
    pfws = stages[i]; delete pfws;
  }
  for (i=0; i<this->numlevels; i++) {
    pdos = outsamples[i]; delete pdos;
    psbo = outblocks[i]; delete psbo;
  }
  stages.clear();
  outsamples.clear();
  outblocks.clear();

  this->numstages = numstages;
  this->numlevels = numstages + 1;

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

  for (i=0; i<numlevels; i++) {
    pdos = new deque<OUTSAMPLE>();
    psbo = new SampleBlock<OUTSAMPLE>;
    outsamples.push_back(pdos);
    outblocks.push_back(psbo);
    index[i] = 0;
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
  if ( (numstages <= 0) || (numstages > MAX_STAGES) ) {
    return false;
  }
  
  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* pfws;
  deque<OUTSAMPLE>* pdos;
  SampleBlock<OUTSAMPLE>* psbo;

  unsigned i;
  for (i=0; i<this->numstages; i++) {
    pfws = stages[i]; delete pfws;
  }
  for (i=0; i<this->numlevels; i++) {
    pdos = outsamples[i]; delete pdos;
    psbo = outblocks[i]; delete psbo;
  }
  stages.clear();
  outsamples.clear();
  outblocks.clear();

  this->numstages = numstages;
  this->numlevels = numstages + 1;

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

  for (i=0; i<numlevels; i++) {
    pdos = new deque<OUTSAMPLE>();
    psbo = new SampleBlock<OUTSAMPLE>();
    outsamples.push_back(pdos);
    outblocks.push_back(psbo);
    index[i] = 0;
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
GetIndexNumberOfLevel(int level)
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
StreamInSample(INSAMPLE &in)
{
  bool result = false;
  ForwardWaveletStage<SAMPLETYPE,OUTSAMPLE,INSAMPLE>* pfws;
  deque<OUTSAMPLE>* pdos;
  OUTSAMPLE out_l, out_h;

  for (unsigned i=0; i<numstages; i++) {
    pfws = stages[i];
    pdos = outsamples[i];

    if (!pfws->PerformSampleOperation(out_l, out_h, in)) {
      break;
    } else {
      out_h.SetSampleIndex(index[i]++);
      pdos->push_back(out_h);
      in.SetSampleValue(out_l.GetSampleValue());
      if (i == numstages-1) {
	out_l.SetSampleIndex(index[i+1]++);
	pdos->push_back(out_l);
      }
      result = true;
    }
  }
  return result;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingSampleOperation(vector<OUTSAMPLE> &out, INSAMPLE &in)
{
  bool result = false;
  ForwardWaveletStage<SAMPLETYPE,OUTSAMPLE,INSAMPLE>* pfws;
  OUTSAMPLE out_l, out_h;

  unsigned i;
  for (i=0; i<numstages; i++) {
    pfws = stages[i];

    if (!pfws->PerformSampleOperation(out_l, out_h, in)) {
      break;
    } else {
      out_h.SetSampleIndex(index[i]++);
      out.push_back(out_h);
      in.SetSampleValue(out_l.GetSampleValue());
      if (i == numstages-1) {
	// Copy the low output
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
StreamInBlock(SampleBlock<INSAMPLE> &inblock)
{
  ForwardWaveletStage<SAMPLETYPE,OUTSAMPLE,INSAMPLE>* pfws;
  SampleBlock<OUTSAMPLE> out_l, out_h;
  SampleBlock<OUTSAMPLE>* psbo;

  unsigned blocklen;
  pfws = stages[0];
  psbo = outblocks[0];
  blocklen = pfws->PerformBlockOperation(out_l, out_h, inblock);
  *psbo->AppendBlock(out_h);

  for (unsigned i=1; i<numstages; i++) {
    SampleBlock<OUTSAMPLE> newinput(out_l);
    pfws = stages[i];
    psbo = outblocks[i];

    blocklen += pfws->PerformBlockOperation(out_l, out_h, newinput);
    *psbo->AppendBlock(out_h);
    if (i == numstages-1) {
      psbo = outblocks[i+1];
      *psbo->AppendBlock(out_l);
      break;
    }
  } 
  return blocklen;
}


template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingBlockOperation(vector<SampleBlock<OUTSAMPLE> *> &outblock,
			SampleBlock<INSAMPLE>            &inblock)
{
  ForwardWaveletStage<SAMPLETYPE,OUTSAMPLE,INSAMPLE>* pfws;
  SampleBlock<OUTSAMPLE> out_l, out_h;
  SampleBlock<OUTSAMPLE>* psbo;

  unsigned blocklen;
  pfws = stages[0];
  psbo = outblock[0];
  blocklen = pfws->PerformBlockOperation(out_l, out_h, inblock);
  *psbo->AppendBlock(out_h);

  for (unsigned i=1; i<numstages; i++) {
    SampleBlock<OUTSAMPLE> newinput(out_l);
    pfws = stages[i];
    psbo = outblock[i];

    blocklen += pfws->PerformBlockOperation(out_l, out_h, newinput);
    *psbo->AppendBlock(out_h);
    if (i == numstages-1) {
      psbo = outblocks[i+1];
      *psbo->AppendBlock(out_l);
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
  for (unsigned i=0; i<=numstages; i++) {
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

  unsigned i;
  for (i=0; i<=numstages; i++) {
    ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* prws = 
      new ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
    stages.push_back(prws);
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticReverseWaveletTransform(const StaticReverseWaveletTransform &rhs) :
  numstages(rhs.numstages), outsignal(rhs.outsignal), stages(rhs.stages)
{
}

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
  
  for (unsigned i=0; i<=numstages; i++) {
    ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* prws = 
      new ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype, 
							       rate_l, 
							       rate_h);
    stages.push_back(prws);
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~StaticReverseWaveletTransform()
{
  ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* prws;

  for (unsigned i=0; i<=numstages; i++) {
    prws = stages[i];
    delete prws;
  }
  stages.clear();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE> & 
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator=(const StaticReverseWaveletTransform &rhs)
{
  numstages = rhs.numstages;
  stages = rhs.stages;
  outsignal = rhs.outsignal;
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
  ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* prws;
  for (i=0; i<=this->numstages; i++) {
    prws = stages[i];
    delete prws;
  }
  stages.clear();

  this->numstages = numstages;

  for (i=0; i<=numstages; i++) {
    prws = new ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
    stages.push_back(prws);
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
  ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* prws;
  for (i=0; i<=this->numstages; i++) {
    prws = stages[i];
    delete prws;
  }
  stages.clear();

  this->numstages = numstages;

  for (i=0; i<=numstages; i++) {
    prws = new ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype,
								    rate_l,
								    rate_h);
    stages.push_back(prws);
  }
  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamInSamples(vector<INSAMPLE> &in)
{
  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingSampleOperation(OUTSAMPLE &out, vector<INSAMPLE> &in)
{
  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamInBlock(vector<SampleBlock<INSAMPLE> *> &inblock)
{
  return 5;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingBlockOperation(SampleBlock<OUTSAMPLE>           &outblock,
			vector<SampleBlock<INSAMPLE> *>  &inblock)
{
  return 5;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
Print(ostream &os) const
{
  os << "Number of stages: " << numstages << endl;

  ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* prws;
  for (unsigned i=0; i<=numstages; i++) {
    os << "STAGE " << i << ":" << endl;
    prws = stages[i];
    os << *prws << endl;
  }
  return os;
}

#endif
