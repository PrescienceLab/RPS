#ifndef _cloadbufferint
#define _cloadbufferint
#ifdef __cplusplus
extern "C" 
#endif
int getloadfrombuffer(const char * ipaddr_str, // IP addr of target
				 double *timeframe, // in/out req timeframe
				 double *loadavg);  // out loadaverage value

#endif

