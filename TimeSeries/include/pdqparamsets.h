#ifndef _pdqparamsets
#define _pdqparamsets

#include "abstract.h"

class PDQParameterSet : public ParameterSet {
 protected:
  int p,d,q;

 public:
  PDQParameterSet(const int p, const int d, const int q) { 
    this->p=p; this->d=d; this->q=q;
  }
  void Get(int &p, int &d, int &q) const {
    p=this->p; d=this->d; q=this->q;
  }
  void Set(const int p, const int d, const int q) {
    this->p=p; this->d=d; this->q=q;
  }
  ParameterSet *Clone() const { 
    return new PDQParameterSet(*this);
  }
  ParameterSetType GetType() const {
    return PDQ;
  }
  void Print(FILE *out=stdout) const {
    fprintf(out,"PDQParameterSet(%d,%d,%d)\n",p,d,q);
  }
};

class RefittingPDQParameterSet : public PDQParameterSet {
 protected: 
  int refitint;
 public:
  RefittingPDQParameterSet(const int p,
			   const int d,
			   const int q,
			   const int refit) :
    PDQParameterSet(p,d,q), refitint(refit) {}
  void GetRefit(int &refit) const { 
    refit=refitint;
  }
  void SetRefit(const int &refit) { 
    refitint=refit;
  }
  ParameterSet *Clone() const { 
    return new RefittingPDQParameterSet(*this);
  }
  ParameterSetType GetType() const { 
    return RefittingPDQ;
  }
  void Print(FILE *out=stdout) const {
    fprintf(out,"RefittingPDQParameterSet(%d,%d,%d,%d)\n",p,d,q,refitint);
  }
};

class AwaitingPDQParameterSet : public PDQParameterSet {
 protected: 
  int awaitint;
 public:
  AwaitingPDQParameterSet(const int p,
			   const int d,
			   const int q,
			  const int await)  :
    PDQParameterSet(p,d,q), awaitint(await) {}
  void GetAwait(int &await) const { 
    await=awaitint;
  }
  void SetAwait(const int &await) { 
    awaitint=await;
  }
  ParameterSet *Clone() const { 
    return new AwaitingPDQParameterSet(*this);
  }
  ParameterSetType GetType() const { 
    return AwaitingPDQ;
  }
  void Print(FILE *out=stdout) const {
    fprintf(out,"AwaitingPDQParameterSet(%d,%d,%d,%d)\n",p,d,q,awaitint);
  }
};

class ManagedPDQParameterSet : public PDQParameterSet {
 protected: 
  int num_await;
  int num_refit;
  int min_num_test;
  double errlimit;
  double varlimit;
 public:
  ManagedPDQParameterSet(const int p,
			 const int d,
			 const int q,
			 const int await,
			 const int refit,
			 const int mintest,
			 const double errlim,
			 const double varlim) :
    PDQParameterSet(p,d,q), num_await(await),
    num_refit(refit), min_num_test(mintest),
    errlimit(errlim), varlimit(varlim) 
    {}
  void GetAwait(int &await) const { 
    await=num_await;
  }
  void SetAwait(const int &await) { 
    num_await=await;
  }
  void GetRefit(int &refit) const { 
    refit=num_refit;
  }
  void SetRefit(const int &refit) { 
    num_refit=refit;;
  }
  void GetMinTest(int &mintest) const { 
    mintest=min_num_test;
  }
  void SetMinTest(const int &mintest) { 
    min_num_test=mintest;
  }
  void GetErrorLimit(double &errlim) const { 
    errlim=errlimit;
  }
  void SetErrorLimit(const double &errlim) { 
    errlimit=errlim;
  }
  void GetVarLimit(double &varlim) const { 
    varlim=varlimit;
  }
  void SetVarLimit(const double &varlim) { 
    varlimit=varlim;
  }
  ParameterSet *Clone() const { 
    return new ManagedPDQParameterSet(*this);
  }
  ParameterSetType GetType() const { 
    return ManagedPDQ;
  }
  void Print(FILE *out=stdout) const {
    fprintf(out,"ManagedPDQParameterSet(%d,%d,%d,%d,%d,%d,%f,%f)\n",p,d,q,
	    num_await,num_refit,min_num_test,errlimit,varlimit);
  }
};

#endif
