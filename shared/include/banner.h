#ifndef _banner
#define _banner

#include <stdlib.h>
#include <stdio.h>

inline char *GetRPSBanner()
{
  char *b = new char [1000];
  
  snprintf(b,1000,
	   "RPS: Resource Prediction System Toolkit\n"
	   "---------------------------------------\n\n"
	   "Copyright (c) 1999-2002 by Peter A. Dinda\n"
	   "Use subject to license (%s/LICENSE)\n\n"
	   "http://www.cs.northwestern.edu/~RPS\n"
	   "rps-help@cs.northwestern.edu\n",
	   getenv("RPS_DIR") ? getenv("RPS_DIR") : "$RPS_DIR");
  return b;
}

#endif
