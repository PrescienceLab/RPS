#ifndef _refit
#define _refit

#include "abstract.h"

template <class MODELER> 
class RefittingPredictor : public Predictor {
 private:
  ParameterSet *ps;
  double *seq;
  int     seqlen;
  int     cur;
  int     refitint;
  Model  *curmodel;
  Predictor *curpred;
 protected:
  void FitNow() {
    // cur points to the *oldest* data item - the one that
    // will be replaced bext
    int numoldest = seqlen - (cur%seqlen);
    int i;
    CHK_DEL(curmodel);
    CHK_DEL(curpred);
    double *linseq=new double[seqlen];
    memcpy(linseq,&(seq[cur%seqlen]),numoldest*sizeof(double));
    memcpy(&(linseq[numoldest]),seq,(seqlen-numoldest)*sizeof(double));
    curmodel = MODELER::Fit(linseq,seqlen,*ps);
    curpred = curmodel->MakePredictor();
    for (i=0;i<seqlen;i++) { 
      curpred->Step(linseq[i]);
    }
    CHK_DEL_MAT(linseq);
  }
 public:
  RefittingPredictor(double *initseq, int seqlen, const ParameterSet &ps, int refitint) {
    this->seqlen=seqlen;
    seq = new double [seqlen];
    memcpy(seq,initseq,sizeof(double)*seqlen);
    this->ps = ps.Clone();
    this->refitint = refitint;
    cur=seqlen;
    curmodel=0;
    curpred=0;
    FitNow();
  }
  ~RefittingPredictor() { 
    CHK_DEL_MAT(seq);
    seqlen=cur=refitint=0;
    CHK_DEL(curmodel);
    CHK_DEL(curpred);
  }
  int Begin() {
    return 0;
  }
  int StepsToPrime() { 
    return curpred->StepsToPrime();
  }
  double Step(double obs) {
    if (!((cur-seqlen)%refitint)) {
      FitNow();
    } 
    double res = curpred->Step(obs);
    seq[cur%seqlen]=obs;
    cur++;
    return res;
  }
  
  int Predict(int maxahead, double *predictions) {
    return curpred->Predict(maxahead,predictions);
  }
  
  int ComputeVariances(int maxahead, double *vars, 
		       enum VarianceType vtype=POINT_VARIANCES) {
    return curpred->ComputeVariances(maxahead,vars,vtype);
  }

  void Dump(FILE *out=stdout) {
    fprintf(out, "RefittingPredictor cur=%d seqlen=%d refitint=%d curmodel and curpredictor follow\n",
	    cur,seqlen,refitint);
    curmodel->Dump(out);
    curpred->Dump(out);
  }

};


template <class MODELER>
class RefittingModel : public Model {
 private:
  double        *seq;
  int           seqlen;
  ParameterSet  *params; 
  int            refitinterval;
 public:
  RefittingModel(double *seq, int seqlen, const ParameterSet &ps, int refitinterval) {
    this->seqlen = seqlen;
    this->seq = new double [seqlen];
    memcpy(this->seq,seq,seqlen*sizeof(double));
    params = ps.Clone();
    this->refitinterval=refitinterval;
  }
  ~RefittingModel() { 
    CHK_DEL_MAT(seq);
    CHK_DEL(params);
  }
  Predictor * MakePredictor() { 
    return new RefittingPredictor<MODELER>(seq,seqlen,*params,refitinterval);
  }
  void Dump(FILE *out=stdout) {
    fprintf(out,"RefittingModel, seqlen=%d refitinterval=%d\n",
	    seqlen, refitinterval);
  }
};


template <class MODELER>
class RefittingModeler : public Modeler {
 public:
  static Model *Fit(double *sequence, int len, const ParameterSet &ps) {
    assert(0);
  }
  static RefittingModel<MODELER> *Fit(double *seq, 
				      int seqlen, 
				      const ParameterSet &ps,
				      int refitinterval) {
    return new RefittingModel<MODELER>(seq,seqlen,ps,refitinterval);
  }
};


#endif
