#include <stdlib.h>
#include <stdio.h>

#include "EstimateExecTime.h"

#include "maths.h"
#include "util.h"
#include "tools.h"

void ExecTimeEstimationRequest::Print(FILE *out)
{
  fprintf(out,"ExecTimeEstimationRequest: cputime=%lf conf=%lf\n",
	  cputime, confidence);
}

void ExecTimeEstimationReply::Print(FILE *out)
{
  fprintf(out,"ExecTimeEstimationReply: cputime=%lf conf=%lf, expected=%lf [%lf,%lf] meanload=%lf discountedmeadload=%lf varload=%lf\n",
	  cputime, confidence, expectedtime, cilower, ciupper, meanload, discountedmeanload, varload);
}

int GetLatestPrediction(PredBufferRef &ref, PredictionResponse &resp)
{
  BufferDataRequest breq;
  BufferDataReply<PredictionResponse> bresp;

  breq.num=1;

  ref.Call(breq,bresp);

  if (bresp.num>0) {
    double c;
    c = 1.0;
    c += bresp.data[0].preds[0];
    resp=bresp.data[0];
    return 0;
  } else {
    return -1;
  }
}



#define PRINT 0
  

int EstimateExecTimeApproachOne(PredictionResponse        &presp,
				ExecTimeEstimationRequest &req,
				ExecTimeEstimationReply   &repl,
				double tau)
  
{
  int i;
  double exectime;
  double steps;
  double sample_interval;
  double sumerrs;
  double sumpreds;
  double fractionofstepused;
  double cputimeinstep;
  double cputimeleft;
  double discountedpred;

  steps=0;
  exectime=0;
  sample_interval = ((double)(presp.period_usec))/1e6;
  sumerrs=sumpreds=0.0;
  cputimeleft=req.cputime;
  for (i=0;i<presp.numsteps && cputimeleft>0 ;i++) { 
#if DISCOUNT_LOAD
    discountedpred = presp.preds[i]*(1-exp(-(i+0.5)*sample_interval/tau));
#else
    discountedpred = presp.preds[i];
#endif
    cputimeinstep = sample_interval/(1+MAX(0.0,discountedpred));
    fractionofstepused= MIN(1.0,cputimeleft/cputimeinstep);
    steps+=fractionofstepused;
    cputimeleft-=cputimeinstep*fractionofstepused;
    exectime+=sample_interval*fractionofstepused;
    sumpreds+=discountedpred*fractionofstepused;
    sumerrs+=SQUARE(sqrt(presp.errs[i])*fractionofstepused);
#if PRINT
    fprintf(stderr,"%d:\n"
                   "preds[%d]            %lf\n"
                   "errs[%d]             %lf\n"
                   "discountedpred       %lf\n"
	           "cputimeinstep        %lf\n"
                   "fractionofstepused   %lf\n"
	           "steps                %lf\n"
                   "cputimeleft          %lf\n"
                   "exectime             %lf\n"
                   "sumpreds             %lf\n"       
                   "sumerrs              %lf\n",
	    i, i, presp.preds[i], i, presp.errs[i], discountedpred,
	    cputimeinstep, fractionofstepused, steps, cputimeleft,
	    exectime, sumpreds, sumerrs);
#endif
  }
  if (cputimeleft>0) { 
    // prediction horizon isn't long enough
#if PRINT
    fprintf(stderr,"Warning: ran off the end of a predicted sequence - assuming last value continues\n");
#endif
    while (cputimeleft>0) { 
#if DISCOUNT_LOAD
      discountedpred = presp.preds[i]*(1-exp(-(i+0.5)*sample_interval/tau));
#else
      discountedpred = presp.preds[i];
#endif
      cputimeinstep = 
        sample_interval/(1+MAX(0.0,discountedpred));
      fractionofstepused= MIN(1.0,cputimeleft/cputimeinstep);
      steps+=fractionofstepused;
      cputimeleft-=cputimeinstep*fractionofstepused;
      exectime+=sample_interval*fractionofstepused;
      sumpreds+=discountedpred*fractionofstepused;
      sumerrs+=SQUARE(sqrt(presp.errs[presp.numsteps-1])*fractionofstepused);
#if PRINT
      fprintf(stderr,"%d:\n"
	      "preds[%d]            %lf\n"
	      "errs[%d]             %lf\n"
	      "discountedpred       %lf\n"
	      "cputimeinstep        %lf\n"
	      "fractionofstepused   %lf\n"
	      "steps                %lf\n"
	      "cputimeleft          %lf\n"
	      "exectime             %lf\n"
	      "sumpreds             %lf\n"       
	      "sumerrs              %lf\n",
	      i, i, presp.preds[presp.numsteps-1], i, 
	      presp.errs[presp.numsteps-1], discountedpred,
	      cputimeinstep, fractionofstepused, steps, cputimeleft,
	      exectime, sumpreds, sumerrs);
#endif
      i++;
    }
  }

  double numsigmas = ConfToSigmas(req.confidence);

  double expectedloadduringexec = 1+sumpreds/steps;
  double ciloadduringexec = 2*numsigmas*sqrt(sumerrs)/steps;

  repl.confidence=req.confidence;
  repl.cputime=req.cputime;
  repl.expectedtime=req.cputime*expectedloadduringexec;
  repl.cilower=MAX(req.cputime,
		   repl.expectedtime-repl.expectedtime*ciloadduringexec);
  repl.ciupper=repl.expectedtime+repl.expectedtime*ciloadduringexec;


#if PRINT
  fprintf(stderr,"expectedloadduringexec  %lf\n"
	         "ciloadduringexec        %lf\n"
                 "confidence              %lf\n"    
                 "cputime                 %lf\n"
                 "expectedtime            %lf  [%lf,%lf] @ %lf%%\n",
	         expectedloadduringexec,
	         ciloadduringexec,
	         repl.confidence,
                 repl.cputime,
                 repl.expectedtime,
	         repl.cilower, repl.ciupper,
                 100.0*repl.confidence);
#endif
                 
  return 0;

}

enum EstimateType {LOWER_BOUND, EXPECTED, UPPER_BOUND} ;


int EstimateExecTimeApproachTwo(EstimateType estimatetype,
				PredictionResponse &presp,
				double conf, 
				double cputime,
				double tau,
				double &exectime){
  int i;
  double sample_interval;
  double fractionofstepused;
  double cputimeinstep;
  double cputimeleft;
  double numsigmas;
  double load, error;

  if (estimatetype!=EXPECTED) { 
    numsigmas = ConfToSigmas(conf);
  }
  exectime=0;
  cputimeleft=cputime;
  sample_interval = ((double)(presp.period_usec))/1e6;

  for (i=0;i<presp.numsteps && cputimeleft>0; i++) { 
    load = presp.preds[i];
    error= presp.errs[i];
    if (estimatetype==UPPER_BOUND) {
      load+=numsigmas*sqrt(error);
    } else if (estimatetype==LOWER_BOUND) {
      load-=numsigmas*sqrt(error);
    }
#if DISCOUNT_LOAD
    load = load*(1-exp(-(i+0.5)*sample_interval/tau));
#else
    load=load;
#endif
    load = 1.0 + MAX(0.0,load);
    cputimeinstep = sample_interval/load;
    fractionofstepused= MIN(1.0,cputimeleft/cputimeinstep);
    cputimeleft-=cputimeinstep*fractionofstepused;
    exectime+=sample_interval*fractionofstepused;
#if PRINT
      fprintf(stderr,"%d:\n"
	      "preds[%d]            %lf\n"
	      "errs[%d]             %lf\n"
              "load                 %lf\n"
	      "cputimeinstep        %lf\n"
	      "fractionofstepused   %lf\n"
	      "cputimeleft          %lf\n"
	      "exectime             %lf\n",
	      i, i, presp.preds[i], i, presp.errs[i], load,
	      cputimeinstep, fractionofstepused, cputimeleft,
	      exectime);
#endif
  }
  if (cputimeleft>0) { 
#if PRINT
    fprintf(stderr,"Warning: ran off the end of a predicted sequence - assuming last value continues\n");
#endif
    while (cputimeleft) { 
      load = presp.preds[presp.numsteps-1];
      error= presp.errs[presp.numsteps-1];
      if (estimatetype==UPPER_BOUND) {
	load+=numsigmas*sqrt(error);
      } else if (estimatetype==LOWER_BOUND) {
	load-=numsigmas*sqrt(error);
      }
#if DISCOUNT_LOAD
      load = load*(1-exp(-(i+0.5)*sample_interval/tau));
#else
      load = load;
#endif
      load = 1.0 + MAX(0.0,load);
      cputimeinstep = sample_interval/load;
      fractionofstepused= MIN(1.0,cputimeleft/cputimeinstep);
      cputimeleft-=cputimeinstep*fractionofstepused;
      exectime+=sample_interval*fractionofstepused;
#if PRINT
      fprintf(stderr,"%d:\n"
	      "preds[%d]            %lf\n"
	      "errs[%d]             %lf\n"
              "load                 %lf\n"
	      "cputimeinstep        %lf\n"
	      "fractionofstepused   %lf\n"
	      "cputimeleft          %lf\n"
	      "exectime             %lf\n",
	      i, i, presp.preds[presp.numsteps-1], i, 
	      presp.errs[presp.numsteps-1],load,
	      cputimeinstep, fractionofstepused, cputimeleft,
	      exectime);
#endif
      i++;
    }
  }

  return 0;
}

  

			    


int EstimateExecTimeApproachTwo(PredictionResponse        &presp, 
				ExecTimeEstimationRequest &req,
				ExecTimeEstimationReply   &repl,
				double tau)
{
  double upper, lower;
  
  repl.confidence=req.confidence;
  repl.cputime=req.cputime;

  if (EstimateExecTimeApproachTwo(LOWER_BOUND, 
				  presp,
				  req.confidence,
				  req.cputime,
				  tau,
				  repl.cilower)) {
    return -1;
  }
  if (EstimateExecTimeApproachTwo(EXPECTED, 
				  presp,
				  req.confidence,
				  req.cputime,
				  tau,
				  repl.expectedtime)) {
    return -1;
  }
  if (EstimateExecTimeApproachTwo(UPPER_BOUND, 
				  presp,
				  req.confidence,
				  req.cputime,
				  tau,
				  repl.ciupper)) {
    return -1;
  }

  return 0;
}


#define VARIANCES_ARE_SUM_VARIANCES 1

int EstimateExecTimeApproachThree(PredictionResponse        &presp, 
				  ExecTimeEstimationRequest &req,
				  ExecTimeEstimationReply   &repl,
				  double tau)
{
  int i;
  int n = presp.numsteps;
  double sample_interval = ((double)(presp.period_usec))/1e6;
  double numsigmas = ConfToSigmas(req.confidence);
  double cputime=req.cputime;

  double * discountedpreds = new double[n];
  double * predmeans = new double[n];
  double * predmeanstds = new double[n];
  double * comptimeexp = new double[n];
  double * comptimelb = new double[n];
  double * comptimeub = new double[n];

  if  (discountedpreds==0 ||
       predmeans==0 ||
       predmeanstds==0 ||
       comptimeexp==0 ||
       comptimelb==0 ||
       comptimeub==0) { 
    goto FAIL;
  }

  for (i=0;i<n;i++) { 
#if DISCOUNT_LOAD
      discountedpreds[i] = presp.preds[i]*(1-exp(-(i+0.5)*sample_interval/tau));
#else
    discountedpreds[i] = presp.preds[i];
#endif
  }


  for (i=0;i<n;i++) { 
    predmeans[i] = Mean(discountedpreds,i+1);
#if VARIANCES_ARE_SUM_VARIANCES 
    predmeanstds[i] = sqrt(presp.errs[i]/((i+1)*(i+1)));
#else
    predmeanstds[i] = sqrt(Sum(presp.errs,i+1)/((i+1)*(i+1)));
#endif
    //    fprintf(stderr,"%d\t%lf\t%lf\n",i,predmeans[i],predmeanstds[i]);
  }

  for (i=0;i<n;i++) { 
    comptimeexp[i] = sample_interval*(i+1)/(1+MAX(0.0,predmeans[i]));
    comptimelb[i] = sample_interval*(i+1)/(1+MAX(0.0,predmeans[i]+numsigmas*predmeanstds[i]));
    comptimeub[i] = sample_interval*(i+1)/(1+MAX(0.0,predmeans[i]-numsigmas*predmeanstds[i]));
    // fprintf(stderr,"%d\t%lf\t%lf\t%lf\n",i,comptimeexp[i],comptimelb[i],comptimeub[i]);
  }
  
  // Now we have estimates for cumulative available exectime

  if (comptimelb[n-1]<cputime) { 
    fprintf(stderr,"Not enough predictions comptimelb[%d]=%lf\n",n-1,comptimelb[n-1]);
    goto FAIL;
  }

  double curtime;

  
  for (i=0;i<n;i++) {
    if (comptimeexp[i]>=cputime) {
      double basetime = i==0 ? 0.0 : comptimeexp[i-1];
      double nextfrac = comptimeexp[i]==cputime ? 1.0 :
                    (cputime-basetime)/(comptimeexp[i]-basetime);
      repl.expectedtime = (i+nextfrac)*sample_interval;
      repl.discountedmeanload = predmeans[i];
      repl.meanload = Mean(presp.preds,i+1);
      repl.varload = predmeanstds[i]*predmeanstds[i];
      break;
    }
  }
  for (i=0;i<n;i++) {
    if (comptimelb[i]>=cputime) {
      double basetime = i==0 ? 0.0 : comptimelb[i-1];
      double nextfrac = comptimelb[i]==cputime ? 1.0 :
                    (cputime-basetime)/(comptimelb[i]-basetime);
      repl.ciupper = (i+nextfrac)*sample_interval;
      break;
    }
  }
  for (i=0;i<n;i++) {
    if (comptimeub[i]>=cputime) {
      double basetime = i==0 ? 0.0 : comptimeub[i-1];
      double nextfrac = comptimeub[i]==cputime ? 1.0 :
                    (cputime-basetime)/(comptimeub[i]-basetime);
      repl.cilower = (i+nextfrac)*sample_interval;
      break;
    }
  }

  repl.confidence=req.confidence;
  repl.cputime=req.cputime;
  
  CHK_DEL_MAT(discountedpreds);
  CHK_DEL_MAT(predmeans);
  CHK_DEL_MAT(predmeanstds);
  CHK_DEL_MAT(comptimeexp);
  CHK_DEL_MAT(comptimelb);
  CHK_DEL_MAT(comptimeub);

  return 0;
  
 FAIL:
  CHK_DEL_MAT(discountedpreds);
  CHK_DEL_MAT(predmeans);
  CHK_DEL_MAT(predmeanstds);
  CHK_DEL_MAT(comptimeexp);
  CHK_DEL_MAT(comptimelb);
  CHK_DEL_MAT(comptimeub);
  return -1;



}


int EstimateExecTime(PredBufferRef &ref, 
		     ExecTimeEstimationRequest &req,
		     ExecTimeEstimationReply   &repl,
		     double tau)
{
  PredictionResponse presp;

  if (GetLatestPrediction(ref,presp)) { 
    return -1;
  }

  return EstimateExecTime(presp,req,repl,tau);
}

int EstimateExecTime(PredictionResponse        &presp, 
		     ExecTimeEstimationRequest &req,
		     ExecTimeEstimationReply   &repl,
		     double tau)

{
  int rc;

  ExecTimeEstimationRequest thereq=req;
  ExecTimeEstimationReply   therepl=repl;

  thereq.Print(stderr);

#if 0
  rc=EstimateExecTimeApproachOne(presp,thereq,therepl,tau);
  fprintf(stderr,"Approach One estimate\n");
  therepl.Print(stderr);
#if 0
  repl=therepl;
#endif

  rc=EstimateExecTimeApproachTwo(presp,thereq,therepl,tau);
  fprintf(stderr,"Approach Two estimate\n");
  therepl.Print(stderr);

#if 0
  repl=therepl;
#endif
#endif

  rc=EstimateExecTimeApproachThree(presp,thereq,therepl,tau);
  fprintf(stderr,"Approach Three estimate\n");
  therepl.Print(stderr);

#if 1
  repl=therepl;
#endif
  return rc;
}
