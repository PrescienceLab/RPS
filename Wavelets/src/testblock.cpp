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
  cerr << "testblock [wavelet-type]   [numstages]   [infile]\n";
  cerr << "           --------------   -----------   --------\n";
  cerr << "           D2 (Haar)= 0     # stages in   file formatted\n";
  cerr << "           D4       = 1       decomp       as sample per\n";
  cerr << "           D6       = 2       ( > 0 )      line\n";
  cerr << "           D8       = 3\n";
  cerr << "           D10      = 4\n";
  cerr << "           D12      = 5\n";
  cerr << "           D14      = 6\n";
  cerr << "           D16      = 7\n";
  cerr << "           D18      = 8\n";
  cerr << "           D20      = 9\n";
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

  // Read the data from file into an input vector
  deque<WaveletInputSample> samples;
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

  // Since we are working in block transforms, create an input block of samples
  WaveletInputSampleBlock inputblock(samples);

  // Instantiate a static forward wavelet transform
  cout << "StaticForwardWaveletTransform instantiation" << endl;
  StaticForwardWaveletTransform<double, WaveletOutputSample, WaveletInputSample>
    sfwt(numstages,wt,2,2,0);

  // Parameterize the delay block
  unsigned wtcoefnum = numcoefs[type];
  cout << "The number of levels: " << numstages+1 << endl;
  int *delay = new int[numstages+1];
  CalculateWaveletDelayBlock(wtcoefnum, numstages+1, delay);

  // Print the delay components
  cout << "Delay values: " << endl;
  for (int j=0; j<numstages+1; j++) {
    cout << "\tLevel " << j << ":\t" << delay[j] << endl;
  }

  // Instantiate a delay block
  DelayBlock<double, WaveletOutputSample, WaveletOutputSample>
    dlyblk(numstages+1, delay);

  // Instantiate a static reverse wavelet transform
  cout << "StaticReverseWaveletTransform instantiation" << endl;
  StaticReverseWaveletTransform<double, WaveletInputSample, WaveletOutputSample>
    srwt(numstages,wt,2,2);

  vector<SampleBlock<WaveletOutputSample> *> forwardoutput;
  vector<SampleBlock<WaveletOutputSample> *> delayoutput;
  WaveletInputSampleBlock            reverseoutput;
  for (int j=0; j<numstages+1; j++) {
    WaveletOutputSampleBlock *wosb1 = new WaveletOutputSampleBlock();
    WaveletOutputSampleBlock *wosb2 = new WaveletOutputSampleBlock();
    forwardoutput.push_back(wosb1);
    delayoutput.push_back(wosb2);
  }

  sfwt.StreamingBlockOperation(forwardoutput, inputblock);
  dlyblk.StreamingBlockOperation(delayoutput, forwardoutput);
  srwt.StreamingBlockOperation(reverseoutput, delayoutput);

  // Print the outputs

  if (delay != 0) {
    delete[] delay;
    delay=0;
  }

  return 0;
}