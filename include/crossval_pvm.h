#ifndef _crossval_pvm
#define _crossval_pvm

#include "pvm3.h"

#define LOADTRACE 1
#define READY     2
#define JOB       4
#define RESULT    8
#define DONE      16
#define TASKFAIL  32

int PvmPackDoubles(double *v, int num)
{
  return pvm_pkdouble(v,num,1);
}

int PvmUnpackDoubles(double *v, int num)
{
  return pvm_upkdouble(v,num,1);
}

int PvmPackInts(int *v, int num)
{
  return pvm_pkint(v,num,1);
}

int PvmUnpackInts(int *v, int num)
{
  return pvm_upkint(v,num,1);
}

int PvmPackString(char *c, int num)
{
  return pvm_pkstr(c);
}

int PvmUnpackString(char *c, int num)
{
  return pvm_upkstr(c);
}

#endif
