#ifndef _WaveletInterface
#define _WaveletInterface

#include <iostream>
#include <stdio.h>
#include "ioutil.h"

#include "Serializable.h"
#include "TimeStamp.h"
#include "Measurement.h"
#include "waveletinfo.h"
#include "waveletsample.h"
#include "waveletsampleblock.h"


#define WAVELET_MAX_BLOCK_LEN 65536


enum WaveletRepresentationType { WAVELET_DOMAIN, TIME_DOMAIN, FREQUENCY_DOMAIN };
enum WaveletTransformDirection { FORWARD, REVERSE };
enum WaveletBlockEncodingType  { PREORDER, INORDER, POSTORDER};

ostream & operator<<(ostream &os, const WaveletType &x);
ostream & operator<<(ostream &os, const WaveletRepresentationType &x);
ostream & operator<<(ostream &os, const WaveletTransformDirection &x);
ostream & operator<<(ostream &os, const WaveletBlockEncodingType &x);

//
// This defines the type of the representation
//
// If rtype=TIME_DOMAIN, then wtype and levels are meaningless
// if rtype=FREQUENCY_DOMAIN, then wtype and levels are meaningless
// if rtype=WAVELET_DOMAIN, then wtype and levels have meaning
// 
// period_usec is the period of the underlying signal
//
// Levels are counted from signal/2 upwards
// level 0 is signal/2, level 1 is signal/4, etc.
//
// if levels=0, this implies streaming.
struct WaveletRepresentationInfo : public SerializeableInfo {
  WaveletRepresentationType rtype;
  WaveletType               wtype;
  unsigned                  levels;
  unsigned                  period_usec;
  
  WaveletRepresentationInfo();
  WaveletRepresentationInfo(const WaveletRepresentationType dir,
			    const WaveletType wt,
			    const unsigned levels,
			    const unsigned period_us);
  WaveletRepresentationInfo(const WaveletRepresentationInfo &rhs);
  virtual ~WaveletRepresentationInfo();

  virtual WaveletRepresentationInfo & operator = (const WaveletRepresentationInfo &rhs);
  
  int GetPackedSize() const ;
  int GetMaxPackedSize() const ;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;
  
  void Print(FILE *out=stdout) const;
  ostream & Print(ostream &os) const;
};


//
// This represents a single sample (for streaming)
// if rinfo.rtype==TIME_DOMAIN, then index is the index of the sample within the stream
//   in time order
// if rinfo.rtype==FREQ_DOMAIN, then index is the index of the sample within the stream
//   in frequency order
// if rinfo.rtype==WAVELET_DOMAIN, then index and level are interpreted in terms of 
//   the remaining fields of rinfo
//
//
struct WaveletIndividualSample : public SerializeableInfo {
  unsigned tag;
  TimeStamp timestamp;
  WaveletRepresentationInfo rinfo;
  unsigned index;
  unsigned level;
  double   value;

  WaveletIndividualSample();
  WaveletIndividualSample(const unsigned tagval,
			  const TimeStamp &ts,
			  const WaveletRepresentationInfo &rinf,
			  const unsigned index,
			  const unsigned level,
			  const double val);
  WaveletIndividualSample(const WaveletIndividualSample &rhs);
    
  virtual ~WaveletIndividualSample();
  virtual WaveletIndividualSample & operator= (const WaveletIndividualSample &rhs);
  
  void PutAsMeasurement(Measurement &m) const;
  void GetFromMeasurement(const Measurement &m);

  void PutAsWaveletInputSample(WaveletInputSample &m) const;
  void GetFromWaveletInputSample(const WaveletInputSample &m);

  void PutAsWaveletOutputSample(WaveletOutputSample &m) const;
  void GetFromWaveletOutputSample(const WaveletOutputSample &m);

  int GetPackedSize() const;
  int GetMaxPackedSize() const;
  int Pack(Buffer &buf) const;
  int Unpack(Buffer &buf);

  void Print(FILE *out=stdout) const ;
  ostream & Print(ostream &os) const;
};


//
// This is a block of samples.  One must look at rinfo to understand how to interpret it.
//
// rinfo.rtype=TIME_DOMAIN => this is the same as a Measurement (values in time order)
// rinfo.rtype=FREQUENCY_DOMAIN => freq domain values in frequency order
// rinfo.type=WAVLET_DOMAIN => look at rest of rinfo to figure out how to 
//   interpret series.
struct WaveletBlock : public SerializeableInfo {
  unsigned tag;
  TimeStamp timestamp;
  WaveletRepresentationInfo rinfo;
  WaveletBlockEncodingType  btype;
  int       serlen;
  double    *series;
  
  WaveletBlock(const int len=0);
  WaveletBlock(const unsigned tagval,
	       const TimeStamp &ts,
	       const WaveletRepresentationInfo &rinf,
	       const WaveletBlockEncodingType  btype,
	       const int len,
	       const double *data);
  WaveletBlock(const WaveletBlock &rhs);
  virtual ~WaveletBlock();
  virtual WaveletBlock & operator=(const WaveletBlock &rhs);

  int Resize(int len, bool copy=true);
  int SetSeries(double *ser, int len);

  void PutAsMeasurement(Measurement &m) const;
  void GetFromMeasurement(const Measurement &m);

  void PutAsWaveletInputSampleBlock(WaveletInputSampleBlock &m) const;
  void GetFromWaveletInputSampleBlock(const WaveletInputSampleBlock &m);

  void PutAsWaveletOutputSampleBlock(WaveletOutputSampleBlock &m) const;
  void GetFromWaveletOutputSampleBlock(const WaveletOutputSampleBlock &m);

  int GetPackedSize() const;
  int GetMaxPackedSize() const;
  int Pack(Buffer &buf) const;
  int Unpack(Buffer &buf);

  void Print(FILE *out=stdout) const;
  ostream & Print(ostream &os) const;
};

//
struct WaveletTransformRequestType : public SerializeableInfo {
  WaveletTransformDirection direction;
  WaveletRepresentationInfo rinfoin;
  WaveletBlockEncodingType  bin;
  WaveletRepresentationInfo rinfoout;
  WaveletBlockEncodingType  bout;
  
  WaveletTransformRequestType();
  WaveletTransformRequestType(const WaveletTransformDirection dir,
			      const WaveletRepresentationInfo &tin,
			      const WaveletBlockEncodingType  bi,
			      const WaveletBlockEncodingType  bo,
			      const WaveletRepresentationInfo &tout);
  WaveletTransformRequestType(const WaveletTransformRequestType &rhs);
  virtual ~WaveletTransformRequestType();

  virtual WaveletTransformRequestType & operator = (const WaveletTransformRequestType &rhs);
  
  int GetPackedSize() const ;
  int GetMaxPackedSize() const ;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;
  
  void Print(FILE *out=stdout) const;
  ostream & Print(ostream &os) const;
};

//
// A block request and response are identical.
// A client should fill in ttype, timein, and the block
// The server will rewrite the block, ttype, and timeout
//
struct WaveletTransformBlockRequestResponse : public SerializeableInfo {
  unsigned             tag;
  WaveletTransformRequestType ttype;
  TimeStamp            timein;
  TimeStamp            timeout;
  WaveletBlock         block;

  WaveletTransformBlockRequestResponse();
  WaveletTransformBlockRequestResponse(const unsigned tagval,
			      const WaveletTransformRequestType &tt,
			      const TimeStamp  &timein,
			      const TimeStamp  &timeout,
			      const WaveletBlock &data);
  WaveletTransformBlockRequestResponse(const WaveletTransformBlockRequestResponse &right);

  virtual ~WaveletTransformBlockRequestResponse();

  virtual WaveletTransformBlockRequestResponse & operator = (const WaveletTransformBlockRequestResponse &right) ;

  int GetPackedSize() const;
  int GetMaxPackedSize() const;
  int Pack(Buffer &buf) const;
  int Unpack(Buffer &buf);

  void Print(FILE *out=stdout) const;
  ostream & Print(ostream &os) const;
};

typedef WaveletTransformBlockRequestResponse WaveletTransformBlockRequest;
typedef WaveletTransformBlockRequestResponse WaveletTransformBlockResponse;



#endif
