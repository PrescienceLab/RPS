#include "transforms.h"

bool StructureOptimizer(SignalSpec &optim,
			unsigned &stages,
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
    optim.approximations.push_back(approx_low);

    // Grab a set of remaining details
    for (i=0; i<spec.details.size(); i++) {
      if (spec.details[i] <= approx_low) {
	optim.details.push_back(spec.details[i]);
      }
    }
  } else {
    // Details only
    for (i=0; i<spec.details.size(); i++) {
      optim.details.push_back(spec.details[i]);
    }
  }

  // Determine if transform is option and number of levels
  if (optim.approximations.size()) {
    if (optim.approximations[0]==(int)optim.details.size()+1) {
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
		      const unsigned numlevels)
{
  unsigned i, j;
  bool in_levels;
  for (i=0; i<numlevels; i++) {
    in_levels = false;
    for (j=0; j<spec.size(); j++) {
      if ((int)i == spec[j]) {
	in_levels = true;
	break;
      }
    }
    if (!in_levels) {
      inversion.push_back(i);
    }
  }
}

void FlattenSignalSpec(vector<int> &flatspec, const SignalSpec &spec)
{
  if (spec.approximations.size()) {
    flatspec.push_back(spec.approximations[0]);
  }

  unsigned size;
  if ((size = spec.details.size())) {
    for (unsigned i=0; i<size; i++) {
      flatspec.push_back(spec.details[i]);
    }
  }
}
