#ifndef _TRACE
#define _TRACE

#ifdef __cplusplus
extern "C" {
#endif

void BEGIN_TRACE(char *filename);
void END_TRACE();
void BEGIN_EVENT(char *eventname);
void END_EVENT();

void *BeginTrace(char *filename);
void *BeginEvent(void *trace, char *eventname);
int   EndEvent(void *event);
int   EndTrace(void *trace);

#ifdef __cplusplus
	   }
#endif


#endif
