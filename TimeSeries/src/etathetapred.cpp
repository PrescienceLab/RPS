#include <new>
#include "etathetapred.h"
#include "util.h"

EtaThetaPredictor::EtaThetaPredictor()
{
   numeta=numtheta=0;
   etas=thetas=values=errors=0;
   variance=0;
}

EtaThetaPredictor::EtaThetaPredictor(const EtaThetaPredictor &rhs)
{
  numeta=rhs.numeta;
  etas = new double [numeta];
  memcpy(etas,rhs.etas,sizeof(etas[0])*numeta);
  numtheta=rhs.numtheta;
  thetas = new double [numtheta];
  memcpy(thetas,rhs.thetas,sizeof(thetas[0])*numtheta);
  variance=rhs.variance;
  values = new double[numeta];
  memcpy(values,rhs.values,sizeof(values[0])*numeta);
  errors = new double[numtheta];
  memcpy(errors,rhs.errors,sizeof(errors[0])*numtheta);
  numsamples=rhs.numsamples;
  next_val=rhs.next_val;
  mean=rhs.mean;
}

EtaThetaPredictor::~EtaThetaPredictor()
{
   CHK_DEL_MAT(etas);
   CHK_DEL_MAT(thetas);
   CHK_DEL_MAT(values);
   CHK_DEL_MAT(errors);
   numeta=numtheta=0;
   variance=0;
}

EtaThetaPredictor & EtaThetaPredictor::operator=(const EtaThetaPredictor &rhs)
{
  this->~EtaThetaPredictor();
  return *(new(this) EtaThetaPredictor(rhs));
}

int EtaThetaPredictor::Initialize(const Polynomial *et, const Polynomial *th, const double var, const double mn)
{
   int i;

   CHK_DEL_MAT(etas);
   CHK_DEL_MAT(thetas);
   CHK_DEL_MAT(values);
   CHK_DEL_MAT(errors);

   numeta = et->GetNumCoeffs();
   numtheta = th->GetNumCoeffs();
   etas = new double [numeta];
   thetas = new double [numtheta];
   for (i=0;i<numeta;i++) {
      etas[i]=  et->GetCoeff(i);
   }
   for (i=0;i<numtheta;i++) {
      thetas[i]=th->GetCoeff(i);
   }
   variance = var;
   mean = mn;

   numsamples=0;
   values = new double[numeta];
   errors = new double[numtheta];

   next_val=0.0;
   numsamples=0;
   return 0;
}

int EtaThetaPredictor::Begin()
{
   next_val = 0;
   numsamples=0;
   return 0;
}

int EtaThetaPredictor::StepsToPrime() const
{
  return MAX(numeta,numtheta) - 1;
}

double EtaThetaPredictor::Step(const double observation)
{
   int i;

   values[numsamples%numeta] = observation-mean;
   errors[numsamples%numtheta] = values[numsamples%numeta] - next_val;
   ++numsamples;

   // Compute expected next_val
   next_val=0;
   for (i=1;i<MIN(numsamples+1,numeta);i++) {
      next_val += (etas[i]) * values[(numsamples-i)%numeta];
   }
   for (i=1;i<MIN(numsamples+1,numtheta);i++) {
      next_val += (-thetas[i]) * errors[(numsamples-i)%numtheta];
   }
   return next_val+mean;
}


int EtaThetaPredictor::Predict(const int l,double *out) const
{
   int i,j;

   if (l==1) {
      out[0]=next_val+mean;
      return 0;
   }

   // i is the current lead time -1 
   for (i=0;i<l;i++) {
      out[i] = 0;
      // etas - first we gobble up the previously computed outs
      for (j=1;j<MIN(i+1,numeta);j++) {
         out[i] += (etas[j]) * out[i-j];
      }
      // etas - next we flit through the remaining values in the filter state
      for (j=i+1;j<MIN(i+1+numsamples,numeta);j++) {
         out[i] += (etas[j]) * values[(numsamples-j+i)%numeta];
      }
      // thetas - all zero except for those before the current time
      for (j=i+1;j<MIN(i+1+numsamples,numtheta);j++) {
         out[i] += (-thetas[j]) * errors[(numsamples-j+i)%numtheta];
      }
   }

   for (i=0;i<l;i++) {
     out[i]=out[i]+mean;
   }

   return 0;
}

// ests must be of size maxahead*maxahead
// order of covariances is: (+1,+1),(+1,+2),(+1,+3),...,(+2,+1),...
int EtaThetaPredictor::ComputeCoVariances(const int maxahead, double *ests) const
{
  int l1,l2,i,j;
  
  // Convert to truncated psi form
  // z_t = psi(B)x_t = (1 + psi_1B + psi_2B^2...)x_t
  // from the eta - theta form
  // eta(B)z_t = theta(B)x_t
  // (1 - eta_1B - eta_2B^2 -...)z_t = (1 - theta_1B - theta_2B^2 -  ...)x_t
  
  Polynomial pos_eta, pos_theta;
  
  pos_eta.SetPower(0);
  pos_eta.SetCoeff(0,1);
  for (i=1;i<numeta;i++) {
    pos_eta.SetCoeff(i,-etas[i]);
  }
  
  pos_theta.SetPower(0);
  pos_theta.SetCoeff(0,1);
  for (i=1;i<numtheta;i++) {
    pos_theta.SetCoeff(i,-thetas[i]);
  }
  
  pos_theta.DivideBy(&pos_eta,maxahead*3);
  
  double cur=0.0;
  
  for (l1=0;l1<maxahead;l1++) {
    for (l2=l1;l2<maxahead;l2++) {
      j=l2-l1;
      cur=0.0;
      for (i=0;i<=l1;i++) {
	cur += pos_theta.GetCoeff(i) * pos_theta.GetCoeff(i+j);
      }
      ests[l1*maxahead+l2]=ests[l2*maxahead+l1] = cur*variance;
    }
  }
  return 0;
}

int EtaThetaPredictor::ComputePointVariances(const int maxahead, double *ests) const
{
   int i;

   // Convert to truncated psi form
   // z_t = psi(B)x_t = (1 + psi_1B + psi_2B^2...)x_t
   // from the eta - theta form
   // eta(B)z_t = theta(B)x_t
   // (1 - eta_1B - eta_2B^2 -...)z_t = (1 - theta_1B - theta_2B^2 -  ...)x_t

   Polynomial pos_eta, pos_theta;

   pos_eta.SetPower(0);
   pos_eta.SetCoeff(0,1);
   for (i=1;i<numeta;i++) {
     pos_eta.SetCoeff(i,-etas[i]);
   }

   pos_theta.SetPower(0);
   pos_theta.SetCoeff(0,1);
   for (i=1;i<numtheta;i++) {
     pos_theta.SetCoeff(i,-thetas[i]);
   }

   pos_theta.DivideBy(&pos_eta,maxahead);

   double cur=0.0;

   for (i=0;i<maxahead;i++) {
      cur += pos_theta.GetCoeff(i) * pos_theta.GetCoeff(i);
      ests[i] = cur*variance;
   }
   return 0;
}


int EtaThetaPredictor::ComputeSumVariances(const int maxahead, double *ests) const
{
   int i,j,k;

   double *covars = new double[maxahead*maxahead];

   if (ComputeCoVariances(maxahead,covars)) { 
     delete [] covars;
     return -1;
   }
   for (i=0;i<maxahead;i++) {
     double cur=0.0;
     for (j=0;j<=i;j++) {
       for (k=0;k<=i;k++) {
	 cur+=covars[j*maxahead+k];
       }
     }
     ests[i]=cur;
   }
   delete [] covars;
   return 0;
}


int EtaThetaPredictor::ComputeVariances(const int maxahead, double *ests, 
					const enum VarianceType vtype) const
{
   switch (vtype) { 
   case POINT_VARIANCES:
     return ComputePointVariances(maxahead,ests);
     break;
   case CO_VARIANCES:
     return ComputeCoVariances(maxahead,ests);
     break;
   case SUM_VARIANCES:
     return ComputeSumVariances(maxahead,ests);
     break;
   }
   return -1;
}


#define NUM_COVARS_TO_PRINT 4
      
void EtaThetaPredictor::Dump(FILE *out) const
{
   int i,j;

   fprintf(out,"EtaThetaPredictor (numeta=%d, numtheta=%d, variance=%f, mean=%f)\n",
            numeta, numtheta, variance,mean);
   fprintf(out,"etas:");
   for (i=0;i<numeta;i++) {
      fprintf(out," %f",etas[i]);
   }
   fprintf(out,"\nthetas:");
   for (i=0;i<numtheta;i++) {
      fprintf(out," %f",thetas[i]);
   }
   fprintf(out,"\nAfter %d samples, state is\n",numsamples);
   fprintf(out,"values:");
   for (i=0;i<MIN(numsamples,numeta);i++) {
      fprintf(out," %f",values[(numsamples-MIN(numsamples,numeta)+i)%numeta]);
   }
   fprintf(out,"\nerrors:");
   for (i=0;i<MIN(numsamples,numtheta);i++) {
      fprintf(out," %f",errors[(numsamples-MIN(numsamples,numtheta)+i)%numtheta]);
   }
   fprintf(out,"\nnext predicted value=%f\n",next_val);

#if NUM_COVARS_TO_PRINT>0
   double *covars = new double [NUM_COVARS_TO_PRINT*NUM_COVARS_TO_PRINT];
   ComputeCoVariances(NUM_COVARS_TO_PRINT,covars);
   fprintf(out,"First %dx%d Covariances are:\n",NUM_COVARS_TO_PRINT,NUM_COVARS_TO_PRINT);
   for (i=0;i<NUM_COVARS_TO_PRINT;i++) {
     for (j=0;j<NUM_COVARS_TO_PRINT;j++) { 
       fprintf(out,"\t%f",covars[i*NUM_COVARS_TO_PRINT+j]);
     }
     fprintf(out,"\n");
   }
   delete [] covars;
#endif
}


ostream & EtaThetaPredictor::operator<<(ostream &os) const
{
   int i,j;

   os << "EtaThetaPredictor(numeta="<<numeta<<", numtheta="<<numtheta<<", variance="<<variance<<", mean="<<mean<<", etas=(";
   for (i=0;i<numeta;i++) {
     if (i>0) {
       os << ", ";
     } 
     os << etas[i];
   }
   os <<"), thetas=(";
   for (i=0;i<numtheta;i++) {
     if (i>0) {
       os << ", ";
     }
     os << thetas[i];
   }
   os <<"), numsamples="<<numsamples<<", values=(";
   for (i=0;i<MIN(numsamples,numeta);i++) {
     if (i>0) { 
       os << ", ";
     } 
     os << values[(numsamples-MIN(numsamples,numeta)+i)%numeta];
   }
   os <<"), errors=(";
   for (i=0;i<MIN(numsamples,numtheta);i++) {
     if (i>0) {
       os <<", ";
     }
     os << errors[(numsamples-MIN(numsamples,numtheta)+i)%numtheta];
   }
   os <<") next_predicted_value="<<next_val;

#if NUM_COVARS_TO_PRINT>0
   os <<", covariancematrix=(("<<NUM_COVARS_TO_PRINT<<"x"<<NUM_COVARS_TO_PRINT<<"), ";
   double *covars = new double [NUM_COVARS_TO_PRINT*NUM_COVARS_TO_PRINT];
   ComputeCoVariances(NUM_COVARS_TO_PRINT,covars);
   for (i=0;i<NUM_COVARS_TO_PRINT;i++) {
     if (i>0) {
       os <<",";
     }
     os << "(";
     for (j=0;j<NUM_COVARS_TO_PRINT;j++) { 
       if (j>0) {
	 os << ", ";
       }
       os << covars[i*NUM_COVARS_TO_PRINT+j];
     }
     os << ")";
   }
   os << ")";
   delete [] covars;
#endif
   os << ")";
   return os;
}
