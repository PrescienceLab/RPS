#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#include "waveletsample.h"
#include "waveletsampleblock.h"
#include "transforms.h"

void usage()
{
  cerr << "testSFWT [wavelet-type]   [numstages]   [infile]\n";
  cerr << "         --------------   -----------   --------\n";
  cerr << "         D2 (Haar)= 0     # stages in   file formatted\n";
  cerr << "         D4       = 1       decomp       as sample per\n";
  cerr << "         D6       = 2       ( > 0 )      line\n";
  cerr << "         D8       = 3\n";
  cerr << "         D10      = 4\n";
  cerr << "         D12      = 5\n";
  cerr << "         D14      = 6\n";
  cerr << "         D16      = 7\n";
  cerr << "         D18      = 8\n";
  cerr << "         D20      = 9\n";
}

void print() {
}

int main(int argc, char *argv[])
{
  if (argc!=4) {
    usage();
    exit(-1);
  }

  int type = atoi(argv[1]);
  if ((type < 0) || (type >= NUM_WAVELET_TYPES)) {
    usage();
    exit(-1);
  }

  cout << "WaveletType: " << type << endl;

  int numstages = atoi(argv[2]);
  if (numstages <= 0) {
    cerr << "Number of stages must be positive.\n";
    usage();
    exit(-1);
  }

  ifstream infile(argv[3]);
  if (!infile) {
    cerr << "Cannot open input file.\n";
    exit(-1);
  }

  WaveletType wt = (WaveletType) type;

  // Instantiate a static forward wavelet transform
  cout << "StaticForwardWaveletTransform instantiation" << endl;
  StaticForwardWaveletTransform<double, WaveletOutputSample, WaveletInputSample>
    sfwt(numstages,wt,2,2,0);

  vector<WaveletOutputSample> outsamples;

  // Read the data from file into an input vector
  vector<WaveletInputSample> samples;
  double sample;
  while (infile >> sample) {
    WaveletInputSample wavesample;
    wavesample.SetSampleValue(sample);
    samples.push_back(wavesample);
  }
  infile.close();

  unsigned i;
  cout << "The Samples of the input file: " << endl;
  for (i=0; i<samples.size(); i++) {
    cout << "\t" << samples[i];
  }


  for (i=0; i<samples.size(); i++) {
    sfwt.StreamingSampleOperation(outsamples, samples[i]);
    
    // Print the new samples
    cout << "Output for input sample " << i << ":" << endl;
    for (unsigned j=0; j<outsamples.size(); j++) {
      cout << outsamples[j];
    }
    outsamples.clear();
  }
  return 0;
}
