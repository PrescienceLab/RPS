#include <new>
#include "bestmean.h"
#include "tools.h"
#include "util.h"
#include "pdqparamsets.h"
#include "ar.h"

//#define BESTMEAN_AS_AR

BestMeanModel::BestMeanModel()
{
  order=0;
  variance=0.0;
  mean=0.0;
}

BestMeanModel::BestMeanModel(const BestMeanModel &rhs) :
  order(rhs.order), variance(rhs.variance), mean(rhs.mean)
{
}


BestMeanModel::~BestMeanModel()
{
}

BestMeanModel & BestMeanModel::operator=(const BestMeanModel &rhs)
{
  this->~BestMeanModel();
  return *(new(this)BestMeanModel(rhs));
}

void BestMeanModel::Initialize(int const order) 
{
  this->order=order;
}

#define CHECK(num) ((num)>=0 && (num)<order)
#define ADJUST(num) ((num))

void BestMeanModel::SetVariance(const double var)
{
  variance=var;
}

double BestMeanModel::GetVariance() const 
{
  return variance;
}

void BestMeanModel::SetMean(const double mn)
{
  mean=mn;
}

double BestMeanModel::GetMean() const 
{
  return mean;
}

int BestMeanModel::GetOrder() const  
{
  return order;
}

void BestMeanModel::Dump(FILE *out) const 
{
  if (out==0) {
    out=stdout;
  }

  fprintf(out,"BestMean(%d) model with mean=%f and noise variance=%f\n",
              order,mean,variance);
}


ostream & BestMeanModel::operator<<(ostream &os) const 
{
  os << "BestMeanModel(order="<<order<<", mean="<<mean<<", variance="<<variance<<")";
  return os;
}

#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define MIN(x,y) ((x)<(y) ? (x) : (y))

#define LEAVE() goto leave_error


Predictor *BestMeanModel::MakePredictor() const
{
#ifdef BESTMEAN_AS_AR
   int i;
   Polynomial et, th;

   et.SetPower(0);
   et.SetCoeff(0,1);
   for (i=0;i<order;i++) {
     et.SetCoeff(i+1,1.0/((double)order));
   }

   th.SetPower(0);
   th.SetCoeff(0,1);

   EtaThetaPredictor *pred = new EtaThetaPredictor;
   
   pred->Initialize(&et,&th,variance,mean);

   return pred;
#else
  BestMeanPredictor *pred= new BestMeanPredictor;	
  pred->Initialize(order,variance);
  return pred;
#endif
}



BestMeanPredictor::BestMeanPredictor()
{
  samples=0;
  order=0;
  numsamples=0;
  mult=0.0;
}

BestMeanPredictor::BestMeanPredictor(const BestMeanPredictor &rhs)
{
  samples=0; order=0; numsamples=0; mult=0.0;
  Initialize(rhs.order,rhs.variance);
  memcpy(samples,rhs.samples,sizeof(samples[0])*order);
  numsamples=rhs.numsamples;
}

BestMeanPredictor::~BestMeanPredictor()
{
  CHK_DEL_MAT(samples);
}

BestMeanPredictor & BestMeanPredictor::operator=(const BestMeanPredictor &rhs)
{
  this->~BestMeanPredictor();
  return *(new(this)BestMeanPredictor(rhs));
}

int BestMeanPredictor::Initialize(const int order, const double variance)
{
  this->order=order;
  mult = 1.0/(double)order;
  CHK_DEL_MAT(samples);
  samples=new double [order];
  numsamples=0;
  this->variance=variance;
  return 0;
}

int BestMeanPredictor::Begin()
{
  numsamples=0;
  return 0;
}


int BestMeanPredictor::StepsToPrime() const 
{
  return order;
}


double BestMeanPredictor::Step(const double obs)
{
  samples[(numsamples)%order]=obs;
  ++numsamples;
  double res;
  int i;
  res=0.0;
  for (i=0;i<MIN(numsamples,order);i++) {
    res+=mult*samples[i];
  }
  return res;

}


int BestMeanPredictor::Predict(const int maxahead, double *predictions) const
{
  double res;
  int i;
  res=0.0;
  for (i=0;i<MIN(numsamples,order);i++) {
    res+=mult*samples[i];
  }
  for (i=0;i<maxahead;i++) {
    predictions[i]=res;
  }
  return 0;
}



int BestMeanPredictor::ComputeVariances(const int maxahead, double *vars,
					const VarianceType vtype) const
{
  int i;
  // Evalauate as an ar(p) model

  ARModel *model = new ARModel;
  model->Initialize(order);
  for (i=0;i<order;i++) {
    model->SetCoeff(i,1.0/((double)order));
  }
  model->SetVariance(variance);
  Predictor *p = model->MakePredictor();
  int rc = p->ComputeVariances(maxahead,vars,vtype);
  delete p;
  delete model;
  return rc;

#if 0  
  int i,j;
  switch (vtype) { 
  case POINT_VARIANCES:
    for (i=0;i<maxahead;i++) { 
      vars[i]=variance;
    }
    return 0;
    break;
  case SUM_VARIANCES:
    vars[0]=variance;
    for (i=1;i<maxahead;i++) { 
      vars[i]+=vars[i-1];
    }
    return 0;
    break;
  case CO_VARIANCES:
    for (i=0;i<maxahead;i++) {
      for (j=0;j<maxahead;j++) { 
	vars[i*maxahead+j] = j==0 ? variance : 0.0;
      }
    }
    return 0;
    break;
  }
  return -1;
#endif
}


void BestMeanPredictor::Dump(FILE *out) const
{
  fprintf(out,"BestMeanPredictor(order=%d,variance=%f,numsamples=%d)\nsamples are:\n",order,variance,numsamples);
  for (int i=0;i<order;i++) {
    fprintf(out,"%f\n",samples[i]);
  }
}	

ostream & BestMeanPredictor::operator<<(ostream &os) const
{
  os<<"BestMeanPredictor(order="<<order<<", variance="<<variance<<", numsamples="<<numsamples<<", samples=(";
  for (int i=0;i<order;i++) {
    if (i>0) { 
      os <<", ";
    }
    os <<samples[i];
  }
  os << "))";
  return os;
}


BestMeanModeler::BestMeanModeler()
{
}

BestMeanModeler::BestMeanModeler(const BestMeanModeler &rhs)
{
}

BestMeanModeler::~BestMeanModeler()
{
}

BestMeanModeler & BestMeanModeler::operator=(const BestMeanModeler &rhs)
{
  this->~BestMeanModeler();
  return *(new(this)BestMeanModeler(rhs));
}



#define ABS(x) ((x)> 0 ? (x) : (-(x)))

BestMeanModel *BestMeanModeler::Fit(const double *seq, const int len, const int maxord)
{
  int i;
  int ord;

  int bestord;
  double seqmean=Mean(seq,len);
  double seqvar=Mean(seq,len);

  double msq,bestmsq=99e99;
  double next;

  BestMeanModel *model=new BestMeanModel;
  Predictor *pred;

  for (ord=1;ord<=maxord;ord++) {
    model->Initialize(ord);
#ifdef BESTMEAN_AS_AR
    model->SetMean(seqmean);
#else
    model->SetMean(0.0);
#endif
    model->SetVariance(seqvar);
    pred=model->MakePredictor(); // Model variances invalid at this point
    pred->Begin();
    msq=0.0;
    next=seqmean;
    for (i=0;i<len;i++) {
      msq+=SQUARE(next-seq[i]);
      next=pred->Step(seq[i]);
    }
    if (msq<bestmsq) {
      bestmsq=msq;
      bestord=ord;
    }
    CHK_DEL(pred);
  }

  model->Initialize(bestord);
#ifdef BESTMEAN_AS_AR
  model->SetMean(seqmean);
#else
  model->SetMean(0.0);
#endif
  model->SetVariance(bestmsq/len);
  
  return model;
}

Model *BestMeanModeler::Fit(const double *seq, const int len, const ParameterSet &ps)
{
  int p,d,q;
  ((const PDQParameterSet &)ps).Get(p,d,q);
  return Fit(seq,len,p);
}
			  

void BestMeanModeler::Dump(FILE *out) const
{
  fprintf(out,"BestMeanModeler()\n");
}

ostream & BestMeanModeler::operator<<(ostream &os) const
{
  os << "BestMeanModeler()";
  return os;
}

  

  
