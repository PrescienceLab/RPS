#include <new>
#include <string.h>
#include "ar.h"
#include "tools.h"
#include "util.h"

ARModel::ARModel()
{
  coeffs=0;
}

ARModel::ARModel(const ARModel &rhs)
{
  coeffs=0;
  Initialize(rhs.order);
  memcpy(coeffs,rhs.coeffs,sizeof(coeffs[0])*order);
  variance=rhs.variance;
  mean=rhs.mean;
}

ARModel::~ARModel()
{
  if (coeffs!=0) {
    delete [] coeffs;
  }
}

ARModel & ARModel::operator=(const ARModel &rhs)
{
  this->~ARModel();
  return *(new(this)ARModel(rhs));
}


void ARModel::Initialize(const int order)
{
  if (coeffs!=0) {
    delete [] coeffs;
  }
  
  this->order=order;

  coeffs=new double [order];
}

#define CHECK(num) ((num)>=0 && (num)<order)
#define ADJUST(num) ((num))

void ARModel::SetCoeff(const int num, const double value)
{
  if (CHECK(num)) {
    coeffs[ADJUST(num)]=value;
  }
}

double ARModel::GetCoeff(const int num) const
{
  if (CHECK(num)) {
    return coeffs[ADJUST(num)];
  } else {
	return 0.0;
  }
}
void ARModel::SetVariance(const double var)
{
  variance=var;
}

double ARModel::GetVariance() const
{
  return variance;
}

void ARModel::SetMean(const double mn)
{
  mean=mn;
}

double ARModel::GetMean() const
{
  return mean;
}

int ARModel::GetOrder() const
{
  return order;
}

void ARModel::Dump(FILE *out) const
{
  if (out==0) {
    out=stdout;
  }

  fprintf(out,"AR(%d) model with mean=%f and noise variance=%f\n",
              order,mean,variance);
  fprintf(out,"Coeffs:");

  int i;

  for (i=0;i<order;i++) {
    fprintf(out," %f",coeffs[i]);
  }

  fprintf(out,"\n");
}

ostream & ARModel::operator<<(ostream &os) const
{
  os << "ARModel(p="<<order<<", mean="<<mean<<", variance="<<variance<<", coeffs=(";
  for (int i=0;i<order;i++) {
    if (i>0) {
      os <<", ";
    } 
    os << coeffs[i];
  }
  os <<"))";
  return os;
}


#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define MIN(x,y) ((x)<(y) ? (x) : (y))

#define LEAVE() goto leave_error

Predictor *ARModel::MakePredictor() const
{
   int i;
   Polynomial et, th;

   // Note that the internal model is of the form
   // phi(B)z_t = x_t
   // (1 - phi_1B - phi_2B^2 + ...)z_t = x_t
   // where phi_1 ... are in coeff[0..]
   // To Construct the eta theta representation
   // (1 - eta_1B - eta_2B^2 - ...)z_t = (1)x(t)
   // We must maintain the signs of the coeffs

   et.SetPower(0);
   et.SetCoeff(0,1);
   for (i=0;i<order;i++) {
     et.SetCoeff(i+1,coeffs[i]);
   }

   th.SetPower(0);
   th.SetCoeff(0,1);

   EtaThetaPredictor *pred = new EtaThetaPredictor;
   
   pred->Initialize(&et,&th,variance,mean);

   return pred;
}

ARModeler::ARModeler()
{
}

ARModeler::ARModeler(const ARModeler &rhs)
{
}

ARModeler::~ARModeler()
{
}


ARModeler & ARModeler::operator=(const ARModeler &rhs)
{
  this->~ARModeler();
  return *(new(this)ARModeler(rhs));
}


Model *ARModeler::Fit(const double *seq, const int len, const ParameterSet &ps) 
{
  int p,d,q;
  
  ((const PDQParameterSet &) ps).Get(p,d,q);
  
  return Fit(seq,len,p);
}


#define ABS(x) ((x)> 0 ? (x) : (-(x)))

ARModel *ARModeler::Fit(const double *seq, const int len, const int maxord)
{
  double mean;

  if (len<=0) { 
    return 0;	
  }

  double *acovf = new double [maxord+1];

  if (acovf==0) {
    return 0;
  }

  mean = ComputeAutoCov(seq,len,acovf,maxord+1);

  ARModel *model = Fit(mean,acovf,maxord+1,maxord);

  CHK_DEL_MAT(acovf);

  return model;

}

//#define OLD

ARModel *ARModeler::Fit(const double mean, const double *acovf, const int len, const int maxord)
{

  ARModel *model;
  int i;

  if (len<=0 || len<maxord) { 
    return 0;	
  }
  if (len<maxord) {
    return 0;
  }

  if (acovf[0]==0.0) { 
  // Yikes, no variation at all, so the right model is all zeros,
  // which we implement here to avoid stability problems with main
  // evaluation loop
    model = new ARModel;
    if (model==0) { 
      return 0;
    }
    model->Initialize(maxord);
    for (i=0;i<maxord;i++) {
      model->SetCoeff(i,0.0);
    }
    model->SetVariance(0.0);
    return model;
  }

  // Otherwise there is some variation and we are set for more
  // exciting things.
 
#ifdef OLD
  ARModel *models=new ARModel [maxord];
  
  if (models==0) { 
    return 0;
  }

  // Compute the AR(1) model
  
  models[0].Initialize(1);
  models[0].SetCoeff(0,acovf[1]/acovf[0]);
  models[0].SetVariance(acovf[0] - acovf[1]*acovf[1]/acovf[0]);
  models[0].SetMean(mean);
  // Now apply the Levinson-Durbin Recursion to compute the rest of the models
  
  for (k=1;k<maxord;k++) {
    models[k].Initialize(k+1);

    // Compute the new coefficient "akk"

    akk = 0.0;
    for (j=0;j<k;j++) {
      akk += models[k-1].GetCoeff(j) * acovf[ABS(j-k)];
    }
    akk = (acovf[k+1] - akk) / models[k-1].GetVariance();

    for (j=0;j<k;j++) {
      models[k].SetCoeff(j,models[k-1].GetCoeff(j) 
			 - akk * models[k-1].GetCoeff(k-j-1));
    }
    models[k].SetCoeff(k,akk);
    models[k].SetVariance(models[k-1].GetVariance()
			  * (1.0 - akk*akk));
    models[k].SetMean(mean);
  }

  // This is pretty hideous and I'll make it more efficient later
  model = new ARModel;
  model->Initialize(maxord);
  model->SetVariance(models[maxord-1].GetVariance());
  model->SetMean(models[maxord-1].GetMean());
  for (i=0;i<maxord;i++) {
     model->SetCoeff(i,models[maxord-1].GetCoeff(i));
  }
  CHK_DEL_MAT(models);
  return model;
#else
  // Toeplitz version

  double *phis = new double [maxord];
  // construct symettric toeplitz matrix
  double *toepmat = new double [maxord*2-1];
  
  for (i=0;i<maxord;i++) {
    toepmat[i+maxord-1] = toepmat[maxord-i-1] = acovf[i];
  }

  if (SolveToeplitz(toepmat,phis,&(acovf[1]),maxord)) {
    goto FAIL;
  }

  double var;

  var=1.0;
  for (i=0;i<maxord;i++) {
    var -= (acovf[i+1]/acovf[0])*phis[i];
  }
  var = var * acovf[0];

  model = new ARModel;
  if (model==0) {
    goto FAIL;
  }
  model->Initialize(maxord);
  model->SetVariance(var);
  model->SetMean(mean);
  for (i=0;i<maxord;i++) {
     model->SetCoeff(i,phis[i]);
  }
  CHK_DEL_MAT(phis);
  CHK_DEL_MAT(toepmat);
  return model;

FAIL:
  CHK_DEL(model);
  CHK_DEL_MAT(phis);
  CHK_DEL_MAT(toepmat);
  return 0;

#endif

}
			  
void ARModeler::Dump(FILE *out) const
{
  fprintf(out,"ARModeler\n");
}

ostream & ARModeler::operator<<(ostream &os) const
{
  os <<"ARModeler()";
  return os;
}


  
