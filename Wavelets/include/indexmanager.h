#ifndef _indexmanager
#define _indexmanager

#include <iostream>

class IndexManager {
private:
  bool index_set;
  unsigned index;

public:
  IndexManager(bool index_set=false, unsigned index=0) {
    this->index_set = index_set; this->index = index;
  };

  IndexManager(const IndexManager &rhs) :
    index_set(rhs.index_set), index(rhs.index) {};

  virtual ~IndexManager() {};

  IndexManager & operator=(const IndexManager &rhs) {
    this->index_set = rhs.index_set;
    this->index = rhs.index;
  };

  bool InOrder(unsigned nxt_index) {
    if (nxt_index == index+1) {
      index++;
      return true;
    } else {
      return false;
    }
  };

  inline bool Initialized() const { return index_set; };
  inline void ClearIndexSetFlag() { index_set=false; };
  inline void SetIndexSetFlag() { index_set=true; };

  inline void SetIndex(unsigned index) { 
    this->index=index; 
    index_set=true;
  };

  inline unsigned GetIndex() {
    return index;
  };

  ostream & Print(ostream &os) const {
    os << "IndexInitialized? " << index_set << endl;
    os << "Index: " << index << endl;
    return os;
  };
};

#endif
