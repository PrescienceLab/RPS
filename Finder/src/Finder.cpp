#include "Finder.h"
#include <new>

Host::Host() : name(0) {}

Host::Host(const Host &rhs) 
{
  if (rhs.name==0) { 
    name=0;
  } else {
    name = new char [strlen(rhs.name)+1];
    strcpy(name,rhs.name);
  }
}
 
Host::Host(char *name) 
{
  if (name==0) { 
    this->name=0;
  } else {
    this->name = new char [strlen(name)+1];
    strcpy(this->name,name);
  }
}

Host & Host::operator=(const Host &rhs)
{
  return *(new (this) Host(rhs));
}

Host::~Host()
{
  if (name) {
    delete [] name;
  }
}

Flow::Flow() {}
Flow::Flow(const Flow &rhs) : from(rhs.from), to(rhs.to) {}
Flow::Flow(char *f, char *t) : from(f), to(t) {}
Flow & Flow::operator=(const Flow &rhs) 
{
  from=rhs.from; to=rhs.to;
  return *this;
}
Flow::~Flow() {}


Mapping::Mapping()
{
  endpoints=0;
  numendpoints=0;
}

Mapping::~Mapping()
{
  if (endpoints) {
    delete [] endpoints;
  }
}

Mapping *FindComponent(const Host &h, const Component &c)
{
  char buf[1024];

  switch (c) { 
  case RPS_LOADBUFFER:
    if (getenv("HOSTLOADSERVERBUFFERPORT")==0) {
      return 0;
    } else {
      sprintf(buf,"client:tcp:%s:%s",h.name,getenv("HOSTLOADSERVERBUFFERPORT"));
      Mapping *m=new Mapping;
      m->c=RPS_LOADBUFFER;
      m->d=RPS_BUFFERRESPONSE;
      m->numendpoints=1;
      m->endpoints = new EndPoint [1] ;
      m->endpoints->Parse(buf);
      return m;
    }
  case RPS_PREDBUFFER:
    if (getenv("HOSTLOADPREDBUFFERPORT")==0) {
      return 0;
    } else {
      sprintf(buf,"client:tcp:%s:%s",h.name,getenv("HOSTLOADPREDBUFFERPORT"));
      Mapping *m=new Mapping;
      m->c=RPS_PREDBUFFER;
      m->d=RPS_BUFFERRESPONSE;
      m->numendpoints=1;
      m->endpoints = new EndPoint [1] ;
      m->endpoints->Parse(buf);
      return m;
    }
  default:
    return 0;
  }
       
} 







Mapping *FindComponent(const Flow &f, const Component &c)
{
  return 0;
}
