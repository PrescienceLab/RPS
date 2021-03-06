#ifndef _FlowBWMeasurement
#define _FlowBWMeasurement

#include <stdio.h>
#include <iostream>

#include "Serializable.h"
#include "TimeStamp.h"

#define DEFAULT_PERIOD_USEC 1000000


using namespace std;

struct FlowBWMeasurement : public SerializeableInfo {
  unsigned  fromip;
  unsigned  toip;
  TimeStamp timestamp;
  int       period_usec;
  double    bw;

  FlowBWMeasurement();
  FlowBWMeasurement(const FlowBWMeasurement &right);
  virtual ~FlowBWMeasurement();

  FlowBWMeasurement & operator = (const FlowBWMeasurement &right);

  int GetPackedSize() const;
  int GetMaxPackedSize() const ;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;

  void Print(FILE *out=stdout) const ;
  ostream &Print(ostream &os) const;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<<(ostream &os, const FlowBWMeasurement &rhs) { return rhs.operator<<(os);}


struct FlowBWMeasurementConfigurationRequest : public SerializeableInfo {
  TimeStamp timestamp;
  unsigned fromip;
  unsigned toip;
  int period_usec;

  FlowBWMeasurementConfigurationRequest();
  FlowBWMeasurementConfigurationRequest(const FlowBWMeasurementConfigurationRequest &right);
  virtual ~FlowBWMeasurementConfigurationRequest();
  FlowBWMeasurementConfigurationRequest & operator = (const FlowBWMeasurementConfigurationRequest &right);

  int GetPackedSize() const;
  int GetMaxPackedSize() const ;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;

  void Print(FILE *out=stdout) const ;
  ostream &Print(ostream &os) const;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<<(ostream &os, const FlowBWMeasurementConfigurationRequest &rhs) { return rhs.operator<<(os);}

struct FlowBWMeasurementConfigurationReply: public SerializeableInfo {
  TimeStamp reqtimestamp;
  TimeStamp changetimestamp;
  unsigned fromip;
  unsigned toip;
  int period_usec;

  FlowBWMeasurementConfigurationReply();
  FlowBWMeasurementConfigurationReply(const FlowBWMeasurementConfigurationReply &right);
  virtual ~FlowBWMeasurementConfigurationReply();
  FlowBWMeasurementConfigurationReply & operator = (const FlowBWMeasurementConfigurationReply &right);

  int GetPackedSize() const;
  int GetMaxPackedSize() const ;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;

  void Print(FILE *out=stdout) const ;
  ostream &Print(ostream &os) const;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<<(ostream &os, const FlowBWMeasurementConfigurationReply &rhs) { return rhs.operator<<(os);}

#endif
