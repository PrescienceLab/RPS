#ifndef _Spin
#define _Spin

#include "Serializable.h"

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

#endif
