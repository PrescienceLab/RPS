#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#if defined(WIN32) && !defined(__CYGWIN__)
#define strcasecmp stricmp
#define snprintf _snprintf
#include <winsock.h>
#else
#include <strings.h>
#include <arpa/inet.h>
#endif
#include <assert.h>

#include <string>

#include "ar.h"
#include "ma.h"
#include "arma.h"
#include "arima.h"
#include "arfima.h"
#include "bestmean.h"
#include "bestmedian.h"
#include "newton.h"
#include "wavelet.h"
#include "none.h"
#include "mean.h"
#include "last.h"
#include "refit.h"
#include "await.h"
#include "managed.h"
#include "fit.h"

#include "pdqparamsets.h"
#include "fileparameterset.h"

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
  case AwaitingPDQ:
    return 4+4+4*4;
    break;
  case ManagedPDQ:
    return 4+4+6*4+2*8;
    break;
  case File: {
    string s;
    ((FileParameterSet *)ps)->Get(s);
    return 4+4+4+s.size();
  }
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
  //  return 4+4+6*4+2*8;
  return 256;
}

#if !defined(IEEE_DOUBLE_LSB) && !defined(IEEE_DOUBLE_MSB)
#if defined (__alpha) || defined (__alpha__)
#define IEEE_DOUBLE_LSB
#elif defined(sun) || defined(sparc)
#define IEEE_DOUBLE_MSB
#elif defined(__i386__) || defined(i386) || defined(__i386) || defined(I386)
#define IEEE_DOUBLE_LSB
#else
#error Can not determine IEEE double byte order
#endif
#endif

inline static void Swap(const char *in, char *out, int num)
{
  int i;
  for (i=0;i<num;i++) {
    out[i] = in[num-i-1];
  }
}

inline static void htond(const double x, char out[8])
{
  double x2=x;
  char *in = (char *) &x2;
#ifdef IEEE_DOUBLE_LSB
  Swap(in,out,8);
#else
  memcpy(out,in,8);
#endif
}


inline static void ntohd(const char in[8], double *x) 
{
  char *out=(char*)x;
#ifdef IEEE_DOUBLE_LSB
  Swap(in,out,8);
#else
  memcpy(out,in,8);
#endif
}

int ModelTemplate::_Pack(ByteStream &bs) const
{
  int bi[20];
  int p,d,q,r,a,m;
  double errlimit, varlimit;

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
  case AwaitingPDQ:
    ((AwaitingPDQParameterSet *)ps)->Get(p,d,q);
    ((AwaitingPDQParameterSet *)ps)->GetAwait(a);
    bi[2]=htonl(p);
    bi[3]=htonl(d);
    bi[4]=htonl(q);
    bi[5]=htonl(a);
    return 6*4==bs.Put((char*)bi,6*4);
    break;
  case ManagedPDQ:
    ((ManagedPDQParameterSet *)ps)->Get(p,d,q);
    ((ManagedPDQParameterSet *)ps)->GetAwait(a);
    ((ManagedPDQParameterSet *)ps)->GetRefit(r);
    ((ManagedPDQParameterSet *)ps)->GetMinTest(m);
    ((ManagedPDQParameterSet *)ps)->GetErrorLimit(errlimit);
    ((ManagedPDQParameterSet *)ps)->GetVarLimit(varlimit);
    bi[2]=htonl(p);
    bi[3]=htonl(d);
    bi[4]=htonl(q);
    bi[5]=htonl(a);
    bi[6]=htonl(r);
    bi[7]=htonl(m);
    htond(errlimit,(char *) &(bi[8]));
    htond(varlimit,(char *) &(bi[10]));
    return 12*4==bs.Put((char*)bi,12*4);
    break;
  case File: {
    string s;
    int len;
    ((FileParameterSet *)ps)->Get(s);
    len=s.size();
    bi[2]=htonl(len);
    int rc1 = (3*4)==bs.Put((const char*)bi,3*4);
    int rc2 = len==bs.Put((const char*)(s.c_str()),len);
    return rc1||rc2;
  }
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
  int bi[20];
  int p,d,q,r,a,m;
  double errlimit, varlimit;

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
  case AwaitingPDQ:
    bs.Get((char*)&(bi[2]),4*4);
    p=ntohl(bi[2]);
    d=ntohl(bi[3]);
    q=ntohl(bi[4]);
    r=ntohl(bi[5]);
    ps = new AwaitingPDQParameterSet(p,d,q,r);
    break;
  case ManagedPDQ:
    bs.Get((char*)&(bi[2]),10*4);
    p=ntohl(bi[2]);
    d=ntohl(bi[3]);
    q=ntohl(bi[4]);
    a=ntohl(bi[5]);
    r=ntohl(bi[6]);
    m=ntohl(bi[7]);
    ntohd((const char*)&(bi[8]),&errlimit);
    ntohd((const char*)&(bi[10]),&varlimit);
    ps = new ManagedPDQParameterSet(p,d,q,a,r,m,errlimit,varlimit);
    break;
  case File: {
    bs.Get((char*)&(bi[2]),1*4);
    int len = ntohl(bi[2]);
    char *buf = new char[len+1];
    int rc1=bs.Get(buf,len)==len;
    buf[len]=0;
    ps = new FileParameterSet(buf);
    delete [] buf;
    return rc1;
  }
    break;
  // Add other types here
  default:
    assert(0);
    break;
  }
  return 0;
}

char *GetAvailableModels()
{
  char *s = new char [10000];

  snprintf(s,10000,
	   "A Model is in the form [optional modifier] [required underlying model]\n\n"
	   "Optional Modifiers That Affect Predictors Produced From the Underlying Model\n"
	   "----------------------------------------------------------------------------\n"
	   "REFIT r\n"
	   " predictor will refit itself every r data elements\n"
	   "AWAIT a\n"
	   " predictor will wait for a data elements before fitting\n"
	   "MANAGED a r m e v\n"
	   " predictor will wait for a data elements before fitting\n"
	   " predictor will refit after r data elements\n"
	   " predictor will refit if, after m samples, the relative error of one-step\n"
	   "  ahead predictions exceeds e (avg(abs(obs-pred)/abs(pred)) > e)\n"
	   " predictor will refit if, after m samples, the actual error variance of\n"
	   "  one-step ahead predictions exceeds their predicted variance by \n"
	   "  a factor of v (variance(error)/predictedvariance > v)\n"
	   "\n"
	   "Underlying Models\n"
	   "-----------------\n"
	   "NONE\n"
	   " No model\n"
	   "MEAN\n"
	   " Long-term mean\n"
	   "LAST\n"
	   " Last value seen\n"
	   "BM p | BESTMEAN p\n"
	   " Windowed average, window length chosen to minimize msqerr\n"
	   "BMED p | BESTMEDIAN p\n"
	   " Windowed median, window length chosen to minimize msqerr\n"
	   "NEWTON p\n"
	   " Newton's interpolating polynomial\n"
	   "AR p\n"
	   " Autoregressive model of order p\n"
	   "MA q\n"
	   " Moving average model of order q\n"
	   "ARMA p q\n"
	   " Autoregressive moving average model of order p+q\n"
	   "ARIMA p d q\n"
	   " Autoregressive integrated moving average model of order p+q with\n"
	   "  d-order difference\n"
	   "ARFIMA p d q\n"
	   " Fractionally integrated ARIMA model of order p+q.  d is ignored and\n"
	   "  and determined by the model fitting process\n"
	   "WAVELET filename\n"
	   " Wavelet-based prediction as specified in the filename\n"
	   "  There are several limitations to be aware of\n"
           "    - No modifiers may be used\n"
           "    - Underlying predictors must be MANAGED or AWAIT modified\n"
	   "    - Predictions at all time horizons are reported as the next\n"
           "      value produced by the structure specified in the config file\n"
           "    - Error variance is reported as zero\n");
  return s;
}
	   

Model *FitThis(const ModelType mclass,
	       const double *seq,
	       const int numsamples,
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
   case BESTMEDIAN:
     return BestMedianModeler::Fit(seq,numsamples,params);
     break;
   case NEWTON:
     return NewtonModeler::Fit(seq,numsamples,params);
     break;
   case WAVELET:
     // not possible
     return 0;
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


Model *FitThis(const ModelType mclass,
	       const double *seq,
	       const int numsamples,
	       const FileParameterSet &params)
{
   switch (mclass) {
     // only possible case now
   case WAVELET:   
     return WaveletModeler::Fit(seq,numsamples,params);
     break;
   default:
     return 0;
     break;
   }
}


Model *FitThis(const ModelType mclass,
	       const double *seq, const int numsamples, 
	       const int p, const double d, const int q)
{
  PDQParameterSet params(p,(int)d,q);
  return FitThis(mclass,seq,numsamples,params);
}


Model *FitThis(const ModelType mclass,
	       const double *seq,
	       const int numsamples,
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
   case BESTMEDIAN:
     return RefittingModeler<BestMedianModeler>::Fit(seq,numsamples,params,refitinterval);
     break;
   case NEWTON:
     // note - nothing to refit
     return RefittingModeler<NewtonModeler>::Fit(seq,numsamples,params,refitinterval);
     break;
   case WAVELET:
     // not possible currently
     return 0;
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


Model *FitThis(const ModelType mclass,
	       const double *seq, const int numsamples,
 	       const int p, const double d, const int q, const int refitinterval)
{
  RefittingPDQParameterSet ps(p,(int)d,q,refitinterval);
  return FitThis(mclass,seq,numsamples,ps);
}

Model *FitThis(const ModelType mclass,
	       const AwaitingPDQParameterSet &params)
{
  int await;
  params.GetAwait(await);

   switch (mclass) {
   case AR:
     return AwaitingModeler<ARModeler>::Fit(params,await);
     break;
   case MA:
     return AwaitingModeler<MAModeler>::Fit(params,await);
     break;
   case ARMA:
     return AwaitingModeler<ARMAModeler>::Fit(params,await);
     break;
   case ARIMA:
     return AwaitingModeler<ARIMAModeler>::Fit(params,await);
     break;
   case ARFIMA:
     return AwaitingModeler<ARFIMAModeler>::Fit(params,await);
     break;
   case BESTMEAN:
     return AwaitingModeler<BestMeanModeler>::Fit(params,await);
     break;
   case BESTMEDIAN:
     return AwaitingModeler<BestMedianModeler>::Fit(params,await);
     break;
   case NEWTON:
     // Nothing to await
     return AwaitingModeler<NewtonModeler>::Fit(params,await);
     break;
   case WAVELET:
     // not possible currently
     return 0;
     break;
   case MEAN:
     return AwaitingModeler<MeanModeler>::Fit(params,await);
     break;
   case LAST:
     return AwaitingModeler<LastModeler>::Fit(params,await);
   case NONE:
     return AwaitingModeler<NoneModeler>::Fit(params,await);
     break;
   default:
     return 0;
     break;
   }
}


Model *FitThis(const ModelType mclass,
	       const int p, const double d, const int q, const int await)
{
  AwaitingPDQParameterSet ps(p,(int)d,q,await);
  return FitThis(mclass,ps);
}


Model *FitThis(const ModelType mclass,
	       const ManagedPDQParameterSet &params)
{
  int await;
  int refit;
  int mintest;
  double errlimit;
  double varlimit;

  params.GetAwait(await);
  params.GetRefit(refit);
  params.GetMinTest(mintest);
  params.GetErrorLimit(errlimit);
  params.GetVarLimit(varlimit);

   switch (mclass) {
   case AR:
     return ManagedModeler<ARModeler>::Fit(params,await,refit,mintest,errlimit,varlimit);
     break;
   case MA:
     return ManagedModeler<MAModeler>::Fit(params,await,refit,mintest,errlimit,varlimit);
     break;
   case ARMA:
     return ManagedModeler<ARMAModeler>::Fit(params,await,refit,mintest,errlimit,varlimit);
     break;
   case ARIMA:
     return ManagedModeler<ARIMAModeler>::Fit(params,await,refit,mintest,errlimit,varlimit);
     break;
   case ARFIMA:
     return ManagedModeler<ARFIMAModeler>::Fit(params,await,refit,mintest,errlimit,varlimit);
     break;
   case BESTMEAN:
     return ManagedModeler<BestMeanModeler>::Fit(params,await,refit,mintest,errlimit,varlimit);
     break;
   case BESTMEDIAN:
     return ManagedModeler<BestMedianModeler>::Fit(params,await,refit,mintest,errlimit,varlimit);
     break;
   case NEWTON:
     // nothing to manage
     return ManagedModeler<NewtonModeler>::Fit(params,await,refit,mintest,errlimit,varlimit);
     break;
   case WAVELET:
     // not possible currently
     return 0;
     break;
   case MEAN:
     return ManagedModeler<MeanModeler>::Fit(params,await,refit,mintest,errlimit,varlimit);
     break;
   case LAST:
     return ManagedModeler<LastModeler>::Fit(params,await,refit,mintest,errlimit,varlimit);
   case NONE:
     return ManagedModeler<NoneModeler>::Fit(params,await,refit,mintest,errlimit,varlimit);
     break;
   default:
     return 0;
     break;
   }
}


Model *FitThis(const ModelType mclass,
	       const int p, const double d, const int q, 
	       const int await, const int refit, const int mintest,
	       const double errlimit, const double varlimit)
{
  ManagedPDQParameterSet ps(p,(int)d,q,await,refit,mintest,errlimit,varlimit);
  return FitThis(mclass,ps);
}

Model *FitThis(const double *seq, const int numsamples, const ModelTemplate &mt)
{

  if (mt.ps->GetType()==PDQ) { 
    return FitThis(mt.mt,seq,numsamples,*((PDQParameterSet*)(mt.ps)));
  } else if (mt.ps->GetType()==RefittingPDQ) {
    return FitThis(mt.mt,seq,numsamples,*((RefittingPDQParameterSet*)(mt.ps)));
  } else if (mt.ps->GetType()==AwaitingPDQ) {
    return FitThis(mt.mt,*((AwaitingPDQParameterSet*)(mt.ps)));
  } else if (mt.ps->GetType()==ManagedPDQ) {
    return FitThis(mt.mt,*((ManagedPDQParameterSet*)(mt.ps)));
  } else if (mt.ps->GetType()==File) {
    return FitThis(mt.mt,seq,numsamples,*((FileParameterSet*)(mt.ps)));
  } else {
    return 0;
  }
}
    
ModelTemplate *ParseModel(const int argc, char *argv[])
{
  int p=0;
  double d=0;
  int q=0;
  bool refitting=false;
  bool awaiting=false;
  bool managed=false;
  bool file=false;
  int refitinterval=0;
  int await=0;
  int mintest=0;
  double errlimit;
  double varlimit;
  int first_model;
  ModelType mclass;
  string thefile;

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
  } else if (!strcasecmp(argv[0],"AWAIT")) {
    awaiting=true;
    if (argc<2) { 
      return 0;
    }
    await = atoi(argv[1]);
    first_model=2;
  } else if (!strcasecmp(argv[0],"MANAGED")) {
    managed=true;
    if (argc<7) {
      return 0;
    } else {
      await=atoi(argv[1]);
      refitinterval=atoi(argv[2]);
      mintest=atoi(argv[3]);
      errlimit=atof(argv[4]);
      varlimit=atof(argv[5]);
      first_model=6;
    }
  } else {
    refitting=false;
    awaiting=false;
    refitinterval=0;
    await=0;
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
  if (!strcasecmp(argv[first_model],"BESTMEDIAN") ||
      !strcasecmp(argv[first_model],"BMED") ) {
      if (argc!=first_model+2) {
	return 0;
      }
      p=atoi(argv[first_model+1]);
      mclass=BESTMEDIAN;
      goto done;
   }
  if (!strcasecmp(argv[first_model],"NEWTON")) {
      if (argc!=first_model+2) {
	return 0;
      }
      p=atoi(argv[first_model+1]);
      mclass=NEWTON;
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

   if (!strcasecmp(argv[first_model],"WAVELET")) {
      if (argc!=first_model+2) {
	return 0;
      }
      thefile = argv[first_model+1];
      file=true;
      mclass=WAVELET;
      goto done;
   }

   // matches none of the known models!  die
   return 0;

 done:
   ModelTemplate *mt = new ModelTemplate;

   mt->mt = mclass;
   if (refitting) { 
     mt->ps = new RefittingPDQParameterSet(p,(int)d,q,refitinterval);
   } else if (awaiting) {
     mt->ps = new AwaitingPDQParameterSet(p,(int)d,q,await);
   } else if (managed) {
     mt->ps = new ManagedPDQParameterSet(p,(int)d,q,await,refitinterval,
					 mintest,errlimit,varlimit);
   } else if (file) { 
     mt->ps = new FileParameterSet(thefile);
   } else {
     mt->ps = new PDQParameterSet(p,(int)d,q);
   }
   
   return mt;
}   


#define MAX_COMPS 100
ModelTemplate *ParseModel(const char *buf)
{
  char **argv = new char * [MAX_COMPS] ;
  char *temp=new char [strlen(buf)+1];
  int i;
  ModelTemplate *mt;
  
  memcpy(temp,buf,strlen(buf)+1);
  
  i=0;
  if ((argv[i]=strtok(temp," \t"))==NULL) { 
    mt=0;
    goto out;
  }
  i++;
  while ((argv[i]=strtok(NULL," \t"))!=NULL) {
    i++;
  }
  
  // note that I *know* that ParseModel will not
  // alias argv and screw it up.  
  // furthermore, I am deleting argv very soon
  // at the end of the function
  mt = ParseModel(i,argv);
  
 out:
  delete [] temp;
  delete [] argv;
  return mt;
}

  






void ModelTemplate::Print(FILE *out) const
{
  char *temp = GetName();

  fprintf(out,"ModelTemplate: mt=%d human-name='%s' Parameter Set Follows\n", mt, temp);
  if (ps) {
    ps->Dump(out);
  } else {
    fprintf(out, "No Parameter Set\n");
  }
  
  delete [] temp;
}

void ModelTemplate::Dump(FILE *out) const
{
  Print(out);
}

ostream &ModelTemplate::Print(ostream &os) const
{
  char *temp = GetName();

  os<<"ModelTemplate(mt="<<mt<<", ps=";
  if (ps) { 
    os << *ps;
  } else {
    os <<"none";
  }
  os <<", human-name='"<<temp<<"')";
  delete [] temp;
  return os;
}

ostream &ModelTemplate::operator<<(ostream &os) const
{
  return Print(os);
}

char *ModelTemplate::GetName() const
{
  int p,d,q,a,r,m;
  double errlimit, varlimit;
  bool refit=false;
  bool await=false;
  bool managed=false;
  string filename;

  char *name = new char [1024];

  if (ps->GetType()==PDQ) { 
    refit=false;
    ((PDQParameterSet*)ps)->Get(p,d,q);
  } else if (ps->GetType()==RefittingPDQ) { 
    refit=true;
    ((RefittingPDQParameterSet*)ps)->Get(p,d,q);
    ((RefittingPDQParameterSet*)ps)->GetRefit(r);
  } else if (ps->GetType()==AwaitingPDQ) { 
    await=true;
    ((AwaitingPDQParameterSet*)ps)->Get(p,d,q);
    ((AwaitingPDQParameterSet*)ps)->GetAwait(a);
  } else if (ps->GetType()==ManagedPDQ) { 
    managed=true;
    ((ManagedPDQParameterSet*)ps)->Get(p,d,q);
    ((ManagedPDQParameterSet*)ps)->GetAwait(a);
    ((ManagedPDQParameterSet*)ps)->GetRefit(r);
    ((ManagedPDQParameterSet*)ps)->GetMinTest(m);
    ((ManagedPDQParameterSet*)ps)->GetErrorLimit(errlimit);
    ((ManagedPDQParameterSet*)ps)->GetVarLimit(varlimit);
  } else if (ps->GetType()==File) { 
    ((FileParameterSet*)ps)->Get(filename);
  }
    

  if (refit) {
    sprintf(name,"REFIT %d ",r);
  } else if (await) {
    sprintf(name,"AWAIT %d ",a);
  } else if (managed) {
    sprintf(name,"MANAGED %d %d %d %f %f ",a,r,m,errlimit,varlimit);
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
    sprintf(&(name[strlen(name)]), "BESTMEAN(%d)",p);
    break;
  case BESTMEDIAN:
    sprintf(&(name[strlen(name)]), "BESTMEDIAN(%d)",p);
    break;
  case NEWTON:
    sprintf(&(name[strlen(name)]), "NEWTON(%d)",p);
    break;
  case WAVELET:
    snprintf(&(name[strlen(name)]), 1024-strlen(name)-1,"WAVELET(%s)",filename.c_str());
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


