#include <iostream>
#include "TimeSeries.h"

int main(int argc, char *argv[])
{
  double val, next[4];
  Model *model;
  Predictor *pred;
  ModelTemplate *mt=ParseModel(argc-1,&(argv[1]));
  
  mt->Dump(stderr);
  
  model = FitThis((double*)0,0,*mt);
  pred = model->MakePredictor();

  model->Dump(stderr);
  pred->Dump(stderr);

  while (!(cin.eof())) {
    cin >> val;
    pred->Step(val);
    pred->Predict(4,next);
    cout << val << "\t"<< next[0]<<"\t"<<next[1]<<"\t"<<next[2]<<"\t"
	 <<next[3] << endl;
  }

  model->Dump(stderr);
  pred->Dump(stderr);
}


  
