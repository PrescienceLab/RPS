#include <new>
#include "WaveletInterface.h"


ostream & operator<<(ostream &os, const WaveletType &x)
{
  os <<  "WaveletType(" << 
    (x==DAUB2 ? "DAUB2" :
    x==DAUB4 ? "DAUB4" :
    x==DAUB6 ? "DAUB6" :
    x==DAUB8 ? "DAUB8" :
    x==DAUB10 ? "DAUB10" :
    x==DAUB12 ? "DAUB12" :
    x==DAUB14 ? "DAUB14" :
    x==DAUB16 ? "DAUB16" :
    x==DAUB18 ? "DAUB18" :
    x==DAUB20 ? "DAUB20" : "UNKNOWN") << ")";
  return os;
}

ostream & operator<<(ostream &os, const WaveletRepresentationType &x)
{
  os << "WaveletRepresentationType(" <<
    (x==WAVELET_DOMAIN_TRANSFORM ? "WAVELET_DOMAIN_TRANSFORM" :
    x==WAVELET_DOMAIN_APPROX ? "WAVELET_DOMAIN_APPROX" :
    x==WAVELET_DOMAIN_DETAIL ? "WAVELET_DOMAIN_DETAIL" :
    x==TIME_DOMAIN ? "TIME_DOMAIN" :
     x==FREQUENCY_DOMAIN ? "FREQUENCY_DOMAIN" : "UNKNOWN") << ")";
  return os;
}

ostream & operator<<(ostream &os, const WaveletTransformDirection &x)
{
  os << "WaveletTransformDirection("<<
    (x==WAVELET_FORWARD ? "FORWARD" :
     x==WAVELET_REVERSE ? "REVERSE" : "UNKNOWN")<<")";
  return os;
}

ostream & operator<<(ostream &os, const WaveletBlockEncodingType &x)
{
  os << "WaveletBlockEncodingType("<<
    (x==PREORDER ? "PREORDER" :
     x==INORDER ? "INORDER" :
     x==POSTORDER ? "POSTORDER" : "UNKNOWN")<<")";
  return os;
}

WaveletRepresentationInfo::WaveletRepresentationInfo() : 
  rtype(TIME_DOMAIN), wtype(DAUB2),  levels(0), period_usec(0)
{}

WaveletRepresentationInfo::WaveletRepresentationInfo(const WaveletRepresentationType rt,
						     const WaveletType wt,
						     const unsigned l,
						     const unsigned per) :
  rtype(rt), wtype(wt), levels(l), period_usec(per)
{}

WaveletRepresentationInfo::WaveletRepresentationInfo(const WaveletRepresentationInfo &rhs) :
  rtype(rhs.rtype), wtype(rhs.wtype), levels(rhs.levels), period_usec(rhs.period_usec)
{}

WaveletRepresentationInfo::~WaveletRepresentationInfo()
{}

WaveletRepresentationInfo & WaveletRepresentationInfo::operator = (const WaveletRepresentationInfo &rhs)
{
  this->~WaveletRepresentationInfo();
  return *(new (this) WaveletRepresentationInfo(rhs));
}

int WaveletRepresentationInfo::GetPackedSize() const 
{
  return sizeof(rtype)+sizeof(wtype)+sizeof(levels)+sizeof(period_usec);
}

int WaveletRepresentationInfo::GetMaxPackedSize() const 
{
  return GetPackedSize();
}

int WaveletRepresentationInfo::Pack(Buffer &buf) const 
{
  buf.Pack((int&)rtype);
  buf.Pack((int&)wtype);
  buf.Pack(levels);
  buf.Pack(period_usec);
  return 0;
}

int WaveletRepresentationInfo::Unpack(Buffer &buf) 
{
  buf.Unpack((int&)rtype);
  buf.Unpack((int&)wtype);
  buf.Unpack(levels);
  buf.Unpack(period_usec);
  return 0;
}

void WaveletRepresentationInfo::Print(FILE *out) const
{
  fprintf(out,"WaveletRepresentationInfo: rtype=%s, wtype=%s, levels=%u, period_usec=%u\n",
	  rtype==WAVELET_DOMAIN_TRANSFORM ? "WAVELET_DOMAIN_TRANSFORM" :
	  rtype==WAVELET_DOMAIN_APPROX ? "WAVELET_DOMAIN_APPROX" :
	  rtype==WAVELET_DOMAIN_DETAIL ? "WAVELET_DOMAIN_DETAIL" :
	  rtype==TIME_DOMAIN ? "TIME_DOMAIN" :
	  rtype==FREQUENCY_DOMAIN ? "FREQUENCY_DOMAIN" : "UNKNOWN",
	  wtype==DAUB2 ? "DAUB2" :
	  wtype==DAUB4 ? "DAUB4" :
	  wtype==DAUB6 ? "DAUB6" :
	  wtype==DAUB8 ? "DAUB8" :
	  wtype==DAUB10 ? "DAUB10" :
	  wtype==DAUB12 ? "DAUB12" :
	  wtype==DAUB14 ? "DAUB14" :
	  wtype==DAUB16 ? "DAUB16" :
	  wtype==DAUB18 ? "DAUB18" :
	  wtype==DAUB20 ? "DAUB20" : "UNKNOWN",
	  levels, period_usec);
}

ostream& WaveletRepresentationInfo::Print(ostream &os) const
{
  os << "WaveletRepresentationInfo(rtype="<<rtype<<", wtype="<<wtype
     <<", levels="<<levels<<", period_usec="<<period_usec<<")";
  return os;
}


ostream& WaveletRepresentationInfo::operator<<(ostream &os) const
{
  return Print(os);
}


WaveletIndividualSample::WaveletIndividualSample() : 
  tag(0), timestamp(TimeStamp(0)), index(0), level(0), value(0.0)
{}

WaveletIndividualSample::WaveletIndividualSample(const unsigned tagval,
						 const TimeStamp &ts,
						 const WaveletRepresentationInfo &rinf,
						 const unsigned idx,
						 const unsigned lev,
						 const double val) :
  tag(tagval), timestamp(ts), rinfo(rinf), index(idx), level(lev), value(val)
{}


WaveletIndividualSample::WaveletIndividualSample(const WaveletIndividualSample &rhs) :
  tag(rhs.tag), timestamp(rhs.timestamp), rinfo(rhs.rinfo), 
  index(rhs.index), level(rhs.level),  value(rhs.value)
{}


WaveletIndividualSample::~WaveletIndividualSample()
{}

WaveletIndividualSample & WaveletIndividualSample::operator= (const WaveletIndividualSample &rhs)
{
  return *(new (this) WaveletIndividualSample(rhs));
}

void WaveletIndividualSample::PutAsMeasurement(Measurement &m) const
{
  m.tag=tag;
  m.timestamp=timestamp;
  m.period_usec=rinfo.period_usec;
  m.Resize(1,false);
  m.series[0]=value;
}

void WaveletIndividualSample::GetFromMeasurement(const Measurement &m)
{
  tag=m.tag;
  timestamp=m.timestamp;
  rinfo=WaveletRepresentationInfo(TIME_DOMAIN,DAUB2,0,m.period_usec);
  index=0;
  level=0;
  if (m.serlen>=1) {
    value=m.series[0];
  } else {
    value=0.0;
  }
}

void WaveletIndividualSample::PutAsWaveletInputSample(WaveletInputSample<double> &m) const
{
  m.SetSampleValue(value);
  m.SetSampleIndex(index);
}

void WaveletIndividualSample::GetFromWaveletInputSample(const WaveletInputSample<double> &m)
{
  assert(rinfo.rtype==TIME_DOMAIN);
  index=m.GetSampleIndex();
  value=m.GetSampleValue();
  level=0;
  rinfo=WaveletRepresentationInfo(TIME_DOMAIN,DAUB2,0,1);
}

void WaveletIndividualSample::PutAsWaveletOutputSample(WaveletOutputSample<double> &m) const
{
  assert(rinfo.rtype==WAVELET_DOMAIN_APPROX || rinfo.rtype==WAVELET_DOMAIN_DETAIL || rinfo.rtype==WAVELET_DOMAIN_TRANSFORM);
  m.SetSampleLevel(level);
  m.SetSampleValue(value);
  m.SetSampleIndex(index);
}

void WaveletIndividualSample::GetFromWaveletOutputSample(const WaveletOutputSample<double> &m)
{
  index=m.GetSampleIndex();
  level=m.GetSampleLevel();
  value=m.GetSampleValue();
}

int WaveletIndividualSample::GetPackedSize() const
{
  return sizeof(tag)+timestamp.GetPackedSize()+rinfo.GetPackedSize()+
    sizeof(index)+sizeof(level)+sizeof(value);
}

int WaveletIndividualSample::GetMaxPackedSize() const
{
  return sizeof(tag)+timestamp.GetMaxPackedSize()+rinfo.GetMaxPackedSize()+
    sizeof(index)+sizeof(level)+sizeof(value);
}

int WaveletIndividualSample::Pack(Buffer &buf) const
{
  buf.Pack(tag);
  timestamp.Pack(buf);
  rinfo.Pack(buf);
  buf.Pack(index);
  buf.Pack(level);
  buf.Pack(value);
  return 0;
}

int WaveletIndividualSample::Unpack(Buffer &buf)
{
  buf.Unpack(tag);
  timestamp.Unpack(buf);
  rinfo.Unpack(buf);
  buf.Unpack(index);
  buf.Unpack(level);
  buf.Unpack(value);
  return 0;
}

void WaveletIndividualSample::Print(FILE *out) const
{
  fprintf(out,"WaveletIndividualSample: tag=%u, timestamp=%f, rinfo=", tag,(double)timestamp);
  rinfo.Print(out);
  fprintf(out,", index=%u, level=%u, value=%f\n",index,level,value);
}

ostream & WaveletIndividualSample::Print(ostream &os) const
{
  os<<"WaveletIndividualSample(tag="<<tag<<", timestamp="<<timestamp
    <<", rinfo="<<rinfo<<", index="<<index<<", level="<<level<<", value="<<value<<")";
  return os;
}

ostream& WaveletIndividualSample::operator<<(ostream &os) const
{
  return Print(os);
}


WaveletBlock::WaveletBlock(const int len) :
  tag(0), timestamp(TimeStamp(0)), btype(PREORDER), serlen(0), series(0)
{
  Resize(len);
}
  
WaveletBlock::WaveletBlock(const unsigned tagval,
			   const TimeStamp &ts,
			   const WaveletRepresentationInfo &rt,
			   const WaveletBlockEncodingType bt,
			   const int datalen,
			   const double *data) :
  tag(tagval), timestamp(ts), rinfo(rt), btype(bt), serlen(0)
{
  Resize(datalen);
  memcpy(series,data,sizeof(double)*serlen);
}

WaveletBlock::WaveletBlock(const WaveletBlock &rhs) :
  tag(rhs.tag), timestamp(rhs.timestamp), rinfo(rhs.rinfo), btype(rhs.btype),
  serlen(rhs.serlen)
{
  Resize(serlen);
  memcpy(series,rhs.series,serlen*sizeof(double));
}
  
WaveletBlock::~WaveletBlock()
{
  if (series) { 
    delete [] series;
    series=0;
  }
}

WaveletBlock & WaveletBlock::operator=(const WaveletBlock &rhs)
{
  if (series) { 
    delete [] series;
    series=0;
  }
  serlen=0;
  return *(new (this) WaveletBlock(rhs));
}


int WaveletBlock::Resize(int len, bool copy) 
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

int WaveletBlock::SetSeries(double *ser, int len)
{
  Resize(len,false);
  memcpy(series,ser,len*sizeof(double));
  return 0;
}

void WaveletBlock::PutAsMeasurement(Measurement &m) const
{
  m.tag=tag;
  m.timestamp=timestamp;
  m.period_usec=rinfo.period_usec;
  m.Resize(serlen);
  m.SetSeries(series,serlen);
}
  
void WaveletBlock::GetFromMeasurement(const Measurement &m)
{
  tag=m.tag;
  timestamp=m.timestamp;
  rinfo=WaveletRepresentationInfo(TIME_DOMAIN,DAUB2,0,m.period_usec);
  Resize(m.serlen);
  SetSeries(m.series,m.serlen);
}

void WaveletBlock::PutAsWaveletInputSampleBlock(WaveletInputSampleBlock<wisd> &m) const
{
  assert(rinfo.rtype==TIME_DOMAIN);
  m.SetBlockIndex(0);
  m.SetSamples(series, serlen);
}

void WaveletBlock::GetFromWaveletInputSampleBlock(const WaveletInputSampleBlock<wisd> &m)
{
  assert(rinfo.rtype==TIME_DOMAIN);
  tag=m.GetBlockIndex();
  rinfo=WaveletRepresentationInfo(TIME_DOMAIN,DAUB2,0,0);
  btype=INORDER;
  serlen=m.GetBlockSize();
  Resize(serlen);
  m.GetSamples(series);
}

void WaveletBlock::PutAsWaveletOutputSampleBlock(WaveletOutputSampleBlock<wosd> &m) const
{
  assert(rinfo.rtype==WAVELET_DOMAIN_APPROX || 
	 rinfo.rtype==WAVELET_DOMAIN_DETAIL || 
	 rinfo.rtype==WAVELET_DOMAIN_TRANSFORM);
  m.SetSamples(series, serlen);
}

void WaveletBlock::GetFromWaveletOutputSampleBlock(const WaveletOutputSampleBlock<wosd> &m)
{
  assert(rinfo.rtype==WAVELET_DOMAIN_APPROX || 
	 rinfo.rtype==WAVELET_DOMAIN_DETAIL || 
	 rinfo.rtype==WAVELET_DOMAIN_TRANSFORM);
  tag=m.GetBlockIndex();
  rinfo=WaveletRepresentationInfo(rinfo.rtype,DAUB2,0,0);
  btype=INORDER;
  serlen=m.GetBlockSize();
  Resize(serlen);
  m.GetSamples(series);
}

int WaveletBlock::GetPackedSize() const
{
  return sizeof(tag) + timestamp.GetPackedSize() + rinfo.GetPackedSize() +
   sizeof(btype)+ sizeof(serlen) + ((serlen>0)? serlen*sizeof(double) : 0);
}

int WaveletBlock::GetMaxPackedSize() const
{
  return sizeof(tag) + timestamp.GetMaxPackedSize() + rinfo.GetMaxPackedSize()+
    sizeof(btype) + sizeof(serlen) + WAVELET_MAX_BLOCK_LEN*sizeof(double);
}

int WaveletBlock::Pack(Buffer &buf) const
{
  buf.Pack(tag);
  timestamp.Pack(buf);
  rinfo.Pack(buf);
  buf.Pack((int&)btype);
  buf.Pack(serlen);
  buf.Pack(series,serlen);
  return 0;
}

int WaveletBlock::Unpack(Buffer &buf)
{
  buf.Unpack(tag);
  timestamp.Unpack(buf);
  rinfo.Unpack(buf);
  buf.Unpack((int&)btype);
  int lentemp;
  buf.Unpack(lentemp);
  Resize(lentemp);
  buf.Unpack(series,serlen);
  return 0;
}

void WaveletBlock::Print(FILE *out) const
{
  int i;
  fprintf(out,"WaveletBlock: tag=%u, rinfo=",tag);
  rinfo.Print(out);
  fprintf(out,"btype=%s, serlen=%d, data follows...\n", 
	  btype==INORDER ? "INORDER" :
	  btype==PREORDER ? "PREORDER" :
	  btype==POSTORDER ? "POSTORDER" : "UNKNOWN", serlen);
  for (i=0;i<serlen;i++) { 
    fprintf(out,"%u\t%f\t%f\t%s\n",
	    tag,
	    (double)(timestamp) + (1.0e-6*i)*rinfo.period_usec,
	    series[i],
	    i==0 ? "*" : "");
  }
}

ostream & WaveletBlock::Print(ostream &os) const
{
  os<<"WaveletBlock(tag="<<tag<<", rinfo="<<rinfo
    <<", btype="<<btype<<", serlen="<<serlen<<", series=(";
  for (int i=0;i<serlen;i++) { 
    if (i>0) { 
      os << ", ";
    }
    os << series[i];
  }
  os << ")"<<")";
  return os;
}

ostream& WaveletBlock::operator<<(ostream &os) const
{
  return Print(os);
}


WaveletTransformRequestType::WaveletTransformRequestType() :
  direction(WAVELET_FORWARD)
{}

WaveletTransformRequestType::WaveletTransformRequestType(const WaveletTransformDirection dir,
							 const WaveletRepresentationInfo &tin,
							 const WaveletBlockEncodingType  bi,
							 const WaveletBlockEncodingType  bo,
							 const WaveletRepresentationInfo &tout) :
  direction(dir), rinfoin(tin), bin(bi), rinfoout(tout), bout(bo)
{}

WaveletTransformRequestType::WaveletTransformRequestType(const WaveletTransformRequestType &rhs) :
  direction(rhs.direction), rinfoin(rhs.rinfoin), bin(rhs.bin), rinfoout(rhs.rinfoout), bout(rhs.bout)
{}

WaveletTransformRequestType::~WaveletTransformRequestType()
{}

WaveletTransformRequestType & WaveletTransformRequestType::operator = (const WaveletTransformRequestType &rhs)
{
  return *(new (this) WaveletTransformRequestType(rhs));
}

int WaveletTransformRequestType::GetPackedSize() const 
{
  return sizeof(direction) + rinfoin.GetPackedSize() + sizeof(bin) + rinfoout.GetPackedSize() + sizeof(bout);
}


int WaveletTransformRequestType::GetMaxPackedSize() const 
{
  return sizeof(direction) + rinfoin.GetMaxPackedSize() + sizeof(bin) + rinfoout.GetMaxPackedSize() + sizeof(bout);
}

int WaveletTransformRequestType::Pack(Buffer &buf) const 
{
  buf.Pack((int&)direction);
  rinfoin.Pack(buf);
  buf.Pack(bin);
  rinfoout.Pack(buf);
  buf.Pack(bout);
  return 0;
}

int WaveletTransformRequestType::Unpack(Buffer &buf) 
{
  buf.Unpack((int&)direction);
  rinfoin.Unpack(buf);
  buf.Unpack((int&)bin);
  rinfoout.Unpack(buf);
  buf.Unpack((int&)bout);
  return 0;
}

void WaveletTransformRequestType::Print(FILE *out) const
{
  fprintf(out,"WaveletTransformRequestType: direction=%s, rinfoin=",
	  direction==WAVELET_FORWARD ? "FORWARD" : 
	  direction==WAVELET_REVERSE ? "REVERSE" : "UNKNOWN");
  rinfoin.Print(out);
  fprintf(out,", bin=%s, rinfoout=", 
	  bin==PREORDER ? "PREORDER" :
	  bin==INORDER ? "INORDER" :
	  bin==POSTORDER ? "POSTORDER" : "UNKNOWN");
  rinfoout.Print(out);
  fprintf(out,", bout=%s\n",
	  bout==PREORDER ? "PREORDER" :
	  bout==INORDER ? "INORDER" :
	  bout==POSTORDER ? "POSTORDER" : "UNKNOWN");

}

ostream & WaveletTransformRequestType::Print(ostream &os) const
{
  os << "WaveletTransformRequestType(direction="<<direction<<", rinfoin="
     << rinfoin << ", bin=" << bin << ", rinfoout="<<rinfoout<<", bout="<<bout<<")";
  return os;
}

ostream& WaveletTransformRequestType::operator<<(ostream &os) const
{
  return Print(os);
}



WaveletTransformBlockRequestResponse::WaveletTransformBlockRequestResponse() :
  tag(0), timein(TimeStamp(0)), timeout(TimeStamp(0))
{}

WaveletTransformBlockRequestResponse::WaveletTransformBlockRequestResponse(const unsigned tagval,
									   const WaveletTransformRequestType &tt,
									   const TimeStamp  &ti,
									   const TimeStamp  &to,
									   const WaveletBlock &data) :
  tag(tagval), ttype(tt), timein(ti), timeout(to), block(data)
{}


WaveletTransformBlockRequestResponse::WaveletTransformBlockRequestResponse(const WaveletTransformBlockRequestResponse &rhs) :
  tag(rhs.tag), ttype(rhs.ttype), timein(rhs.timein), timeout(rhs.timeout), block(rhs.block)
{}

WaveletTransformBlockRequestResponse::~WaveletTransformBlockRequestResponse()
{}

WaveletTransformBlockRequestResponse & WaveletTransformBlockRequestResponse::operator = (const WaveletTransformBlockRequestResponse &right) 
{
  return *(new (this) WaveletTransformBlockRequestResponse(right));
}

int WaveletTransformBlockRequestResponse::GetPackedSize() const
{
  return sizeof(tag)+ttype.GetPackedSize()+timein.GetPackedSize()+timeout.GetPackedSize()+block.GetPackedSize();
}
int WaveletTransformBlockRequestResponse::GetMaxPackedSize() const
{
  return sizeof(tag)+ttype.GetMaxPackedSize()+timein.GetMaxPackedSize()+timeout.GetMaxPackedSize()+block.GetMaxPackedSize();
}


int WaveletTransformBlockRequestResponse::Pack(Buffer &buf) const
{
  buf.Pack(tag);
  ttype.Pack(buf);
  timein.Pack(buf);
  timeout.Pack(buf);
  block.Pack(buf);
  return 0;
}

int WaveletTransformBlockRequestResponse::Unpack(Buffer &buf)
{
  buf.Unpack(tag);
  ttype.Unpack(buf);
  timein.Unpack(buf);
  timeout.Unpack(buf);
  block.Unpack(buf);
  return 0;
}

void WaveletTransformBlockRequestResponse::Print(FILE *out) const
{
  fprintf(out, "WaveletTransformBlockRequestResponse: tag=%u, ttype=", tag);
  ttype.Print(out);
  fprintf(out,", timein=%f, timeout=%f, block=",(double)timein,(double)timeout);
  block.Print(out);
}

ostream & WaveletTransformBlockRequestResponse::Print(ostream &os) const
{
  os<<"WaveletTransformBlockRequestResponse(tag="<<tag<<", ttype="<<ttype
    << ", timein="<<timein<<", timeout="<<timeout<<", block="<<block<<")";
  return os;
}

ostream& WaveletTransformBlockRequestResponse::operator<<(ostream &os) const
{
  return Print(os);
}


