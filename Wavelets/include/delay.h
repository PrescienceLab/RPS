#ifndef _delay
#define _delay

#include <vector>
#include <deque>
#include <iostream>

#include "util.h"
#include "sample.h"
#include "sampleblock.h"

template <typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
class DelayBlock {
private:
  unsigned numlevels;
  unsigned num_delay_levels;
  unsigned low_delay_level;
  unsigned high_delay_level;

  int      *delay_vals;
  vector<FIRFilter<SAMPLETYPE,OUTSAMPLE,INSAMPLE> *> dbanks;

public:
  DelayBlock(unsigned numlevels=2,
	     unsigned num_delay_levels=0,
	     unsigned low_delay_level=0,
	     unsigned high_delay_level=0);
  DelayBlock(const DelayBlock &rhs);

  virtual ~DelayBlock();

  DelayBlock & operator=(const DelayBlock &rhs);

  inline unsigned GetNumberLevels() const;
  inline unsigned GetNumberDelayLevels() const;
  inline unsigned GetLowestDelayedLevel() const;
  inline unsigned GetHighestDelayedLevel() const;
  inline unsigned GetDelayValueOfLevel(unsigned level);

  void SetNumberLevels(unsigned numlevels);
  void SetNumberDelayLevels(unsigned num_delay_levels);
  void SetLowestDelayedLevel(unsigned low_delay_level);
  void SetHighestDelayedLevel(unsigned high_delay_level);
  void SetDelayValueOfLevel(unsigned level);

  void ChangeDelayConfig(unsigned numlevels,
			 unsigned num_delay_levels,
			 unsigned low_delay_level,
			 unsigned high_delay_level);

  bool StreamingSampleOperation(vector<OUTSAMPLE> &out, vector<INSAMPLE> &in);

  unsigned StreamingBlockOperation(vector<SampleBlock<OUTSAMPLE> *> &outblock,
				   vector<SampleBlock<INSAMPLE> *> &inblock);

  ostream & Print(ostream &os) const;
};


template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DelayBlock(unsigned numlevels=2, unsigned num_delay_levels=0,
	   unsigned low_delay_level=0, unsigned high_delay_level=0)
{

}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
DelayBlock(const DelayBlock &rhs)
{

}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
~DelayBlock()
{

}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE> &
DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
operator=(const DelayBlock &rhs)
{

}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetNumberLevels() const
{

}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetNumberDelayLevels() const
{

}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetLowestDelayedLevel() const
{

}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetHighestDelayedLevel() const
{

}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
GetDelayValueOfLevel(unsigned level)
{

}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetNumberLevels(unsigned numlevels)
{

}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetNumberDelayLevels(unsigned num_delay_levels)
{
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetLowestDelayedLevel(unsigned low_delay_level)
{

}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetHighestDelayedLevel(unsigned high_delay_level)
{

}


template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
SetDelayValueOfLevel(unsigned level)
{
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
void DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
ChangeDelayConfig(unsigned numlevels, unsigned num_delay_levels,
		  unsigned low_delay_level, unsigned high_delay_level)
{
}


template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
bool DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingSampleOperation(vector<OUTSAMPLE> &out, vector<INSAMPLE> &in)
{

}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
unsigned DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
StreamingBlockOperation(vector<SampleBlock<OUTSAMPLE> *> &outblock,
			vector<SampleBlock<INSAMPLE> *> &inblock)
{
}

template<typename SAMPLETYPE, class OUTSAMPLE, class INSAMPLE>
ostream & DelayBlock<SAMPLETYPE, OUTSAMPLE, INSAMPLE>::
Print(ostream &os) const
{

}

#endif
