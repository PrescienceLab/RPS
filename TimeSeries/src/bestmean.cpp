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

BestMeanModel::~BestMeanModel()
{
}

void BestMeanModel::Initialize(int order)
{
  this->order=order;
}

#define CHECK(num) ((num)>=0 && (num)<order)
#define ADJUST(num) ((num))

void BestMeanModel::SetVariance(double var)
{
  variance=var;
}

double BestMeanModel::GetVariance() 
{
  return variance;
}

void BestMeanModel::SetMean(double mn)
{
  mean=mn;
}

double BestMeanModel::GetMean()
{
  return mean;
}

int BestMeanModel::GetOrder() 
{
  return order;
}

void BestMeanModel::Dump(FILE *out)
{
  if (out==0) {
    out=stdout;
  }

  fprintf(out,"BestMean(%d) model with mean=%lf and noise variance=%lf\n",
              order,mean,variance);
}


#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define MIN(x,y) ((x)<(y) ? (x) : (y))

#define LEAVE() goto leave_error


Predictor *BestMeanModel::MakePredictor()
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

BestMeanPredictor::~BestMeanPredictor()
{
  CHK_DEL_MAT(samples);
}

int BestMeanPredictor::Initialize(int order,double variance)
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


int BestMeanPredictor::StepsToPrime()
{
  return order;
}


double BestMeanPredictor::Step(double obs)
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


int BestMeanPredictor::Predict(int maxahead, double *predictions)
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



int BestMeanPredictor::ComputeVariances(int maxahead, double *vars,
					VarianceType vtype)
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


void BestMeanPredictor::Dump(FILE *out)
{
  fprintf(out,"BestMeanPredictor(order=%d,variance=%lf)\n",order,variance);
}	

BestMeanModeler::BestMeanModeler()
{
}

BestMeanModeler::~BestMeanModeler()
{
}


#define ABS(x) ((x)> 0 ? (x) : (-(x)))

BestMeanModel *BestMeanModeler::Fit(double *seq, int len, int maxord)
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

Model *BestMeanModeler::Fit(double *seq, int len, const ParameterSet &ps)
{
  int p,d,q;
  ((const PDQParameterSet &)ps).Get(p,d,q);
  return Fit(seq,len,p);
}
			  
  

  
