#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "coefficients.h"
#include "filter.h"

void usage()
{
  cerr << "testfilter [wavelet-type]  [infile]\n";
  cerr << "           --------------  --------\n";
  cerr << "           D2 (Haar)= 0    file formatted\n";
  cerr << "           D4       = 1     as sample per\n";
  cerr << "           D6       = 2     line\n";
  cerr << "           D8       = 3\n";
  cerr << "           D10      = 4\n";
  cerr << "           D12      = 5\n";
  cerr << "           D14      = 6\n";
  cerr << "           D16      = 7\n";
  cerr << "           D18      = 8\n";
  cerr << "           D20      = 9\n";
}

void print(const vector<double> &coefs) {
  for (unsigned i=0; i<coefs.size(); i++) {
    cout << "\t" << coefs[i] << endl;
  }
}

int main(int argc, char *argv[])
{
  if (argc!=3) {
    usage();
    exit(-1);
  }

  int type = atoi(argv[1]);
  if ((type < 0) || (type >= NUM_WAVELET_TYPES)) {
    usage();
    exit(-1);
  }

  ifstream infile(argv[2]);
  if (!infile) {
    cerr << "Cannot open input file.\n";
    exit(-1);
  }

  WaveletType wt = (WaveletType) type;

  // Grab the coefficients for the specified type
  vector<double> translpf;

  WaveletCoefficients wc;
  wc.Initialize(wt);
  cout << "Number of coefs: " << wc.GetNumCoefs() << endl;
  wc.GetInverseCoefsLPF(translpf);

  cout << "The coefficients:\n";
  cout << "  Forward LPF:\n"; print(translpf);
  cout << endl;
  cout << wc;


  // Read the data from file into an input vector
  vector<double> input;
  double sample;
  while (infile >> sample) {
    input.push_back(sample);
  }
  infile.close();


  // Create a filter
  FIRFilter lpfilter(wc.GetNumCoefs());
  lpfilter.SetFilterCoefs(translpf);
  
  vector<double> output;
  lpfilter.GetFilterBufferOutput(output,input);
  cout << " The contents of the filter:\n";
  cout << lpfilter;


  cout << "The output of the filtering operation:" << endl;
  print(output);

  return 0;
}
