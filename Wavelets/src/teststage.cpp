#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#include "waveletsample.h"
#include "waveletsampleblock.h"
#include "stage.h"

void usage()
{
  cerr << "teststage [wavelet-type]  [infile]\n";
  cerr << "          --------------  --------\n";
  cerr << "          D2 (Haar)= 0    file formatted\n";
  cerr << "          D4       = 1     as sample per\n";
  cerr << "          D6       = 2     line\n";
  cerr << "          D8       = 3\n";
  cerr << "          D10      = 4\n";
  cerr << "          D12      = 5\n";
  cerr << "          D14      = 6\n";
  cerr << "          D16      = 7\n";
  cerr << "          D18      = 8\n";
  cerr << "          D20      = 9\n";
}

void print() {
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

  cout << "WaveletType: " << type << endl;

  ifstream infile(argv[2]);
  if (!infile) {
    cerr << "Cannot open input file.\n";
    exit(-1);
  }

  WaveletType wt = (WaveletType) type;
  StageType   st = FORWARD;

  // Instantiate a forward stage
  cout << "ForwardWaveletStage instantiation" << endl;
  ForwardWaveletStage<WaveletOutputSample, WaveletInputSample> 
    fwd_stage(wt,st,2,2,0,1);

  WaveletOutputSampleBlock output_l, output_h;

  // Read the data from file into an input vector
  vector<WaveletInputSample> samples;
  double sample;
  while (infile >> sample) {
    WaveletInputSample wavesample;
    wavesample.SetSampleValue(sample);
    samples.push_back(wavesample);
  }
  infile.close();

  WaveletInputSampleBlock  input(samples);

  fwd_stage.PerformBlockOperation(output_l, output_h, input);

  cout << "The low output of the stage" << endl;
  cout << output_l << endl;
  cout << "The high output of the stage" << endl;
  cout << output_h << endl << endl;

  cout << "----------";
  cout << "----------";
  cout << "----------";
  cout << "----------";
  cout << "----------";
  cout << "----------";
  cout << "----------";
  cout << "----------";
  cout << "----------";
  cout << "----------" << endl << endl;

  st = REVERSE;

  // Instantiate a reverse stage
  cout << "ReverseWaveletStage instantiation" << endl;
  ReverseWaveletStage<WaveletInputSample, WaveletOutputSample> 
    rev_stage(wt,st,2,2);

  WaveletInputSampleBlock reverseout;

  rev_stage.PerformBlockOperation(reverseout, output_l, output_h);

  cout << "The output of the reverse stage" << endl;
  cout << reverseout << endl;

  return 0;
}
