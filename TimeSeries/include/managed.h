#ifndef _managed
#define _managed

#include "abstract.h"
#include "rps_log.h"

template <class MODELER> 
class ManagedPredictor : public Predictor {
 private:
  int num_await;
  int num_refit;
  int min_num_test;
  double errlimit;
  double varlimit;
  ParameterSet *params;
  double *seq;
  int     cur;
  int    err_n;
  int    err_nres;
  double err_s;
  double err_res;
  double err_s2;
  double lastpred;
  Model  *curmodel;
  Predictor *curpred;
 protected:
  void FitNow() {
   // cur points to the *oldest* data item - the one that
    // will be replaced bext
    int numoldest = num_refit - (cur%num_refit);
    int i;
    CHK_DEL(curmodel);
    CHK_DEL(curpred);
    double *linseq=new double[num_refit];
    memcpy(linseq,&(seq[cur%num_refit]),numoldest*sizeof(double));
    memcpy(&(linseq[numoldest]),seq,(num_refit-numoldest)*sizeof(double));
    curmodel = MODELER::Fit(linseq,num_refit,*params);
    curpred = curmodel->MakePredictor();
    for (i=0;i<num_refit;i++) { 
      curpred->Step(linseq[i]);
    }
    CHK_DEL_MAT(linseq);
    err_n=0;
    err_s=0;
    err_res=0;
    err_nres=0;
    err_s2=0;
  }
 public:
  ManagedPredictor(ParameterSet *params,
		   int num_await,
		   int num_refit,
		   int min_num_test,
		   double errlimit,
		   double varlimit) {
    this->num_await=num_await;
    this->num_refit=num_refit;
    this->min_num_test=min_num_test;
    this->errlimit=errlimit;
    this->varlimit=varlimit;
    this->params = params->Clone();
    seq = new double [num_refit];
    memset(seq,0,sizeof(double)*num_refit);
    err_n=0;
    err_s=0;
    err_res=0;
    err_nres=0;
    err_s2=0;
    cur=0;
    curmodel=0;
    curpred=0;
  }
  ~ManagedPredictor() { 
    CHK_DEL_MAT(seq);
    cur=0;
    CHK_DEL(curmodel);
    CHK_DEL(curpred);
    CHK_DEL(params);
  }
  int Begin() {
    return 0;
  }
  int StepsToPrime() { 
    return 0;
  }
  double Step(double obs) {
    seq[cur%num_refit]=obs;
    cur++;
    // First, see if we can now fit for the first time
    if (cur==num_await && curpred==0) {
      FitNow();
    }
    // Now, we may or may now have a predictor
    if (curpred==0) { 
      // if not, then just return the current value
      // we are still awaiting enough data
      lastpred=obs;
      return obs;
    } else {
      // refit if we have hit the refit interval
      if (!(cur%num_refit) && cur!=num_refit) { 
	RPSLog(CONTEXT, 10,"Hit refit interval - refitting\n");
	FitNow();
      } else {
	// Now update our error metrics
	err_n++;
	err_s+=(obs-lastpred);
	if (lastpred!=0) {
	  err_res+=fabs(obs-lastpred)/fabs(lastpred);
	  err_nres++;
	}
	err_s2+=(obs-lastpred)*(obs-lastpred);
	// If we have enough errors and the errors
	// are too bad, we'll force a refit.
	if (err_nres>=min_num_test) { 
	  if (err_res/err_nres > errlimit) {
	    RPSLog(CONTEXT, 10,"Exceeded error variance limit - refitting\n  (mean relative error is %f but limit is %f)\n",err_res/err_nres,errlimit);
	    FitNow();
	  }
	} 
	if (err_n>=min_num_test) {
	  double errmean=err_s/err_n;
	  double errvar=(err_s2-err_n*errmean*errmean)/(err_n-1);
	  double predvar;
	  curpred->ComputeVariances(1,&predvar,POINT_VARIANCES);
	  if (predvar==0) { 
	    if (errvar>0) {
	      RPSLog(CONTEXT,10,"Exceeded error variance prediction error limit - refitting\n     (predvar=%f, errvar=%f)\n",predvar,errvar);
	      FitNow();
	    }
	  } else {
	    if ( (errvar-predvar)/predvar > varlimit) { 
	      RPSLog(CONTEXT,10,"Exceeded error variance prediction error limit - refitting\n     (predvar=%f, errvar=%f, fabs(predvar-errvar)/predvar=%f varlimit=%f)\n",predvar,errvar,fabs(predvar-errvar)/predvar,varlimit);
	      FitNow();
	    }
	  }
	}
      }
      lastpred=curpred->Step(obs);
      return lastpred;
    }
  }
  
  int Predict(int maxahead, double *predictions) {
    if (curpred==0) { 
      for (int i=0;i<maxahead;i++) { 
	predictions[i]=lastpred;
      }
      return 0;
    } else {
      return curpred->Predict(maxahead,predictions);
    }
  }
  
  int ComputeVariances(int maxahead, double *vars, 
		       enum VarianceType vtype=POINT_VARIANCES) {
    if (curpred) { 
      return curpred->ComputeVariances(maxahead,vars,vtype);
    } else {
      int top= CO_VARIANCES ? maxahead*maxahead : maxahead;
      for (int i=0;i<top;i++) { 
	vars[i]=0;
      }
      return 0;
    }
  }

  void Dump(FILE *out=stdout) {
    fprintf(out, "ManagedPredictor cur=%d num_refit=%d num_refit=%d curmodel and curpredictor follow\n",
	    cur,num_refit,num_refit);
    if (curmodel) {
      curmodel->Dump(out);
    } else {
      fprintf(out,"No current model\n");
    }
    if (curpred) { 
      curpred->Dump(out);
    } else {
      fprintf(out,"No current predictor\n");
    }
  }

};


template <class MODELER>
class ManagedModel : public Model {
 private:
  int num_await;
  int num_refit;
  int min_num_test;
  double errlimit;
  double varlimit;
  ParameterSet *params;
 public:
  ManagedModel(const ParameterSet &ps,
	       int num_await,
	       int num_refit,
	       int min_num_test,
	       double errlimit,
	       double varlimit) {
    this->num_await=num_await;
    this->num_refit=num_refit;
    this->min_num_test=min_num_test;
    this->errlimit=errlimit;
    this->varlimit=varlimit;
    this->params = ps.Clone();
  }
  ~ManagedModel() { 
    CHK_DEL(params);
  }
  Predictor * MakePredictor() { 
    return new ManagedPredictor<MODELER>(params,
					 num_await,
					 num_refit,
					 min_num_test,
					 errlimit,
					 varlimit);
					 
  }
  void Dump(FILE *out=stdout) {
    fprintf(out,"ManagedModel, num_await=%d num_refit=%d min_num_test=%d, errlimit=%f varlimit=%f\n", num_await, num_refit, min_num_test, errlimit, varlimit);

  }
};


/*
  A managed predictor is one where the one-step ahead error is 
  continually monitored.  When the relative error of the predictor
  exceeds the error limit, the model is refit and a new predictor
  is created.  This also happens if the measured error variance
  exceeds the predicted error variance by a percentage
*/

template <class MODELER>
class ManagedModeler : public Modeler {
 public:
  static Model *Fit(double *sequence, int len, const ParameterSet &ps) {
    assert(0);
  }
  static ManagedModel<MODELER> *Fit(const ParameterSet &ps,
				    int num_await,
				    int num_refit,
				    int min_num_test,
				    double errlimit,
				    double varlimit) {
    assert(num_refit>=num_await);
    return new ManagedModel<MODELER>(ps,num_await,num_refit,min_num_test,
				     errlimit, varlimit);
  }
};


#endif
