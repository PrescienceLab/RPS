#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "waveletsample.h"
#include "waveletsampleblock.h"
#include "transforms.h"
#include "delay.h"

void usage()
{
  cerr << "teststream [wavelet-type]   [numstages]   [infile]\n";
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

  typedef WaveletInputSample<double> wisd;
  typedef WaveletOutputSample<double> wosd;

  WaveletType wt = (WaveletType) type;

  // Read the data from file into an input vector
  vector<wisd> samples;
  double sample;
  unsigned index=0;
  while (infile >> sample) {
    wisd wavesample;
    wavesample.SetSampleValue(sample);
    wavesample.SetSampleIndex(index++);
    samples.push_back(wavesample);
  }
  infile.close();

  unsigned i;
  cout << "The Samples of the input file: " << endl;
  for (i=0; i<samples.size(); i++) {
    cout << "\t" << samples[i];
  }

  // Instantiate a static forward wavelet transform
  cout << "StaticForwardWaveletTransform instantiation" << endl;
  StaticForwardWaveletTransform<double, wosd, wisd>
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
  DelayBlock<wosd> dlyblk;
  dlyblk = DelayBlock<wosd>(numstages+1,0,delay);
  //    dlyblk(numstages+1, 0, delay);

  cout << dlyblk << endl;

  // Instantiate a static forward wavelet transform
  cout << "StaticReverseWaveletTransform instantiation" << endl;
  StaticReverseWaveletTransform<double, wisd, wosd> srwt;
  srwt = StaticReverseWaveletTransform<double, wisd, wosd>(numstages,wt,2,2,0);
  //    srwt(numstages,wt,2,2,0);
  cout << srwt << endl;


  // Create result buffers
  vector<wosd> outsamples;
  vector<wosd> delaysamples;
  vector<wisd> finaloutput;
  vector<wisd> outsamp;


  for (i=0; i<samples.size(); i++) {
    sfwt.StreamingTransformSampleOperation(outsamples, samples[i]);
    
    dlyblk.StreamingSampleOperation(delaysamples, outsamples);

    if (srwt.StreamingTransformSampleOperation(outsamp, delaysamples)) {
      for (unsigned j=0; j<outsamp.size(); j++) {
	finaloutput.push_back(outsamp[j]);
      }
    }

    outsamp.clear();
    outsamples.clear();
    delaysamples.clear();
  }

  cout << "The size of the output: " << finaloutput.size() << endl;
  cout << "The final output samples: " << endl;
  for (i=0; i<finaloutput.size(); i++) {
    cout << finaloutput[i];
  }

  if (delay != 0) {
    delete[] delay;
    delay=0;
  }

  return 0;
}
