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

#endif
