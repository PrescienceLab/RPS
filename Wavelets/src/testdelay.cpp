#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#include "waveletsample.h"
#include "waveletsampleblock.h"
#include "transforms.h"
#include "delay.h"

void usage()
{
  cerr << "testdelay [wavelet-type]   [numstages]   [infile]\n";
  cerr << "          --------------   -----------   --------\n";
  cerr << "          D2 (Haar)= 0     # stages in   file formatted\n";
  cerr << "          D4       = 1       decomp       as sample per\n";
  cerr << "          D6       = 2       ( > 0 )      line\n";
  cerr << "          D8       = 3\n";
  cerr << "          D10      = 4\n";
  cerr << "          D12      = 5\n";
  cerr << "          D14      = 6\n";
  cerr << "          D16      = 7\n";
  cerr << "          D18      = 8\n";
  cerr << "          D20      = 9\n";
}

const unsigned numcoefs[10] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20};

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

  // Create a delay block to delay the high frequency information
  unsigned wtcoefnum = numcoefs[type];
  int *delay = new int[numstages+1];
  CalculateWaveletDelayBlock(wtcoefnum, numstages+1, delay);

  // Print the delay components
  cout << "Delay values: " << endl;
  for (int j=0; j<numstages+1; j++) {
    cout << "\tLevel " << j << ":\t" << delay[j] << endl;
  }

  DelayBlock<WaveletOutputSample>
    dlyblk(numstages+1, 0, delay);

  // Create a buffer for the delay samples
  vector<WaveletOutputSample> delaysamples;

  for (i=0; i<samples.size(); i++) {
    sfwt.StreamingTransformSampleOperation(outsamples, samples[i]);

    unsigned j;
#if 0    
    // Print the new samples
    cout << "Output for input sample " << i << ":" << endl;
    for (j=0; j<outsamples.size(); j++) {
      cout << outsamples[j];
    }
#endif

    dlyblk.StreamingSampleOperation(delaysamples, outsamples);

    // Print the new delayed samples
    cout << "Delay output for input sample " << i << ":" << endl;
    for (j=0; j<delaysamples.size(); j++) {
      cout << delaysamples[j];
    }
    outsamples.clear();
    delaysamples.clear();
  }
  return 0;
}
