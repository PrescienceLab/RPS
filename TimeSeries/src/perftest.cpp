#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "fit.h"
#include "bestmean.h"
#include "tools.h"
#include "evaluate_core.h"
#include "random.h"
#include "util.h"

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#define DO_EVAL 0
#define DO_FINEGRAIN_STEPPRED

void GetRusage(double &systime, double &usrtime)
{
  struct rusage x;

  if (getrusage(RUSAGE_SELF,&x)) {
    perror("getrusage");
    exit(-1);
  }

  systime=x.ru_stime.tv_sec + x.ru_stime.tv_usec/1.0e6;
  usrtime=x.ru_utime.tv_sec + x.ru_utime.tv_usec/1.0e6;
}



void usage() 
{
   fprintf(stderr,
      "perftest [inputfile] [tag] [numahead] [numpasses] [fitint] [testint] [flat|nofloat] [REFIT interval] \n"
      "                                                 MEAN\n"
      "                                               | LAST\n" 
      "                                               | BESTMEAN [p]\n"
      "                                               | AR [p]\n"
      "                                               | MA [q]\n"
      "                                               | ARMA [p] [q]\n"
      "                                               | ARIMA [p] [d] [q]\n"
      "                                               | ARFIMA [p] [d] [q]\n");
}


void main(int argc, char *argv[])
{
  int i,j;
  int p,d,q,r;

  const int first_model=8;
  char *infile;
  int numahead;
  int fitint, testint;
  int numint;

  FILE *inp;
  int     numsamples;
  double junk;
  double *seq;
  bool flat;



  Model *model=0;
  Predictor *pred=0;
#if DO_EVAL
  Evaluator eval;
#endif

  if (argc<first_model+1) {
    usage();
    exit(-1);
  }

  infile=argv[1];
  char *tag=argv[2];
  numahead=atoi(argv[3]);
  numint = atoi(argv[4]);
  fitint = atoi(argv[5]);
  testint = atoi(argv[6]);
  flat = argv[7][0]=='f' || argv[7][0]=='F';

  double *predictions = new double [numahead];
  double *variances = new double [numahead];

  double *fittimes = new double [numint];
  double *makepredtimes = new double [numint];
  double *primepredtimes = new double [numint];
  double *computevarstimes = new double [numint];

  double *meanpredsteppredicttimes = new double [numint];

  double *predstepn = new double [numint];
  double *predstepsums = new double [numint];
  double *predstepsum2s = new double [numint];

  double *predpredn = new double [numint];
  double *predpredsums = new double [numint];
  double *predpredsum2s = new double [numint];

  double *evaltimes = new double [numint];

  if (predictions==0 || 
      variances==0 || 
      fittimes==0 ||
      makepredtimes==0 ||
      primepredtimes==0 ||
      computevarstimes==0 ||
      meanpredsteppredicttimes==0 ||
      predstepn==0 ||
      predstepsums==0 ||
      predstepsum2s==0 ||
      predpredn==0 ||
      predpredsums==0 ||
      predpredsum2s==0 ||
      evaltimes==0) {
    fprintf(stderr,"Insufficient memory\n");
    exit(-1);
  }

  ModelTemplate *mt = ParseModel(argc-first_model,&(argv[first_model]));

  char *mtstring = mt->GetName();
 

  if (mt==0) { 
    usage();
    exit(-1);
  }

  // icky
  if (mt->ps->GetType()==PDQ) {
    ((PDQParameterSet*)(mt->ps))->Get(p,d,q);
    r=0;
  } else if (mt->ps->GetType()==RefittingPDQ) { 
    ((RefittingPDQParameterSet*)(mt->ps))->Get(p,d,q);
    ((RefittingPDQParameterSet*)(mt->ps))->GetRefit(r);
  }
    

  inp = fopen(infile,"r");
  if (inp==0) {
    fprintf(stderr,"%s not found.\n",infile);
    exit(-1);
  }

  numsamples=0;
  while ((fscanf(inp,"%lf %lf\n",&junk,&junk)==2)) {
    ++numsamples;
  }
  rewind(inp);

  seq = new double [numsamples];
  if (seq==0) {
    fprintf(stderr,"insufficient memory to read %s\n",infile);
    exit(-1);
  }


  for (i=0;i<numsamples;i++) { 
    fscanf(inp,"%lf %lf\n",&junk,&(seq[i]));
  }

  fclose(inp);


  int step,fitfirst,testfirst,testnum,fitnum;
  double usrbegin, sysbegin, usrend, sysend;
  double stepusrbegin, stepsysbegin, stepusrend, stepsysend;
  double ustepusrbegin, ustepsysbegin, ustepusrend, ustepsysend;

  double runtotal, runsteptotal;

  runtotal=runsteptotal=0;
  
  InitRandom();

  for (step=0;step<numint;step++) {
    // Choose random testcase
    do {
      testnum = testint;
      fitnum = fitint;
      testfirst = rand()%(numsamples-testnum);
      fitfirst=testfirst-fitnum;
    } while (fitfirst<0 || testfirst<0);
     
    GetRusage(stepsysbegin,stepusrbegin);

    CHK_DEL(model);

    
    GetRusage(sysbegin,usrbegin);

    model=FitThis(&(seq[fitfirst]),fitnum,*mt);

    GetRusage(sysend,usrend);

    fittimes[step] = (sysend-sysbegin) + (usrend-usrbegin);

    //fprintf(stderr,"%d\t%lf                   \r",step,fittimes[step]);

    if (model==0) {
      fprintf(stderr,"model fit failed\n");
      exit(-1);
    }

    CHK_DEL(pred);

    GetRusage(sysbegin,usrbegin);

    pred = model->MakePredictor();

    GetRusage(sysend,usrend);

    makepredtimes[step] = (sysend-sysbegin) + (usrend-usrbegin);

    if (pred==0) {
      fprintf(stderr,"couldn't make predictor from model\n");
      exit(-1);
    }
     
    GetRusage(sysbegin,usrbegin);

    // prime predictor
    pred->Begin();
    int primesteps = pred->StepsToPrime();
    for (i=fitfirst+fitnum-primesteps;i<fitfirst+fitnum;i++) {
      pred->Step(seq[i]);
    }

    GetRusage(sysend,usrend);

    primepredtimes[step] = (sysend-sysbegin) + (usrend-usrbegin);

     
    GetRusage(sysbegin,usrbegin);

    pred->ComputeVariances(numahead,variances);

    GetRusage(sysend,usrend);

    computevarstimes[step] = (sysend-sysbegin) + (usrend-usrbegin);


#if DO_EVAL
    if (eval.Initialize(numahead)) {
      fprintf(stderr,"Out of memory for evaluator\n");
      exit(-1);
    }

#endif

    GetRusage(sysbegin,usrbegin);

    for (i=testfirst;i<testfirst+testnum;i++) {

      pred->Step(seq[i]);
      pred->Predict(numahead,predictions);
#if DO_EVAL
      eval.Step(seq[i],predictions);
#endif
    }
    GetRusage(sysend,usrend);
    meanpredsteppredicttimes[step] = 
      ((sysend-sysbegin) + (usrend-usrbegin)) / testnum; 

#if DO_EVAL
    eval.Drain();
    PredictionStats *teststats = eval.GetStats();
    bmeval.Drain();
    PredictionStats *bmstats = bmeval.GetStats();
    for (i=0;i<numahead;i++) {
      printf("%s %d +%d %d %d %d %d %d %d %lf %lf %lf\n",
	     tag,
	     step,
	     i+1,
	     fitnum,
	     testnum,
	     p,
	     d,
	     q,
	     bmlimit,
	     testvar,
	     teststats->GetMeanSquaredError(i+1),
	     bmstats->GetMeanSquaredError(i+1));
      /*       printf("eval:\t+%d\t%lf\t%lf\t%lf%%\t%lf\t%lf%%\t%lf%%\n", 
	       i+1, 
	       testvar, 
	       msqerr[i],
	       testvar==0 ? 0 : 100.0*(testvar-msqerr[i])/testvar,
	       bmmsqerr[i],
	       testvar==0 ? 0 : 100.0*(testvar-bmmsqerr[i])/testvar,
	       100.0*(bmmsqerr[i]-msqerr[i])/bmmsqerr[i]);
      */
      meantestvar[i] += testvar;
      meanmsqerr[i] += teststats->GetMeanSquaredError(i+1);
      meanbmmsqerr[i] += bmstats->GetMeanSquaredError(i+1);
      meanimproveoversig[i] += (testvar-teststats->GetMeanSquaredError(i+1));
      meanimproveoverbm[i] +=
	(bmstats->GetMeanSquaredError(i+1)-teststats->GetMeanSquaredError(i+1));

    }
    delete bmstats;
    delete teststats;
    /*     printf("Predictor\tMinErr\tMaxErr\tMeanAbsErr\tMeanSquareError\tMeanErr\n");
	   for (i=0;i<numahead;i++) { 
	   printf(" t+%-3u\t%10.3le\t%10.3le\t%10.3le\t%10.3le\t%10.3le\n",
	   i+1,minerr[i],maxerr[i],meanabserr[i],msqerr[i],meanerr[i]);
	   }
    */
#endif
    GetRusage(stepsysend,stepusrend);
    double steptime=(stepsysend-stepsysbegin)+(stepusrend-stepusrbegin);
    double steptotal=	    fittimes[step]+makepredtimes[step]+
	    primepredtimes[step]+computevarstimes[step]+
	    meanpredsteppredicttimes[step]*testnum;

#if 0
    fprintf(stderr,"%d\t%lf\t%lf\t%lf\n",
	    step, steptime,steptotal,steptime-steptotal);
#endif

    runtotal+=steptime;
    runsteptotal+=steptotal;

	      
  }

  if (flat) { 
    fprintf(stdout,"# tag mt p d q r numahead numint fitint testint meanfittime stdfittime meanmakepredtime stdmakepredtime meanprimetime stdprimetime meancompvartime stdcompvartime meanpredsteptime stdpredsteptime\n");

    fprintf(stdout, "%s %s %d %d %d %d %d %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
	  tag,
	  mtstring,
	  p,
	  d,
	  q,
	  r,
	  numahead,
	  numint,
	  fitint,
	  testint,
	  1000.0*Mean(fittimes,numint),
	  1000.0*StandardDeviation(fittimes,numint),
	  1000.0*Mean(makepredtimes,numint),
	  1000.0*StandardDeviation(makepredtimes,numint),
	  1000.0*Mean(primepredtimes,numint),
	  1000.0*StandardDeviation(primepredtimes,numint),
	  1000.0*Mean(computevarstimes,numint),
	  1000.0*StandardDeviation(computevarstimes,numint),
	  1000.0*Mean(meanpredsteppredicttimes,numint),
	  1000.0*StandardDeviation(meanpredsteppredicttimes,numint));	  
  } else {

    fprintf(stdout,
	    "tag              %s\n"
	    "mtstring         %s\n"
	    "p                %d\n"
	    "d                %d\n"
	    "q                %d\n"
	    "r                %d\n"
	    "numahead         %d\n"
	    "numint           %d\n"
	    "fitint           %d\n"
	    "testint          %d\n"
	    "meanfittime      %lf ms\n"
	    "stdfittime       %lf ms\n"
	    "meanmakepredtime %lf ms\n"
	    "stdmakepredtime  %lf ms\n"
	    "meanprimetime    %lf ms\n"
	    "stdprimetime     %lf ms\n"
	    "meancompvartime  %lf ms\n"
	    "stdcompvartime   %lf ms\n"
	    "meanpredsteptime %lf ms\n"
	    "stdpredsteptime  %lf ms\n",
	    tag,
	    mtstring,
	    p,
	    d,
	    q,
	    r,
	    numahead,
	    numint,
	    fitint,
	    testint,
	    1000.0*Mean(fittimes,numint),
	    1000.0*StandardDeviation(fittimes,numint),
	    1000.0*Mean(makepredtimes,numint),
	    1000.0*StandardDeviation(makepredtimes,numint),
	    1000.0*Mean(primepredtimes,numint),
	    1000.0*StandardDeviation(primepredtimes,numint),
	    1000.0*Mean(computevarstimes,numint),
	    1000.0*StandardDeviation(computevarstimes,numint),
	    1000.0*Mean(meanpredsteppredicttimes,numint),
	    1000.0*StandardDeviation(meanpredsteppredicttimes,numint));	  
  }
#if 0
    fprintf(stderr,"runtotal=%lf, runsteptotal=%lf\n",
	    runtotal,runsteptotal);
#endif
}

