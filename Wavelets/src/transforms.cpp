#include "transforms.h"

bool StructureOptimizer(SignalSpec &optim,
			unsigned numlevels,
			const SignalSpec &spec)
{
  bool transform=false;
  unsigned i;

  // Find the lowest approximation signal
  if (spec.approximations.size()) {    
    int approx_low=spec.approximations[0];
    for (i=1; i<spec.approximations.size(); i++) {
      if (approx_low < spec.approximations[i]) {
	approx_low = spec.approximations[i]; 
      }
    }
    optim.approximations.push_back(approx_low);
    numlevels=1;

    // Grab a set of remaining details
    for (i=0; i<spec.details.size(); i++) {
      if (spec.details[i] <= approx_low) {
	optim.details.push_back(spec.details[i]);
	numlevels++;
      }
    }
  } else {
    // Details only
    numlevels=1;  // The non-existent approximation level
    for (i=0; i<spec.details.size(); i++) {
      optim.details.push_back(spec.details[i]);
      numlevels++;
    }
  }
  return transform;
}
