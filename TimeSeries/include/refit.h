#ifndef _refit
#define _refit

#include <assert.h>
#include <new>
#include <typeinfo>
#include "abstract.h"
#include "pdqparamsets.h"
#include "rps_log.h"

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
  RefittingPredictor() : ps(0), seq(0), seqlen(0), cur(0), refitint(0), curmodel(0), curpred(0) 
  {}
  RefittingPredictor(const RefittingPredictor<MODELER> &rhs) {
    ps=rhs.ps->Clone();
    seqlen=rhs.seqlen;
    seq = new double [seqlen];
    memcpy(seq,rhs.seq,sizeof(double)*seqlen);
    cur=rhs.cur;
    refitint=rhs.refitint;
    curmodel=rhs.curmodel->Clone();
    curpred=rhs.curpred->Clone();
  }
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
  RefittingPredictor<MODELER> & operator=(const RefittingPredictor<MODELER> &rhs) {
    this->~RefittingPredictor();
    return *(new(this)RefittingPredictor<MODELER>(rhs));
  }

  int Begin() {
    return 0;
  }
  int StepsToPrime() const { 
    assert(curpred!=0);
    return curpred->StepsToPrime();
  }
  double Step(const double obs) {
    if (!((cur-seqlen)%refitint)) {
      RPSLog(CONTEXT,10,"Refitting Model at step %d\n",cur);
      FitNow();
    } 
    double res = curpred->Step(obs);
    seq[cur%seqlen]=obs;
    cur++;
    return res;
  }
  
  int Predict(const int maxahead, double *predictions) const {
    return curpred->Predict(maxahead,predictions);
  }
  
  int ComputeVariances(const int maxahead, double *vars, 
		       const enum VarianceType vtype=POINT_VARIANCES) const {
    return curpred->ComputeVariances(maxahead,vars,vtype);
  }

  void Dump(FILE *out=stdout) const {
    fprintf(out, "RefittingPredictor<%s> cur=%d seqlen=%d refitint=%d parameterset, curmodel, curpredictor, and sequence follow\n",
	    typeid(MODELER).name(),cur,seqlen,refitint);
    if (ps) {
      ps->Dump(out);
    } else {
      fprintf(out,"No Parameter Set\n");
    }
    if (curmodel) {
      curmodel->Dump(out);
    } else {
      fprintf(out,"No Current Model\n");
    }
    if (curpred) {
      curpred->Dump(out);
    } else {
      fprintf(out,"No Current Predictor\n");
    }
    for (int i=0;i<seqlen;i++) {
      fprintf(out,"%f\n",seq[0]);
    }
  }

  ostream & operator<<(ostream &os) const {
    os << "RefittingPredictor<"<<typeid(MODELER).name()<<">(refitint="<<refitint<<", ps=";
    if (ps) {
      os << *ps;
    } else {
      os <<"None";
    }
    os <<", curmodel=";
    if (curmodel) {
      os << *curmodel;
    } else {
      os <<"None";
    }
    os <<", curpred=";
    if (curpred) {
      os << *curpred;
    } else {
      os <<"None";
    }
    os << ", cur="<<cur<<", seqlen="<<seqlen<<", seq=("; 
    for (int i=0;i<seqlen;i++) {
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
inline ostream & operator<< (ostream &os, const RefittingPredictor<MODELER> &p) {
  return p.operator<<(os);
};


template <class MODELER>
class RefittingModel : public Model {
 private:
  double        *seq;
  int           seqlen;
  ParameterSet  *params; 
  int            refitinterval;
 public:
  RefittingModel() : seq(0), seqlen(0), params(0), refitinterval(0) 
  {}
  RefittingModel(const RefittingModel &rhs) {
    seqlen=rhs.seqlen;
    seq=new double [seqlen];
    memcpy(seq,rhs.seq,sizeof(double)*seqlen);
    params=rhs.params->Clone();
    refitinterval=rhs.refitinterval;
  }
  RefittingModel(const double *seq, const int seqlen, const ParameterSet &ps, const int refitinterval) {
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
  RefittingModel<MODELER> & operator=(const RefittingModel<MODELER> &rhs) {
    this->~RefittingModel();
    return *(new(this)RefittingModel<MODELER>(rhs));
  }
  
  Predictor * MakePredictor() const { 
    return new RefittingPredictor<MODELER>(seq,seqlen,*params,refitinterval);
  }
  void Dump(FILE *out=stdout) const {
    fprintf(out,"RefittingModel<%s>, seqlen=%d refitinterval=%d Parameter Set and Sequence Follow\n",
	    typeid(MODELER).name(),seqlen, refitinterval);
    if (params) { 
      params->Dump(out);
    } else {
      fprintf(out,"No Parameter Set\n");
    }
    for (int i=0;i<seqlen;i++) {
      fprintf(out,"%f\n",seq[i]);
    }
  }
  ostream & operator<<(ostream &os) const { 
    os << "RefittingModel<"<<typeid(MODELER).name()<<">(refitinterval="<<refitinterval<<", params=";
    if (params) {
      os << *params;
    } else {
      os <<"None";
    }
    os <<", seqlen="<<seqlen<<", seq=(";
    for (int i=0;i<seqlen;i++) {
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
inline ostream & operator<< (ostream &os, const RefittingModel<MODELER> &p) {
  return p.operator<<(os);
};


template <class MODELER>
class RefittingModeler : public Modeler {
 public:
  RefittingModeler() {};
  RefittingModeler(const RefittingModeler<MODELER> &rhs) {}
  ~RefittingModeler();
  RefittingModeler<MODELER> & operator= (const RefittingModeler<MODELER> &rhs) {
    this->~RefittingModeler();
    return *(new(this)RefittingModeler<MODELER>(rhs));
  }
  static RefittingModel<MODELER> *Fit(const double *seq, 
				      const int seqlen, 
				      const ParameterSet &ps,
				      const int refitinterval) {
    return new RefittingModel<MODELER>(seq,seqlen,ps,refitinterval);
  }
  static Model *Fit(const double *sequence, const int len, const ParameterSet &ps) {
    assert(ps.GetType()==RefittingPDQ);
    const RefittingPDQParameterSet &aps = (const RefittingPDQParameterSet &)ps;
    PDQParameterSet p(aps);
    int refit; aps.GetRefit(refit);
    return Fit(sequence,len,p,refit);
  }
  void Dump(FILE *out=stdout) const {
    fprintf(out,"RefittingModeler<%s>\n",typeid(MODELER).name());
  }
  ostream & operator<<(ostream &os) const {
    os <<"RefittingModeler<"<<typeid(MODELER).name()<<">()";
    return os;
  }
};

template <class MODELER>
inline ostream & operator<< (ostream &os, const RefittingModeler<MODELER> &p) {
  return p.operator<<(os);
};


#endif
