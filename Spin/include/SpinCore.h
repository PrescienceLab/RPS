#ifndef _SpinCore
#define _SpinCore

#include "Mirror.h"
#include "RPSInterface.h"

class Spin : public SerializeableInfo {
 private:
  double min_iters;
  double iters_per_second;
  double getrusage_overhead;
  double loop_overhead;
  void Time(void (Spin::*call)(int arg,...), 
	    int    thearg, 
	    int    iters,
	    double &walltime, 
	    double &systime, 
	    double &usrtime);
  // thearg (arg) will be set to -1 if the call is being calibrated
  void GetRusage(int junk, double &systime, double &usrtime);
  void Null(int junk);
  void SpinInternalRaw(int iters);
  void SpinInternal(int iters, double &walltime, double &systime, double &usrtime);
 public:
  Spin();
  virtual ~Spin();
// Spin(const Spin &rhs); // Default OK
// Spin & operator = (const Spin &rhs); // Default OK
  void Calibrate(); 
  void SpinFor(double seconds, double &walltime, double &systime, double &usrtime);


  virtual int GetPackedSize() const;
  virtual int GetMaxPackedSize() const;
  virtual int Pack(Buffer &buf) const;
  virtual int Unpack(Buffer &buf);

  virtual int Serialize(char *fn);
  virtual int Unserialize(char *fn);

};  


extern Spin the_global_spin;


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
