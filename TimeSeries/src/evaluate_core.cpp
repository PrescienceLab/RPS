#include <new>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "evaluate_core.h"
#include "util.h"
#include "maths.h"
#include "random.h"
#include "tools.h"

using namespace std;

// evaluate [numpred] < sequence 
//
// Where sequence is of the form
//
//    value_t   pred_t+1 pred_t+2 pred_t+3 ... pred_t+numpred
//    value_t+1 
//
// For each prediction column, statistics are computed
//


Evaluator::Evaluator()
{
  numpred=numsamples=0;	
  row=0;
  maxerr=minerr=msqerr=meanabserr=meanerr=0;
  resids=0;
}

Evaluator::Evaluator(const Evaluator &rhs) 
{
  numpred=numsamples=0;
  row=0;
  maxerr=minerr=msqerr=meanabserr=meanerr=0;
  resids=0;
  Initialize(rhs.numpred,rhs.seensamples);
  numpred=rhs.numpred;
  numsamples=rhs.numsamples;
  seensamples=rhs.seensamples;
  for (int i=0;i<numpred+1;i++) {
    memcpy(row[i],rhs.row[i],sizeof(double)*(numpred+1));
  }
  memcpy(maxerr,rhs.maxerr,sizeof(double)*numpred);
  memcpy(minerr,rhs.minerr,sizeof(double)*numpred);
  memcpy(msqerr,rhs.msqerr,sizeof(double)*numpred);
  memcpy(meanabserr,rhs.meanabserr,sizeof(double)*numpred);
  memcpy(meanerr,rhs.meanerr,sizeof(double)*numpred);
  memcpy(resids,rhs.resids,sizeof(double)*residbufsize);
}

Evaluator & Evaluator::operator=(const Evaluator &rhs)
{
  this->~Evaluator();
  return *(new(this)Evaluator(rhs));
}
  

Evaluator::~Evaluator()
{
  Clear();
}
int Evaluator::Realloc(const int numresids)
{
  int size = numresids<=0 ? 2*residbufsize : numresids;

  double *temp = new double [size];

  memcpy(temp,resids,sizeof(double)*MIN(size,residbufsize));

  CHK_DEL_MAT(resids);
  resids = temp;
  residbufsize = size;
  return 0;
}

int Evaluator::Clear() 
{
  if (row!=0) {
    int i;
    for (i=0;i<numpred+1;i++) {
      CHK_DEL_MAT(row[i]);
    }
  }
  CHK_DEL_MAT(maxerr);
  CHK_DEL_MAT(minerr);
  CHK_DEL_MAT(msqerr);
  CHK_DEL_MAT(meanabserr);
  CHK_DEL_MAT(meanerr);
  CHK_DEL_MAT(resids);
  numpred=numsamples=0;
  return 0;
}  
  
int Evaluator::Initialize(const int numpred, const int numresids)
{
  int i;

  if (Clear()) {
    return -1;
  }
  this->numpred=numpred;

  row = new double * [numpred+1];

  for (i=0;i<numpred+1;i++) {
    row[i] = new double [numpred+1];
  }
  
  maxerr = new double [numpred];
  minerr = new double [numpred];
  msqerr = new double [numpred];
  meanabserr= new double [numpred];
  meanerr = new double [numpred];
  resids = new double [numresids];
  residbufsize = numresids;
  for (i=0;i<numpred;i++) {
    maxerr[i] = -999999999999999999e99;
    minerr[i] = +999999999999999999e99;
    msqerr[i] = 0.0;
    meanabserr[i] = 0.0;
    meanerr[i]=0.0;
  }
  numsamples=0;
  seensamples=0;
  return 0;
}


int Evaluator::Step(const double *curandpreds)
{
  return Step(curandpreds[0],&(curandpreds[1]));
}

int Evaluator::Step(const double cur, const double *pred)
{
  int j;
  double err;

  if (numpred<1) {
    return -1;
  }

  if (seensamples<numpred+1) {
    // prime the pipe - must have at least this many numbers
    row[seensamples][0]=cur;
    for (j=1;j<numpred+1;j++) {
      row[seensamples][j]=pred[j-1];
    }
    ++seensamples;
    return 0;
  } else {
    for (j=0;j<numpred;j++) {
      err = row[numsamples%(numpred+1)][j+1]      // previous prediction
	- row[(numsamples+j+1)%(numpred+1)][0];   // actual value
                                 // store +1 residuals
      if (j==0) {
	if (numsamples>=residbufsize) {
	  Realloc();
	}
	resids[numsamples]=err;
      }
      if (err>maxerr[j]) { 
	maxerr[j]=err;
      }	
      if (err<minerr[j]) { 
	minerr[j]=err;
      }
      msqerr[j]+=err*err;
      meanabserr[j]+=fabs(err);
      meanerr[j]+=err;
    }
    row[numsamples%(numpred+1)][0]=cur;
    for (j=1;j<numpred+1;j++) {
      row[(numsamples)%(numpred+1)][j]=pred[j-1];
    }
    numsamples++;
    seensamples++;
  }
  return 0;
}


int Evaluator::Drain()
{
  int i,j;
  double err;

  // drain pipe
  for (i=0;i<numpred;i++) {
    for (j=0;j<(numpred-i);j++) {
      err = row[numsamples%(numpred+1)][j+1] 
	- row[(numsamples+j+1)%(numpred+1)][0];
      if (j==0) {                                 // store +1 residuals
	if (numsamples>=residbufsize) {
	  Realloc();
	}
	resids[numsamples]=err;
      }
      if (err>maxerr[j]) { 
	maxerr[j]=err;
      }
      if (err<minerr[j]) { 
	minerr[j]=err;
      }
      msqerr[j]+=err*err;
      meanabserr[j]+=fabs(err);
      meanerr[j]+=err;
    }	
    numsamples++;
  }
  return 0;
}

PredictionStats *Evaluator::GetStats(const int maxlag, const double acfconf) const
{
  int j;

  if (numsamples==0) {
    return (PredictionStats*)0;
  }
  
  PredictionStats *stats = new PredictionStats;

  stats->Initialize(numpred);
  
  stats->numsamples=numsamples;
  stats->numpred=numpred;
  stats->maxlag=maxlag;
  stats->usertags=0;
  stats->acfconf=acfconf;

  //Compute the MSQ error and mean abs err and the mean err
  for (j=0;j<numpred;j++) { 
    stats->minerr[j] = this->minerr[j];
    stats->maxerr[j] = this->maxerr[j];
    stats->msqerr[j] = this->msqerr[j]/=(numsamples-j);
    stats->meanabserr[j] = this->meanabserr[j]/(numsamples-j);
    stats->meanerr[j] = this->meanerr[j]/(numsamples-j);
  }

  // Compute median eror
  stats->medianresid = Median(resids,numsamples);
  if (numsamples<maxlag+30) {
    stats->sigacffrac=-1;
    stats->portmanteauQ=-1;
  } else {
    stats->sigacffrac =
      (double)AcfTestForIID(resids,numsamples,maxlag,acfconf)/((double)(maxlag));	
    PortmanteauTestForIID(resids,numsamples,
			  &(stats->portmanteauQ),
			  maxlag);
  }
  int num;
  if (numsamples<3) {
    stats->tpfrac=-1;
  } else {
    TurningPointTestForIID(resids,numsamples,&num);
    stats->tpfrac = ((double)num)/((double)numsamples-2.0);
  }
  if (numsamples<2) {
    stats->scfrac=-1;
    stats->r2normfit=-1;
  } else {
    SignTestForIID(resids,numsamples,&num);
    stats->scfrac  = ((double)num)/((double)numsamples-1.0);
    stats->r2normfit=QuantilesOfNormal(resids,numsamples);
  }

  stats->valid=1;
  return stats;	
}

double Evaluator::GetCurrentPlusOneMeanSquareError() const
{
  if (numsamples>0) {
    return msqerr[0]/numsamples;
  } else {
    return 0.0;
  }
}
  
    


int Evaluator::Dump(FILE *out) const
{
  fprintf(out,"Evaluator:  numpred=%d, numsamples=%d, seensamples=%d, residbufsize=%d rows follow\n",
	  numpred,numsamples,seensamples,residbufsize);
  int i;
  for (i=0;i<numpred+1;i++) {
    for (int j=0;j<numpred+1;j++) {
      if (j>0) {
	fprintf(out,", ");
      }
      fprintf(out,"%f",row[i][j]);
    }
    fprintf(out,"\n");
  }
  fprintf(out,"maxerror data follows\n");
  for (i=0;i<numpred;i++) {
    fprintf(out,"%f\n",maxerr[i]);
  }
  fprintf(out,"minerror data follows\n");
  for (i=0;i<numpred;i++) {
    fprintf(out,"%f\n",minerr[i]);
  }
  fprintf(out,"msqerror data follows\n");
  for (i=0;i<numpred;i++) {
    fprintf(out,"%f\n",msqerr[i]);
  }
  fprintf(out,"meanabserror data follows\n");
  for (i=0;i<numpred;i++) {
    fprintf(out,"%f\n",meanabserr[i]);
  }
  fprintf(out,"meanerror data follows\n");
  for (i=0;i<numpred;i++) {
    fprintf(out,"%f\n",meanerr[i]);
  }
  fprintf(out,"residuals follow");
  for (i=0;i<numsamples;i++) {
    fprintf(out,"%f\n",resids[i]);
  }
  return 0;
}
	  

ostream & Evaluator::operator<<(ostream &os) const
{
  os << "Evaluator(numpred="<<numpred<<", numsamples="<<numsamples<<", seensamples="<<seensamples;
  os << "residbufsize="<<residbufsize<<"rows=(";
  int i;
  for (i=0;i<numpred+1;i++) {
    if (i>0) {
      os <<", ";
    }
    os << "(";
    for (int j=0;j<numpred+1;j++) {
      if (j>0) {
	os <<", ";
      }
      os << row[i][j];
    }
    os << ")";
  }
  os <<"), maxerror=(";
  for (i=0;i<numpred;i++) {
    if (i>0) {
      os << ", ";
    }
    os << maxerr[i];
  }
  os <<"), minerror=(";
  for (i=0;i<numpred;i++) {
    if (i>0) {
      os <<", ";
    }
    os << minerr[i];
  }
  os <<"), msqerror=(";
  for (i=0;i<numpred;i++) {
    if (i>0) {
      os <<", ";
    }
    os << msqerr[i];
  }
  os <<"), meanabserror=(";
  for (i=0;i<numpred;i++) {
    if (i>0) {
      os <<", ";
    }
    os << meanabserr[i];
  }
  os <<"), meanerror=(";
  for (i=0;i<numpred;i++) {
    if (i>0) {
      os <<", ";
    }
    os << meanerr[i];
  }
  os <<"), resids=(";
  for (i=0;i<numsamples;i++) {
    if (i>0) {
      os <<", ";
    }
    os << resids[i];
  }
  return os;
}


PredictionStats::PredictionStats()
{
  valid=0; usertags=0;
  msqerr=meanabserr=meanerr=minerr=maxerr=0;
}

PredictionStats::PredictionStats(const PredictionStats &rhs)
{
  valid=rhs.valid;
  usertags=rhs.usertags;
  numsamples=rhs.numsamples;
  numpred=rhs.numsamples;
  maxlag=rhs.maxlag;
  acfconf=rhs.acfconf;
  msqerr = new double [numpred]; memcpy(msqerr,rhs.msqerr,sizeof(double)*numpred);
  meanabserr = new double [numpred]; memcpy(meanabserr,rhs.meanabserr,sizeof(double)*numpred);
  meanerr = new double [numpred]; memcpy(meanerr,rhs.meanerr,sizeof(double)*numpred);
  minerr = new double [numpred]; memcpy(minerr,rhs.minerr,sizeof(double)*numpred);
  maxerr = new double [numpred]; memcpy(maxerr,rhs.maxerr,sizeof(double)*numpred);
  medianresid=rhs.medianresid;
  sigacffrac=rhs.sigacffrac;
  tpfrac=rhs.tpfrac;
  scfrac=rhs.scfrac;
  portmanteauQ=rhs.portmanteauQ;
  r2normfit=rhs.r2normfit;
}

PredictionStats & PredictionStats::operator=(const PredictionStats &rhs)
{
  this->~PredictionStats();
  return *(new(this)PredictionStats(rhs));
}


int PredictionStats::Initialize(int numpred)
{
  this->numpred=numpred;
  CHK_DEL_MAT(msqerr); msqerr = new double [numpred];
  CHK_DEL_MAT(meanabserr); meanabserr = new double [numpred];
  CHK_DEL_MAT(meanerr); meanerr = new double [numpred];
  CHK_DEL_MAT(minerr); minerr = new double [numpred];
  CHK_DEL_MAT(maxerr); maxerr = new double [numpred];
  return 0;
}

PredictionStats::~PredictionStats()
{
  CHK_DEL_MAT(msqerr);
  CHK_DEL_MAT(meanabserr);
  CHK_DEL_MAT(meanerr);
  CHK_DEL_MAT(minerr);
  CHK_DEL_MAT(maxerr);
}

double PredictionStats::GetMeanSquaredError(int pred) const
{
  if (pred<1 || pred > numpred) {
    return 9.9e99;
  } else {
    return msqerr[pred-1];
  }
}


double PredictionStats::GetMeanAbsError(int pred) const
{
  if (pred<1 || pred > numpred) {
    return 9.9e99;
  } else {
    return meanabserr[pred-1];
  }
}
double PredictionStats::GetMeanError(int pred) const
{
  if (pred<1 || pred > numpred) {
    return 9.9e99;
  } else {
    return meanerr[pred-1];
  }
}

double PredictionStats::GetMinError(int pred) const
{
  if (pred<1 || pred > numpred) {
    return 9.9e99;
  } else {
    return minerr[pred-1];
  }
}
  
double PredictionStats::GetMaxError(int pred) const
{
  if (pred<1 || pred > numpred) {
    return 9.9e99;
  } else {
    return maxerr[pred-1];
  }
}



int PredictionStats::Pack(PackDoubles PD,
			  PackInts    PI) 
{
  PI(&valid,1);
  PI(&usertags,1);
  if (valid) {
    PI(&numsamples,1);
    PI(&numpred,1);
    PI(&maxlag,1);
    PD(&acfconf,1);
    PD(msqerr,numpred);
    PD(meanabserr,numpred);
    PD(meanerr,numpred);
    PD(minerr,numpred);
    PD(maxerr,numpred);
    PD(&medianresid,1);
    PD(&sigacffrac,1);
    PD(&tpfrac,1);
    PD(&scfrac,1);
    PD(&portmanteauQ,1);
    PD(&r2normfit,1);
  }
  return 0;
}

int PredictionStats::Unpack(UnpackDoubles UD,
			    UnpackInts    UI) 
{
  UI(&valid,1);
  UI(&usertags,1);
  if (valid) {
    UI(&numsamples,1);
    UI(&numpred,1);
    UI(&maxlag,1);
    UD(&acfconf,1);
    Initialize(numpred);
    UD(msqerr,numpred);
    UD(meanabserr,numpred);
    UD(meanerr,numpred);
    UD(minerr,numpred);
    UD(maxerr,numpred);
    UD(&medianresid,1);
    UD(&sigacffrac,1);
    UD(&tpfrac,1);
    UD(&scfrac,1);
    UD(&portmanteauQ,1);
    UD(&r2normfit,1);
  }
  return 0;
}

#define STRINGIZE(T) #T
#define PRINT_HEX(out,name) fprintf((out),"%s:\t0x%x\n",STRINGIZE(name),name)
#define PRINT_INT(out,name) fprintf((out),"%s:\t%d\n",STRINGIZE(name),name)
#define PRINT_FLOAT(out,name) fprintf((out),"%s:\t%f\n",STRINGIZE(name),name)

int PredictionStats::Dump(FILE *out) const
{
  fprintf(out,"PredictionStats::Dump\n");
  PRINT_HEX(out,valid);
  PRINT_HEX(out,usertags);
  PRINT_INT(out,numsamples);
  PRINT_INT(out,numpred);
  PRINT_INT(out,maxlag);
  PRINT_INT(out,valid);
  PRINT_INT(out,valid);
  PRINT_FLOAT(out,acfconf);
  PRINT_FLOAT(out,sigacffrac);
  PRINT_FLOAT(out,tpfrac);
  PRINT_FLOAT(out,scfrac);
  PRINT_FLOAT(out,portmanteauQ);
  PRINT_FLOAT(out,r2normfit);
  PRINT_FLOAT(out,medianresid);

  fprintf(out,"Lead\tmsqerr\tmeanabserr\tmeanerr\tminerr\tmaxerr\n");
  int i;

  for (i=0;i<numpred;i++) {
    fprintf(out,"+%d\t%f\t%f\t%f\t%f\t%f\n",
	    i+1, msqerr[i], meanabserr[i],
	    meanerr[i], minerr[i], maxerr[i]);
  }
  return 0;
}


ostream & PredictionStats::operator<<(ostream &os) const
{
  os<<"PredictionStats(valid="<<valid<<", usertags="<<usertags<<", numsamples="<<numsamples
    <<", numpred="<<numpred<<", maxlag="<<maxlag<<", acfconf="<<acfconf<<", medianresid="
    <<medianresid<<", sigacffrac="<<sigacffrac<<", tpfrac="<<tpfrac<<", scfrac="<<scfrac
    <<", portmanteauQ="<<portmanteauQ<<", r2normfit="<<r2normfit;
  os <<", msqerr=(";
  int i;
  for (i=0; i<numpred;i++) {
    if (i>0) {
      os << ", ";
    }
    os << msqerr[i];
  }
  os <<")";
  os <<", msqerr=(";
  for (i=0; i<numpred;i++) {
    if (i>0) {
      os << ", ";
    }
    os << msqerr[i];
  }
  os <<")";
  os <<", meanabserr=(";
  for (i=0; i<numpred;i++) {
    if (i>0) {
      os << ", ";
    }
    os << meanabserr[i];
  }
  os <<")";
  os <<", meanerr=(";
  for (i=0; i<numpred;i++) {
    if (i>0) {
      os << ", ";
    }
    os << meanerr[i];
  }
  os <<")";
  os <<", minerr=(";
  for (i=0; i<numpred;i++) {
    if (i>0) {
      os << ", ";
    }
    os << minerr[i];
  }
  os <<")";
  os <<", maxerr=(";
  for (i=0; i<numpred;i++) {
    if (i>0) {
      os << ", ";
    }
    os << maxerr[i];
  }
  os <<")";
  os <<")";
  return os;
}
