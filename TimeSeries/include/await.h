#ifndef _await
#define _await

#include <new>
#include <typeinfo>
#include "abstract.h"
#include "pdqparamsets.h"

template <class MODELER> 
class AwaitingPredictor : public Predictor {
 private:
  ParameterSet *ps;
  double lastobs;
  double *seq;
  int     seqlen;
  int     cur;
  Model  *model;
  Predictor *pred;
 protected:
  void FitNow() {
    model = MODELER::Fit(seq,seqlen,*ps);
    pred = model->MakePredictor();
    for (int i=0;i<seqlen;i++) { 
      pred->Step(seq[i]);
    }
  }
 public:
  AwaitingPredictor() : ps(0), lastobs(0), seq(0), seqlen(0), cur(0), model(0), pred(0) {}
  AwaitingPredictor(const AwaitingPredictor &rhs) {
    ps=rhs.ps->Clone();
    lastobs=rhs.lastobs;
    seqlen=rhs.seqlen;
    seq=new double[seqlen];
    memcpy(seq,rhs.seq,sizeof(seq[0])*seqlen);
    cur=rhs.cur;
    model=rhs.model.Clone();
    pred=rhs.pred.Clone();
  }
  AwaitingPredictor(const ParameterSet &ps, const int awaitint) {
    this->seqlen=awaitint;
    seq = new double [seqlen];
    this->ps = ps.Clone();
    cur=0;
    model=0;
    pred=0;
    lastobs=0;
    if (cur==seqlen) {
      FitNow();
    }
  }
  ~AwaitingPredictor() { 
    CHK_DEL_MAT(seq);
    seqlen=cur=0;
    CHK_DEL(model);
    CHK_DEL(pred);
  }
  AwaitingPredictor &operator=(const AwaitingPredictor &rhs) {
    return *(new(this)AwaitingPredictor(rhs));
  }
  int Begin() {
    return 0;
  }
  int StepsToPrime() const { 
    return 0;
  }
  double Step(const double obs) {
    lastobs=obs;
    if (cur<seqlen) { 
      seq[cur]=obs;
      cur++;
      if (cur==seqlen) { 
	RPSLog(CONTEXT,10,"Fitting model given that we now have %d points\n",cur);
	FitNow();
      } 
      return obs;
    } else {
      return pred->Step(obs);
    }
  }
  
  int Predict(const int maxahead, double *predictions) const {
    if (pred) { 
      return pred->Predict(maxahead,predictions);
    } else {
      for (int i=0;i<maxahead;i++) {
	predictions[i]=lastobs;
      }
      return 0;
    }
  }
  
  int ComputeVariances(const int maxahead, double *vars, 
		       const VarianceType vtype=POINT_VARIANCES) const {
    if (pred) { 
      return pred->ComputeVariances(maxahead,vars,vtype);
    } else {
      int top= (vtype==CO_VARIANCES ? maxahead*maxahead : maxahead);
      for (int i=0;i<top;i++) { 
	vars[i]=0;
      }
      return 0;
    }
  }

  void Dump(FILE *out=stdout) const {
    fprintf(out, "AwaitingPredictor<%s> cur=%d seqlen=%d lastobs=%f parameterset, model, predictor and sequence follow\n",
	    typeid(MODELER).name(),cur,seqlen,lastobs);
    if (ps) {
      ps->Dump(out);
    } else {
      fprintf(out,"No Parameter Set\n");
    }
    if (model) {
      model->Dump(out);
    } else {
      fprintf(out,"No Current Model\n");
    }
    if (pred) {
      pred->Dump(out);
    } else {
      fprintf(out,"No Current Predictor\n");
    }
    for (int i=0;i<seqlen;i++) {
      fprintf(out,"%f\n",seq[i]);
    }
  }
  ostream & operator<<(ostream &os) const {
    os << "AwaitingPredictor<"<<typeid(MODELER).name()<<">(cur="<<cur<<", seqlen="<<seqlen<<", lastobs="<<lastobs<<", ps=";
    if (ps) {
      os << *ps;
    } else {
      os<<"none";
    }
    os << ", model=";
    if (model) {
      os << *model;
    } else {
      os <<"none";
    }
    os <<", predictor=";
    if (pred) {  
      os << *pred;
    } else {
      os <<"none";
    }
    os << ", seq=("; 
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
inline ostream & operator<< (ostream &os, const AwaitingPredictor<MODELER> &p) {
  return p.operator<<(os);
};


template <class MODELER>
class AwaitingModel : public Model {
 private:
  ParameterSet  *params; 
  int            awaitinterval;
 public:
  AwaitingModel() : params(0), awaitinterval(0) {}
  AwaitingModel(const AwaitingModel &rhs) {
    params=rhs.params->Clone();
    awaitinterval=rhs.awaitinterval;
  }
  AwaitingModel(const ParameterSet &ps, int awaitinterval) {
    params = ps.Clone();
    this->awaitinterval=awaitinterval;
  }
  ~AwaitingModel() { 
    CHK_DEL(params);
  }
  AwaitingModel & operator=(const AwaitingModel &rhs) {
    return *(new(this)AwaitingModel(rhs));
  }
  Predictor * MakePredictor() const { 
    return new AwaitingPredictor<MODELER>(*params,awaitinterval);
  }
  void Dump(FILE *out=stdout) const {
    fprintf(out,"AwaitingModel<%s>, awaitinterval=%d, parameterset follows\n", typeid(MODELER).name(),awaitinterval);
    if (params) {
      params->Dump(out);
    } else {
      fprintf(out,"No Parameter Set\n");
    }
  }
  ostream & operator<<(ostream &os) const {
    os <<"AwaitingModel<"<<typeid(MODELER).name()<<">(awaitinterval="<<awaitinterval<<", params=";
    if (params) {
      os << *params;
    } else {
      os <<"none";
    }
    os <<")";
    return os;
  }
};

template <class MODELER>
inline ostream & operator<< (ostream &os, const AwaitingModel<MODELER> &p) {
  return p.operator<<(os);
};


template <class MODELER>
class AwaitingModeler : public Modeler {
 public:
  AwaitingModeler() {}
  AwaitingModeler(const AwaitingModeler<MODELER> &rhs) {}
  ~AwaitingModeler() {}
  AwaitingModeler &operator=(const AwaitingModeler<MODELER> &rhs) { 
    this->~AwaitingModeler();
    return *(new(this)AwaitingModeler<MODELER>(rhs));}
  static AwaitingModel<MODELER> *Fit(const ParameterSet &ps,
				     const int awaitinterval) {
    return new AwaitingModel<MODELER>(ps,awaitinterval);
  }
  static Model *Fit(const double *sequence, const int len, const ParameterSet &ps) {
    assert(ps.GetType()==AwaitingPDQ);
    const AwaitingPDQParameterSet &aps = (const AwaitingPDQParameterSet &)ps;
    PDQParameterSet p(aps);
    int await; aps.GetAwait(await);
    return Fit(p,await);
  }
  void Dump(FILE *out=stdout) const {
    fprintf(out,"AwaitingModeler<%s>\n",typeid(MODELER).name());
  }
  ostream & operator<<(ostream &os) const {
    os <<"AwaitingModeler<"<<typeid(MODELER).name()<<">()";
    return os;
  }
};

template <class MODELER>
inline ostream & operator<< (ostream &os, const AwaitingModeler<MODELER> &p) {
  return p.operator<<(os);
};


#endif
