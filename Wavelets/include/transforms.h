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
#include "waveletsampleblock.h"
#include "util.h"

const unsigned BITS_PER_BYTE=8;

struct SignalSpec {
  vector<int> approximations;
  vector<int> details;
};

// Returns true if TRANSFORM, false if ZEROFILL
bool StructureOptimizer(SignalSpec &optim, 
			unsigned &stages,
			const unsigned numstages,
			const int low_level,
			const SignalSpec &spec);

void InvertSignalSpec(vector<int> &inversion,
		      const vector<int> &spec,
		      const unsigned numlevels,
		      const int low_level);

void FlattenSignalSpec(vector<int> &flatspec, const SignalSpec &spec);

// Used in discrete transforms to find number of levels from block length
unsigned NumberOfLevels(const unsigned length);

template <class SAMPLE>
void OutputSamplesToSpec(vector<SAMPLE> &out,
			 const vector<SAMPLE> &in,
			 const vector<int> &spec)
{
  unsigned i, j;
  for (i=0; i<in.size(); i++) {
    int level=in[i].GetSampleLevel();
    for (j=0; j<spec.size(); j++) {
      if (level == spec[j]) {
	out.push_back(in[i]);
	break;
      }
    }
  }
};

template <class SAMPLE>
void OutputBlocksToSpec(vector<WaveletOutputSampleBlock<SAMPLE> > &out,
			const vector<WaveletOutputSampleBlock<SAMPLE> > &in,
			const vector<int> &spec)
{
  unsigned i, j;
  for (i=0; i<in.size(); i++) {
    int level=in[i].GetBlockLevel();
    for (j=0; j<spec.size(); j++) {
      if (level == spec[j]) {
	out.push_back(in[i]);
	break;
      }
    }
  }
};

template <class SAMPLE>
void OutputDWTBlocksToSpec(vector<WaveletOutputSampleBlock<SAMPLE> > &out,
			   const DiscreteWaveletOutputSampleBlock<SAMPLE> &in,
			   const vector<int> &spec)
{
  unsigned i;
  for (i=0; i<spec.size(); i++) {
    deque<SAMPLE> buf;
    in.GetSamplesAtLevel(buf, spec[i]);
    WaveletOutputSampleBlock<SAMPLE> wosb(spec[i]);
    wosb.SetSamples(buf);
    out.push_back(wosb);
    buf.clear();
  }
};

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
  int lowest_outlvl;
  unsigned index_a[MAX_STAGES+1];
  unsigned index_d[MAX_STAGES+1];

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* first_stage;
  vector<ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE> *> stages;

public:
  StaticForwardWaveletTransform(const unsigned numstages=1,
				const int lowest_outlvl=0);
  StaticForwardWaveletTransform(const StaticForwardWaveletTransform &rhs);
  StaticForwardWaveletTransform(const unsigned numstages,
				const WaveletType wavetype,
				const unsigned rate_l,
				const unsigned rate_h,
				const int lowest_outlvl);

  virtual ~StaticForwardWaveletTransform();

  StaticForwardWaveletTransform & operator=
    (const StaticForwardWaveletTransform &rhs);

  inline unsigned GetNumberStages() const;
  bool ChangeNumberStages(const unsigned numstages);
  bool ChangeNumberStages(const unsigned numstages,
			  const WaveletType wavetype,
			  const unsigned rate_l,
			  const unsigned rate_h,
			  const int lowest_outlvl);

  inline int GetLowestOutputLevel() const;
  inline void SetLowestOutputLevel(const int lowest_outlvl);

  inline unsigned GetIndexNumberOfApproxLevel(const int level) const;
  inline unsigned GetIndexNumberOfDetailLevel(const int level) const;
  inline void SetIndexNumberOfApproxLevel(const int level,
					  const unsigned newindex);
  inline void SetIndexNumberOfDetailLevel(const int level,
					  const unsigned newindex);

  // Used to obtain all signals (approx, detail) at all levels in streaming 
  //  operation
  bool StreamingSampleOperation(vector<OUTSAMPLE> &approx_out,
				vector<OUTSAMPLE> &detail_out,
				const Sample<SAMPLETYPE> &in);

  // Used to obtain one approximation level, and numlevels-1 details
  bool StreamingTransformSampleOperation(vector<OUTSAMPLE> &out,
					 const Sample<SAMPLETYPE> &in);

  // Used to obtain all approx signals at all levels in streaming operation
  bool StreamingApproxSampleOperation(vector<OUTSAMPLE> &approx_out,
				      const Sample<SAMPLETYPE> &in);

  // Used to obtain all detail signals at all levels in streaming operation
  bool StreamingDetailSampleOperation(vector<OUTSAMPLE> &detail_out,
				      const Sample<SAMPLETYPE> &in);

  // Used to obtain a mix of approx and detail signals based on SignalSpec
  bool StreamingMixedSampleOperation(vector<OUTSAMPLE> &approx_out,
				     vector<OUTSAMPLE> &detail_out,
				     const Sample<SAMPLETYPE> &in,
				     const SignalSpec &spec);

  // Used to obtain all signals (approx, detail) at all levels in block operation
  unsigned StreamingBlockOperation
    (vector<WaveletOutputSampleBlock<OUTSAMPLE> > &approx_outblock,
     vector<WaveletOutputSampleBlock<OUTSAMPLE> > &detail_outblock,
     const SampleBlock<INSAMPLE> &inblock);

  // Used to obtain one approximation level, and numlevels-1 details
  unsigned StreamingTransformBlockOperation
    (vector<WaveletOutputSampleBlock<OUTSAMPLE> > &outblock,
     const SampleBlock<INSAMPLE> &inblock);

  // Used to obtain all approx signals at all levels in block operation
  unsigned StreamingApproxBlockOperation
    (vector<WaveletOutputSampleBlock<OUTSAMPLE> > &approx_outblock,
     const SampleBlock<INSAMPLE> &inblock);

  // Used to obtain all detail signals at all levels in streaming operation
  unsigned StreamingDetailBlockOperation
    (vector<WaveletOutputSampleBlock<OUTSAMPLE> > &detail_outblock,
     const SampleBlock<INSAMPLE> &inblock);

  // Used to obtain a mix of approx and detail signals based on SignalSpec
  unsigned StreamingMixedBlockOperation
    (vector<WaveletOutputSampleBlock<OUTSAMPLE> > &approx_outblock,
     vector<WaveletOutputSampleBlock<OUTSAMPLE> > &detail_outblock,
     const SampleBlock<INSAMPLE> &inblock,
     const SignalSpec &spec);

  ostream & Print(ostream &os) const;
  ostream & operator<<(ostream &os) const;
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
inline ostream & operator<<
(ostream &os, 
 const StaticForwardWaveletTransform<SAMPLETYPE,OUTSAMPLE,INSAMPLE> &rhs)
{
  return rhs.operator<<(os);
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
  int      lowest_inlvl;
  unsigned index;

  // Zero fill vars
  unsigned indices[MAX_STAGES+1];
  unsigned sampletime;
  bool     sync;
  unsigned sync_level;

  vector<SampleBlock<INSAMPLE> *> insignals;
  vector<SampleBlock<INSAMPLE> *> intersignals;

  vector<ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE> *> stages;
  ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>* last_stage;

  // Protected member functions
  inline void ClearAllDelayLines();
  inline bool SamplePairReady(const SampleBlock<INSAMPLE> &block_l,
			      const SampleBlock<INSAMPLE> &block_h) const;

  inline bool BlockPairReady(const SampleBlock<INSAMPLE> &block_l,
			     const SampleBlock<INSAMPLE> &block_h) const;

  void AddRemainingBlockToInsignals(const SampleBlock<INSAMPLE> &block,
				    const unsigned minsize,
				    const unsigned level);

  void AddBlockToInsignals(const SampleBlock<INSAMPLE> &block,
			   const unsigned level);

  void AddRemainingBlockToIntersignals(const SampleBlock<INSAMPLE> &block,
				       const unsigned minsize,
				       const unsigned level);

  void AddBlockToIntersignals(const SampleBlock<INSAMPLE> &block,
			      const unsigned level);

  void AddZeroSamplesToInput(vector<INSAMPLE> &zeros,
			     const vector<int> &zerolevels);

public:
  StaticReverseWaveletTransform(const unsigned numstages=1,
				const int lowest_inlvl=0);
  StaticReverseWaveletTransform(const StaticReverseWaveletTransform &rhs);
  StaticReverseWaveletTransform(const unsigned numstages,
				const WaveletType wavetype,
				const unsigned rate_l,
				const unsigned rate_h,
				const int lowest_inlvl);

  virtual ~StaticReverseWaveletTransform();

  StaticReverseWaveletTransform & operator=
    (const StaticReverseWaveletTransform &rhs);

  inline unsigned GetNumberStages() const;
  bool ChangeNumberStages(const unsigned numstages);
  bool ChangeNumberStages(const unsigned numstages,
			  const WaveletType wavetype,
			  const unsigned rate_l,
			  const unsigned rate_h,
			  const int lowest_inlvl);

  inline int GetLowestInputLevel() const;
  inline void SetLowestInputLevel(const int lowest_inlvl);

  inline unsigned GetIndexNumber() const;
  inline void SetIndexNumber(const unsigned index);

  inline void ClearIncomingIndices();

  inline unsigned GetSampleTime() const;
  inline void SetSampleTime(const unsigned sampletime);

  inline bool GetSyncStatus() const;
  inline void SetSyncStatus(const bool sync);

  // This routine expects a range of sample levels upon which to reconstruct,
  //  range=[lowest_inlvl,lowest_inlvl+numlevels]
  bool StreamingTransformSampleOperation(vector<OUTSAMPLE> &out,
					 const vector<INSAMPLE> &in);

  // This routine performs a streaming transform, but zero fills missing samples
  //  based on the zerolevels specification.  Assumes that the first call to this
  //  routine comes at a sampletime of sample % 2^numstages in order to synchronize
  //  the zero filling.  This is also the time when we should receive a sample from
  //  each level.
  bool StreamingTransformZeroFillSampleOperation(vector<OUTSAMPLE> &out,
						 const vector<INSAMPLE> &in,
						 const vector<int> &zerolevels);

  // This routine takes a subset of approximations and details, and figures out
  //  the correct combination to reconstruct with
  bool StreamingMixedSampleOperation(vector<OUTSAMPLE> &out,
				     const vector<INSAMPLE> &approx_in,
				     const vector<INSAMPLE> &detail_in,
				     const SignalSpec &spec);

  // This routine expects blocks of sample levels upon which to reconstruct,
  //  range=[lowest_inlvl,lowest_inlvl+numlevels]
  unsigned StreamingTransformBlockOperation
    (SampleBlock<OUTSAMPLE> &outblock,
     const vector<WaveletOutputSampleBlock<INSAMPLE> > &inblock);


  // This routine performs a streaming transform, but zero fills missing samples
  //  based on the zerolevels specification.  Assumes that the first call to this
  //  routine comes at a sampletime of sample % 2^numstages in order to synchronize
  //  the zero filling.  This is also the time when we should receive a sample from
  //  each level.
  unsigned StreamingTransformZeroFillBlockOperation
    (SampleBlock<OUTSAMPLE> &outblock,
     const vector<WaveletOutputSampleBlock<INSAMPLE> > &inblock,
     const vector<int> &zerolevels);

  // This routine takes a subset of approximations and detail blocks, and 
  //  figures out the correct combination to reconstruct with
  unsigned StreamingMixedBlockOperation
    (SampleBlock<OUTSAMPLE> &outblock,
     const vector<WaveletOutputSampleBlock<INSAMPLE> > &approx_block,
     const vector<WaveletOutputSampleBlock<INSAMPLE> > &detail_block,
     const SignalSpec &spec);

  ostream & Print(ostream &os) const;
  ostream & operator<<(ostream &os) const;
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
inline ostream & operator<<
(ostream &os, 
 const StaticReverseWaveletTransform<SAMPLETYPE,OUTSAMPLE,INSAMPLE> &rhs)
{ 
  return rhs.operator<<(os);
};


template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class DynamicForwardWaveletTransform : public 
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE> {
public:
  DynamicForwardWaveletTransform() :
    StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>() {};

  DynamicForwardWaveletTransform(const unsigned numstages=1,
				 const int lowest_outlvl=0) :
    StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(numstages,
								   lowest_outlvl){};

  DynamicForwardWaveletTransform(const unsigned numstages,
				 const WaveletType wavetype,
				 const unsigned rate_l,
				 const unsigned rate_h,
				 const int lowest_outlvl) :
    StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(numstages,
								   wavetype,
								   rate_l,
								   rate_h,
								   lowest_outlvl){};

  DynamicForwardWaveletTransform(const DynamicForwardWaveletTransform &rhs) :
    StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(rhs) {};
  virtual ~DynamicForwardWaveletTransform() {};

  bool AddStage();
  bool AddStage(const WaveletType wavetype,
		const unsigned rate_l,
		const unsigned rate_h);

  bool RemoveStage();

  bool ChangeAllWaveletTypes(const WaveletType wavetype);
  bool ChangeStageWaveletTypes(const WaveletType wavetype,
			       const unsigned stagenum);

  bool ChangeStructure(const unsigned new_numstages, const WaveletType new_wavetype);

  ostream & operator<<(ostream &os) const {
    return (os << "DynamicForwardWaveletTransform...");
  };
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
inline ostream & operator<<
(ostream &os,
 const DynamicForwardWaveletTransform<SAMPLETYPE,OUTSAMPLE,INSAMPLE> &rhs)
{ 
  return rhs.operator<<(os);
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class DynamicReverseWaveletTransform : public 
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE> {
public:
  DynamicReverseWaveletTransform() :
    StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>() {};

  DynamicReverseWaveletTransform(const unsigned numstages=1,
				 const int lowest_inlvl=0) :
    StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(numstages,
								   lowest_inlvl){};

  DynamicReverseWaveletTransform(const unsigned numstages,
				 const WaveletType wavetype,
				 const unsigned rate_l,
				 const unsigned rate_h,
				 const int lowest_inlvl) :
    StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(numstages,
								   wavetype,
								   rate_l,
								   rate_h,
								   lowest_inlvl){};

  DynamicReverseWaveletTransform(const DynamicReverseWaveletTransform &rhs) :
    StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(rhs) {};
  virtual ~DynamicReverseWaveletTransform() {};

  bool AddStage();
  bool AddStage(const WaveletType wavetype,
		const unsigned rate_l,
		const unsigned rate_h);

  bool RemoveStage();

  bool ChangeAllWaveletTypes(const WaveletType wavetype);
  bool ChangeStageWaveletTypes(const WaveletType wavetype,
			       const unsigned stagenum);

  bool ChangeStructure(const unsigned new_numstages, const WaveletType new_wavetype);

  ostream & operator<<(ostream &os) const { 
    return (os << "DynamicReverseWaveletTransform...");
  };
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
inline ostream & operator<<
(ostream &os,
 const DynamicReverseWaveletTransform<SAMPLETYPE,OUTSAMPLE,INSAMPLE> &rhs) 
{ 
  return rhs.operator<<(os);
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class ForwardDiscreteWaveletTransform {
private:
  WaveletType wavetype;
  CQFWaveletCoefficients wavecoefs;

  int lowest_outlvl;
  unsigned index_a[MAX_STAGES+1];
  unsigned index_d[MAX_STAGES+1];

  // functions
  void MultiplyAccumulateVectorsAndScale(vector<SAMPLETYPE> &output,
					 const vector<double> &coefs,
					 const vector<SAMPLETYPE> &input,
					 const double scale);
public:
  ForwardDiscreteWaveletTransform(const WaveletType wavetype=DAUB2,
				  const int lowest_outlvl=0);
  ForwardDiscreteWaveletTransform(const ForwardDiscreteWaveletTransform &rhs);
  virtual ~ForwardDiscreteWaveletTransform();

  ForwardDiscreteWaveletTransform & operator=
    (const ForwardDiscreteWaveletTransform &rhs);

  inline int GetLowestOutputLevel() const;
  inline void SetLowestOutputLevel(const int lowest_outlvl);

  inline unsigned GetIndexNumberOfApproxLevel(const int level) const;
  inline unsigned GetIndexNumberOfDetailLevel(const int level) const;

  inline void SetIndexNumberOfApproxLevel(const int level,
					  const unsigned newindex);
  inline void SetIndexNumberOfDetailLevel(const int level,
					  const unsigned newindex);

  inline WaveletType GetWaveletType() const;
  bool ChangeWaveletType(const WaveletType wavetype);

  // This routine implements the circular wavelet transform based on the work
  //  by Mallat and Strang (See tech report for citations)
  unsigned DiscreteWaveletOperation
    (DiscreteWaveletOutputSampleBlock<OUTSAMPLE> &approxblock,
     DiscreteWaveletOutputSampleBlock<OUTSAMPLE> &detailblock,
     const SampleBlock<INSAMPLE> &inblock);

  unsigned DiscreteWaveletTransformOperation
    (DiscreteWaveletOutputSampleBlock<OUTSAMPLE> &outblock,
     const SampleBlock<INSAMPLE> &inblock);

  unsigned DiscreteWaveletApproxOperation
    (DiscreteWaveletOutputSampleBlock<OUTSAMPLE> &approxblock,
     const SampleBlock<INSAMPLE> &inblock);

  unsigned DiscreteWaveletDetailOperation
    (DiscreteWaveletOutputSampleBlock<OUTSAMPLE> &detailblock,
     const SampleBlock<INSAMPLE> &inblock);

  unsigned DiscreteWaveletMixedOperation
    (vector<WaveletOutputSampleBlock<OUTSAMPLE> > &approxblock,
     vector<WaveletOutputSampleBlock<OUTSAMPLE> > &detailblock,
     const SampleBlock<INSAMPLE> &inblock,
     const SignalSpec &spec);

  ostream & operator<<(ostream &os) const { 
    return (os << "ForwardDiscreteWaveletTransform...");
  };
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
inline ostream & operator<<
(ostream &os,
 const ForwardDiscreteWaveletTransform<SAMPLETYPE,OUTSAMPLE,INSAMPLE> &rhs) 
{ 
  return rhs.operator<<(os);
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class ReverseDiscreteWaveletTransform {
private:
  WaveletType wavetype;
  CQFWaveletCoefficients wavecoefs;
  int lowest_inlvl;

  unsigned index;

  void MultiplyAccumulateMatrixVectorAndScale(vector<SAMPLETYPE> &output,
					      const vector<double *> &filter,
					      const unsigned filterlen,
					      const vector<SAMPLETYPE> &input,
					      const double scale);
  void AddVectors(vector<SAMPLETYPE> &output,
		  const vector<SAMPLETYPE> &highout,
		  const vector<SAMPLETYPE> &lowout);

public:
  ReverseDiscreteWaveletTransform(const WaveletType wavetype=DAUB2,
				  const int lowest_inlvl=0);
  ReverseDiscreteWaveletTransform(const ReverseDiscreteWaveletTransform &rhs);
  virtual ~ReverseDiscreteWaveletTransform();

  ReverseDiscreteWaveletTransform & operator=
    (const ReverseDiscreteWaveletTransform &rhs);

  inline unsigned GetIndexNumber() const;
  inline void SetIndexNumber(const unsigned newindex);

  inline int GetLowestInputLevel() const;
  inline void SetLowestInputLevel(const int lowest_inlvl);

  inline WaveletType GetWaveletType() const;
  bool ChangeWaveletType(const WaveletType wavetype);

  unsigned DiscreteWaveletTransformOperation
    (SampleBlock<OUTSAMPLE> &outblock,
     const DiscreteWaveletOutputSampleBlock<INSAMPLE> &inblock);

  unsigned DiscreteWaveletTransformZeroFillOperation
    (SampleBlock<OUTSAMPLE> &outblock,
     DiscreteWaveletOutputSampleBlock<INSAMPLE> &inblock,
     const vector<int> &zerolevels);

  unsigned DiscreteWaveletMixedOperation
    (SampleBlock<OUTSAMPLE> &outblock,
     const vector<WaveletOutputSampleBlock<INSAMPLE> > &approxblock,
     const vector<WaveletOutputSampleBlock<INSAMPLE> > &detailblock,
     const unsigned numlevels,
     const SignalSpec &spec);

  ostream & operator<<(ostream &os) const {
    return (os << "ReverseDiscreteWaveletTransform...");
  };
};

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
inline ostream & operator<<
(ostream &os,
 const ReverseDiscreteWaveletTransform<SAMPLETYPE,OUTSAMPLE,INSAMPLE> &rhs) 
{ 
  return rhs.operator<<(os);
};

/********************************************************************************
 * 
 * Member functions for the StaticForwardWaveletTransform class
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticForwardWaveletTransform(const unsigned numstages,
			      const int lowest_outlvl)
{
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    this->numstages = 1;
  } else {
    this->numstages = numstages;
  }
  this->numlevels = this->numstages + 1;
  this->lowest_outlvl = lowest_outlvl;

  unsigned i;

  for (i=0; i<numlevels; i++) {
    index_a[i] = 0;
    index_d[i] = 0;
  }

  int outlvl = lowest_outlvl;

  // The lowest stage converts from INSAMPLES to OUTSAMPLES
  first_stage = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
  first_stage->SetOutputLevelHigh(outlvl);
  first_stage->SetOutputLevelLow(outlvl);

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws;
  for (i=0; i<this->numstages-1; i++) {
    outlvl++;
    pfws = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>();
    pfws->SetOutputLevelHigh(outlvl);
    pfws->SetOutputLevelLow(outlvl);
    stages.push_back(pfws);
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticForwardWaveletTransform(const StaticForwardWaveletTransform &rhs) : 
  numstages(rhs.numstages), numlevels(rhs.numlevels),
  lowest_outlvl(rhs.lowest_outlvl)
{
  DEBUG_PRINT("StaticForwardWaveletTransform::StaticForwardWaveletTransform"
	      <<"(const StaticForwardWaveletTransform &rhs)");

  unsigned i;
  first_stage = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>
    (*rhs.first_stage);

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws;
  for (i=0; i<numstages-1; i++) {
    pfws = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>
      (*rhs.stages[i]);
    stages.push_back(pfws);
  }

  for (i=0; i<numlevels; i++) {
    index_a[i] = rhs.index_a[i];
    index_d[i] = rhs.index_d[i];
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticForwardWaveletTransform(const unsigned numstages,
			      const WaveletType wavetype,
			      const unsigned rate_l, 
			      const unsigned rate_h,
			      const int lowest_outlvl)
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
    index_a[i] = 0;
    index_d[i] = 0;
  }

  int outlvl = lowest_outlvl;

  // The lowest stage converts from INSAMPLES to OUTSAMPLES
  first_stage = new 
    ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype,
							 rate_l,
							 rate_h,
							 outlvl,
							 outlvl);

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws;
  for (i=0; i<this->numstages-1; i++) {
    outlvl++;
    pfws = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>(wavetype, 
								     rate_l, 
								     rate_h,
								     outlvl,
								     outlvl);
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
  DEBUG_PRINT("StaticForwardWaveletTransform::operator="
	      <<"(const StaticForwardWaveletTransform &rhs");

  this->~StaticForwardWaveletTransform();
  return *(new (this) StaticForwardWaveletTransform(rhs));
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetNumberStages() const
{
  return numstages;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeNumberStages(const unsigned numstages)
{
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    return false;
  }

  if (this->numstages != numstages) {

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
      index_a[i] = 0;
      index_d[i] = 0;
    }

    int outlvl = lowest_outlvl;

    // The lowest stage converts from INSAMPLES to OUTSAMPLES
    first_stage = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
    first_stage->SetOutputLevelHigh(outlvl);
    first_stage->SetOutputLevelLow(outlvl);

    for (i=0; i<numstages-1; i++) {
      outlvl++;
      pfws = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>();
      pfws->SetOutputLevelHigh(outlvl);
      pfws->SetOutputLevelLow(outlvl);
      stages.push_back(pfws);
    }
  }
  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeNumberStages(const unsigned numstages,
		   const WaveletType wavetype,
		   const unsigned rate_l,
		   const unsigned rate_h, 
		   const int lowest_outlvl)
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
    index_a[i] = 0;
    index_d[i] = 0;
  }

  int outlvl = lowest_outlvl;

  // The lowest stage converts from INSAMPLES to OUTSAMPLES
  first_stage = new 
    ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>(wavetype,
							 rate_l,
							 rate_h,
							 outlvl,
							 outlvl);

  for (i=0; i<numstages-1; i++) {
    outlvl++;
    pfws = new ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>(wavetype,
								     rate_l,
								     rate_h,
								     outlvl,
								     outlvl);
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
SetLowestOutputLevel(const int lowest_outlvl)
{
  this->lowest_outlvl = lowest_outlvl;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetIndexNumberOfApproxLevel(const int level) const
{
  unsigned indx=0;
  int lvl = level - lowest_outlvl;
  if ((lvl >= 0) && (lvl <= MAX_STAGES+1)) {
      indx = index_a[lvl];
  }
  return indx;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetIndexNumberOfDetailLevel(const int level) const
{
  unsigned indx=0;
  int lvl = level - lowest_outlvl;
  if ((lvl >= 0) && (lvl <= MAX_STAGES+1)) {
    indx = index_d[lvl];
  }
  return indx;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetIndexNumberOfApproxLevel(const int level, const unsigned newindex)
{
  int lvl = level - lowest_outlvl;
  if ((lvl >= 0) && (lvl <= MAX_STAGES+1)) {
    index_a[lvl] = newindex;
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetIndexNumberOfDetailLevel(const int level, const unsigned newindex)
{
  int lvl = level - lowest_outlvl;
  if ((lvl >= 0) && (lvl <= MAX_STAGES+1)) {
    index_d[lvl] = newindex;
  }
}

#define SIGNAL_OUTPUT(aout, dout, low, high, level) \
  (high).SetSampleIndex(index_d[(level)]++);        \
  (low).SetSampleIndex(index_a[(level)]++);         \
  (dout).push_back((high));                         \
  (aout).push_back((low));                          \


template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingSampleOperation(vector<OUTSAMPLE> &approx_out,
			 vector<OUTSAMPLE> &detail_out,
			 const Sample<SAMPLETYPE> &in)
{
  bool result = false;
  unsigned i;

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws;
  Sample<SAMPLETYPE> newin;
  OUTSAMPLE out_l, out_h;

  newin = in;

  // Handle the case of just one stage
  if (numstages==1) {
    if (first_stage->PerformSampleOperation(out_l, out_h, newin)) {
      SIGNAL_OUTPUT(approx_out, detail_out, out_l, out_h, 0);
      result = true;
    }
  } else {
    if (first_stage->PerformSampleOperation(out_l, out_h, newin)) {
      SIGNAL_OUTPUT(approx_out, detail_out, out_l, out_h, 0);
      newin.SetSampleValue(out_l.GetSampleValue());
      result = true;

      // Do the rest of the stages
      for (i=0; i<numstages-1; i++) {
	pfws = stages[i];
	if (!pfws->PerformSampleOperation(out_l, out_h, newin)) {
	  break;
	} else {
	  SIGNAL_OUTPUT(approx_out, detail_out, out_l, out_h, i+1);
	  newin.SetSampleValue(out_l.GetSampleValue());
	}
      }
    }
  }
  return result;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingTransformSampleOperation(vector<OUTSAMPLE> &out,
				  const Sample<SAMPLETYPE> &in)
{
  vector<OUTSAMPLE> approx;
  bool result = StreamingSampleOperation(approx, out, in);

  // Copy the last level of approximations into out, and increase the level to
  //  level+1
  if (result) {
    for (unsigned i=0; i<approx.size(); i++) {
      int level = approx[i].GetSampleLevel();
      if (level == lowest_outlvl + (int) numstages - 1) {
	approx[i].SetSampleLevel(lowest_outlvl + numstages);
	out.push_back(approx[i]);
      }
    }
  }
  return result;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingApproxSampleOperation(vector<OUTSAMPLE> &approx_out,
			       const Sample<SAMPLETYPE> &in)
{
  vector<OUTSAMPLE> unused;
  return StreamingSampleOperation(approx_out, unused, in);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingDetailSampleOperation(vector<OUTSAMPLE> &detail_out,
			       const Sample<SAMPLETYPE> &in)
{
  vector<OUTSAMPLE> unused;
  return StreamingSampleOperation(unused, detail_out, in);
}

  // Used to obtain a mix of approx and detail signals based on SignalSpec
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingMixedSampleOperation(vector<OUTSAMPLE> &approx_out,
			      vector<OUTSAMPLE> &detail_out,
			      const Sample<SAMPLETYPE> &in,
			      const SignalSpec &spec)
{
  vector<OUTSAMPLE> aout, dout;
  bool result = StreamingSampleOperation(aout, dout, in);

  OutputSamplesToSpec<OUTSAMPLE>(approx_out, aout, spec.approximations);
  OutputSamplesToSpec<OUTSAMPLE>(detail_out, dout, spec.details);

  return result;
}

#define SIGNAL_BLOCK_OUTPUT(avecout, dvecout, low, high, outlen, level) \
  (high).SetBlockIndex(index_d[(level)]);                               \
  (low).SetBlockIndex(index_a[(level)]);                                \
  index_d[(level)] += (outlen); index_a[(level)] += (outlen);           \
  (dvecout).push_back((high));                                          \
  (avecout).push_back((low));                                           \


template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingBlockOperation
(vector<WaveletOutputSampleBlock<OUTSAMPLE> > &approx_outblock,
 vector<WaveletOutputSampleBlock<OUTSAMPLE> > &detail_outblock,
 const SampleBlock<INSAMPLE> &inblock)
{
  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws;
  WaveletOutputSampleBlock<OUTSAMPLE> out_l, out_h;
  unsigned blocklen=0, i;

  SampleBlock<INSAMPLE> newinput(inblock);

  // Start with the lowest stage
  if (numstages == 1) {
    blocklen = first_stage->PerformBlockOperation(out_l, out_h, newinput);
    SIGNAL_BLOCK_OUTPUT(approx_outblock, detail_outblock, out_l, out_h, 
			blocklen, 0);

  } else {
    blocklen = first_stage->PerformBlockOperation(out_l, out_h, newinput);
    if (blocklen) {
      SIGNAL_BLOCK_OUTPUT(approx_outblock, detail_outblock, out_l, out_h,
			  blocklen, 0);

      for (i=0; i<numstages-1; i++) {
	SampleBlock<OUTSAMPLE> nextinput(out_l);
	pfws = stages[i];
	out_l.ClearBlock(); out_h.ClearBlock();
	unsigned blocksize = pfws->PerformBlockOperation(out_l,
							 out_h,
							 nextinput);
	if (blocksize) {
	  SIGNAL_BLOCK_OUTPUT(approx_outblock, detail_outblock, out_l, out_h,
			      blocksize, i+1);
	  blocklen += blocksize;
	}
      }
    }
  }
  return blocklen;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingTransformBlockOperation
(vector<WaveletOutputSampleBlock<OUTSAMPLE> > &outblock,
 const SampleBlock<INSAMPLE> &inblock)
{
  vector<WaveletOutputSampleBlock<OUTSAMPLE> > approx;
  unsigned blen = StreamingBlockOperation(approx, outblock, inblock);
  if (blen) {
    for (unsigned i=0; i<approx.size(); i++) {
      int block_level = approx[i].GetBlockLevel();
      if (block_level == lowest_outlvl + (int) numstages - 1) {
	approx[i].SetBlockLevel(numstages + lowest_outlvl);
	outblock.push_back(approx[i]);
	break;
      }
    }
  }
  return blen;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingApproxBlockOperation
(vector<WaveletOutputSampleBlock<OUTSAMPLE> > &approx_outblock,
 const SampleBlock<INSAMPLE> &inblock)
{
  vector<WaveletOutputSampleBlock<OUTSAMPLE> > unused;
  return StreamingBlockOperation(approx_outblock, unused, inblock);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingDetailBlockOperation
(vector<WaveletOutputSampleBlock<OUTSAMPLE> > &detail_outblock,
 const SampleBlock<INSAMPLE> &inblock)
{
  vector<WaveletOutputSampleBlock<OUTSAMPLE> > unused;
  return StreamingBlockOperation(unused, detail_outblock, inblock);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingMixedBlockOperation
(vector<WaveletOutputSampleBlock<OUTSAMPLE> > &approx_outblock,
 vector<WaveletOutputSampleBlock<OUTSAMPLE> > &detail_outblock,
 const SampleBlock<INSAMPLE> &inblock,
 const SignalSpec &spec)
{
  vector<WaveletOutputSampleBlock<OUTSAMPLE> > aoutblock, doutblock;
  unsigned blen = StreamingBlockOperation(aoutblock, doutblock, inblock);

  OutputBlocksToSpec<OUTSAMPLE>(approx_outblock, aoutblock, spec.approximations);
  OutputBlocksToSpec<OUTSAMPLE>(detail_outblock, doutblock, spec.details);
  return blen;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
Print(ostream &os) const
{
  os << "Number of stages: " << numstages << endl;
  os << "Level range:      " << "[" << lowest_outlvl 
     << "," << lowest_outlvl+numstages << "]" << endl;
  os << endl;

  os << "First Stage:" << endl;
  first_stage->Print(os);

  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws;
  for (unsigned i=0; i<numstages-1; i++) {
    os << "STAGE " << i << ":" << endl;
    pfws = stages[i];
    pfws->Print(os);
  }
  return os;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & StaticForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator <<(ostream &os) const
{ 
  return Print(os);
}

/********************************************************************************
 * 
 * Member functions for the StaticReverseWaveletTransform class
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticReverseWaveletTransform(const unsigned numstages,
			      const int lowest_inlvl)
{
  DEBUG_PRINT("StaticReverseWaveletTransform::StaticReverseWaveletTransform"
	      <<"(const unsigned numstages, const int lowest_inlvl)");

  unsigned i;
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    this->numstages = 1;
  } else {
    this->numstages = numstages;
  }
  this->numlevels = this->numstages+1;
  this->lowest_inlvl = lowest_inlvl;
  this->index = 0;

  for (i=0; i<MAX_STAGES+1; i++) {
    this->indices[i]=0;
  }
  this->sampletime=0;
  this->sync=false;
  this->sync_level=0;

  for (i=0; i<numlevels; i++) {
    SampleBlock<INSAMPLE>* psbis = new SampleBlock<INSAMPLE>();
    insignals.push_back(psbis);
  }

  for (i=0; i<this->numstages-1; i++) {
    SampleBlock<INSAMPLE>* psbis = new SampleBlock<INSAMPLE>();
    intersignals.push_back(psbis);    
  }

  last_stage = new ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
  for (i=0; i<this->numstages-1; i++) {
    ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>* prws = 
      new ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>();
    stages.push_back(prws);
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticReverseWaveletTransform(const StaticReverseWaveletTransform &rhs) :
  numstages(rhs.numstages), numlevels(rhs.numlevels),
  lowest_inlvl(rhs.lowest_inlvl), index(rhs.index), sampletime(rhs.sampletime),
  sync(rhs.sync), sync_level(rhs.sync_level)
{
  DEBUG_PRINT("StaticReverseWaveletTransform::StaticReverseWaveletTransform"
	      <<"(const StaticReverseWaveletTransform &rhs)");

  unsigned i;
  for (i=0; i<MAX_STAGES+1; i++) {
    this->indices[i]=rhs.indices[i];
  }
  for (i=0; i<rhs.numlevels; i++) {
    SampleBlock<INSAMPLE>* psbis = new SampleBlock<INSAMPLE>(*rhs.insignals[i]);
    insignals.push_back(psbis);
  }

  for (i=0; i<rhs.numstages-1; i++) {
    SampleBlock<INSAMPLE>* psbis = new SampleBlock<INSAMPLE>
      (*rhs.intersignals[i]);
    intersignals.push_back(psbis);    
  }

  for (i=0; i<rhs.numstages-1; i++) {
    ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>* prws = 
      new ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>(*rhs.stages[i]);
    stages.push_back(prws);
  }
  last_stage = new ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>
    (*rhs.last_stage);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StaticReverseWaveletTransform(const unsigned numstages,
			      const WaveletType wavetype,
			      const unsigned rate_l,
			      const unsigned rate_h,
			      const int lowest_inlvl)
{
  DEBUG_PRINT("StaticReverseWaveletTransform::StaticReverseWaveletTransform"
	      <<endl<<"  (const unsigned numstages,"
	      <<endl<<"   const WaveletType wavetype,"
	      <<endl<<"   const unsigned rate_l,"
	      <<endl<<"   const unsigned rate_h,"
	      <<endl<<"   const int lowest_inlvl)");
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    this->numstages = 1;
  } else {
    this->numstages = numstages;
  }
  unsigned i;
  this->numlevels = this->numstages+1;
  this->lowest_inlvl = lowest_inlvl;
  this->index = 0;

  for (i=0; i<MAX_STAGES+1; i++) {
    this->indices[i]=0;
  }
  this->sampletime = 0;
  this->sync=false;
  this->sync_level=0;


  for (i=0; i<numlevels; i++) {
    SampleBlock<INSAMPLE>* psbis = new SampleBlock<INSAMPLE>();
    insignals.push_back(psbis);
  }

  for (i=0; i<this->numstages-1; i++) {
    SampleBlock<INSAMPLE>* psbis = new SampleBlock<INSAMPLE>();
    intersignals.push_back(psbis);    
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
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~StaticReverseWaveletTransform()
{
  DEBUG_PRINT("StaticReverseWaveletTransform::~StaticReverseWaveletTransform()");

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
  DEBUG_PRINT("StaticReverseWaveletTransform::operator="
	      <<"(const StaticReverseWaveletTransform &rhs)");

  this->~StaticReverseWaveletTransform();
  return *(new (this) StaticReverseWaveletTransform(rhs));
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetNumberStages() const
{
  return numstages;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeNumberStages(const unsigned numstages)
{
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    return false;
  }

  if (this->numstages != numstages) {
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
    this->index = 0;

    for (i=0; i<MAX_STAGES+1; i++) {
      this->indices[i]=0;
    }
    this->sampletime=0;
    this->sync=false;
    this->sync_level=0;

    for (i=0; i<numlevels; i++) {
      SampleBlock<INSAMPLE>* psbis = new SampleBlock<INSAMPLE>();
      insignals.push_back(psbis);
    }

    for (i=0; i<numstages-1; i++) {
      SampleBlock<INSAMPLE>* psbis = new SampleBlock<INSAMPLE>();
      intersignals.push_back(psbis);    
    }

    last_stage = ReverseWaveletStage<SAMPLETYPE, OUTSAMPLE, INSAMPLE>();
    for (i=0; i<numstages-1; i++) {
      prws = new ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>();
      stages.push_back(prws);
    }
  }
  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeNumberStages(const unsigned numstages,
		   const WaveletType wavetype,
		   const unsigned rate_l,
		   const unsigned rate_h,
		   const int lowest_inlvl)
{
  if ( (numstages == 0) || (numstages > MAX_STAGES) ) {
    return false;
  }
  
  this->~StaticReverseWaveletTransform();
  this = *(new (this) StaticReverseWaveletTransform(numstages,
						    wavetype,
						    rate_l,
						    rate_h,
						    lowest_inlvl));
  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
int StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetLowestInputLevel() const
{
  return lowest_inlvl;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetLowestInputLevel(const int lowest_inlvl)
{
  this->lowest_inlvl = lowest_inlvl;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetIndexNumber() const
{
  return index;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetIndexNumber(const unsigned index)
{
  this->index = index;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ClearIncomingIndices()
{
  for (unsigned i=0; i<MAX_STAGES+1; i++) {
    this->indices[i]=0;
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetSampleTime() const
{
  return this->sampletime;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetSampleTime(const unsigned sampletime)
{
  this->sampletime=sampletime;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetSyncStatus() const
{
  return this->sync;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetSyncStatus(const bool sync)
{
  this->sync=sync;
}

#define RUN_STAGE_SAMPLE_OPERATION(pstage, output, in_low, in_high) \
  if (SamplePairReady((in_low), (in_high))) {                       \
    (pstage)->PerformSampleOperation((output),                      \
				     (in_low).Back(),               \
				     (in_high).Back());             \
    (in_low).PopSampleBack();                                       \
    (in_high).PopSampleBack();                                      \
  }                                                                 \

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingTransformSampleOperation(vector<OUTSAMPLE> &out,
				  const vector<INSAMPLE> &in)
{
  if (in.size() == 0) {
    return false;
  }

  // Clear the output vector to signal new output to the calling routine
  out.clear();
  vector<INSAMPLE> tempout;

  // Sort the input vector into insignals
  unsigned i, j;
  unsigned samplelevel;
  for (i=0; i<in.size(); i++) {
    samplelevel = in[i].GetSampleLevel() - lowest_inlvl;

    if (samplelevel < numlevels) {
      insignals[samplelevel]->PushSampleFront(in[i]);
    }
  }

  if (numstages == 1) {
    // Handle the only stage
    RUN_STAGE_SAMPLE_OPERATION(last_stage,
    			       out,
    			       *insignals[numstages],
    			       *insignals[numstages-1]);
  } else {
    // Perform the topstage using reverse iteration
    typename vector<ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE> *>::
      reverse_iterator r_iter=stages.rbegin();

    RUN_STAGE_SAMPLE_OPERATION(*r_iter,
			       tempout,
			       *insignals[numstages],
			       *insignals[numstages-1]);
    
    // Store the outputs into the appropriate intermediate outputs
    for (i=0; i<tempout.size(); i++) {
      intersignals[numstages-2]->PushSampleFront(tempout[i]);
    }

    // Perform the rest of the stages
    r_iter++;
    for (i=numstages-2; r_iter != stages.rend(); r_iter++, i--) {
      tempout.clear();
      RUN_STAGE_SAMPLE_OPERATION(*r_iter,
				 tempout,
				 *intersignals[i],
				 *insignals[i]);

      // Store the outputs into the appropriate intermediate outputs
      for (j=0; j<tempout.size(); j++) {
	intersignals[i-1]->PushSampleFront(tempout[j]);
      }
    }

    // Handle the bottom stage
    RUN_STAGE_SAMPLE_OPERATION(last_stage,
			       out,
			       *intersignals[0],
			       *insignals[0]);
  }

  // Set the output indexes
  for (i=0; i<out.size(); i++) {
    out[i].SetSampleIndex(index++);
  }
  return out.size();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingTransformZeroFillSampleOperation(vector<OUTSAMPLE> &out,
					  const vector<INSAMPLE> &in,
					  const vector<int> &zerolevels)
{
  if (in.size() == 0) {
    return false;
  }

  unsigned i, j;
  unsigned curr_sampletime;

  out.clear();
  vector<INSAMPLE> newin(in);

  // If unsynced, sync up and find the reference level
  if (!this->sync) {
    unsigned low_level;
    this->sync_level=(unsigned)(newin[0].GetSampleLevel() - this->lowest_inlvl);

    // Set the sync level to the lowest level in the input
    for (i=1; i<newin.size(); i++) {
      low_level=(unsigned)(newin[i].GetSampleLevel() - this->lowest_inlvl);
      if (low_level < this->sync_level) {
	this->sync_level=low_level;
      }
    }
    curr_sampletime=this->sampletime;
    this->sync=true;
  } else {  // In sync mode

    // Estimate the current sampletime using the sync_level
    curr_sampletime = this->sampletime + 
      (2 << (this->sync_level==numlevels-1 ? this->sync_level-1 : this->sync_level));
  }

  // Run sample operations on sampletimes that are missing.  There must be an
  //  optimization here, but this is easiest solution for now
  vector<OUTSAMPLE> tempout;
  vector<INSAMPLE> zero_samples;
  for (; this->sampletime<curr_sampletime; this->sampletime++) {

    // Add missing samples to input
    AddZeroSamplesToInput(zero_samples, zerolevels);

    if (StreamingTransformSampleOperation(tempout, zero_samples)) {
      for (j=0; j<tempout.size(); j++) {
	out.push_back(tempout[j]);
      }
    }
  }

  // Add missing samples to the samples that arrive with non-zero values
  AddZeroSamplesToInput(newin, zerolevels);

  if (StreamingTransformSampleOperation(tempout, newin)) {
    for (j=0; j<tempout.size(); j++) {
      out.push_back(tempout[j]);
    }
  }

  return out.size();
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingMixedSampleOperation(vector<OUTSAMPLE> &out, 
			      const vector<INSAMPLE> &approx_in,
			      const vector<INSAMPLE> &detail_in,
			      const SignalSpec &spec)
{
  if (spec.approximations.size() == 0 && spec.details.size() == 0) {
    return false;
  }

  unsigned i, j;
  int min_approx;
  vector<INSAMPLE> newin;
  vector<int> levels;
  if (spec.approximations.size() != 0) {
    // Find the minimum approximation level in the signal spec
    min_approx = spec.approximations[0];
    for (i=1; i<spec.approximations.size(); i++) {
      min_approx = MIN(spec.approximations[i], min_approx);
    }

    // Put approximation in the new input with updated level
    for (i=0; i<approx_in.size(); i++) {
      if (min_approx == approx_in[i].GetSampleLevel()) {
	INSAMPLE samp = approx_in[i];
	samp.SetSampleLevel(min_approx+1);
	newin.push_back(samp);
      }
    }
    levels.push_back(min_approx+1);

    // Put appropriate details in new input
    for (i=0; i<detail_in.size(); i++) {
      int level=detail_in[i].GetSampleLevel();
      if (level <= min_approx) {
	levels.push_back(level);
	for (j=0; j<spec.details.size(); j++) {
	  if (level == spec.details[j]) {
	    newin.push_back(detail_in[i]);
	    break;
	  }
	}
      }
    }
  } else {
    // Detail only reconstruction
    OutputSamplesToSpec<INSAMPLE>(newin, detail_in, spec.details);
    levels = spec.details;
  }

  // Build the zero sample specification based on !levels
  vector<int> zerolevels;
  InvertSignalSpec(zerolevels, levels, this->numlevels, this->lowest_inlvl);

  return StreamingTransformZeroFillSampleOperation(out,newin,zerolevels);
}

// Macros for StreamingBlockOperation
#define RUN_STAGE_BLOCK_OPERATION(p_stage, out, low, high)     \
  if (BlockPairReady((low), (high))) {                         \
    (p_stage)->PerformBlockOperation((out), (low), (high));    \
    (low).ClearBlock();                                        \
    (high).ClearBlock();                                       \
  } else {                                                     \
    unsigned l_blksize = (low).GetBlockSize();                 \
    unsigned h_blksize = (high).GetBlockSize();                \
    if (h_blksize < l_blksize) {                               \
      deque<INSAMPLE> newblock;                                \
      (low).GetSamples(newblock, 0, h_blksize);                \
      SampleBlock<INSAMPLE> temp_l(newblock);                  \
      newblock.clear();                                        \
      (high).GetSamples(newblock, 0, h_blksize);               \
      SampleBlock<INSAMPLE> temp_h(newblock);                  \
      newblock.clear();                                        \
      (p_stage)->PerformBlockOperation((out), temp_l, temp_h); \
      (low).RemoveSamplesFront(h_blksize);                     \
      (high).RemoveSamplesFront(h_blksize);                    \
    } else {                                                   \
      deque<INSAMPLE> newblock;                                \
      (low).GetSamples(newblock, 0, l_blksize);                \
      SampleBlock<INSAMPLE> temp_l(newblock);                  \
      newblock.clear();                                        \
      (high).GetSamples(newblock, 0, l_blksize);               \
      SampleBlock<INSAMPLE> temp_h(newblock);                  \
      newblock.clear();                                        \
      (p_stage)->PerformBlockOperation((out), temp_l, temp_h); \
      (low).RemoveSamplesFront(l_blksize);                     \
      (high).RemoveSamplesFront(l_blksize);                    \
    }                                                          \
  }

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingTransformBlockOperation
(SampleBlock<OUTSAMPLE> &outblock,
 const vector<WaveletOutputSampleBlock<INSAMPLE> > &inblock)
{
  if (inblock.size() == 0) {
    return 0;
  }

  unsigned i;
  outblock.ClearBlock();

  SampleBlock<INSAMPLE> in_l, in_h;

  // Sort the input vector into insignals
  for (i=0; i<inblock.size(); i++) {
    unsigned block_level = inblock[i].GetBlockLevel()-lowest_inlvl;

    if (block_level < numlevels) {
      AddBlockToInsignals(inblock[i], block_level);
    }
  }

  if (numstages == 1) {
    RUN_STAGE_BLOCK_OPERATION(last_stage,
			      outblock,
			      *insignals[numstages], 
			      *insignals[numstages-1]);
    outblock.SetBlockIndex(index);
    index += outblock.GetBlockSize();
  } else {
    // Perform the topstage and rest using reverse iteration through the stages
    typename vector<ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE> *>::
      reverse_iterator r_iter=stages.rbegin();

    // A temp output for intermediate results
    SampleBlock<INSAMPLE> interblock;

    RUN_STAGE_BLOCK_OPERATION(*r_iter,
			      interblock,
			      *insignals[numstages],
			      *insignals[numstages-1]);

    // Perform the rest of the stages
    r_iter++;
    for (i=numstages-2; r_iter != stages.rend(); r_iter++, i--) {

      AddBlockToIntersignals(interblock, i);
      interblock.ClearBlock();

      RUN_STAGE_BLOCK_OPERATION(*r_iter,
				interblock, 
				*intersignals[i], 
				*insignals[i]);
    }

    // Handle the last stage
    AddBlockToIntersignals(interblock, 0);
    interblock.ClearBlock();

    RUN_STAGE_BLOCK_OPERATION(last_stage,
			      outblock,
			      *intersignals[0],
			      *insignals[0]);
    outblock.SetBlockIndex(index);
    index += outblock.GetBlockSize();
  }
  return outblock.GetBlockSize();
}

#define CREATE_ZERO_BLOCK(outblock, size, index, level)             \
  deque<INSAMPLE> zerodeque((size));                                \
  WaveletOutputSampleBlock<INSAMPLE> zeroblock(zerodeque, (index)); \
  zeroblock.SetBlockLevel((level));                                 \
  (outblock) = zeroblock;                                           \

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingTransformZeroFillBlockOperation
(SampleBlock<OUTSAMPLE> &outblock,
 const vector<WaveletOutputSampleBlock<INSAMPLE> > &inblock,
 const vector<int> &zerolevels)
{
  if (inblock.size() == 0) {
    return 0;
  }

  unsigned i, j;
  vector<WaveletOutputSampleBlock<INSAMPLE> > newinblock(inblock);

  // If unsynced, sync up and find the reference level
  if (!this->sync) {
    for (i=0; i<newinblock.size(); i++) {
      if (newinblock[i].GetBlockSize()) {
	this->sync_level=
	  (unsigned)(newinblock[i].GetBlockLevel() - this->lowest_inlvl);
	break;
      }
    }
    this->sync=true;    
  }
  unsigned ref_blksize=newinblock[this->sync_level].GetBlockSize();

  // Create missing blocks that are smaller than reference blocksize
  if (this->sync_level > 0) {
    for (i=this->sync_level-1; i==0; i--) {
      ref_blksize *= 2;
      for (j=0; j<zerolevels.size(); j++) {
	if ( (int)(lowest_inlvl + i) == zerolevels[j]) {
	  CREATE_ZERO_BLOCK(newinblock[i],
			    ref_blksize,
			    indices[i],
			    i+lowest_inlvl);
	  break;
	}
      }
    }
  }

  // Block 0 if in zerolevels spec
  ref_blksize *= 2;
  for (i=0, j=0; j<zerolevels.size(); j++) {
    if ( (int)(lowest_inlvl + i) == zerolevels[j]) {
      CREATE_ZERO_BLOCK(newinblock[i],
			ref_blksize,
			indices[i],
			i+lowest_inlvl);
      break;
    }
  }

  // Create missing blocks bigger than reference blocksize
  ref_blksize=newinblock[this->sync_level].GetBlockSize();
  for (i=this->sync_level+1; i<numlevels; i++) {
    for (j=0; j<zerolevels.size(); j++) {
      unsigned round = ref_blksize & 0x1;
      ref_blksize = (i==numlevels-1) ? ref_blksize : ref_blksize >> 1;
      ref_blksize += round;
      if ((ref_blksize) && ( (int)(i + lowest_inlvl) == zerolevels[j])) {
	CREATE_ZERO_BLOCK(newinblock[i],
			  ref_blksize,
			  indices[i],
			  i+lowest_inlvl);
	break;
      }
    }
  }
  // Update the sampletime
  this->sampletime += newinblock[this->sync_level].GetBlockSize() * 
    2 << (this->sync_level==numlevels-1 ?
	  this->sync_level-1 : this->sync_level);

  return StreamingTransformBlockOperation(outblock, newinblock);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingMixedBlockOperation
(SampleBlock<OUTSAMPLE> &outblock,
 const vector<WaveletOutputSampleBlock<INSAMPLE> > &approx_block,
 const vector<WaveletOutputSampleBlock<INSAMPLE> > &detail_block,
 const SignalSpec &spec)
{
  if (spec.approximations.size() == 0 && spec.details.size() == 0) {
    return 0;
  }

  int min_approx;
  unsigned i, j;

  vector<WaveletOutputSampleBlock<INSAMPLE> > newinblock;
  vector<int> levels;
  if (spec.approximations.size() != 0) {
    // Find the minimum approximation level in the signal spec
    min_approx = spec.approximations[0];
    for (i=1; i<spec.approximations.size(); i++) {
      min_approx = MIN(spec.approximations[i], min_approx);
    }

    // Put approximation in the new input block with updated level
    for (i=0; i<approx_block.size(); i++) {
      if (min_approx == approx_block[i].GetBlockLevel()) {
	WaveletOutputSampleBlock<INSAMPLE> wosb = approx_block[i];
	wosb.SetBlockLevel(min_approx+1);
	newinblock.push_back(wosb);
      }
    }
    levels.push_back(min_approx+1);

    // Put appropriate details in new input
    for (i=0; i<detail_block.size(); i++) {
      int level=detail_block[i].GetBlockLevel();
      if (level <= min_approx) {
	for (j=0; j<spec.details.size(); j++) {
	  levels.push_back(level);
	  if (level == spec.details[j]) {
	    newinblock.push_back(detail_block[i]);
	    break;
	  }
	}
      }
    }
  } else {
    // Detail only reconstruction
    OutputBlocksToSpec<INSAMPLE>(newinblock, detail_block, spec.details);
    levels = spec.details;
  }

  // Build the zero sample specification based on !levels
  vector<int> zerolevels;
  InvertSignalSpec(zerolevels, levels, this->numlevels, this->lowest_inlvl);

  return StreamingTransformZeroFillBlockOperation(outblock,
						  newinblock,
						  zerolevels);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
Print(ostream &os) const
{
  os << "Number of stages: " << numstages << endl;
  os << "Level range:      " << "[" << lowest_inlvl
     << "," << lowest_inlvl+numstages << "]" << endl;
  os << endl;

  ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE>* prws;
  for (unsigned i=0; i<numstages-1; i++) {
    os << "STAGE " << i << ":" << endl;
    prws = stages[i];
    prws->Print(os);
  }

  os << "Last Stage:" << endl;
  last_stage->Print(os);
  return os;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator <<(ostream &os) const
{ 
  return Print(os);
};

/*******************************************************************************
 * Private member functions
 *******************************************************************************/
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
SamplePairReady(const SampleBlock<INSAMPLE> &block_l,
		const SampleBlock<INSAMPLE> &block_h) const
{
  return !(block_l.Empty() || block_h.Empty());
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
BlockPairReady(const SampleBlock<INSAMPLE> &block_l, 
	       const SampleBlock<INSAMPLE> &block_h) const
{
  bool result=false;
  if (block_l.GetBlockSize() == block_h.GetBlockSize()) {
    result = true;
  }
  return result;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
AddRemainingBlockToInsignals(const SampleBlock<INSAMPLE> &block, 
			     const unsigned minsize,
			     const unsigned level)
{
  if (level <= numlevels-1) {
    // Transfer samples [minsize, blocksize] into insignals
    INSAMPLE samp;
    for (unsigned i=minsize; i<block.GetBlockSize(); i++) {
      samp = block[i];
      insignals[level]->PushSampleBack(samp);
    }
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
AddBlockToInsignals(const SampleBlock<INSAMPLE> &block, const unsigned level)
{
  AddRemainingBlockToInsignals(block, 0, level);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
AddRemainingBlockToIntersignals(const SampleBlock<INSAMPLE> &block, 
				const unsigned minsize,
				const unsigned level)
{
  if (level <= numlevels-1) {
    // Transfer samples [minsize, blocksize] into intersignals
    INSAMPLE samp;
    for (unsigned i=minsize; i<block.GetBlockSize(); i++) {
      samp = block[i];
      intersignals[level]->PushSampleBack(samp);
    }
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
AddBlockToIntersignals(const SampleBlock<INSAMPLE> &block, const unsigned level)
{
  AddRemainingBlockToIntersignals(block, 0, level);
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void StaticReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
AddZeroSamplesToInput(vector<INSAMPLE> &zeros, const vector<int> &zerolevels)
{
  unsigned i, j;
  for (i=0; i<this->numlevels; i++) {
    for (j=0; j<zerolevels.size(); j++) {
      if ( (i == (unsigned)(zerolevels[j] - this->lowest_inlvl)) && 
	   ((this->sampletime % (2 << (i==this->numlevels-1 ? i-1 : i))) == 0)) {
	// Zero fill a sample
	INSAMPLE zero(0.0, i+this->lowest_inlvl, this->indices[i]);
	zeros.push_back(zero);
	this->indices[i]++;
	break;
      }
    }
  }
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
  index_a[numstages-1] = 0;
  index_d[numstages-1] = 0;

  // Actually have to clone one of the stages in the system here
  ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE>* pfws = 
    (stages.back())->clone();
  pfws->ClearAllState();
  pfws->SetOutputLevelLow(lowest_outlvl+numstages-1);
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
  index_a[numstages-1] = 0;
  index_d[numstages-1] = 0;

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

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DynamicForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeAllWaveletTypes(const WaveletType wavetype)
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
    first_stage->ChangeWaveletType(wavetype);
    ForwardWaveletStage<SAMPLETYPE, OUTSAMPLE, OUTSAMPLE> *pfws;
    for (unsigned i=0; i<stages.size(); i++) {
      pfws = stages[i];
      pfws->ChangeWaveletType(wavetype);
    }
    result = true;
    break;
  default:
    break;
  }
  return result;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DynamicForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeStageWaveletTypes(const WaveletType wavetype, const unsigned stagenum)
{
  bool result=false;

  if (stagenum <= 0) {
    return result;
  }

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
    if (stagenum == 0) {
      first_stage->ChangeWaveletType(wavetype);
    } else {
      stages[stagenum-1]->ChangeWaveletType(wavetype);
    }
    result = true;
    break;
  default:
    break;
  }
  return result;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DynamicForwardWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeStructure(const unsigned new_numstages, const WaveletType new_wavetype)
{
  bool result=false;
  unsigned delta;

  if (new_numstages == this->numstages || new_numstages == 0) {
    return result;
  } else if (this->numstages > new_numstages) {
    // Remove stages
    delta = this->numstages - new_numstages;
    for (unsigned i=0; i<delta; i++) {
      if (!RemoveStage()) {
	return result;
      }
    }
  } else {
    // Add stages
    delta = new_numstages - this->numstages;
    for (unsigned i=0; i<delta; i++) {
      if (!AddStage()) {
	return result;
      }
    }
  }
  result = ChangeAllWaveletTypes(new_wavetype);
  return result;
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

  SampleBlock<INSAMPLE>* psbis;

  // Transfer the highest insignal to the newly created intersignal level
  psbis = new SampleBlock<INSAMPLE>(*insignals[numstages]);
  intersignals.push_back(psbis);

  insignals[numstages]->ClearBlock();

  psbis = new SampleBlock<INSAMPLE>();
  insignals.push_back(psbis);

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

  deque<INSAMPLE>* psbis;

  // Transfer the highest insignal to the newly created intersignal level
  psbis = new SampleBlock<INSAMPLE>(*(insignals[numstages]));
  intersignals.push_back(psbis);

  insignals[numstages].ClearBlock();
  psbis = new SampleBlock<INSAMPLE>();
  insignals.push_back(psbis);

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

  // Remove highest two insignals (one will be added later with different 
  //  contents)
  CHK_DEL(insignals[numstages]);
  CHK_DEL(insignals[numstages-1]);
  insignals.pop_back(); 
  insignals.pop_back();

  // Transfer the current contents of highest intersignal to highest insignal
  SampleBlock<INSAMPLE>* psbis = new 
    SampleBlock<INSAMPLE>(*(intersignals[numstages-2]));
  insignals.push_back(psbis);

  CHK_DEL(intersignals[numstages-2]);
  intersignals.pop_back();

  CHK_DEL(stages.back());
  stages.pop_back();

  this->numstages--;
  this->numlevels = this->numstages+1;
  return true;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DynamicReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeAllWaveletTypes(const WaveletType wavetype)
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
    last_stage->ChangeWaveletType(wavetype);
    ReverseWaveletStage<SAMPLETYPE, INSAMPLE, INSAMPLE> *prws;
    for (unsigned i=0; i<stages.size(); i++) {
      prws = stages[i];
      prws->ChangeWaveletType(wavetype);
    }
    result = true;
    break;
  default:
    break;
  }
  return result;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DynamicReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeStageWaveletTypes(const WaveletType wavetype, const unsigned stagenum)
{
  bool result=false;

  if (stagenum <= 0) {
    return result;
  }

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
    if (stagenum == 0) {
      last_stage->ChangeWaveletType(wavetype);
    } else {
      stages[stagenum-1]->ChangeWaveletType(wavetype);
    }
    result = true;
    break;
  default:
    break;
  }
  return result;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DynamicReverseWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeStructure(const unsigned new_numstages, const WaveletType new_wavetype)
{
  bool result=false;
  unsigned delta;

  if (new_numstages == 0) {
    return result;
  }

  if (this->numstages >= new_numstages) {
    // Remove stages
    delta = this->numstages - new_numstages;
    for (unsigned i=0; i<delta; i++) {
      if (!RemoveStage()) {
	return result;
      }
    }
  } else {
    // Add stages
    delta = new_numstages - this->numstages;
    for (unsigned i=0; i<delta; i++) {
      if (!AddStage()) {
	return result;
      }
    }
  }
  result = ChangeAllWaveletTypes(new_wavetype);
  return result;
}

/********************************************************************************
 * 
 * Member functions for the ForwardDiscreteWaveletTransform class
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ForwardDiscreteWaveletTransform(const WaveletType wavetype,
				const int lowest_outlvl) : 
  wavecoefs(wavetype)
{
  this->wavetype = wavetype;
  this->lowest_outlvl = lowest_outlvl;
  for (unsigned i=0; i<MAX_STAGES+1; i++) {
    index_a[i] = 0;
    index_d[i] = 0;
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
    this->index_a[i] = rhs.index_a[i];
    this->index_d[i] = rhs.index_d[i];
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
    this->index_a[i] = rhs.index_a[i];
    this->index_d[i] = rhs.index_d[i];
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
void ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetLowestOutputLevel(const int lowest_outlvl)
{
  this->lowest_outlvl = lowest_outlvl;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetIndexNumberOfApproxLevel(const int level) const
{
  if ((level >= 0) && (level < MAX_STAGES+1)) {
    return index_a[level];
  } else {
    return 0;
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetIndexNumberOfDetailLevel(const int level) const
{
  if ((level >= 0) && (level < MAX_STAGES+1)) {
    return index_d[level];
  } else {
    return 0;
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetIndexNumberOfApproxLevel(const int level, const unsigned newindex)
{
  if ((level >= 0) && (level < MAX_STAGES+1)) {
    index_a[level] = newindex;
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetIndexNumberOfDetailLevel(const int level, const unsigned newindex)
{
  if ((level >= 0) && (level < MAX_STAGES+1)) {
    index_d[level] = newindex;
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
WaveletType ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetWaveletType() const
{
  return wavetype;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeWaveletType(const WaveletType wavetype)
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
unsigned ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DiscreteWaveletOperation
(DiscreteWaveletOutputSampleBlock<OUTSAMPLE> &approxblock,
 DiscreteWaveletOutputSampleBlock<OUTSAMPLE> &detailblock,
 const SampleBlock<INSAMPLE> &inblock)
{
  unsigned J=NumberOfLevels(inblock.GetBlockSize());
  unsigned lenofblock = 0x1 << J;
  unsigned i, j, k;

  // Create the output matrix used for work
  vector<SAMPLETYPE> a_vector, z_vector;
  INSAMPLE insamp;
  for (i=0; i<lenofblock; i++) {
    insamp = inblock[i];
    a_vector.push_back(insamp.GetSampleValue());
  }

  // Get the coefficients for LPF, HPF
  vector<double> lpfcoefs, hpfcoefs;
  unsigned N=wavecoefs.GetNumCoefs();
  wavecoefs.GetTransformCoefsLPF(lpfcoefs);
  wavecoefs.GetTransformCoefsHPF(hpfcoefs);

  // Main DWT processing
  for (i=J; i>0; i--) {
    unsigned m=0x1 << (i-1);
    vector<SAMPLETYPE> approx, detail;
    approx.clear(); detail.clear();
    for (j=0; j<m; j++) {
      for (k=0, z_vector.clear(); k<N; k++) {
	unsigned index=2*j+k;
	while (index >= 2*m) { index -= 2*m; }
	z_vector.push_back(a_vector[index]);
      }
      // Vector multiply lpf*z_vector and hpf*z_vector with 1/2 scale
      MultiplyAccumulateVectorsAndScale(approx,lpfcoefs,z_vector,1);
      MultiplyAccumulateVectorsAndScale(detail,hpfcoefs,z_vector,1);
    }
    // Place outputs in appropriate output blocks with index and level
    for (j=approx.size(); j>0; j--) {
      OUTSAMPLE a_samp(approx[j-1],J-i+lowest_outlvl,index_a[J-i]+j-1);
      approxblock.PushSampleFront(a_samp);
    }
    index_a[J-i] += approx.size();

    for (j=detail.size(); j>0; j--) {
      OUTSAMPLE d_samp(detail[j-1],J-i+lowest_outlvl,index_d[J-i]+j-1);
      detailblock.PushSampleFront(d_samp);
    }
    index_d[J-i] += detail.size();

    for (j=0; j<2*m; j++) {
      a_vector[j] = (j<m) ? approx[j] : detail[j];
    }
  }
  return lenofblock;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DiscreteWaveletTransformOperation
(DiscreteWaveletOutputSampleBlock<OUTSAMPLE> &outblock,
 const SampleBlock<INSAMPLE> &inblock)
{
  unsigned J=NumberOfLevels(inblock.GetBlockSize());
  DiscreteWaveletOutputSampleBlock<OUTSAMPLE> approxblock(J, lowest_outlvl, APPROX);
  DiscreteWaveletOutputSampleBlock<OUTSAMPLE> detailblock(J, lowest_outlvl, DETAIL);
  unsigned lenofblock=DiscreteWaveletOperation(approxblock,detailblock,inblock);

  outblock = detailblock;

  // Overwrite the approximation sample and change the level
  OUTSAMPLE approxsample=approxblock[0];
  approxsample.SetSampleLevel(J+lowest_outlvl);
  outblock.PushSampleFront(approxsample);
  outblock.SetNumberLevels(J+1);
  outblock.SetTransformType(TRANSFORM);
  return lenofblock;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DiscreteWaveletApproxOperation
(DiscreteWaveletOutputSampleBlock<OUTSAMPLE> &approxblock,
 const SampleBlock<INSAMPLE> &inblock)
{
  unsigned J=NumberOfLevels(inblock.GetBlockSize());
  DiscreteWaveletOutputSampleBlock<OUTSAMPLE> detailblock(J, lowest_outlvl,DETAIL);
  approxblock.SetNumberLevels(J);
  unsigned lenofblock=DiscreteWaveletOperation(approxblock,detailblock,inblock);
  return lenofblock;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DiscreteWaveletDetailOperation
(DiscreteWaveletOutputSampleBlock<OUTSAMPLE> &detailblock,
 const SampleBlock<INSAMPLE> &inblock)
{
  unsigned J=NumberOfLevels(inblock.GetBlockSize());
  DiscreteWaveletOutputSampleBlock<OUTSAMPLE> approxblock(J, lowest_outlvl, APPROX);
  detailblock.SetNumberLevels(J);
  unsigned lenofblock=DiscreteWaveletOperation(approxblock,detailblock,inblock);
  return lenofblock;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DiscreteWaveletMixedOperation
(vector<WaveletOutputSampleBlock<OUTSAMPLE> > &approxblock,
 vector<WaveletOutputSampleBlock<OUTSAMPLE> > &detailblock,
 const SampleBlock<INSAMPLE> &inblock,
 const SignalSpec &spec)
{
  unsigned J=NumberOfLevels(inblock.GetBlockSize());
  DiscreteWaveletOutputSampleBlock<OUTSAMPLE> ablock(J, lowest_outlvl, APPROX);
  DiscreteWaveletOutputSampleBlock<OUTSAMPLE> dblock(J, lowest_outlvl, DETAIL);
  DiscreteWaveletOperation(ablock,dblock,inblock);

  OutputDWTBlocksToSpec<OUTSAMPLE>(approxblock,
				   ablock,
				   spec.approximations);
  OutputDWTBlocksToSpec<OUTSAMPLE>(detailblock,
				   dblock,
				   spec.details);

  return J;
}

/********************************************************************************
 * Private member functions
 *******************************************************************************/

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ForwardDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
MultiplyAccumulateVectorsAndScale(vector<SAMPLETYPE> &output,
				  const vector<double> &coefs,
				  const vector<SAMPLETYPE> &input,
				  const double scale)
{
  SAMPLETYPE acc=0;
  unsigned macs=coefs.size();
  for (unsigned i=0; i<macs; i++) {
    acc += coefs[macs-i-1]*input[i];
  }
  acc *= scale;
  output.push_back(acc);
}

/********************************************************************************
 * 
 * Member functions for the ReverseDiscreteWaveletTransform class
 *
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ReverseDiscreteWaveletTransform(const WaveletType wavetype,
				const int lowest_inlvl) :
  wavecoefs(wavetype)
{
  this->wavetype = wavetype;
  this->lowest_inlvl = lowest_inlvl;
  this->index = 0;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ReverseDiscreteWaveletTransform(const ReverseDiscreteWaveletTransform &rhs) :
  wavecoefs(rhs.wavetype)
{
  this->wavetype = rhs.wavetype;
  this->lowest_inlvl = rhs.lowest_inlvl;
  this->index = rhs.index;
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
  this->lowest_inlvl = rhs.lowest_inlvl;
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
SetIndexNumber(const unsigned newindex)
{
  index = newindex;
}


template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
int ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetLowestInputLevel() const
{
  return this->lowest_inlvl;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetLowestInputLevel(const int lowest_inlvl)
{
  this->lowest_inlvl = lowest_inlvl;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
WaveletType ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetWaveletType() const
{
  return this->wavetype;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeWaveletType(const WaveletType wavetype)
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
unsigned ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DiscreteWaveletTransformOperation
(SampleBlock<OUTSAMPLE> &outblock,
 const DiscreteWaveletOutputSampleBlock<INSAMPLE> &inblock)
{
  unsigned J=NumberOfLevels(inblock.GetBlockSize());
  unsigned lenofblock = 0x1 << J;
  unsigned i, j, k;

  // Create the output matrices used for work
  vector<SAMPLETYPE> f_vector, z_vector, zz_vector;
  for (i=0; i<inblock.GetBlockSize(); i++) {
    f_vector.push_back(0);
  }
  f_vector[0] = inblock[0].GetSampleValue();

  // Get the coefficients for LPF, HPF
  vector<double> lpfcoefs, hpfcoefs;
  unsigned N=wavecoefs.GetNumCoefs();
  wavecoefs.GetTransformCoefsLPF(lpfcoefs);
  wavecoefs.GetTransformCoefsHPF(hpfcoefs);

  // Place coefficients in streamlined data structure
  const unsigned NUMROWS=2;
  vector<double *> lpf, hpf;
  for (i=0; i<NUMROWS; i++) {
    double *pd_lpf=new double[N/2];
    double *pd_hpf=new double[N/2];
    lpf.push_back(pd_lpf); hpf.push_back(pd_hpf);
  }

  for (j=0; j<N/2; j++) {
    (lpf[0])[j]=lpfcoefs[2*j+1];
    (lpf[1])[j]=lpfcoefs[2*j];
    (hpf[0])[j]=hpfcoefs[2*j+1];
    (hpf[1])[j]=hpfcoefs[2*j];
  }

  // Main IDWT processing loop
  vector<SAMPLETYPE> outh, outl;
  for (i=1; i<=J; i++) {
    unsigned m=0x1 << (i-1);
    for (j=0; j<m; j++) {
      for (k=0, z_vector.clear(), zz_vector.clear(); k<N/2; k++) {
	int index=m+j-N/2+k+1;
	while (index < (int)m) { index += m; }
	z_vector.push_back(inblock[index].GetSampleValue());
	zz_vector.push_back(f_vector[index-m]);
      }
      MultiplyAccumulateMatrixVectorAndScale(outh, hpf, N/2, z_vector, 2.0);
      MultiplyAccumulateMatrixVectorAndScale(outl, lpf, N/2, zz_vector, 2.0);
    }
    AddVectors(f_vector, outh, outl);
    outh.clear(); outl.clear();
  }

  // Transfer result to outblock
  for (i=0; i<f_vector.size(); i++) {
    outblock.PushSampleBack(OUTSAMPLE(f_vector[i], index++));
  }

  for (i=0; i<NUMROWS; i++) {
    CHK_DEL(lpf[i]);
    CHK_DEL(hpf[i]);
  }

  return lenofblock;
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DiscreteWaveletTransformZeroFillOperation
(SampleBlock<OUTSAMPLE> &outblock,
 DiscreteWaveletOutputSampleBlock<INSAMPLE> &inblock,
 const vector<int> &zerolevels)
{
  unsigned numlevels=inblock.GetNumberLevels();
  for (unsigned i=0; i<zerolevels.size(); i++) {
    unsigned lvl_index = zerolevels[i] - this->lowest_inlvl;
    unsigned lvl_size = (lvl_index==numlevels-1) ? 
      (0x1 << (numlevels - lvl_index - 2)) :
      (0x1);
    deque<INSAMPLE> zeros(lvl_size,INSAMPLE(0,0));
    inblock.SetSamplesAtLevel(zeros, zerolevels[i]);
  }

  this->DiscreteWaveletTransformOperation(outblock, inblock);

  return outblock.GetBlockSize();
}

// Function assumes that the structure optimizer was run before calling
// this routine, and there is therefore 0 or 1 approximation levels
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DiscreteWaveletMixedOperation
(SampleBlock<OUTSAMPLE> &outblock,
 const vector<WaveletOutputSampleBlock<INSAMPLE> > &approxblock,
 const vector<WaveletOutputSampleBlock<INSAMPLE> > &detailblock,
 const unsigned numlevels,
 const SignalSpec &spec)
{
  unsigned blocksize=0x1 << (numlevels-1);

  // Zero out a DiscreteWaveletOutputSampleBlock
  deque<INSAMPLE> zeros(blocksize,INSAMPLE(0,0));
  DiscreteWaveletOutputSampleBlock<INSAMPLE> dwosb(numlevels,
						   this->lowest_inlvl,
						   TRANSFORM);
  dwosb.SetSamples(zeros);

  // Place approximations and details into the dwosb block
  deque<INSAMPLE> samps;
  if (approxblock.size()) {
    int level=approxblock[0].GetBlockLevel();
    approxblock[0].GetSamples(samps);
    dwosb.SetSamplesAtLevel(samps,level);
  }

  samps.clear();
  for (unsigned i=0; i<detailblock.size(); i++, samps.clear()) {
    if (detailblock[i].GetBlockSize()) {
      int level=detailblock[i].GetBlockLevel();
      detailblock[i].GetSamples(samps);
      dwosb.SetSamplesAtLevel(samps,level);
    }
  }

  this->DiscreteWaveletTransformOperation(outblock, dwosb);

  return blocksize;
}

/*******************************************************************************
 * Private member functions
 *******************************************************************************/
template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
MultiplyAccumulateMatrixVectorAndScale(vector<SAMPLETYPE> &output,
				       const vector<double *> &filter,
				       const unsigned filterlen,
				       const vector<SAMPLETYPE> &input,
				       const double scale)
{
  for (unsigned i=0; i<filter.size(); i++) {
    SAMPLETYPE acc=0;
    for (unsigned j=0; j<filterlen; j++) {
      acc += (filter[i])[j]*input[j];
    }
    acc *= scale;
    output.push_back(acc);
  }
}

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void ReverseDiscreteWaveletTransform<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
AddVectors(vector<SAMPLETYPE> &output,
	   const vector<SAMPLETYPE> &highout,
	   const vector<SAMPLETYPE> &lowout)
{
  unsigned i;
  for (i=0; i<MIN(highout.size(), lowout.size()); i++) {
    output[i] = highout[i] + lowout[i];
  }
}

#endif
