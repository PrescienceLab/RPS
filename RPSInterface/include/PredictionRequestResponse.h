#ifndef _PredictionRequestResponse
#define _PredictionRequestResponse

#include <stdio.h>

#include "Serializable.h"
#include "ModelInfo.h"
#include "TimeStamp.h"

#define MAX_SERIES_LENGTH 3600


#define PREDREQ_FLAG_NONE     0
struct PredictionRequest : public SerializeableInfo {
  unsigned  tag;    // unique tag that will be copied to response
  unsigned  flags;
  ModelInfo modelinfo;
  TimeStamp datatimestamp;
  unsigned  period_usec;
  int       numsteps;
  int       serlen;
  double   *series;


  PredictionRequest(unsigned len=0);
  PredictionRequest(ModelInfo &mi, 
		    TimeStamp &ts,
		    int      serlen, 
		    double   *series,
		    unsigned period_usec,
		    int       numsteps);
  PredictionRequest(const PredictionRequest &right);
  virtual ~PredictionRequest() ;

  virtual PredictionRequest & operator = (const PredictionRequest &right);


  int Resize(int len, bool copy=true);
  int SetSeries(double *ser, unsigned len);
  void SetRandomTag();

  int GetPackedSize() const ;
  int GetMaxPackedSize() const ;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;

  void Print(FILE *out=stdout) const;
};


#define PREDFLAG_OK    0
#define PREDFLAG_FAIL  1
#define PREDFLAG_IPADX 2

#define PREDICTION_MAX_NUMSTEPS 60

struct PredictionResponse : public SerializeableInfo {
  unsigned tag;             // To match a response to a request
  unsigned flags;           // flags
  TimeStamp datatimestamp;  // Timestamp of data used in pred
  TimeStamp predtimestamp;  // Timestamp of prediction
  ModelInfo modelinfo;           // Model info
  unsigned  period_usec;          // Period of data and predictions
  int     numsteps;              // number of prediction steps
  double *preds;                 // predictions
  double *errs;                  // errors


  PredictionResponse(unsigned period_usec=0, int numsteps=0);
  PredictionResponse(const PredictionResponse &right);
  virtual ~PredictionResponse() ;

  virtual PredictionResponse & operator = (const PredictionResponse &right);

  void Clone(const PredictionResponse &right);

  int Resize(int numsteps, bool copy=true);

  int GetPackedSize() const;
  int GetMaxPackedSize() const;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;

  void Print(FILE *out=stdout) const ;
};




/*
#define PREDRECONFREQ_FLAG_NONE    0
#define PREDRECONFREQ_FLAG_NULL    1
struct PredictionReconfigurationRequest : public SerializableInfo {
  unsigned  tag;                 // unique tag that will be copied to response
  unsigned  flags;               //
  ModelInfo modelinfo;
  TimeStamp timestamp;
  int       numsteps;
  

  PredictionReconfigurationRequest(unsigned tag,
 				   ModelInfo *mi,
 				   unsigned period_usec,
 				   int numsteps);
  PredictionReconfigurationRequest(const PredictionReconfigurationRequest &right);
  virtual ~PredictionReconfigurationRequest() ;

  virtual PredictionReconfigurationRequest & operator = (const PredictionReconfigurationRequest &right);

  int GetPackedSize() const;
  int GetMaxPackedSize() const;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;

  void MakeNullRequest();
  bool IsNullRequest();
};
*/

// the *semantics of numsteps are different
// here it is the horizon for all future predictions that
// are clocked by measurements
//struct PredictionReconfigurationRequest : public PredictionRequest {};
#define PredictionReconfigurationRequest PredictionRequest

struct PredictionReconfigurationResponse : public SerializeableInfo {
  unsigned  tag;
  unsigned  flags;
  ModelInfo modelinfo;           // Model info
  TimeStamp requesttimestamp;         //
  TimeStamp reconfigdonetimestamp;    //
  unsigned  period_usec;          // Period of data and predictions
  int       numsteps;              // number of prediction steps

  PredictionReconfigurationResponse(unsigned period_usec=0, int numsteps=0);
  PredictionReconfigurationResponse(const PredictionReconfigurationResponse &right);
  virtual ~PredictionReconfigurationResponse() ;

  virtual PredictionReconfigurationResponse & operator = (const PredictionReconfigurationResponse &right);

  virtual void MakeMatchingResponse(const PredictionReconfigurationRequest &req);


  int GetPackedSize() const;
  int GetMaxPackedSize() const;
  int Pack(Buffer &buf) const ;
  int Unpack(Buffer &buf) ;

  void Print(FILE *out=stdout) const ;
};


#endif


