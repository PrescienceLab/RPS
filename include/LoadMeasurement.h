#ifndef _LoadMeasurement
#define _LoadMeasurement

#include <stdio.h>

#include "Serializable.h"
#include "TimeStamp.h"
#if HAVE_EXPAT
#include "xmlint.h"
#endif

#define DEFAULT_PERIOD_USEC 1000000

#define XML 0

struct LoadMeasurement : public SerializeableInfo 
#if XML
, public XmlInterface 
#endif
{
#define SMOOTH_UNIX 0  // Unix style 1, 5, 15 minute averages
#define SMOOTH_MACH 1  // Mach style 5, 30, 60 second averages
  unsigned ipaddress;
  TimeStamp timestamp;
  int    period_usec;
  int    smoothingtype;
  double unsmoothed;
  double avgs[3];

  LoadMeasurement();
  LoadMeasurement(const LoadMeasurement &right);
  virtual ~LoadMeasurement();

  LoadMeasurement & operator = (const LoadMeasurement &right);

  int GetPackedSize() const;
  int GetMaxPackedSize() const ;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;

  void Print(FILE *out=stdout) ;

  static void SetSmoothingType(LoadMeasurement &measure);

#if XML
  int GetXMLPackedSize() const;
  int GetMaxXMLPackedSize() const;
  int PackToXML(Buffer &buf) const;
  int UnpackFromXML(Buffer &buf);
#endif
};


struct LoadMeasurementConfigurationRequest : public SerializeableInfo {
  TimeStamp timestamp;
  int period_usec;

  LoadMeasurementConfigurationRequest();
  LoadMeasurementConfigurationRequest(const LoadMeasurementConfigurationRequest &right);
  virtual ~LoadMeasurementConfigurationRequest();
  LoadMeasurementConfigurationRequest & operator = (const LoadMeasurementConfigurationRequest &right);

  int GetPackedSize() const;
  int GetMaxPackedSize() const ;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;

  void Print(FILE *out=stdout) ;

};


struct LoadMeasurementConfigurationReply: public SerializeableInfo {
  TimeStamp reqtimestamp;
  TimeStamp changetimestamp;
  int period_usec;

  LoadMeasurementConfigurationReply();
  LoadMeasurementConfigurationReply(const LoadMeasurementConfigurationReply &right);
  virtual ~LoadMeasurementConfigurationReply();
  LoadMeasurementConfigurationReply & operator = (const LoadMeasurementConfigurationReply &right);

  int GetPackedSize() const;
  int GetMaxPackedSize() const ;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;

  void Print(FILE *out=stdout) ;

};

#endif
