#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include "crossval_core.h"
#include "fit.h"

int Testcase::Pack(PackInts PI, PackDoubles PD, PackString PS)
{
  PS(tag,strlen(tag));
  PI(&number,1);
  PI(&numahead,1);
  PI(&testfirst,1);
  PI(&testnum,1);
  PI(&fitfirst,1);
  PI(&fitnum,1);
  PI(&p,1);
  PI(&d,1);
  PI(&q,1);
  PI(&bmlimit,1);
  PI(&maxlag,1);
  PD(&acfconf,1);
  return 0;
}

int Testcase::Unpack(UnpackInts UI, UnpackDoubles UD, UnpackString US)
{
  US(tag,TAGLEN);
  UI(&number,1);
  UI(&numahead,1);
  UI(&testfirst,1);
  UI(&testnum,1);
  UI(&fitfirst,1);
  UI(&fitnum,1);
  UI(&p,1);
  UI(&d,1);
  UI(&q,1);
  UI(&bmlimit,1);
  UI(&maxlag,1);
  UD(&acfconf,1);
  return 0;
}

int IntervalStats::Pack(PackInts PI, PackDoubles PD)
{
  PI(&maxlag,1);
  PD(&acfconf,1);
  PD(&mean,1);
  PD(&var,1);
  PD(&median,1);
  PD(&min,1);
  PD(&max,1);
  PD(&Q,1);
  PD(&r2norm,1);
  PD(&sigacffrac,1);
  PD(&tpfrac,1);
  PD(&scfrac,1);
  return 0;
}

int IntervalStats::Unpack(UnpackInts UI, UnpackDoubles UD)
{
  UI(&maxlag,1);
  UD(&acfconf,1);
  UD(&mean,1);
  UD(&var,1);
  UD(&median,1);
  UD(&min,1);
  UD(&max,1);
  UD(&Q,1);
  UD(&r2norm,1);
  UD(&sigacffrac,1);
  UD(&tpfrac,1);
  UD(&scfrac,1);
  return 0;
}

#if 0
Model *FitThis(ModelType mclass,
	       double *seq, int numsamples, 
	       int p, double d, int q)
{
   switch (mclass) {
   case AR:
     return ARModeler::Fit(seq,numsamples,p);
     break;
   case MA:
     return MAModeler::Fit(seq,numsamples,q);
     break;
   case ARMA:
     return ARMAModeler::Fit(seq,numsamples,p,q);
     break;
   case ARIMA:
     return ARIMAModeler::Fit(seq,numsamples,p,(int)d,q);
     break;
   case ARFIMA:
     return ARFIMAModeler::Fit(seq,numsamples,p,d,q);
     break;
   case BESTMEAN:
     return BestMeanModeler::Fit(seq,numsamples,p);
     break;
   default:
     return (Model*)0;
   }
}

#endif

Model *FitThis(double *seq, int numsamples, 
	       int p, double d, int q)
{
  ModelType mclass;

  if (d==-1) {
    mclass=ARFIMA;
  } else {
    if (d>0) {
      mclass=ARIMA;
    } else {
      if (p>0) {
	if (q>0) {
	  mclass=ARMA;
	} else {
	  mclass=AR;
	}
      } else {
	mclass=MA;
      }
    }
  }

  return FitThis(mclass,seq,numsamples, p, d,q);
}


int DoIntervalStats(double *seq, int first, int num, int maxlag,
		    double acfconf,
		    IntervalStats *stats)
{

  assert(maxlag>2);

  stats->maxlag=maxlag;
  stats->acfconf=acfconf;

  stats->var=Variance(&(seq[first]),num);
  stats->mean=Mean(&(seq[first]),num);
  stats->min=Min(&(seq[first]),num);
  stats->max=Max(&(seq[first]),num);
  stats->median=Median(&seq[first],num);
  stats->r2norm = QuantilesOfNormal(&(seq[first]),num);
  PortmanteauTestForIID(&(seq[first]),num,&(stats->Q));

  int count;

  count=AcfTestForIID(&(seq[first]),num,maxlag,acfconf);
  stats->sigacffrac = ((double)count)/((double)(maxlag));
  
  TurningPointTestForIID(&(seq[first]),num,&count);
  stats->tpfrac = ((double)(count))/((double)(num-2));

  SignTestForIID(&(seq[first]),num,&count);
  stats->scfrac = ((double)(count))/((double)(num-1));

    
  return 0;
}


int IsBlowingUp(double msqerr) 
{
  if (msqerr>10000) {
    return 1;
  } else {	
    return 0;
  }
}

#define TEST_SEQ_INTEGRITY

int Simulate(double *seq,
	     Testcase        *testcase,
	     IntervalStats *fit,
	     IntervalStats *test,
	     int    *bmp, 
	     PredictionStats **testmodelstats,
	     PredictionStats **bmmodelstats)
{

  int i,rc;

  Model *model=0;
  Model *bmmodel=0;
  Predictor *pred=0;
  Predictor *bmpred=0;
  Evaluator eval;
  Evaluator bmeval;
  int blowup=0;

#ifdef TEST_SEQ_INTEGRITY
  double *fittemp = new double [testcase->fitnum];
  memcpy(fittemp,&(seq[testcase->fitfirst]),sizeof(double)*testcase->fitnum);
  double *testtemp = new double [testcase->testnum];
  memcpy(testtemp,&(seq[testcase->testfirst]),sizeof(double)*testcase->testnum);
#endif

  rc=0; 
  double *predictions = new double [testcase->numahead];
  double *bmpredictions = new double [testcase->numahead];

  if (predictions==0 || bmpredictions==0) {
    rc=-1; goto done;
  }

#if 0
  fprintf(stderr,"Fit to %d..%d and test on %d..%d\n",
	  testcase->fitfirst,
	  testcase->fitfirst+testcase->fitnum-1,
	  testcase->testfirst,
	  testcase->testfirst+testcase->testnum-1);
#endif

  DoIntervalStats(seq,testcase->fitfirst,testcase->fitnum,
		  testcase->maxlag,testcase->acfconf,fit);

  DoIntervalStats(seq,testcase->testfirst,testcase->testnum,
		  testcase->maxlag,testcase->acfconf,test);
  

  //  fprintf(stderr,"training sequence mean is %f and variance is %f\n", *fitmean, *fitvar);
  //fprintf(stderr,"testing  sequence mean is %f and variance is %f\n", *testmean, *testvar);

  model=FitThis(&(seq[testcase->fitfirst]),testcase->fitnum,
		testcase->p,testcase->d,testcase->q);

  if (model==0) {
    //fprintf(stderr,"model fit failed\n");
    rc=-1; goto done;
  }

  //model->Dump(stderr);

  pred = model->MakePredictor();

  if (pred==0) {
    //fprintf(stderr,"couldn't make predictor from model\n");
    rc=-1; goto done;
  }
  
  // prime predictor
  pred->Begin();
  for (i=testcase->fitfirst;i<testcase->fitfirst+testcase->fitnum;i++) {
    pred->Step(seq[i]);
  }

  //pred->Dump(stderr);
  
  bmmodel=FitThis(BESTMEAN,&(seq[testcase->fitfirst]),
		  testcase->fitnum,testcase->bmlimit,0,0);
  
  if (bmmodel==0) {
    fprintf(stderr,"bm model fit failed\n");
    rc=-1; goto done;
  }	

  *bmp=((BestMeanModel*)bmmodel)->GetOrder();
  //bmmodel->Dump(stderr);
  
  bmpred = bmmodel->MakePredictor();

  if (bmpred==0) {
    fprintf(stderr,"couldn't make bm predictor from model\n");
    rc=-1; goto done;
  }

  // prime predictor
  bmpred->Begin();
  for (i=testcase->fitfirst;i<testcase->fitfirst+testcase->fitnum;i++) {
    bmpred->Step(seq[i]);
  }
  
  //bmpred->Dump(stderr);

  // for the linear models, the variances will not change, so we will
  // compute them here, out of loop
  //pred->ComputeVariances(numahead,variances);
  //bmpred->ComputeVariances(numahead,bmvariances);
  //fprintf(stderr,"Lead\tVariance\tImprove\tBMVar\tBMImprove\n");
  //for (i=0;i<numahead;i++) {
  //  fprintf(stderr,"+%d\t%f\t%f%%\t%f\t%f%%\n",i+1,variances[i],
  //	    100.0*(fitvar-variances[i])/fitvar,
  //    bmvariances[i],
  //    100.0*(fitvar-bmvariances[i])/fitvar);
  //}
  
  if (eval.Initialize(testcase->numahead)) {
    //fprintf(stderr,"Out of memory for evaluator\n");
    exit(-1);
  }
  if (bmeval.Initialize(testcase->numahead)) {
    //fprintf(stderr,"Out of memory for bm evaluator\n");
    exit(-1);
  }
  for (i=testcase->testfirst;i<testcase->testfirst+testcase->testnum;i++) {
    pred->Step(seq[i]);
    pred->Predict(testcase->numahead,predictions);
    eval.Step(seq[i],predictions);
    bmpred->Step(seq[i]);
    bmpred->Predict(testcase->numahead,bmpredictions);
    bmeval.Step(seq[i],bmpredictions);
#ifdef CHECK_FOR_BLOWUP
    if (IsBlowingUp(eval.GetCurrentPlusOneMeanSquareError())) {
      if (IsBlowingUp(bmeval.GetCurrentPlusOneMeanSquareError())) {
	blowup=3;
      } else {
	blowup=1;
      }
      break;
    } else {
      if (IsBlowingUp(bmeval.GetCurrentPlusOneMeanSquareError())) {
	blowup=2;
	break;
      } else {
	blowup=0;
      }
    }
#else
    blowup=0;	
#endif
  }


  if (blowup) {
    *testmodelstats=new PredictionStats;
    *bmmodelstats=new PredictionStats;
    //fprintf(stderr,"%s blew up\n", 
    //    blowup==1 ? "test model" :
    //    blowup==2 ? "bm model" :
    //    blowup==3 ? "both models" : "UNKNOWN");
    if (blowup&0x1) {
      (*testmodelstats)->valid=0;
      (*testmodelstats)->usertags |= BLOWUP_TAG;
    } 
    if (blowup&0x2) {
      (*bmmodelstats)->valid=0;
      (*bmmodelstats)->usertags |= BLOWUP_TAG;
    }
  } else {
    eval.Drain();
    *testmodelstats = eval.GetStats(testcase->maxlag,testcase->acfconf);
    bmeval.Drain();
    *bmmodelstats = bmeval.GetStats(testcase->maxlag,testcase->acfconf);
  }

 done:
  CHK_DEL_MAT(predictions);
  CHK_DEL_MAT(bmpredictions);
  //CHK_DEL_MAT(variances);
  //CHK_DEL_MAT(bmvariances);
  CHK_DEL(bmpred);
  CHK_DEL(pred);
  CHK_DEL(bmmodel);
  CHK_DEL(model);

#ifdef TEST_SEQ_INTEGRITY
  //  fprintf(stderr,"Checking integrity of sequence\n");
  for (i=testcase->fitfirst;i<testcase->fitfirst+testcase->fitnum;i++) {
    if (fittemp[i-testcase->fitfirst]!=seq[i]) {
      fprintf(stderr,"seq[%d] changed from %f to %f\n",
	     i,
	     fittemp[i-testcase->fitfirst],
	     seq[i]);
    }
  }
  for (i=testcase->testfirst;i<testcase->testfirst+testcase->testnum;i++) {
    if (testtemp[i-testcase->testfirst]!=seq[i]) {
      fprintf(stderr,"seq[%d] changed from %f to %f\n",
	     i,
	     testtemp[i-testcase->testfirst],
	     seq[i]);
    }
  }
  CHK_DEL_MAT(testtemp);
  CHK_DEL_MAT(fittemp);
#endif

  

  return rc;
}


void OutputHeader(FILE *out, int outputformat)
{
  switch (outputformat) {
  case 0:
    fprintf(out,"#tag thistest i+1 fitfirst fitnum fitmean fitvar fitmin fitmedian fitmax fitsigacffrac fitQ fittpfrac fitscfrac fitr2norm testfirst testnum testmean testvar testmin testmedian testmax testsigacffrac testQ testtpfrac testscfrac testr2norm p d q bmlimit bmp bmmeanerr bmmsqerr bmmeanabserr bmminerr bmmedresid bmmaxerr bmsigacffrac bmpmQ bmtpfrac bmscfrac bmr2norm meanerr msqerr meanabserr minerr medresid maxerr sigacffrac pmQ tpfrac sscfrac r2norm\n"); 
    fflush(out);
    break;
  case 1:
   fprintf(out,"#%s %s %s %s %s %s %s %s %s %s %s %s\n",
	  "tag",
	  "testcase",
	  "lead",
	  "fitnum",
	  "testnum",
	  "p",
	  "d",
	  "q",
	  "bmlimit",
	  "testvar",
	  "msqerr",
	  "bmmsqerr");
   break;	
  }
}

void OutputTestcaseResults(FILE *out, 
			   Testcase *testcase,
			   IntervalStats *fit,	
			   IntervalStats *test,
			   int              bmp,
			   PredictionStats *testmodelstats,
			   PredictionStats *bmmodelstats,
			   int outputformat)
{
  int i;

  if (testcase->maxlag!=fit->maxlag || 
      fit->maxlag!=test->maxlag || 
      test->maxlag!=bmmodelstats->maxlag ||
      bmmodelstats->maxlag!=testmodelstats->maxlag ||
      testcase->acfconf!=fit->acfconf ||
      fit->acfconf!=test->acfconf || 
      test->acfconf!=bmmodelstats->acfconf ||
      bmmodelstats->acfconf!=testmodelstats->acfconf) {
    fprintf(stderr,"Warning: acfconf and/or maxlags do not match\n");
  }
  for (i=0;i<testcase->numahead;i++) {
    switch (outputformat) {
    case 0:
      fprintf(out,"%s %d +%d %d %d %f %f %f %f %f %f %f %f %f %f %d %d %f %f %f %f %f %f %f %f %f %f %d %d %d %d %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n",
      	     testcase->tag,
	     testcase->number,
	     i+1,
	     testcase->fitfirst,
	     testcase->fitnum,
	     fit->mean,
	     fit->var,
	     fit->min,
	     fit->median,
	     fit->max,
	     fit->sigacffrac,
	     fit->Q,
	     fit->tpfrac,
	     fit->scfrac,
	     fit->r2norm,
	     testcase->testfirst,
	     testcase->testnum,
	     test->mean,	
	     test->var,
	     test->min,
	     test->median,
	     test->max,
	     test->sigacffrac,
	     test->Q,	
	     test->tpfrac,
	     test->scfrac,
	     test->r2norm,
	     testcase->p,
	     testcase->d,
	     testcase->q,
	     testcase->bmlimit,
	     bmp,
	     bmmodelstats->GetMeanError(i+1), 
	     bmmodelstats->GetMeanSquaredError(i+1),
	     bmmodelstats->GetMeanAbsError(i+1), 
	     bmmodelstats->GetMinError(i+1), 
	      bmmodelstats->medianresid,
	      bmmodelstats->GetMaxError(i+1), 
	      bmmodelstats->sigacffrac,
	      bmmodelstats->portmanteauQ,
	      bmmodelstats->tpfrac,
	      bmmodelstats->scfrac,
	      bmmodelstats->r2normfit,
	      testmodelstats->GetMeanError(i+1), 
	      testmodelstats->GetMeanSquaredError(i+1), 
	      testmodelstats->GetMeanAbsError(i+1), 
	      testmodelstats->GetMinError(i+1), 
	      testmodelstats->medianresid,
	      testmodelstats->GetMaxError(i+1), 
	      testmodelstats->sigacffrac,
	      testmodelstats->portmanteauQ,
	      testmodelstats->tpfrac,
	      testmodelstats->scfrac,
	      testmodelstats->r2normfit);
      fflush(out);
      break;
    case 1:
      fprintf(out,"%s %d +%d %d %d %d %d %d %d %f %f %f\n",
	     testcase->tag,
	     testcase->number,
	     i+1,	
	     testcase->fitnum,
	     testcase->testnum,
	     testcase->p,
	     testcase->d,
	     testcase->q,
	     testcase->bmlimit,
	     test->var,
	     testmodelstats->GetMeanSquaredError(i+1), 
	      + bmmodelstats->GetMeanSquaredError(i+1));	          
      break;
    }
  }
}


Command *GetCommand(FILE *cmd)
{
  Command *x = new Command;

  if (feof(cmd) ||
	 fscanf(cmd,"%d %d %d %d %d %d %d %d %d %d %d %d %d",
		&(x->numtests), 
		&(x->numahead), 
		&(x->bmlimit), 
		&(x->pmin), 
		&(x->pmax), 
		&(x->dmin), 
		&(x->dmax), 
		&(x->qmin), 
		&(x->qmax), 
		&(x->minfit), 
		&(x->maxfit),
		&(x->mintest), 
		&(x->maxtest))!=13) {
    delete x;
    return 0;
  } else {
    return x;
  }
}
	

void Pervert8(char *buf)
{
        char temp;
        unsigned i;

        for (i=0;i<4;i++) {
                temp=buf[i];
                buf[i]=buf[7-i];
                buf[7-i]=temp;
        }
}

#define PervertDouble(x) Pervert8((char*)x);

double *LoadBinaryTraceFile(char *name, int *num, int perversion)
{
  FILE *infile;
  struct stat st;
  unsigned numsamples;
  unsigned i;

  if (stat(name,&st)) { 
    fprintf(stderr,"Can't stat %s\n",name);
    return 0;
  }
  if ((st.st_size/16)*16 != st.st_size) { 
    fprintf(stderr,"%s does not contain an integral number of records\n",
	    name); 
    return 0;
  }

  numsamples = st.st_size/16;

  infile=fopen(name,"rb");

  double *alldata = new double [numsamples*2];
  double *output = new double [numsamples];

  if (fread(alldata,2*sizeof(double),numsamples,infile)!=numsamples) {
    fprintf(stderr,"fread failed\n");
    delete [] alldata;
    delete [] output;
    return 0;
  }

  for (i=0;i<numsamples;i++) { 
    output[i]=alldata[1+i*2];
    if (perversion) {
      PervertDouble(&(output[i]));
    }
  }
  fclose(infile);
  delete [] alldata;
  *num=numsamples;
  return output;
}


Testcase *TestcaseGenerator::GenerateRandomTestcase()
{
  Testcase *t = new Testcase;
  strncpy(t->tag,tag,TAGLEN);
  t->number=time(0)+UnsignedRandom();
  t->numahead=cmd.numahead;
  t->maxlag=maxlag;
  t->acfconf=acfconf;
  // choose intervals
  do {
    t->testnum = cmd.mintest + UnsignedRandom()%(cmd.maxtest-cmd.mintest+1);
    t->testfirst=UnsignedRandom()%(numsamples-t->testnum);
    t->fitnum=cmd.minfit + UnsignedRandom()%(cmd.maxfit-cmd.minfit+1);
    t->fitfirst=t->testfirst-t->fitnum;
  } while (t->fitfirst<0 || t->testfirst<0);
  // best mean limit
  t->bmlimit=cmd.bmlimit;
  // Choose p, d, q
  t->p = cmd.pmin + UnsignedRandom()%(cmd.pmax-cmd.pmin+1);
  t->d = cmd.dmin + UnsignedRandom()%(cmd.dmax-cmd.dmin+1);
  t->q = cmd.qmin + UnsignedRandom()%(cmd.qmax-cmd.qmin+1);
  
  t->maxlag=maxlag;
  t->acfconf=acfconf;
  return t;
}
