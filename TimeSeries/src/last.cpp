#include "last.h"

LastModel::LastModel(const BestMeanModel &right)
{
  *((BestMeanModel*)this) = right;
}

void LastModel::Dump(FILE *out)
{
  fprintf(out,"LastModel:");
  BestMeanModel::Dump(out);
}

Predictor *LastModel::MakePredictor()
{
  return BestMeanModel::MakePredictor();
}

void LastPredictor::Dump(FILE *out)
{
  fprintf(out,"LastPredictor:");
  BestMeanPredictor::Dump(out);
}


LastModel *LastModeler::Fit(double *seq, int len)
{
  BestMeanModel *bm =BestMeanModeler::Fit(seq,len,1);
  LastModel *lm = new LastModel(*bm);
  delete bm;
  return lm;
}

Model *LastModeler::Fit(double *seq, int len, const ParameterSet &ps)
{
  return Fit(seq,len);
}
