#ifndef _fit
#define _fit

#include <stdio.h>
#include <iostream>
#include "ioutil.h"

#include "abstract.h"
#include "pdqparamsets.h"
#include "util.h"

struct ModelTemplate;

ModelTemplate *ParseModel(int argc, char *argv[]);

Model *FitThis(double *seq, int numsamples, const ModelTemplate &mt);

Model *FitThis(ModelType mclass,
	       double *seq,
	       int numsamples,
	       const PDQParameterSet &ps);

Model *FitThis(ModelType mclass,
	       double *seq, int numsamples, 
	       int p, double d, int q);

Model *FitThis(ModelType mclass,
	       double *seq,
	       int numsamples,
	       const RefittingPDQParameterSet &ps);

Model *FitThis(ModelType mclass,
	       double *seq, int numsamples, 
	       int p, double d, int q, int refitinterval);



class ByteStream {
 public:
  virtual int Put(char *buf,int len)=0;
  virtual int Get(char *buf, int len)=0;
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
  ostream &Print(ostream &os) const;
};

#endif
