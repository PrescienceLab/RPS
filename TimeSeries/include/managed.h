#ifndef _managed
#define _managed

#include <new>
#include <typeinfo>
#include "abstract.h"
#include "rps_log.h"
#include "pdqparamsets.h"


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
    cerr << "FitNow!";
    this->operator<<(cerr);
    cerr << endl;
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
  ManagedPredictor() :
    num_await(0), num_refit(0), min_num_test(0), errlimit(0), varlimit(0), params(0),
    seq(0),  cur(0), err_n(0), err_nres(0), err_s(0), err_res(0), err_s2(0), lastpred(0), 
    curmodel(0), curpred(0)
  {}

  ManagedPredictor(const ManagedPredictor &rhs) {
    num_await=rhs.numawait;
    num_refit=rhs.numrefit;
    min_num_test=rhs.min_num_test;
    errlimit=rhs.errlimit;
    varlimit=rhs.errlimit;
    params=rhs.params->Clone();
    seq = new double [num_refit];
    memcpy(seq,rhs.seq,num_refit*sizeof(double));
    cur = rhs.cur;
    err_n=rhs.err_n;
    err_nres=rhs.err_nres;
    err_s=rhs.err_s;
    err_s2=rhs.err.s2;
    lastpred=rhs.lastpred;
    curmodel=rhs.curmodel->Clone();
    curpred=rhs.curpred->Clone();
  }
   
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

  ManagedPredictor & operator=(const ManagedPredictor &rhs) {
    this->~ManagedPredictor();
    return *(new(this)ManagedPredictor<MODELER>(rhs));
  }

  int Begin() {
    return 0;
  }
  int StepsToPrime() const { 
    return 0;
  }
  double Step(const double obs) {
    seq[cur%num_refit]=obs;
    cur++;
    // First, see if we can now fit for the first time
    if (cur==num_await && curpred==0) {
      RPSLog(CONTEXT,10,"Have enough intial data - fitting\n");
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
  
  int Predict(const int maxahead, double *predictions) const {
    if (curpred==0) { 
      for (int i=0;i<maxahead;i++) { 
	predictions[i]=lastpred;
      }
      return 0;
    } else {
      return curpred->Predict(maxahead,predictions);
    }
  }
  
  int ComputeVariances(const int maxahead, double *vars, 
		       const enum VarianceType vtype=POINT_VARIANCES) const {
    if (curpred) { 
      return curpred->ComputeVariances(maxahead,vars,POINT_VARIANCES);
    } else {
      int top= (vtype==CO_VARIANCES) ? maxahead*maxahead : maxahead;
      for (int i=0;i<top;i++) { 
	vars[i]=0;
      }
      return 0;
    }
  }

  void Dump(FILE *out=stdout) const {
    fprintf(out, "ManagedPredictor<%s> cur=%d num_await=%d num_refit=%d min_num_test=%d errlimit=%f varlimit=%f err_n=%d err_nres=%d err_s=%f err_res=%f err_s2=%f lastpred=%f, parameter set, curmodel, curpredictor, and sequence follow\n",
	    typeid(MODELER).name(),cur,num_await,num_refit,min_num_test,errlimit,varlimit,err_n,err_nres,err_s,err_res,err_s2,lastpred);
    if (params) { 
      params->Dump(out);
    } else {
      fprintf(out,"No parameter set\n");
    }
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
    for (int i=0;i<num_refit;i++) {
      fprintf(out,"%f\n",seq[i]);
    }
  }
  ostream & operator<<(ostream &os) const {
    os << "ManagedPredictor<"<<typeid(MODELER).name()<<"<(num_await="<<num_await<<", num_refit="<<num_refit<<", min_num_test="<<min_num_test<<", errlimit="<<errlimit<<", varlimit="<<varlimit<<", err_n="<<err_n<<", err_nres="<<err_nres<<", err_s="<<err_s<<", err_res="<<err_res<<", err_s2="<<err_s2<<", lastpred="<<lastpred<<", ps=";
    if (params) {
      os << *params;
    } else {
      os << "none";
    }
    os <<", curmodel=";
    if (curmodel) {
      os <<*curmodel;
    } else {
      os <<"none";
    }
    os <<", curpred=";
    if (curpred) {  
      os << *curpred;
    } else {
      os <<"none";
    }
    os << ", cur="<<cur<<", seq=("; 
    for (int i=0;i<num_refit;i++) {
      if (i>0) {
	os << ", ";
      }
      os << seq[i];
    }
    os << "))";
    return os;
  }
};

template <class MODELER>
inline ostream & operator<< (ostream &os, const ManagedPredictor<MODELER> &p) {
  return p.operator<<(os);
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
  ManagedModel() : num_await(0), num_refit(0), min_num_test(0), errlimit(0), varlimit(0), params(0) 
  {}
  ManagedModel(const ManagedModel<MODELER> &rhs) : num_await(rhs.num_await), num_refit(rhs.num_refit),
    min_num_test(rhs.min_num_test), errlimit(rhs.errlimit), varlimit(rhs.varlimit),
    params(rhs.params->Clone()) 
  {}
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
  ManagedModel<MODELER> & operator=(const ManagedModel<MODELER> &rhs) {
    this->~ManagedModel();
    return *(new(this)ManagedModel<MODELER>(rhs));
  }

  Predictor * MakePredictor() const { 
    return new ManagedPredictor<MODELER>(params,
					 num_await,
					 num_refit,
					 min_num_test,
					 errlimit,
					 varlimit);
					 
  }
  void Dump(FILE *out=stdout) const {
    fprintf(out,"ManagedModel<%s> num_await=%d num_refit=%d min_num_test=%d, errlimit=%f varlimit=%f paramters follow\n", 
	    typeid(MODELER).name(), num_await, num_refit, min_num_test, errlimit, varlimit);
    if (params) {
      params->Dump(out);
    } else {
      fprintf(out,"No Parameter Set\n");
    }
  }
  ostream & operator<< (ostream &os) const {
    os <<"ManagedModel<"<<typeid(MODELER).name()<<">(num_await="<<num_await<<", num_refit="<<num_refit
       <<", min_num_test="<<min_num_test<<", errlimit="<<errlimit<<", varlimit="<<varlimit<<", params=";
    if (params) { 
      os << *params;
    } else {
      os <<"none";
    }
    os << ")";
    return os;
  }
};

template <class MODELER>
inline ostream & operator<< (ostream &os, const ManagedModel<MODELER> &p) {
  return p.operator<<(os);
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
  ManagedModeler() {}
  ManagedModeler(const ManagedModeler &rhs) {}
  ~ManagedModeler() {}
  ManagedModeler & operator= (const ManagedModeler &rhs) {
    this->~ManagedModeler();
    return *(new(this)ManagedModeler<MODELER>(rhs));
  }
  static ManagedModel<MODELER> *Fit(const ParameterSet &ps,
				    const int num_await,
				    const int num_refit,
				    const int min_num_test,
				    const double errlimit,
				    const double varlimit) {
    assert(num_refit>=num_await);
    return new ManagedModel<MODELER>(ps,num_await,num_refit,min_num_test,
				     errlimit, varlimit);
  }
  static Model *Fit(double *sequence, int len, const ParameterSet &ps) {
    assert(ps.GetType()==ManagedPDQ);
    const ManagedPDQParameterSet &aps = (const ManagedPDQParameterSet &)ps;
    PDQParameterSet p(aps);
    int await; aps.GetAwait(await);
    int refit; aps.GetRefit(refit);
    int min_num_test; aps.GetMinTest(min_num_test);
    double errlimit; aps.GetErrorLimit(errlimit);
    double varlimit; aps.GetVarLimit(varlimit);
    return Fit(p,await,refit,min_num_test,errlimit,varlimit);
  }
  void Dump(FILE *out=stdout) const {
    fprintf(out,"ManagedModeler<%s>\n",typeid(MODELER).name());
  }
  ostream & operator<<(ostream &os) const {
    os <<"ManagedModeler<"<<typeid(MODELER).name()<<">()";
    return os;
  }
};

template <class MODELER>
inline ostream & operator<< (ostream &os, const ManagedModeler<MODELER> &p) {
  return p.operator<<(os);
};


#endif
