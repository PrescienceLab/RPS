#ifdef WIN32
#include <windows.h>
#else
#include <arpa/inet.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "LoadMeasurement.h"
#include "socks.h"

#if XML
#include "xmlparse.h"  // expat
#endif

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


void LoadMeasurement::Print(FILE *out) 
{
  struct in_addr ia; ia.s_addr=ipaddress;
  
  struct hostent *he=gethostbyaddr((const char *)&(ia),
				   sizeof(ia),
				   AF_INET);
  
  double hz = 1.0/(1.0e-6*((double)(period_usec)));
  
  fprintf(out,"%-40s (%s %8.5f Hz) @ %f: %f\t[%f\t%f\t%f]\n",
	  he ? he->h_name : inet_ntoa(ia),
	  smoothingtype == SMOOTH_MACH ? "MACH" :
	  (smoothingtype == SMOOTH_UNIX ? "UNIX" : "????"),
	  hz,
	  (double)timestamp,
	  unsmoothed,
	  avgs[0],avgs[1],avgs[2]);
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


#if XML

const char *xml_hostload_template = 
"<?xml version=\"1.0\"?>\n"
"<!DOCTYPE HostLoadList [\n"
"   <!ELEMENT HostLoadList (HostLoadInfo)+>\n"
"   <!ELEMENT HostLoadInfo (Hostname, Period, SmoothingType, TimeStamp, Averages)>\n"
"   <!ELEMENT Period (#PCDATA)>\n"
"   <!ELEMENT SmoothingType (unix|mach)>\n"
"   <!ELEMENT TimeStamp (#PCDATA)>\n"
"   <!ELEMENT Averages (Average+)>\n"
"   <!ELEMENT Average (#PCDATA)>\n"
"      <!ATTLIST Average Number CDATA \"0\">\n"
"]>\n"
"<HostLoadList>\n"
"   <HostLoadInfo>\n" 
"      <Hostname>%s</Hostname>\n"
"      <Period>%d</Period>\n"
"      <SmoothingType>%d</SmoothingType>\n"
"      <TimeStamp>%lf</TimeStamp>\n"
"      <Averages>\n"
"         <Average Number=\"%d\">%lf</Average>\n"
"         <Average Number=\"%d\">%lf</Average>\n"
"         <Average Number=\"%d\">%lf</Average>\n"
"         <Average Number=\"%d\">%lf</Average>\n"
"      </Averages>\n"
"   </HostLoadInfo>\n"
"</HostLoadList>\n";


#define MAX_HOSTNAME_CHARS  80
#define MAX_NUM_CHARS 20
#define MAX_AVG_CHARS 20
#define MAX_TS_CHARS  20
#define MAX_SMOOTH_CHARS 20
#define MAX_PERIOD_CHARS 20


int LoadMeasurement::GetXMLPackedSize() const
{
  return strlen(xml_hostload_template)+
    MAX_HOSTNAME_CHARS+MAX_TS_CHARS+MAX_SMOOTH_CHARS+MAX_PERIOD_CHARS+4*(MAX_AVG_CHARS+MAX_NUM_CHARS);
}

int LoadMeasurement::GetMaxXMLPackedSize() const
{
  return GetXMLPackedSize();
}

int LoadMeasurement::PackToXML(Buffer &buf) const
{
  char *temp = new char [GetMaxXMLPackedSize()];
  char hn[MAX_HOSTNAME_CHARS];
  IPToHostname(ipaddress,hn,MAX_HOSTNAME_CHARS);
  sprintf(temp,xml_hostload_template,
	  hn,
	  period_usec,
	  smoothingtype,
	  (double)timestamp,
	  -1, unsmoothed,
	  0,avgs[0],
	  1,avgs[1],
	  2,avgs[2]);
  //  fprintf(stderr, "generated this xml:\n%s\n",temp);
  buf.Pack(temp,strlen(temp)+1);
  delete [] temp;
  return 0;
}


#define CDATA_BUFSIZE 65536

static int  LoadMeasurementXmlCDataNext;
static char LoadMeasurementXmlCDataBuffer[CDATA_BUFSIZE];

static enum {None,HostLoadList,HostLoadInfo,Hostname,Period,SmoothingType,Timestamp,Averages,Average,Done} LoadMeasurementXmlState; 
static int LoadMeasurementXmlNum;

void LoadMeasurementXmlStartElement(void *object, 
				    const char *name,
				    const char **atts)
{
  LoadMeasurement *lm = (LoadMeasurement *) object;

  //fprintf(stderr,"start element %s\n",name);
  
  if (!strcmp(name,"HostLoadList")) { 
    LoadMeasurementXmlState=HostLoadList;
  } else if (!strcmp(name,"HostLoadInfo")) { 
    LoadMeasurementXmlState=HostLoadInfo;
  } else if (!strcmp(name,"Hostname")) { 
    LoadMeasurementXmlState=Hostname;
  } else if (!strcmp(name,"TimeStamp")) { 
    LoadMeasurementXmlState=Timestamp;
  } else if (!strcmp(name,"Period")) { 
    LoadMeasurementXmlState=Period;
  } else if (!strcmp(name,"SmoothingType")) { 
    LoadMeasurementXmlState=SmoothingType;
  } else if (!strcmp(name,"Averages")) { 
    LoadMeasurementXmlState=Averages;
  } else if (!strcmp(name,"Average")) { 
    LoadMeasurementXmlState=Average;
    int i;
    for (i=0;atts[i]!=0;i+=2) { 
      if (!strcmp(atts[i],"Number")) {
	LoadMeasurementXmlNum = atoi(atts[i+1]);
	//fprintf(stderr,"Number is now %d\n",LoadMeasurementXmlNum);
	break;
      }
    }
  }
  LoadMeasurementXmlCDataNext=0;
}

void LoadMeasurementXmlEndElement(void *object, 
				  const char *name)
{
  LoadMeasurement *lm = (LoadMeasurement *) object;
  
  LoadMeasurementXmlCDataBuffer[LoadMeasurementXmlCDataNext]=0;
  
  //fprintf(stderr,"CDATA : '%s'\n",LoadMeasurementXmlCDataBuffer);

  switch (LoadMeasurementXmlState) {
  case Hostname: 
    lm->ipaddress=ntohl(ToIPAddress(LoadMeasurementXmlCDataBuffer));
    break;
  case Period: 
    lm->period_usec=atoi(LoadMeasurementXmlCDataBuffer);
    break;
  case SmoothingType: 
    lm->smoothingtype=atoi(LoadMeasurementXmlCDataBuffer);
    break;
  case Timestamp: {
    double t = atof(LoadMeasurementXmlCDataBuffer);
    lm->timestamp = TimeStamp((int)t,(int)((t-(double)((int)t))*1e6));
  } 
  break;
  case Average: 
    if (LoadMeasurementXmlNum<0) { 
      lm->unsmoothed=atof(LoadMeasurementXmlCDataBuffer);
    } else {
      lm->avgs[LoadMeasurementXmlNum]=atof(LoadMeasurementXmlCDataBuffer);
    }
    break;
  }
  
  //fprintf(stderr,"End element %s\n",name);

  if (!strcmp(name,"HostLoadList")) { 
    LoadMeasurementXmlState=Done;
  } else if (!strcmp(name,"HostLoadInfo")) { 
    LoadMeasurementXmlState=HostLoadList;
  } else if (!strcmp(name,"Hostname")) { 
    LoadMeasurementXmlState=HostLoadInfo;
  } else if (!strcmp(name,"TimeStamp")) { 
    LoadMeasurementXmlState=HostLoadInfo;
  } else if (!strcmp(name,"Period")) { 
    LoadMeasurementXmlState=HostLoadInfo;
  } else if (!strcmp(name,"SmoothingType")) { 
    LoadMeasurementXmlState=HostLoadInfo;
  } else if (!strcmp(name,"Averages")) { 
    LoadMeasurementXmlState=HostLoadInfo;
  } else if (!strcmp(name,"Average")) { 
    LoadMeasurementXmlState=Averages;
  }
}


//void LoadMeasurementXmlCDataStart(void *object)
//{
//  fprintf(stderr,"start cdata\n");
//   LoadMeasurementXmlCDataNext=0;
//}



void LoadMeasurementXmlCData(void *object, const XML_Char *s, int len)
{
  if (LoadMeasurementXmlCDataNext+len >= (CDATA_BUFSIZE-1)) { 
    fprintf(stderr,"out of memory for xml parse\n");
    exit(-1);
  }
  strncpy(&(LoadMeasurementXmlCDataBuffer[LoadMeasurementXmlCDataNext]),
	  s,len);
  LoadMeasurementXmlCDataNext+=len;
  LoadMeasurementXmlCDataBuffer[LoadMeasurementXmlCDataNext]=0;
  //  fprintf(stderr,"cdata buffer: %s\n",LoadMeasurementXmlCDataBuffer);
}

// void LoadMeasurementXmlCDataEnd(void *object)
// {
//   LoadMeasurement *lm = (LoadMeasurement *) object;

//   LoadMeasurementXmlCDataBuffer[LoadMeasurementXmlCDataNext]=0;
  
//   fprintf(stderr,"end cdata\n");
  
//   fprintf(stderr,"CDATA : '%s'\n",LoadMeasurementXmlCDataBuffer);

//   switch (LoadMeasurementXmlState) {
//   case Hostname: 
//     lm->ipaddress=ToIPAddress(LoadMeasurementXmlCDataBuffer);
//     break;
//   case Period: 
//     lm->period_usec=atoi(LoadMeasurementXmlCDataBuffer);
//     break;
//   case SmoothingType: 
//     lm->smoothingtype=atoi(LoadMeasurementXmlCDataBuffer);
//     break;
//   case Timestamp: {
//     double t = atof(LoadMeasurementXmlCDataBuffer);
//     lm->timestamp = TimeStamp((int)t,(int)((t-(double)((int)t))*1e6));
//   } 
//   break;
//   case Average: 
//     if (LoadMeasurementXmlNum<0) { 
//       lm->unsmoothed=atof(LoadMeasurementXmlCDataBuffer);
//     } else {
//       lm->avgs[LoadMeasurementXmlNum]=atof(LoadMeasurementXmlCDataBuffer);
//     }
//     break;
//   }
// }


int LoadMeasurement::UnpackFromXML(Buffer &buf)
{
  XML_Parser parser = XML_ParserCreate(NULL);

  XML_SetUserData(parser, this);
  XML_SetElementHandler(parser,
			LoadMeasurementXmlStartElement,
			LoadMeasurementXmlEndElement);
  XML_SetCharacterDataHandler(parser,
			      LoadMeasurementXmlCData);

  //  XML_SetCdataSectionHandler(parser,
  //		     LoadMeasurementXmlCDataStart,
  //		     LoadMeasurementXmlCDataEnd);
  
  int i;
  char nextchar;
  int done = 0;

  LoadMeasurementXmlState=None;
  LoadMeasurementXmlCDataNext=0;

  do {
    if (LoadMeasurementXmlState==Done) { 
      nextchar='\n';
    } else {
      buf.Unpack(nextchar);
    }
    if (!XML_Parse(parser, &nextchar, 1, LoadMeasurementXmlState==Done)) {
      fprintf(stderr,
              "%s at line %d\n",
              XML_ErrorString(XML_GetErrorCode(parser)),
              XML_GetCurrentLineNumber(parser));
      XML_ParserFree(parser);
      return -1;
    }
  } while (LoadMeasurementXmlState!=Done);
  
  XML_ParserFree(parser);
}
			
#endif  
			

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

void LoadMeasurementConfigurationRequest::Print(FILE *out) {
  fprintf(out,"LoadMeasurementConfigurationRequest timetamp=%lf period=%d usec\n",
	  (double)timestamp, period_usec);
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

void LoadMeasurementConfigurationReply::Print(FILE *out) {
  fprintf(out,"LoadMeasurementConfigurationReply reqtimestamp=%lf changetimestamp=%lf period=%d usec\n",
	  (double)reqtimestamp,(double)changetimestamp, period_usec);
}
	  
