#ifndef _GetFlowBW
#define _GetFlowBW

int init_getflowbw();

int getflowbw(char *from_host_or_ip, 
	      char *to_host_or_ip, 
	      double *flow_in_megabytes_per_sec);

int deinit_getflowbw();

#endif
