#ifndef _SpinServer
#define _SpinServer

#include <stdio.h>
#include "Serializable.h"
#include "Mirror.h"
#include "Spin.h"
#include "Reference.h"

struct SpinRequest : public SerializeableInfo {
  int    tag;
  double secs;

  SpinRequest(int tag=0, double secs=0);
  SpinRequest(const SpinRequest &right);
  virtual ~SpinRequest();
  
  virtual SpinRequest & operator = (const SpinRequest &right);

  int GetPackedSize() const ;
  int GetMaxPackedSize() const ;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;

   void Print(FILE *out=stdout) const ;
};


struct SpinReply : public SerializeableInfo {
  int    tag;
  double reqsecs;
  double usrsecs;
  double syssecs;
  double wallsecs;

  SpinReply(int tag=0, 
	    double reqsecs=0, 
	    double wallsecs=0,
	    double usrsecs=0,
	    double syssecs=0);
  SpinReply(const SpinReply &right);
  virtual ~SpinReply();

  virtual SpinReply & operator = (const SpinReply &right);

  int GetPackedSize() const ;
  int GetMaxPackedSize() const ;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;

  void Print(FILE *out=stdout) const ;
};

class SpinCompute {
 public:
  static int SetSpin(const Spin &spin);
  static int Compute(const SpinRequest &req, SpinReply &repl);
};


typedef SerializeableRequestResponseMirror<SpinRequest,SpinCompute,SpinReply> SpinServer;

typedef Reference<SpinRequest,SpinReply> SpinServerRef;
  
#endif
