#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#if defined(WIN32) && !defined(__CYGWIN__)
#define strcasecmp stricmp
#include <winsock.h>
#else
#include <strings.h>
#include <arpa/inet.h>
#endif
#include <assert.h>


#include "ar.h"
#include "ma.h"
#include "arma.h"
#include "arima.h"
#include "arfima.h"
#include "bestmean.h"
#include "none.h"
#include "mean.h"
#include "last.h"
#include "refit.h"
#include "fit.h"

#include "pdqparamsets.h"


ModelTemplate::ModelTemplate() : mt(NONE), ps(0) {}

ModelTemplate::ModelTemplate(const ModelTemplate &right) 
{ 
  mt=right.mt; 
  ps=right.ps->Clone();
}
  
ModelTemplate::ModelTemplate(ModelType mt, const ParameterSet &ps) 
{
  this->mt=mt; 
  this->ps = ps.Clone();
  
}


ModelTemplate & ModelTemplate::operator = (const ModelTemplate &right) 
{
  mt=right.mt; 
  CHK_DEL(ps); 
  ps=right.ps->Clone(); 
  return *this;
}
  
ModelTemplate::~ModelTemplate() 
{ 
  delete ps; 
}

int ModelTemplate::_GetPackedSize() const 
{
  ParameterSetType pst = ps->GetType();

  switch (pst) { 
  case PDQ:
    return 4+4+3*4;
    break;
  case RefittingPDQ:
    return 4+4+4*4;
    break;
  // Add other types here
  default:
    assert(0);
    return -1;
    break;
  }
}

int ModelTemplate::_GetMaxPackedSize() const 
{
  return 4+4+4*4;
}

int ModelTemplate::_Pack(ByteStream &bs) const
{
  int bi[6];
  int p,d,q,r;

  bi[0]=htonl((int)mt);

  ParameterSetType pst = ps->GetType();

  bi[1]=htonl(pst);

  switch (pst) { 
  case PDQ:
    ((PDQParameterSet *)ps)->Get(p,d,q);
    bi[2]=htonl(p);
    bi[3]=htonl(d);
    bi[4]=htonl(q);
    return 5*4==bs.Put((char*)bi,5*4);
    break;
  case RefittingPDQ:
    ((RefittingPDQParameterSet *)ps)->Get(p,d,q);
    ((RefittingPDQParameterSet *)ps)->GetRefit(r);
    bi[2]=htonl(p);
    bi[3]=htonl(d);
    bi[4]=htonl(q);
    bi[5]=htonl(r);
    return 6*4==bs.Put((char*)bi,6*4);
    break;
  // Add other types here
  default:
    assert(0);
    break;
  }

  return 0;
}

int ModelTemplate::_Unpack(ByteStream &bs)
{
  int bi[6];
  int p,d,q,r;

  CHK_DEL(ps);

  bs.Get((char*)&(bi[0]),2*4);
  
  mt = (ModelType) ntohl(bi[0]);

  ParameterSetType pst = (ParameterSetType) ntohl(bi[1]);

  switch (pst) { 
  case PDQ:
    bs.Get((char*)&(bi[2]),3*4);
    p=ntohl(bi[2]);
    d=ntohl(bi[3]);
    q=ntohl(bi[4]);
    ps = new PDQParameterSet(p,d,q);
    break;
  case RefittingPDQ:
    bs.Get((char*)&(bi[2]),4*4);
    p=ntohl(bi[2]);
    d=ntohl(bi[3]);
    q=ntohl(bi[4]);
    r=ntohl(bi[5]);
    ps = new RefittingPDQParameterSet(p,d,q,r);
    break;
  // Add other types here
  default:
    assert(0);
    break;
  }
  return 0;
}

Model *FitThis(ModelType mclass,
	       double *seq,
	       int numsamples,
	       const PDQParameterSet &params)
{
   switch (mclass) {
   case AR:
     return ARModeler::Fit(seq,numsamples,params);
     break;
   case MA:
     return MAModeler::Fit(seq,numsamples,params);
     break;
   case ARMA:
     return ARMAModeler::Fit(seq,numsamples,params);
     break;
   case ARIMA:
     return ARIMAModeler::Fit(seq,numsamples,params);
     break;
   case ARFIMA:
     return ARFIMAModeler::Fit(seq,numsamples,params);
     break;
   case BESTMEAN:
     return BestMeanModeler::Fit(seq,numsamples,params);
     break;
   case MEAN:
     return MeanModeler::Fit(seq,numsamples,params);
     break;
   case LAST:
     return LastModeler::Fit(seq,numsamples,params);
     break;
   case NONE:
     return NoneModeler::Fit(seq,numsamples,params);
     break;
   default:
     return 0;
     break;
   }
}


Model *FitThis(ModelType mclass,
	       double *seq, int numsamples, 
	       int p, double d, int q)
{
  PDQParameterSet params(p,(int)d,q);
  return FitThis(mclass,seq,numsamples,params);
}


Model *FitThis(ModelType mclass,
	       double *seq,
	       int numsamples,
	       const RefittingPDQParameterSet &params)
{
  int refitinterval;
  params.GetRefit(refitinterval);

   switch (mclass) {
   case AR:
     return RefittingModeler<ARModeler>::Fit(seq,numsamples,params,refitinterval);
     break;
   case MA:
     return RefittingModeler<MAModeler>::Fit(seq,numsamples,params,refitinterval);
     break;
   case ARMA:
     return RefittingModeler<ARMAModeler>::Fit(seq,numsamples,params,refitinterval);
     break;
   case ARIMA:
     return RefittingModeler<ARIMAModeler>::Fit(seq,numsamples,params,refitinterval);
     break;
   case ARFIMA:
     return RefittingModeler<ARFIMAModeler>::Fit(seq,numsamples,params,refitinterval);
     break;
   case BESTMEAN:
     return RefittingModeler<BestMeanModeler>::Fit(seq,numsamples,params,refitinterval);
     break;
   case MEAN:
     return RefittingModeler<MeanModeler>::Fit(seq,numsamples,params,refitinterval);
     break;
   case LAST:
     return RefittingModeler<LastModeler>::Fit(seq,numsamples,params,refitinterval);
   case NONE:
     return RefittingModeler<NoneModeler>::Fit(seq,numsamples,params,refitinterval);
     break;
   default:
     return 0;
     break;
   }
}


Model *FitThis(ModelType mclass,
	       double *seq, int numsamples, 
	       int p, double d, int q, int refitinterval)
{
  RefittingPDQParameterSet ps(p,(int)d,q,refitinterval);
  return FitThis(mclass,seq,numsamples,ps);
}


Model *FitThis(double *seq, int numsamples, const ModelTemplate &mt)
{

  if (mt.ps->GetType()==PDQ) { 
    return FitThis(mt.mt,seq,numsamples,*((PDQParameterSet*)(mt.ps)));
  } else if (mt.ps->GetType()==RefittingPDQ) {
    return FitThis(mt.mt,seq,numsamples,*((RefittingPDQParameterSet*)(mt.ps)));
  } else {
    return 0;
  }
}
    
ModelTemplate *ParseModel(int argc, char *argv[])
{
  int p=0;
  double d=0;
  int q=0;
  bool refitting=false;
  int refitinterval=0;
  int first_model;
  ModelType mclass;

  first_model=0;

  if (argc<1) { 
    return 0;
  }

  if (!strcasecmp(argv[0],"REFIT")) {
    refitting=true;
    if (argc<2) { 
      return 0;
    }
    refitinterval = atoi(argv[1]);
    first_model=2;
  } else {
    refitting=false;
    refitinterval=0;
    first_model=0;
  }
    
  if (!strcasecmp(argv[first_model],"BESTMEAN") ||
      !strcasecmp(argv[first_model],"BM") ) {
      if (argc!=first_model+2) {
	return 0;
      }
      p=atoi(argv[first_model+1]);
      mclass=BESTMEAN;
      goto done;
   }
  if (!strcasecmp(argv[first_model],"NONE")) {
      if (argc!=first_model+1) {
	return 0;
      }
      mclass=NONE;
      goto done;
   }
  if (!strcasecmp(argv[first_model],"LAST")) {
      if (argc!=first_model+1) {
	return 0;
      }
      mclass=LAST;
      goto done;
   }
  if (!strcasecmp(argv[first_model],"MEAN")) {
      if (argc!=first_model+1) {
	return 0;
      }
      mclass=MEAN;
      goto done;
   }
   if (!strcasecmp(argv[first_model],"AR")) {
      if (argc!=first_model+2) {
	return 0;
      }
      p=atoi(argv[first_model+1]);
      mclass=AR;
      goto done;
   }
   if (!strcasecmp(argv[first_model],"MA")) {
      if (argc!=first_model+2) {
	return 0;
      }
      q=atoi(argv[first_model+1]);
      mclass=MA;
      goto done;
   }
   if (!strcasecmp(argv[first_model],"ARMA")) {
      if (argc!=first_model+3) {
	return 0;
      }
      p=atoi(argv[first_model+1]);
      q=atoi(argv[first_model+2]);
      mclass=ARMA;
      goto done;
   }
   if (!strcasecmp(argv[first_model],"ARIMA")) {
      if (argc!=first_model+4) {
	return 0;
      }
      p=atoi(argv[first_model+1]);
      d=atof(argv[first_model+2]);
      q=atoi(argv[first_model+3]);
      mclass=ARIMA;
      goto done;
   }
   if (!strcasecmp(argv[first_model],"ARFIMA")) {
      if (argc!=first_model+4) {
	return 0;
      }
      p=atoi(argv[first_model+1]);
      d=atof(argv[first_model+2]);
      q=atoi(argv[first_model+3]);
      mclass=ARFIMA;
      goto done;
   }

   // matches none of the known models!  die
   return 0;

 done:
   ModelTemplate *mt = new ModelTemplate;

   mt->mt = mclass;
   if (refitting) { 
     mt->ps = new RefittingPDQParameterSet(p,(int)d,q,refitinterval);
   } else {
     mt->ps = new PDQParameterSet(p,(int)d,q);
   }
   
   return mt;
}   




void ModelTemplate::Print(FILE *out) const
{
  char *temp = GetName();

  fprintf(out,"ModelTemplate: %s\n", temp);
  
  delete [] temp;
}

ostream &ModelTemplate::Print(ostream &os) const
{
  char *temp = GetName();

  os<<"ModelTemplate("<<((const char*)temp)<<")";
  
  delete [] temp;
  
  return os;
}

char *ModelTemplate::GetName() const
{
  int p,d,q,r;

  bool refit;

  char *name = new char [1024];

  if (ps->GetType()==PDQ) { 
    refit=false;
    ((PDQParameterSet*)ps)->Get(p,d,q);
  } else if (ps->GetType()==RefittingPDQ) { 
    refit=true;
    ((RefittingPDQParameterSet*)ps)->Get(p,d,q);
    ((RefittingPDQParameterSet*)ps)->GetRefit(r);
  }

  if (refit) {
    sprintf(name,"REFIT %d ",r);
  } else {
    name[0]=0;
  }

  switch (mt) { 
  case NONE:
    sprintf(&(name[strlen(name)]), "NONE");
    break;
  case LAST:
    sprintf(&(name[strlen(name)]), "LAST");
    break;
  case MEAN:
    sprintf(&(name[strlen(name)]), "MEAN");
    break;
  case BESTMEAN:
    sprintf(&(name[strlen(name)]), "BM(%d)",p);
    break;
  case AR:
    sprintf(&(name[strlen(name)]), "AR(%d)",p);
    break;
  case MA:
    sprintf(&(name[strlen(name)]), "MA(%d)",q);
    break;
  case ARMA:
    sprintf(&(name[strlen(name)]), "ARMA(%d,%d)",p,q);
    break;
  case ARIMA:
    sprintf(&(name[strlen(name)]), "ARIMA(%d,%d,%d)",p,d,q);
    break;
  case ARFIMA:
    sprintf(&(name[strlen(name)]), "ARFIMA(%d,%d,%d)",p,d,q);
    break;
  default:
    sprintf(&(name[strlen(name)]), "UNKNOWN");
    break;
  }
  return name;
}


