#ifndef _rpslog
#define _rpslog

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif

#define CONTEXT __FILE__,__LINE__

inline void RPSLog(char *fn, int line, int level, char *fmt, ...)
{
  static FILE *out=0;
  static char *file=0;
  static int   minlevel=0;

  if (out==0) {
    file=getenv("RPS_LOG");
    if (file) { 
      out=fopen(file,"w");
      if (!out) {
	out=stderr;
      }
    } else {
      out=stderr;
    }
    if (getenv("RPS_MINLOGLEVEL")) {
      minlevel=atoi(getenv("RPS_MINLOGLEVEL"));
    } else {
      minlevel=0;
    }
  }
  va_list list;
  va_start(list,fmt);

  if (level>=minlevel) { 
    fprintf(out,"%s(%d) %d: ",fn,line,level);
    vfprintf(out,fmt,list);
    fflush(out);
  }
}
 
#endif
