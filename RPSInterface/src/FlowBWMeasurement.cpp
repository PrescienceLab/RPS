#include <string.h>
#include <stdio.h>
#include <time.h>

#include "FlowBWMeasurement.h"
#include "socks.h"

FlowBWMeasurement::FlowBWMeasurement() : 
  fromip(0), toip(0), timestamp(0), period_usec(DEFAULT_PERIOD_USEC), bw(0)
{
}
  

FlowBWMeasurement::FlowBWMeasurement(const FlowBWMeasurement &right)
{
  memcpy(this,&right,sizeof(FlowBWMeasurement));
}


FlowBWMeasurement::~FlowBWMeasurement()
{}

FlowBWMeasurement & FlowBWMeasurement::operator = (const FlowBWMeasurement &right)
{
  memcpy(this,&right,sizeof(FlowBWMeasurement));
  return *this;
}
  


int FlowBWMeasurement::GetPackedSize() const 
{
  return 4+4+timestamp.GetPackedSize()+4+8;
}


int FlowBWMeasurement::GetMaxPackedSize() const 
{
  return 4+4+timestamp.GetMaxPackedSize()+4+8;
}

int FlowBWMeasurement::Pack(Buffer &buf) const 
{
  buf.Pack(fromip);
  buf.Pack(toip);
  timestamp.Pack(buf);
  buf.Pack((period_usec));
  buf.Pack(bw);
  return 0;
}

int FlowBWMeasurement::Unpack(Buffer &buf) 
{
  buf.Unpack(fromip);
  buf.Unpack(toip);
  timestamp.Unpack(buf);
  buf.Unpack(period_usec);
  buf.Unpack(bw);
  return 0;
}



void FlowBWMeasurement::Print(FILE *out)  const
{
  char fromhost[1024], tohost[1024];

  IPToHostname(fromip,fromhost,1024);
  IPToHostname(toip,tohost,1024);
  
  double hz = 1.0/(1.0e-6*((double)(period_usec)));
  
  fprintf(out,"%-20s -> %-20s (%8.5f Hz) @ %f: %f\n",
	  fromhost, tohost,
	  hz,
	  (double)timestamp,
	  bw);
}

ostream & FlowBWMeasurement::Print(ostream &os) const 
{
  char fromhost[1024], tohost[1024];

  IPToHostname(fromip,fromhost,1024);
  IPToHostname(toip,tohost,1024);
  
  os << "FlowBWMeasurement(fromip="<<fromip<<"["<< ((const char *)fromhost);
  os << "], toip="<<toip<<"["<<((const char *)tohost)<<"], period_usec="<<period_usec<<", bw="<<bw<<")";
  return os;
}


FlowBWMeasurementConfigurationRequest::FlowBWMeasurementConfigurationRequest() :
  timestamp(0),  fromip(0), toip(0), period_usec(DEFAULT_PERIOD_USEC)
{
}

FlowBWMeasurementConfigurationRequest::FlowBWMeasurementConfigurationRequest(const FlowBWMeasurementConfigurationRequest &right) { 
  timestamp=right.timestamp;
  period_usec=right.period_usec;
  fromip=right.fromip;
  toip=right.toip;
}

  
FlowBWMeasurementConfigurationRequest::~FlowBWMeasurementConfigurationRequest()
{
}

FlowBWMeasurementConfigurationRequest & FlowBWMeasurementConfigurationRequest::operator = (const FlowBWMeasurementConfigurationRequest &right) {
  timestamp=right.timestamp;
  period_usec=right.period_usec;
  fromip=right.fromip;
  toip=right.toip;
  return *this;
}

int FlowBWMeasurementConfigurationRequest::GetPackedSize() const {
  return timestamp.GetPackedSize()+4+4+4;
}

int FlowBWMeasurementConfigurationRequest::GetMaxPackedSize() const {
  return timestamp.GetMaxPackedSize()+4+4+4;
}

int FlowBWMeasurementConfigurationRequest::Pack(Buffer &buf) const {
  timestamp.Pack(buf);
  buf.Pack(fromip);
  buf.Pack(toip);
  buf.Pack(period_usec);
  return 0;
}

int FlowBWMeasurementConfigurationRequest::Unpack(Buffer &buf) {
  timestamp.Unpack(buf);
  buf.Unpack(fromip);
  buf.Unpack(toip);
  buf.Unpack(period_usec);
  return 0;
}

void FlowBWMeasurementConfigurationRequest::Print(FILE *out)  const 
{
  char fromhost[1024], tohost[1024];
  IPToHostname(fromip,fromhost,1024);
  IPToHostname(toip,tohost,1024);

  fprintf(out,"FlowBWMeasurementConfigurationRequest timetamp=%f period=%d usec from=%s to=%s\n",
	  (double)timestamp, period_usec,fromhost,tohost);
}

ostream &FlowBWMeasurementConfigurationRequest::Print(ostream &os)  const 
{
  char fromhost[1024], tohost[1024];
  IPToHostname(fromip,fromhost,1024);
  IPToHostname(toip,tohost,1024);

  os << "FlowBWMeasurementConfigurationRequest(timetamp="<<timestamp
     <<", period_usec="<<period_usec
     <<", fromip="<<fromip<<"["<<((const char*)fromhost)<<"], toip="<<toip
     <<"["<<((const char*)tohost)<<"])";
  return os;
}
	  


FlowBWMeasurementConfigurationReply::FlowBWMeasurementConfigurationReply() :
  reqtimestamp(0), changetimestamp(0),
  fromip(0), toip(0), period_usec(DEFAULT_PERIOD_USEC)
{
}

FlowBWMeasurementConfigurationReply::FlowBWMeasurementConfigurationReply(const FlowBWMeasurementConfigurationReply &right) { 
  reqtimestamp=right.reqtimestamp;
  changetimestamp=right.changetimestamp;
  period_usec=right.period_usec;
  fromip=right.fromip;
  toip=right.toip;
}


  
FlowBWMeasurementConfigurationReply::~FlowBWMeasurementConfigurationReply()
{
}

FlowBWMeasurementConfigurationReply & FlowBWMeasurementConfigurationReply::operator = (const FlowBWMeasurementConfigurationReply &right) {
  reqtimestamp=right.reqtimestamp;
  changetimestamp=right.changetimestamp;
  period_usec=right.period_usec;
  fromip=right.fromip;
  toip=right.toip;
  return *this;
}

int FlowBWMeasurementConfigurationReply::GetPackedSize() const {
  return reqtimestamp.GetPackedSize()+changetimestamp.GetPackedSize()+4+4+4;
}

int FlowBWMeasurementConfigurationReply::GetMaxPackedSize() const {
  return reqtimestamp.GetMaxPackedSize()+changetimestamp.GetMaxPackedSize()+4+4+4;
}

int FlowBWMeasurementConfigurationReply::Pack(Buffer &buf) const {
  reqtimestamp.Pack(buf);
  changetimestamp.Pack(buf);
  buf.Pack(fromip);
  buf.Pack(toip);
  buf.Pack(period_usec);
  return 0;
}

int FlowBWMeasurementConfigurationReply::Unpack(Buffer &buf) {
  reqtimestamp.Unpack(buf);
  changetimestamp.Unpack(buf);
  buf.Unpack(fromip);
  buf.Unpack(toip);
  buf.Unpack(period_usec);
  return 0;
}

void FlowBWMeasurementConfigurationReply::Print(FILE *out) const {
  char fromhost[1024], tohost[1024];

  IPToHostname(fromip,fromhost,1024);
  IPToHostname(toip,tohost,1024);

  fprintf(out,"FlowBWMeasurementConfigurationReply reqtimestamp=%f changetimestamp=%f period=%d usec from=%s to=%s\n",
	  (double)reqtimestamp,(double)changetimestamp, period_usec, fromhost, tohost);
}
	  

ostream &FlowBWMeasurementConfigurationReply::Print(ostream &os) const 
{
  char fromhost[1024], tohost[1024];
  IPToHostname(fromip,fromhost,1024);
  IPToHostname(toip,tohost,1024);

  os << "FlowBWMeasurementConfigurationReply(reqtimetamp="<<reqtimestamp
     <<", changetimestamp="<<changetimestamp<<", period_usec="<<period_usec
     <<", fromip="<<fromip<<"["<<((const char*)fromhost)<<"], toip="<<toip
     <<"["<<((const char*)tohost)<<"])";
  return os;
}
