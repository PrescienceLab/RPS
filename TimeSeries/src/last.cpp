#include <new>
#include "last.h"

#define LAST_AS_AR

LastModel::LastModel() : BestMeanModel()
{}

LastModel::LastModel(const LastModel &rhs) : BestMeanModel(rhs)
{}


LastModel::LastModel(const BestMeanModel &rhs) : BestMeanModel(rhs)
{}

LastModel::~LastModel()
{}

LastModel &LastModel::operator=(const LastModel &rhs)
{
  this->~LastModel();
  return *(new(this)LastModel(rhs));
}

void LastModel::Dump(FILE *out) const
{
  fprintf(out,"LastModel:");
  BestMeanModel::Dump(out);
}

ostream & LastModel::operator<<(ostream &os) const
{
  os <<"LastModel(";
  BestMeanModel::operator<<(os);
  os <<")";
  return os;
}

Predictor *LastModel::MakePredictor() const
{
#ifdef LAST_AS_AR
   Polynomial et, th;

   et.SetPower(0);
   et.SetCoeff(0,1);
   et.SetCoeff(1,1);

   th.SetPower(0);
   th.SetCoeff(0,1);

   EtaThetaPredictor *pred = new EtaThetaPredictor;
   
   pred->Initialize(&et,&th,GetVariance(),GetMean());

   return pred;
#else
  LastPredictor *p= new LastPredictor();
  p->Initialize(1,GetVariance());
  return p;
#endif
}

LastPredictor::LastPredictor() :   BestMeanPredictor()
{}

LastPredictor::LastPredictor(const LastPredictor &rhs) : BestMeanPredictor(rhs)
{}

LastPredictor::~LastPredictor()
{}

LastPredictor & LastPredictor::operator=(const LastPredictor &rhs) 
{
  this->~LastPredictor();
  return *(new(this)LastPredictor(rhs));
}
 

void LastPredictor::Dump(FILE *out) const
{
  fprintf(out,"LastPredictor:");
  BestMeanPredictor::Dump(out);
}

ostream & LastPredictor::operator<<(ostream &os) const
{
  os <<"LastPredictor(";
  BestMeanPredictor::operator<<(os);
  os <<")";
  return os;
}

LastModeler::LastModeler()
{}
LastModeler::LastModeler(const LastModeler &)
{}
LastModeler::~LastModeler()
{}
LastModeler &LastModeler::operator=(const LastModeler &rhs)
{
  this->~LastModeler();
  return *(new(this)LastModeler(rhs));
}


LastModel *LastModeler::Fit(const double *seq, const int len) 
{
  BestMeanModel *bm =BestMeanModeler::Fit(seq,len,1);
  LastModel *lm = new LastModel(*bm);
  delete bm;
  return lm;
}

Model *LastModeler::Fit(const double *seq, const int len, const ParameterSet &ps)
{
  return Fit(seq,len);
}

void LastModeler::Dump(FILE *out) const
{
  fprintf(out,"LastModeler\n");
}

ostream & LastModeler::operator<<(ostream &os) const
{
  os<<"LastModeler()";
  return os;
}



