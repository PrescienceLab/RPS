#include <stdlib.h>
#include <iostream>
#include <vector>

#include "coefficients.h"

void usage()
{
  cerr << "testcoefs [wavelet-type]\n";
  cerr << "          --------------\n";
  cerr << "          D2 (Haar)= 0\n";
  cerr << "          D4       = 1\n";
  cerr << "          D6       = 2\n";
  cerr << "          D8       = 3\n";
  cerr << "          D10      = 4\n";
  cerr << "          D12      = 5\n";
  cerr << "          D14      = 6\n";
  cerr << "          D16      = 7\n";
  cerr << "          D18      = 8\n";
  cerr << "          D20      = 9\n";
}

void print(const vector<double> &coefs) {
  for (unsigned i=0; i<coefs.size(); i++) {
    cout << "\t" << coefs[i] << endl;
  }
}

int main(int argc, char *argv[])
{
  if (argc!=2) {
    usage();
    exit(-1);
  }

  int temp = atoi(argv[1]);
  if ((temp < 0) || (temp >= NUM_WAVELET_TYPES)) {
    usage();
    exit(-1);
  }

  WaveletType wt = (WaveletType) temp;

  vector<double> translpf;
  vector<double> transhpf;
  vector<double> inverselpf;
  vector<double> inversehpf;


  // Test all member types and operations
  CQFWaveletCoefficients wc;
  wc.Initialize(wt);
  cout << "Number of coefs: " << wc.GetNumCoefs() << endl;
  wc.GetTransformCoefsLPF(translpf);
  wc.GetTransformCoefsHPF(transhpf);
  wc.GetInverseCoefsLPF(inverselpf);
  wc.GetInverseCoefsHPF(inversehpf);

  cout << "The coefficients:\n";
  cout << "  Forward LPF:\n"; print(translpf);
  cout << "  Forward HPF:\n"; print(transhpf);
  cout << "  Reverse LPF:\n"; print(inverselpf);
  cout << "  Reverse HPF:\n"; print(inversehpf);

  cout << endl;
  cout << wc;

  // Test Changing the type
  translpf.clear();
  transhpf.clear();
  inverselpf.clear();
  inversehpf.clear();

  wt = DAUB10;
  wc.ChangeType(wt);
  cout << endl;
  cout << "Number of coefs: " << wc.GetNumCoefs() << endl;

  wc.GetTransformCoefsLPF(translpf);
  wc.GetTransformCoefsHPF(transhpf);
  wc.GetInverseCoefsLPF(inverselpf);
  wc.GetInverseCoefsHPF(inversehpf);

  cout << "The coefficients:\n";
  cout << "  Forward LPF:\n"; print(translpf);
  cout << "  Forward HPF:\n"; print(transhpf);
  cout << "  Reverse LPF:\n"; print(inverselpf);
  cout << "  Reverse HPF:\n"; print(inversehpf);

  cout << endl;
  cout << wc;

  return 0;
}
