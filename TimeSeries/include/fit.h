#ifndef _fit
#define _fit

#include <stdio.h>
#include <iostream>

using namespace std;

#include "abstract.h"
#include "pdqparamsets.h"
#include "util.h"

struct ModelTemplate;

char *GetAvailableModels();

ModelTemplate *ParseModel(const int argc, char *argv[]);
ModelTemplate *ParseModel(const char *buf);

Model *FitThis(const double *seq, const int numsamples, const ModelTemplate &mt);

Model *FitThis(const ModelType mclass,
	       const double *seq,
	       const int numsamples,
	       const PDQParameterSet &ps);

Model *FitThis(const ModelType mclass,
	       const double *seq, 
	       const int numsamples, 
	       const int p, const double d, const int q);

Model *FitThis(const ModelType mclass,
	       const double *seq,
	       const int numsamples,
	       const RefittingPDQParameterSet &ps);

Model *FitThis(const ModelType mclass,
	       const double *seq, 
	       const int numsamples, 
	       const int p, const double d, const int q, const int refitinterval);

Model *FitThis(const ModelType mclass,
	       const AwaitingPDQParameterSet &ps);

Model *FitThis(const ModelType mclass,
	       const int p, const double d, const int q, const int await);

Model *FitThis(const ModelType mclass,
	       const ManagedPDQParameterSet &ps);

Model *FitThis(const ModelType mclass,
	       const int p, const double d, const int q, 
	       const int num_await, const int num_refit,
	       const int min_num_test, const double errlimit,
	       const double varlimit);



class ByteStream {
 public:
  virtual int Put(const char *buf,const int len)=0;
  virtual int Get(char *buf, const int len)=0;
};

struct ModelTemplate {
  ModelType        mt;
  ParameterSet     *ps;

  ModelTemplate();
  ModelTemplate(const ModelTemplate &right);
  ModelTemplate(ModelType mt, const ParameterSet &ps);
  ModelTemplate & operator = (const ModelTemplate &right);
  virtual ~ModelTemplate();

  char *GetName() const;

  int _GetPackedSize() const;
  int _GetMaxPackedSize() const;
  int _Pack(ByteStream &bs) const;
  int _Unpack(ByteStream &bs);
  
  void Print(FILE *out=stdout) const;
  void Dump(FILE *out=stdout) const;
  ostream &Print(ostream &os) const;
  ostream &operator<<(ostream &os) const;
};


inline ostream & operator<< (ostream &os, const ModelTemplate &p) {
  return p.operator<<(os);
}

#endif
