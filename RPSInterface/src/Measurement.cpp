#include <string.h>
#include <stdlib.h>

#include "Measurement.h"


#include "junk.h"

Measurement::Measurement(int len) 
{
  tag=0;
  serlen=0; period_usec=0; series=0; 
  Resize(len,false);
}
 

Measurement::Measurement(TimeStamp &ts,
			 int      serlen, 
			 double   *series,
			 unsigned period_usec)
{
  tag=0;
  this->serlen=0;
  this->series=0;
  this->period_usec=period_usec;
  this->timestamp = ts;
  SetSeries(series,serlen);
}

Measurement::Measurement(const Measurement &right) 
{
  tag=right.tag;
  timestamp = right.timestamp;
  period_usec=right.period_usec;
  serlen=0; series=0;
  SetSeries(right.series,right.serlen);
}

Measurement::~Measurement() 
{ 
  if (series) { delete [] series;}
  series=0;
}


Measurement & Measurement::operator = (const Measurement &right) 
{
  this->tag = right.tag;
  this->timestamp = right.timestamp;
  this->period_usec = right.period_usec;
  SetSeries(right.series,right.serlen);
  return *this;
}

  

int Measurement::Resize(int len, bool copy) 
{
  double *newser;
  if (len>0) {
    newser = new double [len];
  } else {
    newser = 0;
  }
  if (series && copy) {
    memcpy(newser,series,sizeof(double)*MIN(len,serlen));
  }
  if (series) {
    delete [] series;
  }
  series = newser;
  serlen=len;
  return 0;
}

int Measurement::SetSeries(double *ser, unsigned len) 
{ 
  Resize(len,false);
  memcpy(series,ser,len*sizeof(double));
  return 0;
}


int Measurement::GetPackedSize() const 
{
  return 4+timestamp.GetPackedSize() + 4+4+serlen*8;
}

int Measurement::GetMaxPackedSize() const 
{
  return 4+timestamp.GetMaxPackedSize() + 4+4+MEASUREMENT_MAXSERLEN*8;
}

int Measurement::Pack(Buffer &buf) const 
{
  buf.Pack(tag);
  timestamp.Pack(buf);
  buf.Pack(period_usec);
  buf.Pack(serlen);
  buf.Pack(series,serlen);
  return 0;
}


int Measurement::Unpack(Buffer &buf) 
{
  buf.Unpack(tag);
  timestamp.Unpack(buf);
  buf.Unpack(period_usec);
  buf.Unpack(serlen);
  Resize(serlen,false);
  buf.Unpack(series,serlen);
  return 0;
}

void Measurement::Print(FILE *out) 
{
  int i;
  for (i=0;i<serlen;i++) { 
    fprintf(out,"%u\t%f\t%f\t%s\n",
	    tag,
	    (double)(timestamp) + (1.0e-6*i)*period_usec,
	    series[i],
	    i==0 ? "*" : "");
  }
}

