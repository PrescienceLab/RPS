#include "etathetapred.h"
#include "util.h"

EtaThetaPredictor::EtaThetaPredictor()
{
   numeta=numtheta=0;
   etas=thetas=values=errors=0;
   variance=0;
}

EtaThetaPredictor::~EtaThetaPredictor()
{
   CHK_DEL_MAT(etas);
   CHK_DEL_MAT(thetas);
   numeta=numtheta=0;
   variance=0;
}

int EtaThetaPredictor::Initialize(Polynomial *et, Polynomial *th, double var, double mn)
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
 
/*
  int i;
   for (i=0;i<numeta;i++) {
      values[i]=0.0;
   }

   for (i=0;i<numtheta;i++) {
      errors[i]=0.0;
   }
*/
   next_val = 0;
   numsamples=0;

   return 0;
}

int EtaThetaPredictor::StepsToPrime()
{
  return MAX(numeta,numtheta) - 1;
}

double EtaThetaPredictor::Step(double observation)
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


int EtaThetaPredictor::Predict(int l,double *out)
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
int EtaThetaPredictor::ComputeCoVariances(int maxahead, double *ests)
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

int EtaThetaPredictor::ComputePointVariances(int maxahead, double *ests)
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


int EtaThetaPredictor::ComputeSumVariances(int maxahead, double *ests)
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


int EtaThetaPredictor::ComputeVariances(int maxahead, double *ests, 
					enum VarianceType vtype)
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
      
void EtaThetaPredictor::Dump(FILE *out)
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
