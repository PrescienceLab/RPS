#ifndef _transforms
#define _transforms

#include <vector>

#include "stage.h"
#include "sampleblock.h"

// This is to limit static transforms to 20 stages
#define ALLOWED_MAX_STAGES 20

/********************************************************************************
 *
 * This class performs a streaming wavelet transform, either block by block or 
 *  sample by sample.  The number of stages in the transform is statically set
 *  up upon construction, but can be changed.  Changing the number of stages
 *  resets the entire structure.  The stages in this class are all uniform, in
 *  that they all have the same upsample rate, and filter coefficients.
 *
 *******************************************************************************/
template <class OUTSAMPLE, class INSAMPLE>
class StaticForwardWaveletTransform {
private:
  const unsigned MAX_STAGES=ALLOWED_MAX_STAGES;
  unsigned       numstages;
  int            lowest_outlvl;

  vector<ForwardWaveletStage<OUTSAMPLE,INSAMPLE> *> stages;

public:
  StaticForwardWaveletTransform(unsigned numstages=1, int lowest_outlvl=0);
  StaticForwardWaveletTransform(const StaticForwardWaveletStage &rhs);
  StaticForwardWaveletTransform(unsigned    numstages,
				WaveletType wavetype,
				unsigned    rate_l,
				unsigned    rate_h,
				int         lowest_outlvl);

  virtual ~StaticForwardWaveletTransform();

  StaticForwardWaveletStage & operator=(const StaticForwardWaveletStage &rhs);

  inline unsigned GetNumberStages() const;
  bool ChangeNumberStages(unsigned    numstages);
  bool ChangeNumberStages(unsigned    numstages,
			  WaveletType wavetype,
			  unsigned    rate_l,
			  unsigned    rate_h,
			  int         lowest_outlvl);

  inline int GetLowestOutputLevel() const;
  inline void SetLowestOutputLevel(int lowest_outlvl);

  bool StreamingSampleOperation(OUTSAMPLE &out_l,
				OUTSAMPLE &out_h,
				INSAMPLE  &in);

  unsigned StreamingBlockOperation(SampleBlock<OUTSAMPLE> &out_l,
				   SampleBlock<OUTSAMPLE> &out_h,
				   SampleBlock<INSAMPLE>  &in);

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
template <class OUTSAMPLE, class INSAMPLE>
class StaticReverseWaveletTransform {
private:
  const unsigned MAX_STAGES=ALLOWED_MAX_STAGES;
  unsigned       numstages;

  vector<ReverseWaveletStage<OUTSAMPLE,INSAMPLE> *> stages;

public:
  StaticReverseWaveletTransform(unsigned numstages=1);
  StaticReverseWaveletTransform(const StaticReverseWaveletStage &rhs);
  StaticReverseWaveletTransform(unsigned    numstages,
				WaveletType wavetype,
				unsigned    rate_l,
				unsigned    rate_h);

  virtual ~StaticReverseWaveletTransform();

  StaticReverseWaveletStage & operator=(const StaticReverseWaveletStage &rhs);

  inline unsigned GetNumberStages() const;
  bool ChangeNumberStages(unsigned    numstages);
  bool ChangeNumberStages(unsigned    numstages,
			  WaveletType wavetype,
			  unsigned    rate_l,
			  unsigned    rate_h);

  bool StreamingSampleOperation(OUTSAMPLE &out,
				INSAMPLE  &in_l,
				INSAMPLE  &in_h);

  unsigned StreamingBlockOperation(SampleBlock<OUTSAMPLE> &out,
				   SampleBlock<INSAMPLE>  &in_l,
				   SampleBlock<INSAMPLE>  &in_h);

  ostream & Print(ostream &os) const;
};


class DynamicForwardWaveletTransform {

};

class DynamicReverseWaveletTransform {

};

class ForwardDiscreteWaveletTransform {

};

class ReverseDiscreteWaveletTransform {

};


/********************************************************************************
 * 
 * Member functions for the StaticForwardWaveletTransform class
 *
 *******************************************************************************/
template <class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<OUTSAMPLE, INSAMPLE>::
StaticForwardWaveletTransform(unsigned numstages=1, int lowest_outlvl=0)
{
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    this->numstages = 1;
  } else {
    this->numstages = numstages;
  }
  this->lowest_outlvl = lowest_outlvl;
  
  for (unsigned i=0; i<numstages; i++) {
    ForwardWaveletStage<OUTSAMPLE, INSAMPLE>* pfws = 
      new ForwardWaveletStage<OUTSAMPLE, INSAMPLE>();
    pfws->SetOutputLevelLow(lowest_outlvl);
    pfws->SetOutputLevelHigh(lowest_outlvl+1);
    stages.push_back(pfws);
  }
}

template <class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<OUTSAMPLE, INSAMPLE>::
StaticForwardWaveletTransform(const StaticForwardWaveletStage &rhs) : 
  numstages(rhs.numstages), lowest_outlvl(rhs.lowest_outlvl), stages(rhs.stages)
{
}

template <class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<OUTSAMPLE, INSAMPLE>::
StaticForwardWaveletTransform(unsigned    numstages,
			      WaveletType wavetype,
			      unsigned    rate_l, 
			      unsigned    rate_h,
			      int         lowest_outlvl)
{
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    this->numstages = 1;
  } else {
    this->numstages = numstages;
  }
  this->lowest_outlvl = lowest_outlvl;
  
  for (unsigned i=0; i<numstages; i++) {
    ForwardWaveletStage<OUTSAMPLE, INSAMPLE>* pfws = 
      new ForwardWaveletStage<OUTSAMPLE, INSAMPLE>(wavetype, 
						   rate_l, 
						   rate_h,
						   lowest_outlvl,
						   lowest_outlvl+1);
    stages.push_back(pfws);
  }
}

template <class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<OUTSAMPLE, INSAMPLE>::
~StaticForwardWaveletTransform()
{
  ForwardWaveletStage<OUTSAMPLE, INSAMPLE>* pfws;

  for (unsigned i=0; i<numstages; i++) {
    pfws = stages[i];
    delete pfws;
  }
  stages.clear();
}

template <class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<OUTSAMPLE, INSAMPLE> &
StaticForwardWaveletTransform<OUTSAMPLE, INSAMPLE>::
operator=(const StaticForwardWaveletStage &rhs)
{
  numstages = rhs.numstages;
  lowest_outlvl = rhs.lowest_outlvl;
  stages = rhs.stages;
  return *this;
}

template <class OUTSAMPLE, class INSAMPLE>
unsigned StaticForwardWaveletTransform<OUTSAMPLE, INSAMPLE>::
GetNumberStages() const
{
  return numstages;
}

template <class OUTSAMPLE, class INSAMPLE>
bool StaticForwardWaveletTransform<OUTSAMPLE, INSAMPLE>::
ChangeNumberStages(unsigned numstages)
{
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    return false;
  }
  
  unsigned i;
  ForwardWaveletStage<OUTSAMPLE, INSAMPLE>* pfws;
  for (i=0; i<this->numstages; i++) {
    pfws = stages[i];
    delete pfws;
  }
  stages.clear();

  this->numstages = numstages;

  for (i=0; i<numstages; i++) {
    pfws = new ForwardWaveletStage<OUTSAMPLE, INSAMPLE>();
    pfws->SetOutputLevelLow(lowest_outlvl);
    pfws->SetOutputLevelHigh(lowest_outlvl+1);
    stages.push_back(pfws);
  }
  return true;
}

template <class OUTSAMPLE, class INSAMPLE>
bool StaticForwardWaveletTransform<OUTSAMPLE, INSAMPLE>::
ChangeNumberStages(unsigned    numstages,
		   WaveletType wavetype,
		   unsigned    rate_l,
		   unsigned    rate_h, 
		   int         lowest_outlvl)
{
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    return false;
  }
  
  unsigned i;
  ForwardWaveletStage<OUTSAMPLE, INSAMPLE>* pfws;
  for (i=0; i<this->numstages; i++) {
    pfws = stages[i];
    delete pfws;
  }
  stages.clear();

  this->numstages = numstages;

  for (i=0; i<numstages; i++) {
    pfws = new ForwardWaveletStage<OUTSAMPLE, INSAMPLE>(wavetype,
							rate_l,
							rate_h,
							lowest_outlvl,
							lowest_outlvl+1);
    stages.push_back(pfws);
  }
  return true;
}

template <class OUTSAMPLE, class INSAMPLE>
int StaticForwardWaveletTransform<OUTSAMPLE, INSAMPLE>::
GetLowestOutputLevel() const
{
  return lowest_outlvl;
}

template <class OUTSAMPLE, class INSAMPLE>
void StaticForwardWaveletTransform<OUTSAMPLE, INSAMPLE>::
SetLowestOutputLevel(int lowest_outlvl)
{
  this->lowest_outlvl = lowest_outlvl;
}


template <class OUTSAMPLE, class INSAMPLE>
bool StaticForwardWaveletTransform<OUTSAMPLE, INSAMPLE>::
StreamingSampleOperation(OUTSAMPLE &out_l, OUTSAMPLE &out_h, INSAMPLE  &in)
{
}

template <class OUTSAMPLE, class INSAMPLE>
unsigned StaticForwardWaveletTransform<OUTSAMPLE, INSAMPLE>::
StreamingBlockOperation(SampleBlock<OUTSAMPLE> &out_l,
			SampleBlock<OUTSAMPLE> &out_h,
			SampleBlock<INSAMPLE>  &in)
{
}

template <class OUTSAMPLE, class INSAMPLE>
ostream & StaticForwardWaveletTransform<OUTSAMPLE, INSAMPLE>::
Print(ostream &os) const
{
  os << "Number of stages: " << numstages << endl;
  os << "Level range:      " << lowest_outlvl << " - " << lowest_outlvl+numstages;
  os << endl;

  ForwardWaveletStage<OUTSAMPLE, INSAMPLE>* pfws;
  for (unsigned i=0; i<numstages; i++) {
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
template <class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<OUTSAMPLE, INSAMPLE>::
StaticReverseWaveletTransform(unsigned numstages=1)
{
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    this->numstages = 1;
  } else {
    this->numstages = numstages;
  }

  for (unsigned i=0; i<numstages; i++) {
    ReverseWaveletStage<OUTSAMPLE, INSAMPLE>* prws = 
      new ReverseWaveletStage<OUTSAMPLE, INSAMPLE>();
    stages.push_back(prws);
  }
}

template <class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<OUTSAMPLE, INSAMPLE>::
StaticReverseWaveletTransform(const StaticReverseWaveletStage &rhs) :
  numstages(rhs.numstages), stages(rhs.stages)
{
}

template <class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<OUTSAMPLE, INSAMPLE>::
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
  
  for (unsigned i=0; i<numstages; i++) {
    ReverseWaveletStage<OUTSAMPLE, INSAMPLE>* prws = 
      new ReverseWaveletStage<OUTSAMPLE, INSAMPLE>(wavetype, 
						   rate_l, 
						   rate_h);
    stages.push_back(prws);
  }
}

template <class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<OUTSAMPLE, INSAMPLE>::
~StaticReverseWaveletTransform()
{
  ReverseWaveletStage<OUTSAMPLE, INSAMPLE>* prws;

  for (unsigned i=0; i<numstages; i++) {
    prws = stages[i];
    delete prws;
  }
  stages.clear();
}

template <class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletStage & StaticReverseWaveletTransform<OUTSAMPLE, INSAMPLE>::
operator=(const StaticReverseWaveletStage &rhs)
{
  numstages = rhs.numstages;
  stages = rhs.stages;
  return *this;
}

template <class OUTSAMPLE, class INSAMPLE>
unsigned StaticReverseWaveletTransform<OUTSAMPLE, INSAMPLE>::
GetNumberStages() const
{
  return numstages;
}

template <class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<OUTSAMPLE, INSAMPLE>::
ChangeNumberStages(unsigned numstages)
{
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    return false;
  }
  
  unsigned i;
  ReverseWaveletStage<OUTSAMPLE, INSAMPLE>* prws;
  for (i=0; i<this->numstages; i++) {
    prws = stages[i];
    delete prws;
  }
  stages.clear();

  this->numstages = numstages;

  for (i=0; i<numstages; i++) {
    prws = new ReverseWaveletStage<OUTSAMPLE, INSAMPLE>();
    stages.push_back(prws);
  }
  return true;
}

template <class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<OUTSAMPLE, INSAMPLE>::
ChangeNumberStages(unsigned    numstages,
		   WaveletType wavetype,
		   unsigned    rate_l,
		   unsigned    rate_h)
{
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    return false;
  }
  
  unsigned i;
  ReverseWaveletStage<OUTSAMPLE, INSAMPLE>* prws;
  for (i=0; i<this->numstages; i++) {
    prws = stages[i];
    delete prws;
  }
  stages.clear();

  this->numstages = numstages;

  for (i=0; i<numstages; i++) {
    prws = new ReverseWaveletStage<OUTSAMPLE, INSAMPLE>(wavetype,
							rate_l,
							rate_h);
    stages.push_back(prws);
  }
  return true;
}

template <class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<OUTSAMPLE, INSAMPLE>::
StreamingSampleOperation(OUTSAMPLE &out, INSAMPLE  &in_l, INSAMPLE  &in_h)
{
}

template <class OUTSAMPLE, class INSAMPLE>
unsigned StaticReverseWaveletTransform<OUTSAMPLE, INSAMPLE>::
StreamingBlockOperation(SampleBlock<OUTSAMPLE> &out,
			SampleBlock<INSAMPLE>  &in_l,
			SampleBlock<INSAMPLE>  &in_h)
{
}

template <class OUTSAMPLE, class INSAMPLE>
ostream & StaticReverseWaveletTransform<OUTSAMPLE, INSAMPLE>::
Print(ostream &os) const
{
  os << "Number of stages: " << numstages << endl;

  ReverseWaveletStage<OUTSAMPLE, INSAMPLE>* prws;
  for (unsigned i=0; i<numstages; i++) {
    prws = stages[i];
    os << *prws << endl;
  }
  return os;
}

#endif
