#include <iostream>
#include "TimeSeries.h"
#include "banner.h"

void usage(const char *n)
{
  char *s=GetAvailableModels();
  char *b=GetRPSBanner();
  fprintf(stdout,
	  "Fit and predict using AWAIT and MANAGED modifiers\n\n"
	  "usage: %s model < input > output\n\n"
	  "model   = prediction model, as below\n"
	  "input   = sequence of numbers (one column)\n"
	  "output  = sequence of predicted value, error, sequence\n\n%s\n%s",n,s,b);
  delete [] s;
  delete [] b;
}


int main(int argc, char *argv[])
{
  double val, next[4], last;
  Model *model;
  Predictor *pred;

  if (argc!=2) {
    usage(argv[0]);
    exit(0);
  }

  ModelTemplate *mt=ParseModel(argc-1,&(argv[1]));
  
  if (!mt) {
    fprintf(stderr,"Can't parse model\n");
    exit(-1);
  }
  
  mt->Dump(stderr);
  
  model = FitThis((double*)0,0,*mt);
  
  if (!model) { 
    fprintf(stderr,"Can't fit model\n");
    exit(-1);
  }

  pred = model->MakePredictor();

  if (!pred) { 
    fprintf(stderr,"Can't make predictor\n");
    exit(-1);
  }

  model->Dump(stderr);
  pred->Dump(stderr);

  last=0;
  while (!(cin.eof())) {
    cin >> val;
    pred->Step(val);
    pred->Predict(4,next);
    //    cout << val << "\t"<< next[0]<<"\t"<<next[1]<<"\t"<<next[2]<<"\t"
    //	 <<next[3] << endl;
    cout << val << "\t" << last << "\t" << (last-val) <<endl;
    last=next[0];
  }

  model->Dump(stderr);
  pred->Dump(stderr);
}


  
