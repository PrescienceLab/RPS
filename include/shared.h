#ifndef _sharedstuff
#define _sharedstuff


/*

#define PREDICTION_MAX_NUMSTEPS 60

#define MEASURE_DATATYPE_NONE   0
#define MEASURE_DATATYPE_LOAD   1


struct Prediction : public SerializeableInfo {
  unsigned ipaddress;            // These two identify the source
  unsigned port;                  // port=0 for local source 
  unsigned underlyingdatatype;   // datatype  
  TimeStamp datatimestamp;  // Timestamp of data used in pred
  TimeStamp predtimestamp;  // Timestamp of prediction
  ModelInfo modelinfo;           // Model info
  unsigned period_usec;          // Period of data and predictions
  unsigned numsteps;              // number of prediction steps
  double *preds;                 // predictions
  double *errs;                  // errors

  Prediction(unsigned period_usec=0, unsigned   numsteps=0) {
    this->period_usec = period_usec;
    this->numsteps = numsteps;
    if (numsteps>0) {
      preds = new double [numsteps];
      errs = new double [numsteps];
    } else {
      preds = errs = 0;
    }
  }

  virtual ~Prediction() {
    if (preds) delete [] preds;
    if (errs) delete [] errs;
    preds=errs=0;
    period_usec=numsteps=0;
  }

  int Resize(unsigned numsteps, bool copy=true) {
    double *newpreds, *newerrs;
    if (numsteps>0) {
      newpreds = new double [numsteps];
      newerrs = new double [numsteps];
    } else {
      newpreds = newerrs = 0;
    }
    if (this->numsteps>0 && copy) {
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


  unsigned GetPackedSize() const { 
    return 4+4+4+datatimestamp.GetPackedSize()+predtimestamp.GetPackedSize()+
      modelinfo.GetPackedSize()+4+4+2*8*numsteps;
  }

  unsigned GetMaxPackedSize() const { 
    return 4+4+4+datatimestamp.GetMaxPackedSize()+predtimestamp.GetMaxPackedSize()+
      modelinfo.GetMaxPackedSize()+4+4+2*8*PREDICTION_MAX_NUMSTEPS;
  }

  int Pack(Buffer &buf) const {
    buf.Pack(ipaddress);
    buf.Pack(port);
    buf.Pack(underlyingdatatype);
    datatimestamp.Pack(buf);
    predtimestamp.Pack(buf);
    modelinfo.Pack(buf);
    buf.Pack(period_usec);
    buf.Pack(numsteps);
    buf.Pack(preds,numsteps);
    buf.Pack(errs,numsteps);
    return 0;
  }

  int Unpack(Buffer &buf) {
    buf.Unpack(ipaddress);
    buf.Unpack(port);
    buf.Unpack(underlyingdatatype);
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


  void Print(FILE *out=stdout) {
    struct in_addr ia; ia.s_addr=ipaddress;
    
    struct hostent *he=gethostbyaddr((const char *)&(ia),
				     sizeof(ia),
				     AF_INET);
    
    double hz = 1.0/(1.0e-6*((double)(period_usec)));
    
    fprintf(out,"%-40s(%4d) : F(%8.5f Hz) P(%-9d.%-9d) M(%-9d.%-9d) D(%2d) MOD(%2d,%3d) N(%3d):\n",
	    he ? he->h_name : inet_ntoa(ia),
            port,
	    hz,
	    predtimestamp.tv_sec, predtimestamp.tv_usec,
	    datatimestamp.tv_sec, datatimestamp.tv_usec,
            underlyingdatatype,
	    modelinfo.modelclass, modelinfo.len,
            numsteps);
    unsigned i;

    for (i=0;i<numsteps;i++) {
      fprintf(out,"\t+%u\t%f\t%f\n",
	      i+1, preds[i], errs[i]);
    }
  }
};


*/

/*
struct MeasurementInfo : public SerializeableInfo {
  unsigned ipaddress;
  struct timeval timestamp;
  int period_usec;
  int GetPackedSize() const {
    return 4+(4+4)+4;
  }
  int Pack(Buffer &thebuf) {
    assert(thebuf.Resize(GetPackedSize())>=GetPackedSize());
    char *buf=thebuf.Data();
    ::Pack(&(buf[0*sizeof(int)+0*sizeof(double)]),(int)ipaddress);
    ::Pack(&(buf[1*sizeof(int)+0*sizeof(double)]),timestamp.tv_sec);
    ::Pack(&(buf[2*sizeof(int)+0*sizeof(double)]),timestamp.tv_usec);
    ::Pack(&(buf[3*sizeof(int)+0*sizeof(double)]),period_usec);
    return 0;
  }
  int Unpack(const Buffer &thebuf) {
    assert(thebuf.Size()>=GetPackedSize());
    char *buf=thebuf.Data();
    ::Unpack(&(buf[0*sizeof(int)+0*sizeof(double)]),(int*)&ipaddress);
    ::Unpack(&(buf[1*sizeof(int)+0*sizeof(double)]),&(timestamp.tv_sec));
    ::Unpack(&(buf[2*sizeof(int)+0*sizeof(double)]),&(timestamp.tv_usec));
    ::Unpack(&(buf[3*sizeof(int)+0*sizeof(double)]),&(period_usec));
    return 0;
  }
};
*/  

/*
struct PredictionInfo {
  unsigned ipaddress;
  struct timeval timestamp;
  int period_usec;
  int model[3];  // p,d,q
  inline int Pack(char *buf, int len) {
    if (len<(sizeof(int)*7 + sizeof(double)*0)) {
      return -1;
    } else {
      ::Pack(&(buf[0*sizeof(int)+0*sizeof(double)]),(int)ipaddress);
      ::Pack(&(buf[1*sizeof(int)+0*sizeof(double)]),timestamp.tv_sec);
      ::Pack(&(buf[2*sizeof(int)+0*sizeof(double)]),timestamp.tv_usec);
      ::Pack(&(buf[3*sizeof(int)+0*sizeof(double)]),period_usec);
      ::Pack(&(buf[4*sizeof(int)+0*sizeof(double)]),model[0]);
      ::Pack(&(buf[5*sizeof(int)+0*sizeof(double)]),model[1]);
      ::Pack(&(buf[6*sizeof(int)+0*sizeof(double)]),model[2]);
      return 0;
    }
  }
  inline int Unpack(const char *buf, int len) {
    if (len<(sizeof(int)*7 + sizeof(double)*0)) {
      return -1;
    } else {
      ::Unpack(&(buf[0*sizeof(int)+0*sizeof(double)]),(int*)&ipaddress);
      ::Unpack(&(buf[1*sizeof(int)+0*sizeof(double)]),&(timestamp.tv_sec));
      ::Unpack(&(buf[2*sizeof(int)+0*sizeof(double)]),&(timestamp.tv_usec));
      ::Unpack(&(buf[3*sizeof(int)+0*sizeof(double)]),&(period_usec));
      ::Unpack(&(buf[4*sizeof(int)+0*sizeof(double)]),&(model[0]));
      ::Unpack(&(buf[5*sizeof(int)+0*sizeof(double)]),&(model[1]));
      ::Unpack(&(buf[6*sizeof(int)+0*sizeof(double)]),&(model[2]));
      return 0;
    }
  }
};


struct PredictionRequest {
  unsigned ipaddress;
  struct timeval timestamp;
  int numsteps;
  inline int Pack(char *buf, int len) {
    if (len<(sizeof(int)*4 + sizeof(double)*0)) {
      return -1;
    } else {
      ::Pack(&(buf[0*sizeof(int)+0*sizeof(double)]),(int)ipaddress);
      ::Pack(&(buf[1*sizeof(int)+0*sizeof(double)]),timestamp.tv_sec);
      ::Pack(&(buf[2*sizeof(int)+0*sizeof(double)]),timestamp.tv_usec);
      ::Pack(&(buf[3*sizeof(int)+0*sizeof(double)]),numsteps);
      return 0;
    }
  }
  inline int Unpack(const char *buf, int len) {
    if (len<(sizeof(int)*4 + sizeof(double)*0)) {
      return -1;
    } else {
      ::Unpack(&(buf[0*sizeof(int)+0*sizeof(double)]),(int*)&ipaddress);
      ::Unpack(&(buf[1*sizeof(int)+0*sizeof(double)]),&(timestamp.tv_sec));
      ::Unpack(&(buf[2*sizeof(int)+0*sizeof(double)]),&(timestamp.tv_usec));
      ::Unpack(&(buf[3*sizeof(int)+0*sizeof(double)]),&(numsteps));
      return 0;
    }
  }
};

#define OK               0
#define UNSUPPORTED_HOST 1

*/
#endif
