#ifndef _jitter
#define _jitter

#include <list>
#include <vector>

#include "sample.h"
#include "sampleblock.h"
#include "waveletsampleblock.h"
#include "util.h"

const unsigned DEFAULT_BACKLOG_THRESH=5;

/********************************************************************************
 * class:   JitterProtectStream
 * purpose: This class provides jitter protection for a stream of samples typed
 *  by INSAMPLE.  The incoming samples must have an index in order to determine
 *  ordering.  The JITTERACTION class template argument is a class that must
 *  contain a member function named JitterAction.  A default JITTERACTION class
 *  is in this file, and it simply zero fills missing indices.
 *******************************************************************************/
template <class INSAMPLE, class JITTERACTION>
class JitterProtectStream {
private:
  unsigned backlog_thresh;

  bool     curr_index_init;
  unsigned curr_index;

  list<INSAMPLE> jitter_buffer;

  inline bool NextIndex(const unsigned index);
  void RemoveFromJitterBufferIntoOutput(list<INSAMPLE> &out);
  void InsertOrderedIntoJitterBuffer(const INSAMPLE &in);

public:
  JitterProtectStream(const unsigned backlog_thresh=DEFAULT_BACKLOG_THRESH);
  JitterProtectStream(const JitterProtectStream &rhs);
  virtual ~JitterProtectStream();

  JitterProtectStream & operator=(const JitterProtectStream &rhs);

  void ChangeBacklogThresh(const unsigned backlog_thresh);
  inline unsigned GetBacklogThresh() const;

  inline void SetCurrentIndex(const unsigned curr_index);
  inline unsigned GetCurrentIndex() const;

  // One sample comes in, possibly multiple samples come out (in order)
  void JitterProtectSampleOperation(list<INSAMPLE> &out, const INSAMPLE &in);
  void JitterProtectBlockOperation(SampleBlock<INSAMPLE> &out,
				   const SampleBlock<INSAMPLE> &in);

  ostream & Print(ostream &os) const;
  ostream & operator<<(ostream &os) const;
};

/********************************************************************************
 * class:   JitterProtectMultiStreams
 * purpose: This class provides jitter protection for multiple streams of
 *  samples.  It encapsulates the JitterProtect class for each of the streams.
 *  The samples that are streamed into this class must have level information as
 *  well as indices.
 *******************************************************************************/
template <class INSAMPLE, class JITTERACTION>
class JitterProtectMultiStreams {
private:
  unsigned numlevels;
  int      lowest_level;
  unsigned *backlogs;
  vector<JitterProtectStream<INSAMPLE,JITTERACTION>* > jitter_levels;

public:
  JitterProtectMultiStreams(const unsigned numlevels=1,
			    const int lowest_level=0,
			    unsigned* backlogs=0);
  JitterProtectMultiStreams(const JitterProtectMultiStreams &rhs);
  virtual ~JitterProtectMultiStreams();

  JitterProtectMultiStreams & operator=(const JitterProtectMultiStreams &rhs);

  bool ChangeNumberOfLevels(const unsigned numlevels);
  inline unsigned GetNumberOfLevels() const;

  inline void ChangeLowestLevel(const int lowest_level);
  inline int GetLowestLevel() const;

  void JitterProtectSampleOperation(vector<list<INSAMPLE> > &out,
				    const vector<INSAMPLE> &in);
  void JitterProtectBlockOperation
    (vector<WaveletOutputSampleBlock<INSAMPLE> > &outblock,
     const vector<WaveletOutputSampleBlock<INSAMPLE> > &inblock);

  ostream & Print(ostream &os) const;
  ostream & operator<<(ostream &os) const;
};

/********************************************************************************
 * class:   ZeroFillAction
 * purpose: This class provides the action that should be taken when the jitter
 *  backlog for a particular stream has been reached.  In this case, zero samples
 *  are inserted into the stream with the appropriate indices set.
 *******************************************************************************/
template <class INSAMPLE>
class ZeroFillAction {
public:
  unsigned JitterAction(list<INSAMPLE> &samples, const unsigned curr_index);
};


/********************************************************************************
 * Member functions for JitterProtectStream class
 *******************************************************************************/

// Private functions
template <class INSAMPLE, class JITTERACTION>
bool JitterProtectStream<INSAMPLE,JITTERACTION>::
NextIndex(const unsigned index)
{
  return (index == curr_index+1) ? true : false;
}

template <class INSAMPLE, class JITTERACTION>
void JitterProtectStream<INSAMPLE,JITTERACTION>::
RemoveFromJitterBufferIntoOutput(list<INSAMPLE> &out)
{
  typename list<INSAMPLE>::iterator jb_iter=jitter_buffer.begin();
  while ( jb_iter != jitter_buffer.end() ) {
    if ( NextIndex(jb_iter->GetSampleIndex()) ) {
      out.insert(out.begin(), *jb_iter);
      curr_index++;
      jb_iter = jitter_buffer.erase(jb_iter);
    } else {
      // Because the jitter buffer is ordered, if the current index is not the 
      //  next index, then there is no point in checking the rest
      break;
    }
  }
}

template <class INSAMPLE, class JITTERACTION>
void JitterProtectStream<INSAMPLE,JITTERACTION>::
InsertOrderedIntoJitterBuffer(const INSAMPLE &in)
{
  if (jitter_buffer.size() == 0) {
    jitter_buffer.insert(jitter_buffer.begin(), in);
  } else {
    // Need to find its place
    typename list<INSAMPLE>::iterator jb_iter=jitter_buffer.begin();
    unsigned index_of_samp=in.GetSampleIndex();
    while ( jb_iter != jitter_buffer.end() ) {
      if (index_of_samp < jb_iter->GetSampleIndex() ) {
	jitter_buffer.insert(jb_iter, in);
	break;
      } else {
	jb_iter++;
      }
    }
    if (jb_iter == jitter_buffer.end()) {
      // Place the sample at back
      jitter_buffer.insert(jitter_buffer.end(), in);
    }
  }
}

// Public functions
template <class INSAMPLE, class JITTERACTION>
JitterProtectStream<INSAMPLE,JITTERACTION>::
JitterProtectStream(const unsigned backlog_thresh=DEFAULT_BACKLOG_THRESH) :
  curr_index_init(false), curr_index(0)
{
  this->backlog_thresh = backlog_thresh;
}

template <class INSAMPLE, class JITTERACTION>
JitterProtectStream<INSAMPLE,JITTERACTION>::
JitterProtectStream(const JitterProtectStream &rhs) :
  backlog_thresh(rhs.backlog_thresh), curr_index_init(rhs.curr_index_init),
  curr_index(rhs.curr_index), jitter_buffer(rhs.jitter_buffer)
{
}

template <class INSAMPLE, class JITTERACTION>
JitterProtectStream<INSAMPLE,JITTERACTION>::
~JitterProtectStream()
{
}

template <class INSAMPLE, class JITTERACTION>
JitterProtectStream & JitterProtectStream<INSAMPLE,JITTERACTION>::
operator=(const JitterProtectStream &rhs)
{
  this->~JitterProtectStream();
  return *(new (this) JitterProtectStream(rhs));
}

template <class INSAMPLE, class JITTERACTION>
void JitterProtectStream<INSAMPLE,JITTERACTION>::
ChangeBacklogThresh(const unsigned backlog_thresh)
{
  this->backlog_thresh = backlog_thresh;
}

template <class INSAMPLE, class JITTERACTION>
unsigned JitterProtectStream<INSAMPLE,JITTERACTION>::
GetBacklogThresh() const
{
  return this->backlog_thresh;
}

template <class INSAMPLE, class JITTERACTION>
void JitterProtectStream<INSAMPLE,JITTERACTION>::
SetCurrentIndex(const unsigned curr_index)
{
  this->curr_index = curr_index;
  curr_index_init = true;
}

template <class INSAMPLE, class JITTERACTION>
unsigned JitterProtectStream<INSAMPLE,JITTERACTION>::
GetCurrentIndex() const
{
  return curr_index;
}

template <class INSAMPLE, class JITTERACTION>
unsigned JitterProtectStream<INSAMPLE,JITTERACTION>::
JitterProtectSampleOperation(list<INSAMPLE> &out, const INSAMPLE &in)
{
  out.clear();

  if (!curr_index_init) {
    curr_index = in.GetSampleIndex();
  }

  if ( NextIndex(in.GetSampleIndex() ) {
    out.insert(out.begin(),in);
    curr_index++;
    
    // Also check for other samples from the jitter buffer
    if ( jitter_buffer.size() ) {
      RemoveFromJitterBufferIntoOutput(out);
    }
  } else {
    // Add to jitter buffer
    InsertOrderedIntoJitterBuffer(in);

    // Check if we need to take action based on backlog
    if (jitter_buffer.size() > backlog_thresh) {
      out = jitter_buffer;
      curr_index = JITTERACTION::JitterAction(out,curr_index+1);
      jitter_buffer.clear();
  }
  return out.size();
}

template <class INSAMPLE, class JITTERACTION>
unsigned JitterProtectStream<INSAMPLE,JITTERACTION>::
JitterProtectBlockOperation(SampleBlock<INSAMPLE> &outblock,
			    const SampleBlock<INSAMPLE> &inblock)
{
  if (inblock.GetBlockSize() == 0) {
    return 0;
  }

  list<INSAMPLE> out;
  for (unsigned i=0; i<inblock.GetBlockSize(); i++) {
    JitterProtectSampleOperation(out, inblock[i]);

    // Transfer the output list to the outblock
    typename list<INSAMPLE>::iterator out_iter=out.begin();
    while ( out_iter != out.end() ) {
      outblock.PushSampleBack(*out_iter);
      out_iter++;
    }
    out.clear();
  }
  return outblock.GetBlockSize();
}

template <class INSAMPLE, class JITTERACTION>
ostream & JitterProtectStream<INSAMPLE,JITTERACTION>::
Print(ostream &os) const
{
  os << "Backlog Threshold: " << backlog_thresh << endl;
  os << "Current Backlog Count: " << curr_backlog_count << endl;
  os << "Current Index: " << curr_index << endl;
  
  typename list<INSAMPLE>::iterator l_iter=jitter_buffer.begin();
  os << "The samples in the jitter buffer: " << endl;
  while (l_iter != jitter_buffer.end()) {
    os << "\t" << *l_iter;
    l_iter++;
  }
  return os;
}

template <class INSAMPLE, class JITTERACTION>
ostream & JitterProtectStream<INSAMPLE,JITTERACTION>::
operator<<(ostream &os) const
{
  return Print(os);
}

/********************************************************************************
 * Member functions for JitterProtectMultiStreams class
 *******************************************************************************/
template <class INSAMPLE, class JITTERACTION>
JitterProtectMultiStreams<INSAMPLE,JITTERACTION>::
JitterProtectMultiStreams(const unsigned numlevels, 
			  const int lowest_level, 
			  unsigned* backlogs) :
  lowest_level(lowest_level)
{
  unsigned i;
  if (numlevels == 0) {
    this->numlevels = 1;

    this->backlogs = new unsigned[1];
    this->backlogs[0] = DEFAULT_BACKLOG_THRESH;

    JitterProtectStream<INSAMPLE,JITTERACTION>* pjps = 
      new JitterProtectStream<INSAMPLE,JITTERACTION>(this->backlogs[0]);
    jitter_levels.push_back(pjps);
  } else {
    this->backlogs = new unsigned[numlevels];
    if (backlogs == 0) {
      for (i=0; i<this->numlevels; i++) {
	this->backlogs[i] = DEFAULT_BACKLOG_THRESH;
      }
    } else {
      for (i=0; i<this->numlevels; i++) {
	this->backlogs[i] = backlogs[i];
      }
    }

    // Create the jitter protectors
    JitterProtectStream<INSAMPLE,JITTERACTION>* pjps;
    for (i=0; i<this->numlevels; i++) {
      pjps = new JitterProtectStream<INSAMPLE,JITTERACTION>(this->backlogs[i]);
      jitter_levels.push_back(pjps);
    }
  }
}

template <class INSAMPLE, class JITTERACTION>
JitterProtectMultiStreams<INSAMPLE,JITTERACTION>::
JitterProtectMultiStreams(const JitterProtectMultiStreams &rhs) :
  numlevels(rhs.numlevels), lowest_level(rhs.lowest_level)
{
  unsigned i;
  if (rhs.numlevels == 0) {
    this->numlevels = 1;

    this->backlogs = new unsigned[1];
    this->backlogs[0] = DEFAULT_BACKLOG_THRESH;

    JitterProtectStream<INSAMPLE,JITTERACTION>* pjps = 
      new JitterProtectStream<INSAMPLE,JITTERACTION>(this->backlogs[0]);
    jitter_levels.push_back(pjps);
  } else {
    this->backlogs = new unsigned[numlevels];
    if (rhs.backlogs == 0) {
      for (i=0; i<rhs.numlevels; i++) {
	this->backlogs[i] = DEFAULT_BACKLOG_THRESH;
      }
    } else {
      for (i=0; i<rhs.numlevels; i++) {
	this->backlogs[i] = rhs.backlogs[i];
      }
    }

    // Create the jitter protectors
    JitterProtectStream<INSAMPLE,JITTERACTION>* pjps;
    for (i=0; i<rhs.numlevels; i++) {
      pjps = new JitterProtectStream<INSAMPLE,JITTERACTION>(this->backlogs[i]);
      jitter_levels.push_back(pjps);
    }
  }
}

template <class INSAMPLE, class JITTERACTION>
JitterProtectMultiStreams<INSAMPLE,JITTERACTION>::
~JitterProtectMultiStreams()
{
  if (backlogs != 0) {
    delete[] backlogs;
    backlogs=0;
  }
  for (unsigned i=0; i<numlevels; i++) {
    CHK_DEL(jitter_levels[i]);
  }
  jitter_levels.clear();
}

template <class INSAMPLE, class JITTERACTION>
JitterProtectMultiStreams & JitterProtectMultiStreams<INSAMPLE,JITTERACTION>::
operator=(const JitterProtectMultiStreams &rhs)
{
  this->~JitterProtectMultiStreams();
  return *(new (this) JitterProtectMultiStreams(rhs));
}

template <class INSAMPLE, class JITTERACTION>
bool JitterProtectMultiStreams<INSAMPLE,JITTERACTION>::
ChangeNumberOfLevels(const unsigned numlevels)
{
  unsigned i;
  if (this->numlevels == numlevels) {
    return false;
  } else if (this->numlevels < numlevels) {
    unsigned* p_unsigned = new int[numlevels];
    for (i=0; i<numlevels; i++) {
      if (i < this->numlevels) {
	p_unsigned[i] = this->backlogs[i];
      } else {
	p_unsigned[i] = DEFAULT_BACKLOG_THRESH;
      }
    }

    if (backlogs != 0) {
      delete[] backlogs;
      backlogs=0;
    }

    this->backlogs = p_unsigned;
    p_unsigned = 0;

    // Add new jitter levels
    JitterProtectStream<INSAMPLE,JITTERACTION>* pjps;
    for (i=0; i<numlevels - this->numlevels; i++) {
      pjps = new JitterProtectStream<INSAMPLE,JITTERACTION>
	(DEFAULT_BACKLOG_THRESH);
      jitter_levels.push_back(pjps);
    }
  } else {
    // Remove some jitter levels
    for (i=0; i<this->numlevels - numlevels; i++) {
      jitter_levels.pop_back();
    }
  }
  return true;
}

template <class INSAMPLE, class JITTERACTION>
unsigned JitterProtectMultiStreams<INSAMPLE,JITTERACTION>::
GetNumberOfLevels() const
{
  return numlevels;
}

template <class INSAMPLE, class JITTERACTION>
bool JitterProtectMultiStreams<INSAMPLE,JITTERACTION>::
ChangeBacklogThresholds(unsigned* backlogs)
{
  if (backlogs == 0) {
    return false;
  }

  for (unsigned i=0; i<numlevels; i++) {
    this->backlogs[i] = backlogs[i];
  }
  return true;
}

template <class INSAMPLE, class JITTERACTION>
void JitterProtectMultiStreams<INSAMPLE,JITTERACTION>::
ChangeLowestLevel(const int lowest_level)
{
  this->lowest_level = lowest_level;
}

template <class INSAMPLE, class JITTERACTION>
int JitterProtectMultiStreams<INSAMPLE,JITTERACTION>::
GetLowestLevel() const
{
  return lowest_level;
}

template <class INSAMPLE, class JITTERACTION>
void JitterProtectMultiStreams<INSAMPLE,JITTERACTION>::
JitterProtectSampleOperation(vector<list<INSAMPLE> > &out,
			     const vector<INSAMPLE> &in)
{
  if (in.size() != 0) {
    unsigned i;
    JitterProtectStream<INSAMPLE,JITTERACTION>* pjps;
    for (i=0; i<in.size(); i++) {
      list<INSAMPLE> outlist;
      unsigned nlevel =  in[i].GetSampleLevel() - lowest_level;
      jitter_levels[nlevel]->JitterProtectSampleOperation(outlist, in[i]);
      out.push_back(outlist);
    }
  }
}

template <class INSAMPLE, class JITTERACTION>
void JitterProtectMultiStreams<INSAMPLE,JITTERACTION>::
JitterProtectBlockOperation
(vector<WaveletOutputSampleBlock<INSAMPLE> > &outblock,
 const vector<WaveletOutputSampleBlock<INSAMPLE> > &inblock)
{
  if (inblock.size() != 0) {
    unsigned i;
    JitterProtectStream<INSAMPLE,JITTERACTION>* pjps;
    for (i=0; i<inblock.size(); i++) {
      WaveletOutputSampleBlock<INSAMPLE> outwosb;
      unsigned nlevel =  inblock[i].GetBlockLevel() - lowest_level;
      jitter_levels[nlevel]->JitterProtectBlockOperation(outwosb, inblock[i]);
      outblock.push_back(outwosb);
    }
  }
}

template <class INSAMPLE, class JITTERACTION>
ostream & JitterProtectMultiStreams<INSAMPLE,JITTERACTION>::
Print(ostream &os) const
{
  unsigned i;
  os << "The number of levels: " << numlevels << endl;

  os << "The backlogs: " << endl;
  for (i=0; i<numlevels; i++) {
    os << "\tLevel " << i << ": " << backlogs[i] << endl;
  }

  os << "The Jitter levels:" << endl;
  for (i=0; i<numlevels; i++) {
    os << "\tLevel " << i << ": " << endl;
    os << *jitter_levels[i];
  }
  return os;
}

template <class INSAMPLE, class JITTERACTION>
ostream & JitterProtectMultiStreams<INSAMPLE,JITTERACTION>::
operator<<(ostream &os) const
{
  return Print(os);
}

/********************************************************************************
 * Member functions for ZeroFillAction class
 *******************************************************************************/
template <class INSAMPLE>
unsigned ZeroFillAction<INSAMPLE>::
JitterAction(list<INSAMPLE> &samples, const unsigned index)
{
  typename list<INSAMPLE>::iterator iter=samples.begin();
  unsigned prev_index=index;
  while (iter != samples.end()) {
    if (prev_index < iter->GetSampleIndex()) {
      INSAMPLE zero(0.0, prev_index);
      samples.insert(iter,zero);
    } else {
      iter++;
    }
    prev_index++;
  }
  return prev_index-1;
}

#endif
