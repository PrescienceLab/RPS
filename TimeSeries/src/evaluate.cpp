#include <stdio.h>
#include <stdlib.h>
#include "evaluate_core.h"
#include "util.h"

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
  int done, i,j;

  if (argc!=2) { 
    fprintf(stderr,"usage: evaluate [numpred]\n");
    exit(0);
  }

  unsigned numpred=atoi(argv[1]);
  Evaluator eval;


  double *row = new double [numpred+1];
  double *maxerr = new double [numpred];
  double *minerr = new double [numpred];
  double *msqerr = new double [numpred];
  double *meanabserr= new double [numpred];
  double *meanerr = new double [numpred];

  eval.Initialize(numpred);
  
  done=0;
  while (!feof(stdin) && !done) { 
    for (j=0;j<numpred+1;j++) {
      if (scanf("%lf",&(row[j]))!=1) {
	done=1;
	break;
      }
    }
    if (!done) {
      eval.Step(row);
    }
  }
  double conf;
  eval.Drain();
  PredictionStats *stats = eval.GetStats();

  printf("Predictor    MinErr     MaxErr     MeanAbsErr  MeanSquareError MeanErr\n");
  for (i=0;i<numpred;i++) { 
    printf(" t+%-3u      %10.3le %10.3le %10.3le  %10.3le %10.3le\n",
	   i+1,stats->minerr[i],stats->maxerr[i],stats->meanabserr[i],stats->msqerr[i],stats->meanerr[i]);
  }
  
  CHK_DEL(stats);
}
