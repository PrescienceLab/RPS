#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// evaluate [numpred] < sequence 
//
// Where sequence is of the form
//
//    value_t   pred_t+1 pred_t+2 pred_t+3 ... pred_t+numpred
//    value_t+1 
//
// For each prediction column, statistics are computed
//


void main(int argc, char *argv[]) 
{
  if (argc!=2) { 
    fprintf(stderr,"usage: evaluate [numpred]\n");
    exit(0);
  }

  unsigned numpred=atoi(argv[1]);
  unsigned i,j,k;
  unsigned numsamples;
  unsigned seensamples=0;
  double err;

  double **row = new double * [numpred+1];

  for (i=0;i<numpred+1;i++) {
    row[i] = new double [numpred+1];
  }
  
  double *maxerr = new double [numpred];
  double *minerr = new double [numpred];
  double *msqerr = new double [numpred];
  double *meanabserr= new double [numpred];
  double *meanerr = new double [numpred];
  for (i=0;i<numpred;i++) {
    maxerr[i] = -999999999999999999e99;
    minerr[i] = +999999999999999999e99;
    msqerr[i] = 0.0;
    meanabserr[i] = 0.0;
    meanerr[i]=0.0;
  }
  
  // prime the pipe - must have at least this many numbers

  for (i=0;i<numpred+1;i++) {
    for (j=0;j<numpred+1;j++) {
      scanf("%lf",&(row[i][j]));
    }
    ++seensamples;
  }

#define DO_ERR() \
      err = row[numsamples%(numpred+1)][j+1] \
	- row[(numsamples+j+1)%(numpred+1)][0];\
      if (err>maxerr[j]) { \
	maxerr[j]=err;\
      }\
      if (err<minerr[j]) { \
	minerr[j]=err;\
      }\
      msqerr[i]+=err*err;\
      meanabserr[i]+=fabs(err);\
      meanerr[i]+=err;\

  unsigned done=0;
  numsamples=0;
  while (!feof(stdin) && !done) { 
    for (j=0;j<numpred;j++) {
      err = row[numsamples%(numpred+1)][j+1] 
	- row[(numsamples+j+1)%(numpred+1)][0];
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
    for (j=0;j<numpred+1;j++) {
      if (scanf("%lf",&(row[(numsamples)%(numpred+1)][j]))!=1) {
	done=1;
	break;
      }
    }
    if (!done) {
      numsamples++;
      seensamples++;
    }
  }
  
  // drain the pipe

  for (i=1;i<numpred+1;i++) {
    for (j=0;j<(numpred-i);j++) {
      err = row[numsamples%(numpred+1)][j+1] 
	- row[(numsamples+j+1)%(numpred+1)][0];
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
    seensamples++;
  }

  //Compute the MSQ error and mean abs err and the mean err
  for (j=0;j<numpred;j++) { 
    msqerr[j]/=(numsamples-j);
    meanabserr[j]/=(numsamples-j);
    meanerr[j]/=(numsamples-j);
  }

  // Print
  fprintf(stderr,"(numpred=%d,numsamples=%d,seensamples=%d)\n",
	  numpred,numsamples,seensamples);
  printf("Predictor    MinErr     MaxErr     MeanAbsErr  MeanSquareError MeanErr\n");
  for (i=0;i<numpred;i++) { 
    printf(" t+%-3u      %10.3le %10.3le %10.3le  %10.3le %10.3le\n",
	   i+1,minerr[i],maxerr[i],meanabserr[i],msqerr[i],meanerr[i]);
  }
  
}
