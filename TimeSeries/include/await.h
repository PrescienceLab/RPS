#ifndef _await
#define _await

#include <new>
#include "abstract.h"

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
      int top= CO_VARIANCES ? maxahead*maxahead : maxahead;
      for (int i=0;i<top;i++) { 
	vars[i]=0;
      }
      return 0;
    }
  }

  void Dump(FILE *out=stdout) const {
    fprintf(out, "AwaitingPredictor cur=%d seqlen=%d lastobs=%f curmodel and curpredictor follow\n",
	    cur,seqlen,lastobs);
    if (model) { 
      model->Dump(out);
    } else {
      fprintf(out,"No current model\n");
    } 
    if (pred) { 
      pred->Dump(out);  
    } else {
      fprintf(out,"No current predictor\n");
    }
  }
  ostream & operator<<(ostream &os) const {
    os << "AwaitingPredictor(cur="<<cur<<", seqlen="<<seqlen<<", lastobs="<<lastobs<<", model=";
    if (model) {
      os <<*model;
    } else {
      os <<"none";
    }
    os <<", predictor=";
    if (pred) {  
      os << *pred;
    } else {
      os <<"none";
    }
    os <<")";
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
    fprintf(out,"AwaitingModel, awaitinterval=%d, parameterset follows\n", awaitinterval);
    params->Dump(out);
  }
  ostream & operator<<(ostream &os) const {
    os <<"AwaitingModel(awaitinterval="<<awaitinterval<<", params="<<*params<<")";
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
  AwaitingModeler &operator=(const AwaitingModeler<MODELER> &rhs) { return *(new(this)AwaitingModeler<MODELER>(rhs));}
  static Model *Fit(const double *sequence, const int len, const ParameterSet &ps) {
    assert(0);
  }
  static AwaitingModel<MODELER> *Fit(const ParameterSet &ps,
				     const int awaitinterval) {
    return new AwaitingModel<MODELER>(ps,awaitinterval);
  }
  void Dump(FILE *out=stdout) const {
    fprintf(out,"AwaitingModeler\n");
  }
  ostream & operator<<(ostream &os) const {
    os <<"AwaitingModeler()";
    return os;
  }
};

template <class MODELER>
inline ostream & operator<< (ostream &os, const AwaitingModeler<MODELER> &p) {
  return p.operator<<(os);
};


#endif
