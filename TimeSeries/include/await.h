#ifndef _await
#define _await

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
  AwaitingPredictor(const ParameterSet &ps, int awaitint) {
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
  int Begin() {
    return 0;
  }
  int StepsToPrime() { 
    return 0;
  }
  double Step(double obs) {
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
  
  int Predict(int maxahead, double *predictions) {
    if (pred) { 
      return pred->Predict(maxahead,predictions);
    } else {
      for (int i=0;i<maxahead;i++) {
	predictions[i]=lastobs;
      }
      return 0;
    }
  }
  
  int ComputeVariances(int maxahead, double *vars, 
		       enum VarianceType vtype=POINT_VARIANCES) {
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

  void Dump(FILE *out=stdout) {
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
};


template <class MODELER>
class AwaitingModel : public Model {
 private:
  ParameterSet  *params; 
  int            awaitinterval;
 public:
  AwaitingModel(const ParameterSet &ps, int awaitinterval) {
    params = ps.Clone();
    this->awaitinterval=awaitinterval;
  }
  ~AwaitingModel() { 
    CHK_DEL(params);
  }
  Predictor * MakePredictor() { 
    return new AwaitingPredictor<MODELER>(*params,awaitinterval);
  }
  void Dump(FILE *out=stdout) {
    fprintf(out,"AwaitingModel, awaitinterval=%d\n", awaitinterval);
  }
};


template <class MODELER>
class AwaitingModeler : public Modeler {
 public:
  static Model *Fit(double *sequence, int len, const ParameterSet &ps) {
    assert(0);
  }
  static AwaitingModel<MODELER> *Fit(const ParameterSet &ps,
				     int awaitinterval) {
    return new AwaitingModel<MODELER>(ps,awaitinterval);
  }
};


#endif
