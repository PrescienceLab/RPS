#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Serializable.h"
#include "ModelInfo.h"
#include "TimeStamp.h"
#include "junk.h"
#include "random.h"

#include "PredictionRequestResponse.h"

PredictionRequest::PredictionRequest(unsigned len) 
{
  tag=0; flags=0; serlen=0; period_usec=0; series=0; numsteps=0;
  Resize(len,false);
  SetRandomTag();
}
 

PredictionRequest::PredictionRequest(ModelInfo &mi, 
				     TimeStamp &ts,
				     int serlen, 
				     double   *series,
				     unsigned period_usec,
				     int numsteps) 
{
  SetRandomTag();
  this->flags=0;
  this->modelinfo = mi;
  this->datatimestamp = ts;
  this->period_usec=period_usec;
  this->numsteps = numsteps;
  this->series=0; this->serlen=0;
  SetSeries(series,serlen);
}

PredictionRequest::PredictionRequest(const PredictionRequest &right) 
{ 
  tag = right.tag;
  flags = right.flags;
  modelinfo = right.modelinfo;
  datatimestamp = right.datatimestamp;
  period_usec = right.period_usec;
  numsteps = right.numsteps;
  series=0; serlen=0;
  SetSeries(right.series,right.serlen);
}



PredictionRequest::~PredictionRequest() 
{ 
  CHK_DEL_MAT(series);
}


PredictionRequest & PredictionRequest::operator = (const PredictionRequest &right)
{
  CHK_DEL_MAT(series);
  tag = right.tag;
  flags = right.flags;
  modelinfo = right.modelinfo;
  datatimestamp = right.datatimestamp;
  period_usec = right.period_usec;
  numsteps = right.numsteps;
  series=0; serlen=0;
  SetSeries(right.series,right.serlen);
  return *this;
}



int PredictionRequest::Resize(int len, bool copy) 
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


int PredictionRequest::SetSeries(double *ser, unsigned len) 
{ 
  Resize(len,false);
  memcpy(series,ser,len*sizeof(double));
  return 0;
}

void PredictionRequest::SetRandomTag() 
{
  tag = SignedRandom();
}


int PredictionRequest::GetPackedSize() const 
{
  return 4+4+modelinfo.GetPackedSize() + datatimestamp.GetPackedSize() + 
    4+4+serlen*8+4;
}

int PredictionRequest::GetMaxPackedSize() const 
{
  return 4+4+modelinfo.GetMaxPackedSize() + datatimestamp.GetMaxPackedSize() + 
    4+4+MAX_SERIES_LENGTH*8+4;
}

int PredictionRequest::Pack(Buffer &buf) const 
{
  buf.Pack(tag);
  buf.Pack(flags);
  modelinfo.Pack(buf);
  datatimestamp.Pack(buf);
  buf.Pack(serlen);
  buf.Pack(period_usec);
  buf.Pack(numsteps);
  buf.Pack(series,serlen);
  return 0;
}

int PredictionRequest::Unpack(Buffer &buf) 
{
  buf.Unpack(tag);
  buf.Unpack(flags);
  modelinfo.Unpack(buf);
  datatimestamp.Unpack(buf);
  buf.Unpack(serlen);
  buf.Unpack(period_usec);
  buf.Unpack(numsteps);
  Resize(serlen,false);
  buf.Unpack(series,serlen);
  return 0;
}

void PredictionRequest::Print(FILE *out) const
{
  double hz = 1.0/(1.0e-6*((double)(period_usec)));
  
  fprintf(out,"%u : F(%8.5f Hz) M(%f secs) N(%3d) S(%d)",
	  tag,
	  hz,
	  (double)(datatimestamp),
	  numsteps,
	  serlen);
  modelinfo.Print(out);
  int i;
  
  for (i=0;i<serlen;i++) {
    fprintf(out,"%u\t%f\n",
	    i, series[i]);
  }
}


ostream & PredictionRequest::Print(ostream &os) const
{
  os <<"PredictionRequest(tag="<<tag<<", flags="<<flags
     <<", datatimestamp="<<datatimestamp
     <<", modelinfo="<<modelinfo<<", period_usec="<<period_usec
     <<", numsteps="<<numsteps<<", serlen="<<serlen<<", series=(";
  for (int i=0;i<serlen;i++) {
    if (i!=0) { 
      os << ", ";
    }
    os << series[i];
  }
  os <<"))";
  return os;
}




PredictionResponse::PredictionResponse(unsigned period_usec, int numsteps) 
{
  this->period_usec = period_usec;
  this->numsteps = numsteps;
  if (numsteps>0) {
    preds = new double [numsteps];
    errs = new double [numsteps];
  } else {
    preds = errs = 0;
  }
  this->flags=PREDFLAG_OK;
}

PredictionResponse::PredictionResponse(const PredictionResponse &right) 
{
  this->numsteps=0; 
  this->preds=0;
  this->errs=0;
  Clone(right);
}

PredictionResponse::~PredictionResponse() {
  if (preds) delete [] preds;
  if (errs) delete [] errs;
  preds=errs=0;
  period_usec=numsteps=0;
}

PredictionResponse & PredictionResponse::operator = (const PredictionResponse &right) 
{
  Clone(right);
  return *this;
}

void PredictionResponse::Clone(const PredictionResponse &right) 
{
  this->tag = right.tag;
  this->flags = right.flags;
  this->datatimestamp = right.datatimestamp;
  this->predtimestamp = right.predtimestamp;
  this->modelinfo = right.modelinfo;
  this->period_usec = right.period_usec;
  this->Resize(right.numsteps,false);
  memcpy(this->preds,right.preds,sizeof(double)*numsteps);
  memcpy(this->errs,right.errs,sizeof(double)*numsteps);
}


int PredictionResponse::Resize(int numsteps, bool copy) 
{
  double *newpreds, *newerrs;
  if (numsteps>0) {
    newpreds = new double [numsteps];
    newerrs = new double [numsteps];
  } else {
    newpreds = newerrs = 0;
  }
  if (preds  && copy) {
    memcpy(newpreds,preds,MIN(numsteps,this->numsteps));
    memcpy(newerrs,errs,MIN(numsteps,this->numsteps));
  }
  if (preds) { delete [] preds;}
  if (errs) { delete [] errs;}
  preds = newpreds;
  errs = newerrs;
  this->numsteps = numsteps;
  return 0;
}


int PredictionResponse::GetPackedSize() const 
{ 
  return 4+4+datatimestamp.GetPackedSize()+predtimestamp.GetPackedSize()+
    modelinfo.GetPackedSize()+4+4+2*8*numsteps;
}

int PredictionResponse::GetMaxPackedSize() const 
{ 
  return 4+4+datatimestamp.GetMaxPackedSize()+predtimestamp.GetMaxPackedSize()+
    modelinfo.GetMaxPackedSize()+4+4+2*8*PREDICTION_MAX_NUMSTEPS;
}

int PredictionResponse::Pack(Buffer &buf) const 
{
  buf.Pack(tag);
  buf.Pack(flags);
  datatimestamp.Pack(buf);
  predtimestamp.Pack(buf);
  modelinfo.Pack(buf);
  buf.Pack(period_usec);
  buf.Pack(numsteps);
  buf.Pack(preds,numsteps);
  buf.Pack(errs,numsteps);
  return 0;
}

int PredictionResponse::Unpack(Buffer &buf) 
{
  buf.Unpack(tag);
  buf.Unpack(flags);
  datatimestamp.Unpack(buf);
  predtimestamp.Unpack(buf);
  modelinfo.Unpack(buf);
  buf.Unpack(period_usec);
  buf.Unpack(numsteps);
  Resize(numsteps,false);
  buf.Unpack(preds,numsteps);
  buf.Unpack(errs,numsteps);
  return 0;
}


void PredictionResponse::Print(FILE *out) const
{
  double hz = 1.0/(1.0e-6*((double)(period_usec)));
  
  fprintf(out,"%u : F(%8.5f Hz) P(%f secs) M(%f secs) N(%3d) ",
	  tag,
	  hz,
	  (double)(predtimestamp),
	  (double)(datatimestamp),
	  numsteps);
  modelinfo.Print(out);
  int i;
  
  for (i=0;i<numsteps;i++) {
    fprintf(out,"\t+%u\t%f\t%f\n",
	    i+1, preds[i], errs[i]);
  }
}

ostream & PredictionResponse::Print(ostream &os) const
{
  os <<"PredictionResponse(tag="<<tag<<", flags="<<flags
     <<", datatimestamp="<<datatimestamp<<", predtimestamp="<<predtimestamp
     <<", modelinfo="<<modelinfo<<", period_usec="<<period_usec
     <<", numsteps="<<numsteps<<", preds=(";
  for (int i=0;i<numsteps;i++) {
    if (i!=0) { 
      os << ", ";
    }
    os << preds[i];
  }
  os <<"), errs=(";
  for (int i=0;i<numsteps;i++) {
    if (i!=0) { 
      os << ", ";
    }
    os << errs[i];
  }
  os <<"))";
  return os;
}

/*
PredictionReconfigurationRequest::PredictionReconfigurationResponse(unsigned tag,
								    ModelInfo &mi,
								    unsigned period_usec, 
								    int numsteps)
{
  this->tag=tag;
  this->flags=0;
  this->modelinfo=mi;
  this->period_usec=period_usec;
  this->numsteps=numsteps;
}


PredictionReconfigurationRequest::PredictionReconfigurationResponse(const PredictionReconfigurationResponse &right) 
{
  memcpy(this,&right,sizeof(PredictionReconfigurationResponse));
}

PredictionReconfigurationRequest::~PredictionReconfigurationResponse() 
{}


PredictionReconfigurationResponse & PredictionReconfigurationRequest::operator = (const PredictionReconfigurationResponse &right)
{
  memcpy(this,&right,sizeof(PredictionReconfigurationResponse));
  return *this;
}


void PredictionReconfigurationRequest::MakeNullRequest()
{
  flags|=PREDRECONFREQ_FLAG_NULL;
}

bool PredictionReconfigurationRequest::IsNullRequest()
{
  return flags&PREDRECONFREQ_FLAG_NULL;
}


int PredictionReconfigurationRequest::GetPackedSize() 
{
  return 4+4+modelinfo.GetPackedSize()+timestamp.GetPackedSize()+4;
}

int PredictionReconfigurationRequest::GetMaxPackedSize() 
{
  return 4+4+modelinfo.GetMaxPackedSize()+timestamp.GetMaxPackedSize()+4;
}

int PredictionReconfigurationRequest::Pack(Buffer &buf) const 
{
  buf.Pack(tag);
  buf.Pack(flags);
  modelinfo.Pack(buf);
  timestamp.Pack(buf);
  buf.Pack(numsteps);
}

int PredictionReconfigurationRequest::Unpack(Buffer &buf) 
{
  buf.Unpack(tag);
  buf.Unpack(flags);
  modelinfo.Unpack(buf);
  timestamp.Unpack(buf);
  buf.Unpack(numsteps);
}

*/

PredictionReconfigurationResponse::PredictionReconfigurationResponse(unsigned period_usec, int numsteps) 
{
  this->tag=0;
  this->flags=PREDFLAG_OK;
  this->period_usec = period_usec;
  this->numsteps = numsteps;
}

PredictionReconfigurationResponse::PredictionReconfigurationResponse(const PredictionReconfigurationResponse &right) 
{
  this->tag=0;
  this->flags=right.flags;
  this->modelinfo = right.modelinfo;
  this->requesttimestamp = right.requesttimestamp;
  this->reconfigdonetimestamp = right.reconfigdonetimestamp;
  this->period_usec = right.period_usec;
  this->numsteps=right.numsteps;
}

PredictionReconfigurationResponse::~PredictionReconfigurationResponse() {
  tag=flags=period_usec=numsteps=0;
}

PredictionReconfigurationResponse & PredictionReconfigurationResponse::operator = (const PredictionReconfigurationResponse &right) 
{
  this->tag=0;
  this->flags=right.flags;
  this->modelinfo = right.modelinfo;
  this->requesttimestamp = right.requesttimestamp;
  this->reconfigdonetimestamp = right.reconfigdonetimestamp;
  this->period_usec = right.period_usec;
  this->numsteps=right.numsteps;
  return *this;
}


int PredictionReconfigurationResponse::GetPackedSize() const 
{ 
  return 4+4+modelinfo.GetPackedSize()+requesttimestamp.GetPackedSize()
    +reconfigdonetimestamp.GetPackedSize()+4+4;
}

int PredictionReconfigurationResponse::GetMaxPackedSize() const 
{ 
  return GetPackedSize();
}

int PredictionReconfigurationResponse::Pack(Buffer &buf) const 
{
  buf.Pack(tag);
  buf.Pack(flags);
  modelinfo.Pack(buf);
  requesttimestamp.Pack(buf);
  reconfigdonetimestamp.Pack(buf);
  buf.Pack(period_usec);
  buf.Pack(numsteps);
  return 0;
}

int PredictionReconfigurationResponse::Unpack(Buffer &buf) 
{
  buf.Unpack(tag);
  buf.Unpack(flags);
  modelinfo.Unpack(buf);
  requesttimestamp.Unpack(buf);
  reconfigdonetimestamp.Unpack(buf);
  buf.Unpack(period_usec);
  buf.Unpack(numsteps);
  return 0;
}


void PredictionReconfigurationResponse::Print(FILE *out) const
{
  double hz = 1.0/(1.0e-6*((double)(period_usec)));
  
  fprintf(out,"%u : F(%8.5f Hz) R(%f secs) RD(%f secs) N(%3d) \n",
	  tag,
	  hz,
	  (double)(requesttimestamp),
	  (double)(reconfigdonetimestamp),
	  numsteps);
  modelinfo.Print(out);
}

ostream &PredictionReconfigurationResponse::Print(ostream &os) const
{
  os <<"PredictionReconfigurationResponse(tag="<<tag<<", flags="<<flags
     <<", requesttimestamp="<<requesttimestamp<<", reconfigdonetimestamp="<<reconfigdonetimestamp
     <<", modelinfo="<<modelinfo<<", period_usec="<<period_usec
     <<", numsteps="<<numsteps<<")";
  return os;
}


void PredictionReconfigurationResponse::MakeMatchingResponse(const PredictionReconfigurationRequest &req)
{
  tag=req.tag;
  flags=0;
  modelinfo = req.modelinfo;
  requesttimestamp = req.datatimestamp;
  period_usec = req.period_usec;
  numsteps = req.numsteps;
  reconfigdonetimestamp = TimeStamp();
}
