#include <stdio.h>
#include "GetLoadAvg.h"

int main()
{
  double avgs[3];

  int rc= RPSgetloadavg(avgs,3);

  printf("rc=%d, [%f %f %f]\n",rc,
	 avgs[0],avgs[1],avgs[2]);

  return 0;
}
