#include <string.h>
#include "arima.h"
#include "ar.h"
#include "poly.h"
#include "etathetapred.h"
#include "tools.h"
#include "arma.h"


ARIMAModel::ARIMAModel()
{
  phis=thetas=0;
}

ARIMAModel::~ARIMAModel()
{
  CHK_DEL_MAT(phis);
  CHK_DEL_MAT(thetas);
}

void ARIMAModel::Initialize(int P, int D, int Q)
{
  CHK_DEL_MAT(phis);
  CHK_DEL_MAT(thetas);

  p=P;
  d=D;
  q=Q;

  phis= new double [p];
  thetas= new double [q];

}

int ARIMAModel::GetP()
{
   return p;
}

int ARIMAModel::GetQ()
{
   return q;
}

int ARIMAModel::GetD()
{
   return d;
}


#define CHECKP(num) ((num)>=0 && (num)<p)
#define CHECKQ(num) ((num)>=0 && (num)<q)
#define ADJUSTP(num) ((num))
#define ADJUSTQ(num) ((num))

void ARIMAModel::SetARCoeff(int num, double value)
{
  if (CHECKP(num)) {
    phis[ADJUSTP(num)]=value;
  }
}

void ARIMAModel::SetMACoeff(int num, double value)
{
  if (CHECKQ(num)) {
    thetas[ADJUSTQ(num)]=value;
  }
}

double ARIMAModel::GetARCoeff(int num)
{
  if (CHECKP(num)) {
    return phis[ADJUSTP(num)];
  } else {
	return 0.0;
  }
}

double ARIMAModel::GetMACoeff(int num)
{
  if (CHECKQ(num)) {
    return thetas[ADJUSTQ(num)];
  } else {
	return 0.0;
  }
}

void ARIMAModel::SetVariance(double var)
{
  variance=var;
}

double ARIMAModel::GetVariance()
{
  return variance;
}


double ARIMAModel::EstimateVariance(double *seq, int len)
{
  Predictor *predictor=MakePredictor();
  int i;
  double ssd;
  double pred;

  predictor->Begin();
  // prime
  for (i=0;i<p+q+d;i++) {
    predictor->Step(seq[i]);
  }

  ssd=0.0;
  for (i=p+q+d;i<len;i++) {
    predictor->Predict(1,&pred);
    ssd+=SQUARE(pred-seq[i]);
    predictor->Step(seq[i]);
  }

  delete predictor;

  return ssd/((double)len);
}

void ARIMAModel::SetMean(double mn)
{
  mean=mn;
}

double ARIMAModel::GetMean()
{
  return mean;
}

void ARIMAModel::Dump(FILE *out)
{
  fprintf(out,"ARIMA(%d,%d,%d) model\n",GetP(),GetD(),GetQ());
  fprintf(out,"Phis (AR coeffs):");

  int i;

  for (i=0;i<GetP();i++) {
    fprintf(out," %lf",GetARCoeff(i));
  }
  fprintf(out,"\n");
  fprintf(out,"Thetas (MA coeffs):");
  for (i=0;i<GetQ();i++) {
    fprintf(out," %lf",GetMACoeff(i));
  }
  fprintf(out,"\nNoise Variance=%lf\n",GetVariance());
}

Predictor * ARIMAModel::MakePredictor()
{
   int i;
   Polynomial et,dh,th;

   // Note that the internal model is of the form
   // (1-B)^d(1-phi_1B-phi_2B^2-...)z_t = (1 - omega_1B - omega_2B -...) x_t
   // where omega_1 ... are in coeff[0..]
   // The eta-theta representation is identical

   et.SetPower(0);
   et.SetCoeff(0,1);
   // Have to invert the signs in order to do multiplication right
   for (i=0;i<p;i++) {
      et.SetCoeff(i+1,-phis[i]);
   }
   dh.SetPower(0);
   dh.SetCoeff(0,1);
   dh.SetCoeff(1,-1);
   dh.RaiseTo(d);
   et.MultiplyBy(&dh);

   // Now reverse signs on eta to conform to what etatheta expects
   for (i=1;i<et.GetNumCoeffs();i++) {
     et.SetCoeff(i,-et.GetCoeff(i));
   }

   th.SetPower(0);
   th.SetCoeff(0,1);
   for (i=0;i<q;i++) {
      th.SetCoeff(i+1,thetas[i]);
   }

   EtaThetaPredictor *pred = new EtaThetaPredictor;
   pred->Initialize(&et,&th,variance,mean);

   return pred;
}



ARIMAModel *ARIMAModeler::Fit(double *seq, int len, int P, int D, int Q)
{
  double mean;
  int i;


  double *workseq= new double[len];
  memcpy(workseq,seq,len*sizeof(double));

  mean=MeanifySequence(workseq,len);
  
  for (i=0;i<D;i++) {
    DifferenceSequence(workseq,len-i);
  }

  ARMAModel *armamodel = ARMAModeler::Fit(workseq,len-D,P,Q);
  ARIMAModel *model;

  if (armamodel==0) { 
    fprintf(stderr,"Fit of underlying ARMA model failed.\n");
    model=0;
  } else {
    model = new ARIMAModel;
    model->Initialize(P,D,Q);
    for (i=0;i<P;i++) {
      model->SetARCoeff(i,armamodel->GetARCoeff(i));
    }
    for (i=0;i<Q;i++) {
      model->SetMACoeff(i,armamodel->GetMACoeff(i));
    }
    // Fishy
    model->SetVariance(armamodel->GetVariance());
    model->SetMean(mean);
 
    //
    // modified to use worksequence
    //
    //for (i=D-1;i>=0;i--) {
    //  UnDifferenceSequence(seq,len-i,first[i]);
    //}
    //UnMeanifySequence(seq,len,mean);
  }

  CHK_DEL(armamodel);
  CHK_DEL_MAT(workseq);
  return model;
}

Model *ARIMAModeler::Fit(double *seq, int len, const ParameterSet &ps)
{
  int p,d,q;
  
  ((const PDQParameterSet &)ps).Get(p,d,q);
  
  return Fit(seq,len,p,d,q);
}
