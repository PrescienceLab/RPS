#ifndef _jitter
#define _jitter

#include <iostream>

const unsigned DEFAULT_BACKLOG_THRESH=5;

//--------------------------------------------------------------------
// This class makes dealing with jitter associated with a distributed
//  system manageable.
//--------------------------------------------------------------------
class JitterHelper {
private:
  unsigned backlog_thresh;
  unsigned current_backlog;

public:
  JitterHelper(unsigned backlog_thresh=DEFAULT_BACKLOG_THRESH) {
    this->backlog_thresh = backlog_thresh;
    current_backlog = 0;
  };

  JitterHelper(const JitterHelper &rhs) :
    backlog_thresh(rhs.backlog_thresh), current_backlog(rhs.current_backlog) {};

  ~JitterHelper() {};

  JitterHelper & operator=(const JitterHelper &rhs) {
    backlog_thresh = rhs.backlog_thresh;
    current_backlog = rhs.current_backlog;
    return *this;
  };

  inline unsigned GetBacklogThreshold() const {
    return backlog_thresh;
  };

  inline unsigned GetCurrentBacklog() const {
    return current_backlog;
  };

  inline void SetBacklogThreshold(unsigned backlog_thresh) {
    this->backlog_thresh = backlog_thresh;
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

  inline bool ThresholdExceeded() {
    return (current_backlog > backlog_thresh);
  };

  ostream & Print(ostream &os) {
    os << "Backlog threshold (in sample times): " << backlog_thresh << endl;
    os << "Current backlog (in sample times):   " << current_backlog << endl;
    return os;
  };
};

#endif

