#include "delay.h"

bool CalculateWaveletDelayBlock(const unsigned numcoefs, 
				const unsigned numlevels, 
				int* delay_vals)
{
  if (!delay_vals) {
    return false;
  }

  switch (numlevels) {
  case 0:
  case 1:
    return false;

  case 2:
    delay_vals[numlevels-1] = 0;
    delay_vals[numlevels-2] = 0;
    break;

  default:
    delay_vals[numlevels-1] = 0;
    delay_vals[numlevels-2] = 0;
    delay_vals[numlevels-3] = numcoefs;
    for (int i=numlevels-4; i>=0; i--) {
      delay_vals[i] = numcoefs + 2*delay_vals[i+1] - 2;
    }
  }
  return true;
};

bool CalculateMRADelayBlock(const unsigned numcoefs,
			    const unsigned numlevels,
			    int* delay_vals)
{
  if (!delay_vals) {
    return false;
  }

  switch (numlevels) {
  case 0:
    return false;
  case 1:
    delay_vals[0]=0;

  case 2:
    delay_vals[numlevels-1] = 0;
    delay_vals[numlevels-2] = numcoefs;;
    break;

  default:
    delay_vals[numlevels-1] = 0;
    delay_vals[numlevels-2] = numcoefs;
    for (int i=numlevels-3; i>=0; i--) {
      delay_vals[i] = numcoefs + 2*delay_vals[i+1] - 2;
    }
  }
  return true;
};


bool CalculateFilterBankDelayBlock(const unsigned numcoefs,
				   const unsigned numlevels,
				   const unsigned twoband_delay,
				   int* delay_vals)
{
  if (!delay_vals) {
    return false;
  }

  switch (numlevels) {
  case 0:
  case 1:
    return false;

  case 2:
    delay_vals[numlevels-1] = 0;
    delay_vals[numlevels-2] = 0;
    break;

  default:
    unsigned delay=twoband_delay;
    delay_vals[numlevels-1] = 0;
    delay_vals[numlevels-2] = 0;
    delay_vals[numlevels-3] = 0;
    for (int i=numlevels-4; i>=0; i--) {
      delay++;
      delay_vals[i] = delay;
    }
  }
  return true;
};

unsigned CalculateStreamingRealTimeDelay(const unsigned numcoefs,
					 const unsigned numstages)
{
  unsigned delay=0x1;
  delay = delay << numstages;
  delay = delay * (numcoefs-1) + 2 - numcoefs;
  return delay;
};
