#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "LoadMeasurement.h"
#include "socks.h"


LoadMeasurement::LoadMeasurement() : 
  ipaddress(0), period_usec(0), smoothingtype(SMOOTH_UNIX), 
  unsmoothed(0.0)
{
  avgs[0]=avgs[1]=avgs[2]=0.0;
}
  

LoadMeasurement::LoadMeasurement(const LoadMeasurement &right)
{
  memcpy(this,&right,sizeof(LoadMeasurement));
}


LoadMeasurement::~LoadMeasurement()
{}

LoadMeasurement & LoadMeasurement::operator = (const LoadMeasurement &right)
{
  memcpy(this,&right,sizeof(LoadMeasurement));
  return *this;
}
  


int LoadMeasurement::GetPackedSize() const 
{
  return 4+timestamp.GetPackedSize()+4+4+4*8;
}


int LoadMeasurement::GetMaxPackedSize() const 
{
  return 4+timestamp.GetMaxPackedSize()+4+4+4*8;
}

int LoadMeasurement::Pack(Buffer &buf) const 
{
  buf.Pack(ipaddress);
  timestamp.Pack(buf);
  buf.Pack((period_usec));
  buf.Pack((smoothingtype));
  buf.Pack((unsmoothed));
  buf.Pack(avgs,3);
  return 0;
}

int LoadMeasurement::Unpack(Buffer &buf) 
{
  buf.Unpack(ipaddress);
  timestamp.Unpack(buf);
  buf.Unpack((period_usec));
  buf.Unpack((smoothingtype));
  buf.Unpack((unsmoothed));
  buf.Unpack(avgs,3);
  return 0;
}

#define LOOKUP_HOST 1

void LoadMeasurement::Print(FILE *out) const
{
  struct in_addr ia; ia.s_addr=htonl(ipaddress);
 
#if LOOKUP_HOST
  struct hostent *he=gethostbyaddr((const char *)&(ia),
				   sizeof(ia),
				   AF_INET);
#endif
  
  double hz = 1.0/(1.0e-6*((double)(period_usec)));
  
  fprintf(out,"%-40s (%s %8.5f Hz) @ %f: %f\t[%f\t%f\t%f]\n",
#if LOOKUP_HOST
	  he ? he->h_name : inet_ntoa(ia),
#else
          inet_ntoa(ia),
#endif
	  smoothingtype == SMOOTH_MACH ? "MACH" :
	  (smoothingtype == SMOOTH_UNIX ? "UNIX" : "????"),
	  hz,
	  (double)timestamp,
	  unsmoothed,
	  avgs[0],avgs[1],avgs[2]);
}


ostream &LoadMeasurement::Print(ostream &os) const
{
  struct in_addr ia; ia.s_addr=htonl(ipaddress);
 
#if LOOKUP_HOST
  struct hostent *he=gethostbyaddr((const char *)&(ia),
				   sizeof(ia),
				   AF_INET);
#endif
  
  os << "LoadMeasurement(ipaddress="<<ipaddress;
#if LOOKUP_HOST
  os << "[" << ((const char*)(he ? he->h_name : inet_ntoa(ia)))<<"]";
#else
  os << "[" <<inet_ntoa(ia)<<"]";
#endif
  os << ", smoothingtype="<<
    (smoothingtype == SMOOTH_MACH ? "MACH" :
     smoothingtype == SMOOTH_UNIX ? "UNIX" : "UNKNOWN")
     << ", period_usec="<<period_usec
     <<", unsmoothed="<<unsmoothed
     <<", avgs=("<<avgs[0]<<","<<avgs[1]<<","<<avgs[2]<<")"
     << ")";
  return os;
}

ostream &LoadMeasurement::operator<<(ostream &os) const
{
  return Print(os);
}



void LoadMeasurement::SetSmoothingType(LoadMeasurement &measure) 
{
#if defined(__osf__)
  measure.smoothingtype=SMOOTH_MACH;
#else
#if defined(linux) || defined(sun) || defined(__FreeBSD__)
  measure.smoothingtype=SMOOTH_UNIX;
#else
  measure.smoothingtype=SMOOTH_UNIX; // guess
#endif
#endif
}


			

LoadMeasurementConfigurationRequest::LoadMeasurementConfigurationRequest() :
  timestamp(0), period_usec(DEFAULT_PERIOD_USEC)
{
}

LoadMeasurementConfigurationRequest::LoadMeasurementConfigurationRequest(const LoadMeasurementConfigurationRequest &right) { 
  timestamp=right.timestamp;
  period_usec=right.period_usec;
}


  
LoadMeasurementConfigurationRequest::~LoadMeasurementConfigurationRequest()
{
}

LoadMeasurementConfigurationRequest & LoadMeasurementConfigurationRequest::operator = (const LoadMeasurementConfigurationRequest &right) {
  timestamp=right.timestamp;
  period_usec=right.period_usec;
  return *this;
}

int LoadMeasurementConfigurationRequest::GetPackedSize() const {
  return timestamp.GetPackedSize()+4;
}

int LoadMeasurementConfigurationRequest::GetMaxPackedSize() const {
  return timestamp.GetMaxPackedSize()+4;
}

int LoadMeasurementConfigurationRequest::Pack(Buffer &buf) const {
  timestamp.Pack(buf);
  buf.Pack(period_usec);
  return 0;
}

int LoadMeasurementConfigurationRequest::Unpack(Buffer &buf) {
  timestamp.Unpack(buf);
  buf.Unpack(period_usec);
  return 0;
}

void LoadMeasurementConfigurationRequest::Print(FILE *out) const {
  fprintf(out,"LoadMeasurementConfigurationRequest timetamp=%f period=%d usec\n",
	  (double)timestamp, period_usec);
}

ostream &LoadMeasurementConfigurationRequest::Print(ostream &os) const {
  os<<"LoadMeasurementConfigurationRequest(timetamp="<<timestamp
    <<", period_usec="<<period_usec<<")";
  return os;
}
	  
ostream &LoadMeasurementConfigurationRequest::operator<<(ostream &os) const
{
  return Print(os);
}


LoadMeasurementConfigurationReply::LoadMeasurementConfigurationReply() :
  reqtimestamp(0), changetimestamp(0),period_usec(DEFAULT_PERIOD_USEC)
{
}

LoadMeasurementConfigurationReply::LoadMeasurementConfigurationReply(const LoadMeasurementConfigurationReply &right) { 
  reqtimestamp=right.reqtimestamp;
  changetimestamp=right.changetimestamp;
  period_usec=right.period_usec;
}


  
LoadMeasurementConfigurationReply::~LoadMeasurementConfigurationReply()
{
}

LoadMeasurementConfigurationReply & LoadMeasurementConfigurationReply::operator = (const LoadMeasurementConfigurationReply &right) {
  reqtimestamp=right.reqtimestamp;
  changetimestamp=right.changetimestamp;
  period_usec=right.period_usec;
  return *this;
}

int LoadMeasurementConfigurationReply::GetPackedSize() const {
  return reqtimestamp.GetPackedSize()+changetimestamp.GetPackedSize()+4;
}

int LoadMeasurementConfigurationReply::GetMaxPackedSize() const {
  return reqtimestamp.GetMaxPackedSize()+changetimestamp.GetMaxPackedSize()+4;
}

int LoadMeasurementConfigurationReply::Pack(Buffer &buf) const {
  reqtimestamp.Pack(buf);
  changetimestamp.Pack(buf);
  buf.Pack(period_usec);
  return 0;
}

int LoadMeasurementConfigurationReply::Unpack(Buffer &buf) {
  reqtimestamp.Unpack(buf);
  changetimestamp.Unpack(buf);
  buf.Unpack(period_usec);
  return 0;
}

void LoadMeasurementConfigurationReply::Print(FILE *out) const {
  fprintf(out,"LoadMeasurementConfigurationReply reqtimestamp=%f changetimestamp=%f period=%d usec\n",
	  (double)reqtimestamp,(double)changetimestamp, period_usec);
}

ostream &LoadMeasurementConfigurationReply::Print(ostream &os) const {
  os<<"LoadMeasurementConfigurationReply(reqtimestamp="<<reqtimestamp
    <<", changetimestamp="<<changetimestamp<<", period_usec="<<period_usec<<")";
  return os;
}
	  

ostream &LoadMeasurementConfigurationReply::operator<<(ostream &os) const
{
  return Print(os);
}
