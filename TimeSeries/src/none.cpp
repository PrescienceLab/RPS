#include "none.h"

NonePredictor::NonePredictor()
{}

NonePredictor::NonePredictor(const NonePredictor &rhs)
{}

NonePredictor::~NonePredictor()
{}

NonePredictor & NonePredictor::operator=(const NonePredictor &rhs)
{
  this->~NonePredictor();
  return *(new(this)NonePredictor(rhs));
}

int NonePredictor::Begin()
{
  return 0;
}

int NonePredictor::StepsToPrime() const
{
  return 0;
}

double NonePredictor::Step(const double obs) 
{
  return 0.0;
}

int NonePredictor::Predict(const int maxahead, double *predictions) const
{
  int i;
  for (i=0;i<maxahead;i++) { 
    predictions[i]=0.0;
  }
  return 0;
}

int NonePredictor::ComputeVariances(const int maxahead, double *vars, 
				    const VarianceType vtype)  const
{
  int i;
  int max = vtype==CO_VARIANCES ? maxahead*maxahead : maxahead;
  for (i=0;i<max;i++) { 
    vars[i]=0.0;
  }
  return 0;
}

void NonePredictor::Dump(FILE *out) const
{
  fprintf(out,"NonePredictor\n");
}

ostream & NonePredictor::operator<<(ostream &os) const
{
  os << "NonePredictor()";
  return os;
}

Predictor *NoneModel::MakePredictor() const
{
  return new NonePredictor;
}

NoneModel::NoneModel()
{}

NoneModel::NoneModel(const NoneModel &rhs)
{}

NoneModel::~NoneModel()
{}

NoneModel & NoneModel::operator=(const NoneModel &rhs)
{
  this->~NoneModel();
  return *(new(this)NoneModel(rhs));
}


void NoneModel::Dump(FILE *out) const
{
  fprintf(out,"NoneModel\n");
}

ostream & NoneModel::operator<<(ostream &os) const
{
  os << "NoneModel()";
  return os;
}

NoneModeler::NoneModeler()
{}

NoneModeler::NoneModeler(const NoneModeler &rhs)
{}

NoneModeler::~NoneModeler()
{}

NoneModeler & NoneModeler::operator=(const NoneModeler &rhs)
{
  this->~NoneModeler();
  return *(new(this)NoneModeler(rhs));
}

Model *NoneModeler::Fit()
{
  return new NoneModel;
}

Model *NoneModeler::Fit(const double *seq, const int len)
{
  return Fit();
}

Model *NoneModeler::Fit(const double *sequence, const int len, const ParameterSet &ps)
{
  return Fit();
}

void NoneModeler::Dump(FILE *out) const
{
  fprintf(out,"NoneModeler\n");
}

ostream & NoneModeler::operator<<(ostream &os) const
{
  return (os<<"NoneModeler()");
}
