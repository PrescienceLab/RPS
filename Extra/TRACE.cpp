#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include <string.h>
#include "TRACE.h"
#include "TimeStamp.h"
#include "Queues.h"

#define MAX_NAME 1024


struct Trace {
  FILE      *file;
};

struct Event {
  Trace     *trace;
  char      name[MAX_NAME];
  double    starttime;
  double    walltime;
  double    systime;
  double    usrtime;
};

static void GetRusage(double *systime, double *usrtime)
{
  struct rusage x;

  if (getrusage(RUSAGE_SELF,&x)) {
    perror("getrusage");
    exit(-1);
  }

  *systime=(double)(TimeStamp(&(x.ru_stime)));
  *usrtime=(double)(TimeStamp(&(x.ru_utime)));
}

extern "C" void *BeginTrace(char *filename)
{
  Trace *t = new Trace;
  t->file = fopen(filename,"w");
  return (void*) t;
}

extern "C" void *BeginEvent(void *trace, char *eventname)
{
  Event *e = new Event;
  e->trace= (Trace *) trace;
  strncpy(e->name,eventname,MAX_NAME);
  fprintf(e->trace->file,"BE \"%s\"\n",e->name);
  GetRusage(&(e->systime),&(e->usrtime));
  e->starttime = (double) TimeStamp(0);
  return (void *) e;
}

extern "C" int   EndEvent(void *event)
{
  Event *e = (Event *) event;
  double sys, usr;
  TimeStamp now(0);
  GetRusage(&sys,&usr);
  e->walltime = ((double)now) - e->starttime;
  e->systime = sys-e->systime;
  e->usrtime = usr-e->usrtime;
  fprintf(e->trace->file,"EE \"%s\" [%lf %lf %lf %lf]\n",e->name,e->starttime,e->walltime,e->systime,e->usrtime);
  delete e;
  return 0;
}

extern "C" int   EndTrace(void *trace) 
{
  Trace *t = (Trace *) trace;
  fclose(t->file);
  delete t;
  return 0;
}


static Stack<Event> EVENTSTACK;
static Trace *TRACE;

void BEGIN_TRACE(char *filename)
{
  TRACE =(Trace*) BeginTrace(filename);
}

void END_TRACE() 
{
  Event *e;
  while (0!=(e=EVENTSTACK.Pop())) {
    EndEvent(e);
  }
  EndTrace(TRACE);
}

void BEGIN_EVENT(char *eventname)
{
  EVENTSTACK.Push((Event*)BeginEvent(TRACE,eventname));
}

void END_EVENT()
{
  EndEvent((void*)EVENTSTACK.Pop());
}




