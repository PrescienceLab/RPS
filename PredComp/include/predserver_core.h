#ifndef _predserver_core
#define _predserver_core


#include "Serializable.h"

#define NEW_MEASURE 0  // Command is followed by a Measurement and will generate
#define NEW_CONFIG  1  // Command is followed by PredictionReconfigurationRequest 
struct PredServerCoreCommand : public SerializeableInfo {
  unsigned cmd;

  PredServerCoreCommand(unsigned command=NEW_MEASURE) : cmd(command) {}
  // Default copy constructor
  //PredServerCoreCommand(const PredServerCoreCommand &right);
  virtual ~PredServerCoreCommand() {}
  
  // Default =s
  //virtual PredServerCoreCommand & operator = (const PredServerCoreCommand &right);

  int GetPackedSize() const { return 4;}
  int GetMaxPackedSize() const { return 4; };
  int Pack(Buffer &buf) const { buf.Pack(cmd); return 0; }
  int Unpack(Buffer &buf) { buf.Unpack(cmd); return 0; }

  ostream & operator<<(ostream &os) const { return os <<"PredServerCoreCommand(cmd="<<cmd<<")"; }
};

inline ostream &operator<<(ostream &os, const PredServerCoreCommand &rhs) { return rhs.operator<<(os);}

#endif
