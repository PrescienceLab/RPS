#ifndef _jitter
#define _jitter

#include <iostream>
#include <vector>

#include "sample.h"
#include "sampleblock.h"

const unsigned DEFAULT_BACKLOG_THRESH=5;

//--------------------------------------------------------------------
// This class makes dealing with jitter associated with a distributed
//  system manageable.
//--------------------------------------------------------------------
class JitterHelper {
protected:
  unsigned backlog_thresh, current_backlog;

public:
  JitterHelper(const unsigned backlog_thresh=DEFAULT_BACKLOG_THRESH) {
    this->backlog_thresh = backlog_thresh;
  };

  JitterHelper(const JitterHelper &rhs) {
    this->backlog_thresh = rhs.backlog_thresh;
    this->current_backlog = rhs.current_backlog;
  };

  ~JitterHelper() {};

  JitterHelper & operator=(const JitterHelper &rhs) {
    this->backlog_thresh = rhs.backlog_thresh;
    this->current_backlog = rhs.current_backlog;
    return *this;
  };

  // Sample operations
  inline unsigned GetBacklogThreshold() const {
    return backlog_thresh;
  };

  inline void SetBacklogThreshold(const unsigned backlog_thresh) {
    this->backlog_thresh = backlog_thresh;
  };

  inline unsigned GetCurrentBacklog() const {
    return current_backlog;
  };

  inline void SetCurrentBacklog(const unsigned new_backlog) {
    this->current_backlog = new_backlog;
  };

  inline void ClearCurrentBacklog() {
    current_backlog = 0;
  };

  inline void IncCurrentBacklog() {
    current_backlog++;
  };

  inline void DecCurrentBacklog() {
    --current_backlog;
  };

  inline bool ThresholdExceeded() const {
    return (current_backlog > backlog_thresh);
  };

  ostream & Print(ostream &os) const {
    os << "Backlog threshold (in samples): " << backlog_thresh << endl;
    os << "Current backlog (in samples):   " << current_backlog << endl;
    return os;
  };
  
  ostream & operator<<(ostream &os) const { return Print(os);}

};

inline ostream & operator<<(ostream &os, const JitterHelper &rhs) { 
  return rhs.operator<<(os);
};

const unsigned MAX_INDEX=4294967295U;  // This is 2^32-1

template <class INSAMPLE>
class DefaultJitterAction {
public:


  // This default class simply zeros the missing samples - assumes samples
  //  ordered by indices
  static void CreateMissingSamples(const unsigned current_index,
				   const vector<INSAMPLE> &buffer) {
    unsigned i, j, gapsize;
    unsigned sampleindex, index=current_index;

    vector<INSAMPLE> zeros;
    for (i=0; i<buffer.size(); i++, index++) {

      sampleindex = buffer[i].GetSampleIndex();
      if (sampleindex == index) {
	index++;
      } else if (sampleindex > index) {
	gapsize = sampleindex - index - 1;
	for (j=0; j<gapsize; j++) {
	  index++;
	  INSAMPLE zerosample;
	  zerosample.SetSampleIndex(index);
	  zeros.push_back(zerosample);
	}
      } else {
	gapsize = MAX_INDEX - index;
	for (j=0; j<gapsize; j++) {
	  index++;
	  INSAMPLE zerosample;
	  zerosample.SetSampleIndex(index);
	  zeros.push_back(zerosample);
	}
      }
    }
    // Merge the two vectors, zeros and buffer, in order of index (TODO)
  
  };
};

#endif
