#include <new>
#include "pdqparamsets.h"

PDQParameterSet::PDQParameterSet() : p(0), d(0), q(0)
{}

PDQParameterSet::PDQParameterSet(const PDQParameterSet &rhs) : p(rhs.p), d(rhs.d), q(rhs.q)
{}

PDQParameterSet::PDQParameterSet(const int pp, const int dd, const int qq) : p(pp), d(dd), q(qq)
{}

PDQParameterSet::~PDQParameterSet() 
{}

PDQParameterSet & PDQParameterSet::operator=(const PDQParameterSet &rhs)
{
  this->~PDQParameterSet();
  return *(new (this) PDQParameterSet(rhs));
}

void PDQParameterSet::Get(int &p, int &d, int &q) const 
{
  p=this->p; d=this->d; q=this->q;
}

void PDQParameterSet::Set(const int p, const int d, const int q) 
{
  this->p=p; this->d=d; this->q=q;
}

ParameterSet *PDQParameterSet::Clone() const 
{ 
  return new PDQParameterSet(*this);
}

ParameterSetType PDQParameterSet::GetType() const 
{
  return PDQ;
}

void PDQParameterSet::Print(FILE *out) const 
{
  fprintf(out,"PDQParameterSet: p=%d d=%d q=%d\n",p,d,q);
}

void PDQParameterSet::Dump(FILE *out) const 
{
  Print(out);
}

ostream & PDQParameterSet::operator<<(ostream &os) const 
{
  os << "PDQParameterSet(p="<<p<<", d="<<d<<", q="<<q<<")";
  return os;
}


RefittingPDQParameterSet::RefittingPDQParameterSet() :
  PDQParameterSet(), refitint(0)
{}

RefittingPDQParameterSet::RefittingPDQParameterSet(const RefittingPDQParameterSet &rhs) :
  PDQParameterSet(rhs), refitint(rhs.refitint)
{}

RefittingPDQParameterSet::RefittingPDQParameterSet(const int pp,
						   const int dd,
						   const int qq,
						   const int refitt) : 
  PDQParameterSet(pp,dd,qq) , refitint(refitt)
{}

RefittingPDQParameterSet::~RefittingPDQParameterSet()
{}


RefittingPDQParameterSet & RefittingPDQParameterSet::operator=(const RefittingPDQParameterSet &rhs)
{
  this->~RefittingPDQParameterSet();
  return *(new(this)RefittingPDQParameterSet(rhs));
}

void RefittingPDQParameterSet::GetRefit(int &refit) const 
{ 
  refit=refitint;
}

void RefittingPDQParameterSet::SetRefit(const int &refit) 
{ 
  refitint=refit;
}

ParameterSet *RefittingPDQParameterSet::Clone() const 
{ 
  return new RefittingPDQParameterSet(*this);
}

ParameterSetType RefittingPDQParameterSet::GetType() const 
{ 
  return RefittingPDQ;
}

void RefittingPDQParameterSet::Print(FILE *out) const 
{
  fprintf(out,"RefittingPDQParameterSet: p=%d, d=%d, q=%d, refitint=%d\n",p,d,q,refitint);
}

void RefittingPDQParameterSet::Dump(FILE *out) const 
{
  Print(out);
}


ostream & RefittingPDQParameterSet::operator<<(ostream &os) const 
{
  os <<"RefittingPDQParameterSet(";
  PDQParameterSet::operator<<(os);
  os <<", refitint="<<refitint<<")";
  return os;
}

AwaitingPDQParameterSet::AwaitingPDQParameterSet() : PDQParameterSet()
{}

AwaitingPDQParameterSet::AwaitingPDQParameterSet(const AwaitingPDQParameterSet &rhs) :
  PDQParameterSet(rhs), awaitint(rhs.awaitint)
{}

AwaitingPDQParameterSet::~AwaitingPDQParameterSet()
{}

AwaitingPDQParameterSet::AwaitingPDQParameterSet(const int pp,
						 const int dd,
						 const int qq,
						 const int awaitt)  :
  PDQParameterSet(pp,dd,qq), awaitint(awaitt)
{}


AwaitingPDQParameterSet &AwaitingPDQParameterSet::operator=(const AwaitingPDQParameterSet &rhs)
{
  this->~AwaitingPDQParameterSet();
  return *(new(this)AwaitingPDQParameterSet(rhs));
}

void AwaitingPDQParameterSet::GetAwait(int &await) const 
{ 
  await=awaitint;
}

void AwaitingPDQParameterSet::SetAwait(const int &await) 
{ 
  awaitint=await;
}

ParameterSet *AwaitingPDQParameterSet::Clone() const 
{ 
  return new AwaitingPDQParameterSet(*this);
}

ParameterSetType AwaitingPDQParameterSet::GetType() const 
{ 
  return AwaitingPDQ;
}

void AwaitingPDQParameterSet::Print(FILE *out) const 
{
  fprintf(out,"AwaitingPDQParameterSet: p=%d, d=%d, q=%d, awaitint=%d\n",p,d,q,awaitint);
}

void AwaitingPDQParameterSet::Dump(FILE *out) const 
{
  Print(out);
}

ostream & AwaitingPDQParameterSet::operator<<(ostream &os) const 
{
  os <<"AwaitingPDQParameterSet(";
  PDQParameterSet::operator<<(os);
  os <<", awaitint="<<awaitint<<")";
  return os;
}


ManagedPDQParameterSet::ManagedPDQParameterSet() : PDQParameterSet()
{}

ManagedPDQParameterSet::ManagedPDQParameterSet(const ManagedPDQParameterSet &rhs) :
  PDQParameterSet(rhs), num_await(rhs.num_await), num_refit(rhs.num_refit), min_num_test(rhs.min_num_test),
  errlimit(rhs.errlimit), varlimit(rhs.varlimit)
{}

ManagedPDQParameterSet::ManagedPDQParameterSet(const int p,
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


ManagedPDQParameterSet::~ManagedPDQParameterSet() 
{}

ManagedPDQParameterSet & ManagedPDQParameterSet::operator=(const ManagedPDQParameterSet &rhs)
{
  this->~ManagedPDQParameterSet();
  return *(new(this)ManagedPDQParameterSet(rhs));
}

void ManagedPDQParameterSet::GetAwait(int &await) const 
{ 
  await=num_await;
}

void ManagedPDQParameterSet::SetAwait(const int &await) 
{ 
  num_await=await;
}

void ManagedPDQParameterSet::GetRefit(int &refit) const 
{ 
  refit=num_refit;
}

void ManagedPDQParameterSet::SetRefit(const int &refit) 
{ 
  num_refit=refit;;
}

void ManagedPDQParameterSet::GetMinTest(int &mintest) const 
{ 
  mintest=min_num_test;
}

void ManagedPDQParameterSet::SetMinTest(const int &mintest) 
{ 
  min_num_test=mintest;
}

void ManagedPDQParameterSet::GetErrorLimit(double &errlim) const 
{ 
  errlim=errlimit;
}

void ManagedPDQParameterSet::SetErrorLimit(const double &errlim) 
{ 
  errlimit=errlim;
}

void ManagedPDQParameterSet::GetVarLimit(double &varlim) const 
{ 
  varlim=varlimit;
}

void ManagedPDQParameterSet::SetVarLimit(const double &varlim) 
{ 
  varlimit=varlim;
}

ParameterSet *ManagedPDQParameterSet::Clone() const 
{ 
  return new ManagedPDQParameterSet(*this);
}

ParameterSetType ManagedPDQParameterSet::GetType() const 
{ 
  return ManagedPDQ;
}

void ManagedPDQParameterSet::Print(FILE *out) const 
{
  fprintf(out,"ManagedPDQParameterSet: p=%d, d=%d, q=%d, num_await=%d, num_refit=%d, min_num_test=%d, errlimit=%f, varlimit=%f\n",p,d,q,
	  num_await,num_refit,min_num_test,errlimit,varlimit);
}

void ManagedPDQParameterSet::Dump(FILE *out) const 
{
  Print(out);
}

ostream & ManagedPDQParameterSet::operator<<(ostream &os) const 
{
  os<<"ManagedPDQParameterSet(";
  PDQParameterSet::operator<<(os);
  os<<", num_await="<<num_await<<", num_refit="<<num_refit<<", min_num_test="<<min_num_test
    <<", errlimit="<<errlimit<<", varlimit="<<varlimit<<")";
  return os;
}



