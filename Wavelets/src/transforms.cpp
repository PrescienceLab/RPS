#include "transforms.h"

bool StructureOptimizer(SignalSpec &optim,
			unsigned &stages,
			const unsigned numstages,
			const int low_level,
			const SignalSpec &spec)
{
  bool transform=false;
  unsigned i;

  // Find the lowest approximation signal
  if (spec.approximations.size()) {    
    int approx_low=spec.approximations[0];
    for (i=1; i<spec.approximations.size(); i++) {
      if (spec.approximations[i] < approx_low) {
	approx_low = spec.approximations[i]; 
      }
    }
    if ((unsigned)(approx_low - low_level) < numstages) {
      optim.approximations.push_back(approx_low);
    }

    // Grab a set of remaining details
    for (i=0; i<spec.details.size(); i++) {
      if (spec.details[i] <= approx_low &&
	  (unsigned)(spec.details[i] - low_level) < numstages) {
	optim.details.push_back(spec.details[i]);
      }
    }
  } else {
    // Details only
    for (i=0; i<spec.details.size(); i++) {
      if ((unsigned)(spec.details[i] - low_level) < numstages) {
	optim.details.push_back(spec.details[i]);
      }
    }
  }

  // Determine if transform is option and number of levels
  if (optim.approximations.size()) {
    if (optim.approximations[0]+1==(int)optim.details.size()) {
      transform=true;
    }
    stages=optim.approximations[0]+1;
  } else if (optim.details.size()) {
    // Zero fill operation, find maximum detail
    int maxlevel=optim.details[0];
    for (i=1; i<optim.details.size(); i++) {
      if (optim.details[i] > maxlevel) {
	maxlevel=optim.details[i];
      }
    }
    stages=maxlevel+1;
  }
  return transform;
}

void InvertSignalSpec(vector<int> &inversion,
		      const vector<int> &spec,
		      const unsigned numlevels,
		      const int low_level)
{
  unsigned i, j;
  bool in_levels;
  for (i=0; i<numlevels; i++) {
    in_levels = false;
    for (j=0; j<spec.size(); j++) {
      if (i == (unsigned)(spec[j]-low_level)) {
	in_levels = true;
	break;
      }
    }
    if (!in_levels) {
      inversion.push_back(i);
    }
  }
}

// This routine only expects one approximation, and a set of details
void FlattenSignalSpec(vector<int> &flatspec, const SignalSpec &spec)
{
  if (spec.approximations.size()) {
    flatspec.push_back(spec.approximations[0]+1);
  }

  unsigned size;
  if ((size = spec.details.size())) {
    for (unsigned i=0; i<size; i++) {
      flatspec.push_back(spec.details[i]);
    }
  }
}

unsigned NumberOfLevels(const unsigned length)
{
  unsigned J=0, bitsum=0, bittest=length;
  for (unsigned i=0; i<sizeof(unsigned)*BITS_PER_BYTE; i++) {
    if ((bittest & 0x1) == 1) {
      J=i;
      bitsum++;
    }
    bittest = bittest >> 1;
  }

  // This next test could be used in order to increase the level to J+1, and 
  //  append zeros.  At this time it has been decided not to append zeros and
  //  to simply use the first 2^J samples for computing the DWT.
  //
  //if (bitsum != 1) {
  //  J++;
  //}

  return J;
}
