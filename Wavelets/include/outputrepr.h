#ifndef _outputrepr
#define _outputrepr

#include <deque>
#include <vector>

#include "util.h"

/********************************************************************************
 *
 * This class is simply an interface class that forces all output representations
 *  to have certain accessor member functions.  Namely, these accessor functions
 *  are to push samples into the representation and to get samples from the
 *  representation.
 *
 *******************************************************************************/
template <class SAMPLETYPE>
class OutputRepresentation {
public:
  virtual bool GetSample(int level, unsigned index, SAMPLETYPE &sample)=0;
  virtual void PushSample(SAMPLETYPE &sample, int level)=0;
  virtual ostream & Print(ostream &os) const=0;

  virtual ~OutputRepresentation() {};
};

/********************************************************************************
 *
 * This class represents the output samples as queues organized into multi-
 *  levels.  Each level has its own queue (double-ended), and it is indexed 
 *  through a vector of pointers.
 *
 *******************************************************************************/
template <class SAMPLETYPE>
class MultiLevelSampleQueue : public OutputRepresentation<SAMPLETYPE> {
private:
  vector<deque<SAMPLETYPE>*> levels;
  unsigned numlevels;

public:
  MultiLevelSampleQueue();
  MultiLevelSampleQueue(const MultiLevelSampleQueue &rhs);
  MultiLevelSampleQueue(unsigned numlevels);
  virtual ~MultiLevelSampleQueue();

  MultiLevelSampleQueue & operator=(const MultiLevelSampleQueue &rhs);

  void AddLevel();
  void RemoveLevel();

  inline unsigned GetNumberLevels();
  bool ChangeNumberLevels(unsigned numlevels);

  bool GetSample(int level, unsigned index, SAMPLETYPE &sample);
  void PushSample(SAMPLETYPE & sample, int level);

  ostream & Print(ostream &os) const;
};

/********************************************************************************
 *
 * This class represents the output samples as a full binary tree.  Each depth of
 *  the tree represents another level of the decomposition.  This representation
 *  makes it easy to get samples around a certain time, but does not make it
 *  easy to obtain a level of samples.
 *
 *******************************************************************************/
template <class SAMPLETYPE>
class SampleBinaryTree : public OutputRepresentation<SAMPLETYPE> {
private:
  class TreeNode {
    SAMPLETYPE sample;
    TreeNode*  leftchild;
    TreeNode*  rightchild;

    TreeNode() { sample=0; leftchild=0; rightchild=0; };
    TreeNode(SAMPLETYPE sample) { 
      this->sample=sample; leftchild=0; rightchild=0;
    };
    TreeNode(const TreeNode &rhs) : 
      sample(rhs.sample), leftchild(rhs.leftchild), rightchild(rhs.rightchild) {};
    virtual ~TreeNode() {};

    TreeNode & operator=(const TreeNode &rhs) {
      if (this != &rhs) {
	sample = rhs.sample;
	leftchild = rhs.leftchild;
	rightchild = rhs.rightchild;
      }
      return *this;
    };
  };

  TreeNode rootparent;
  unsigned height;

public:
  SampleBinaryTree();
  SampleBinaryTree(const SampleBinaryTree &rhs);
  SampleBinaryTree(unsigned height);

  virtual ~SampleBinaryTree();

  SampleBinaryTree & operator=(const SampleBinaryTree &rhs);

  void AddLevel();
  void RemoveLevel();

  bool GetSample(int level, unsigned index, SAMPLETYPE &sample);
  void PushSample(SAMPLETYPE & sample, int level);

  ostream & Print(ostream &os) const;
};

/********************************************************************************
 *
 * Member functions for the MultiLevelSampleQueue representation.
 *
 *******************************************************************************/
template <class SAMPLETYPE>
MultiLevelSampleQueue::MultiLevelSampleQueue() : numlevels(0)
{
}

template <class SAMPLETYPE>
MultiLevelSampleQueue::MultiLevelSampleQueue(const MultiLevelSampleQueue &rhs) :
  levels(rhs.levels), numlevels(rhs.numlevels)
{
}

template <class SAMPLETYPE>
MultiLevelSampleQueue::MultiLevelSampleQueue(unsigned numlevels)
{
  if (numlevels < 2) {
    numlevels = 0;
  } else {
    this->numlevels = numlevels;
    deque<SAMPLETYPE>* pd;
    
    for (unsigned i=0; i<numlevels; i++) {
      pd = new deque<SAMPLETYPE>;
      levels.push_back(pd);
  }
}

template <class SAMPLETYPE>
MultiLevelSampleQueue::~MultiLevelSampleQueue()
{
  deque<SAMPLETYPE>* pd;
  for (unsigned i=0; i<numlevels; i++) {
    pd = levels[i];
    delete pd;
  }
  levels.clear();
}

template <class SAMPLETYPE>
MultiLevelSampleQueue &  MultiLevelSampleQueue::
operator=(const MultiLevelSampleQueue &rhs)
{
  if (this != &rhs) {
    levels = rhs.levels;
    numlevels = rhs.numlevels;
  }
  return *this;
}

template <class SAMPLETYPE>
void MultiLevelSampleQueue::AddLevel()
{
    this->numlevels++;
    deque<SAMPLETYPE>* pd = new deque<SAMPLETYPE>;
    levels.push_back(pd);
}

template <class SAMPLETYPE>
void MultiLevelSampleQueue::RemoveLevel()
{
  this->numlevels--;
  deque<SAMPLETYPE>* pd;
  pd = levels.back();
  delete pd;
  levels.pop_back();
}

template <class SAMPLETYPE>
unsigned MultiLevelSampleQueue::GetNumberLevels()
{
  return numlevels;
}

// *DANGER*
// This function loses all samples in the representation and starts from scratch
template <class SAMPLETYPE>
bool MultiLevelSampleQueue::ChangeNumberLevels(unsigned numlevels)
{
  bool success=false;
  if (numlevels >= 2) {
    deque<SAMPLETYPE>* pd;
    unsigned i;

    for (i=0; i < this->numlevels; i++) {
      pd = levels[i];
      delete pd;
    }
    levels.clear();

    this->numlevels = numlevels;

    for (i=0; i<numlevels; i++) {
      pd = new deque<SAMPLETYPE>;
      levels.push_back(pd);
    }
    success = true;
  }
  return success;
}

template <class SAMPLETYPE>
bool MultiLevelSampleQueue::
GetSample(int level, unsigned index, SAMPLETYPE &sample)
{
  bool foundsample=false;
  if (levels.size() <= level) {
    
  }

  return foundsample;
}

template <class SAMPLETYPE>
void MultiLevelSampleQueue::PushSample(SAMPLETYPE & sample, int level)
{

}

template <class SAMPLETYPE>
ostream & MultiLevelSampleQueue::Print(ostream &os) const
{

}

/********************************************************************************
 *
 * Member functions for the SampleBinaryTree output representation.
 *
 *******************************************************************************/
template <class SAMPLETYPE>
SampleBinaryTree::SampleBinaryTree()
{

}

template <class SAMPLETYPE>
SampleBinaryTree::SampleBinaryTree(unsigned height)
{

}

template <class SAMPLETYPE>
void SampleBinaryTree::AddLevel()
{

}

template <class SAMPLETYPE>
void SampleBinaryTree::RemoveLevel()
{

}

template <class SAMPLETYPE>
bool SampleBinaryTree::GetSample(int level, unsigned index, SAMPLETYPE &sample)
{

}

template <class SAMPLETYPE>
void SampleBinaryTree::PushSample(SAMPLETYPE & sample, int level)
{

}

template <class SAMPLETYPE>
ostream & SampleBinaryTree::Print(ostream &os) const
{

}

#endif
