#ifndef _pdqparamsets
#define _pdqparamsets

#include "abstract.h"

class PDQParameterSet : public ParameterSet {
 protected:
  int p,d,q;

 public:
  PDQParameterSet();
  PDQParameterSet(const PDQParameterSet &rhs);
  PDQParameterSet(const int p, const int d, const int q);
  ~PDQParameterSet();

  PDQParameterSet & operator=(const PDQParameterSet &rhs);

  void Get(int &p, int &d, int &q) const;
  void Set(const int p, const int d, const int q);
  ParameterSet *Clone() const; 
  ParameterSetType GetType() const;

  void Print(FILE *out=stdout) const;
  void Dump(FILE *out=stdout) const;
  ostream & operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const PDQParameterSet &p) {
  return p.operator<<(os);
}

class RefittingPDQParameterSet : public PDQParameterSet {
 protected: 
  int refitint;
 public:
  RefittingPDQParameterSet();
  RefittingPDQParameterSet(const RefittingPDQParameterSet &rhs);
  RefittingPDQParameterSet(const int p,
			   const int d,
			   const int q,
			   const int refit);
  virtual ~RefittingPDQParameterSet();
  
  RefittingPDQParameterSet & operator=(const RefittingPDQParameterSet &rhs);

  void GetRefit(int &refit) const;
  void SetRefit(const int &refit); 
  ParameterSet *Clone() const;
  ParameterSetType GetType() const;

  void Print(FILE *out=stdout) const;
  void Dump(FILE *out=stdout) const;
  ostream &operator<<(ostream &os) const;

};

inline ostream & operator<< (ostream &os, const RefittingPDQParameterSet &p) {
  return p.operator<<(os);
}

class AwaitingPDQParameterSet : public PDQParameterSet {
 protected: 
  int awaitint;
 public:
  AwaitingPDQParameterSet();
  AwaitingPDQParameterSet(const AwaitingPDQParameterSet &rhs);
  AwaitingPDQParameterSet(const int p,
			  const int d,
			  const int q,
			  const int await);
  ~AwaitingPDQParameterSet();

  AwaitingPDQParameterSet & operator=(const   AwaitingPDQParameterSet &rhs);

  void GetAwait(int &await) const;
  void SetAwait(const int &await);
  ParameterSet *Clone() const;
  ParameterSetType GetType() const;

  void Print(FILE *out=stdout) const;
  void Dump(FILE *out=stdout) const;
  ostream &operator<<(ostream &os) const;
};

inline ostream & operator<< (ostream &os, const AwaitingPDQParameterSet &p) {
  return p.operator<<(os);
}


class ManagedPDQParameterSet : public PDQParameterSet {
 protected: 
  int num_await;
  int num_refit;
  int min_num_test;
  double errlimit;
  double varlimit;
 public:
  ManagedPDQParameterSet();
  ManagedPDQParameterSet(const ManagedPDQParameterSet &rhs);
  ManagedPDQParameterSet(const int p,
			 const int d,
			 const int q,
			 const int await,
			 const int refit,
			 const int mintest,
			 const double errlim,
			 const double varlim);
  ~ManagedPDQParameterSet();

  ManagedPDQParameterSet & operator=(const ManagedPDQParameterSet &rhs);

  void GetAwait(int &await) const;
  void SetAwait(const int &await);
  void GetRefit(int &refit) const ;
  void SetRefit(const int &refit) ;
  void GetMinTest(int &mintest) const ;
  void SetMinTest(const int &mintest) ;
  void GetErrorLimit(double &errlim) const ;
  void SetErrorLimit(const double &errlim) ;
  void GetVarLimit(double &varlim) const ;
  void SetVarLimit(const double &varlim) ;

  ParameterSet *Clone() const;
  ParameterSetType GetType() const;

  void Print(FILE *out=stdout) const;
  void Dump(FILE *out=stdout) const ;
  ostream &operator<<(ostream &os) const;

};

inline ostream & operator<< (ostream &os, const ManagedPDQParameterSet &p) {
  return p.operator<<(os);
}

#endif
