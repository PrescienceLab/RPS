#ifndef _Finder
#define _Finder

#include "Mirror.h"

enum Component {
  RPS_LOADSERVER,
  RPS_LOADBUFFER,
  RPS_LOAD2MEASURE,
  RPS_FLOWSERVER,
  RPS_FLOW2MEASURE,
  RPS_FLOWBUFFER,
  RPS_TEXT2MEASURE,
  RPS_MEASUREBUFFER,
  RPS_PREDSERVER,
  RPS_PREDREQRESPSERVER,
  RPS_PREDBUFFER,
  RPS_HOSTLOADPRED
};

enum DataType {
  RPS_LOADMEASUREMENT,
  RPS_FLOWMEASUREMENT,
  RPS_MEASUREMENT,
  RPS_BUFFERREQUEST,
  RPS_BUFFERRESPONSE,
  RPS_PREDICTIONREQUEST,
  RPS_PREDICTIONRESPONSE
};


struct Host {
  char *name;
  Host();
  Host(const Host &rhs);
  Host(char *name);
  Host & operator=(const Host &rhs);
  virtual ~Host();
};

struct Flow {
  Host from, to;
  Flow();
  Flow(const Flow &rhs);
  Flow(char *from, char *to);
  Flow & operator=(const Flow &rhs);
  virtual ~Flow();
};


struct Mapping {
  Component c;
  DataType  d;
  int        numendpoints;
  EndPoint  *endpoints;
  Mapping();
  virtual ~Mapping();
};


Mapping *FindComponent(const Host &h, const Component &c);
Mapping *FindComponent(const Flow &f, const Component &c);

// RegisterComponent
// DeleteRegistration

#endif
