#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <deque>

#include "waveletsample.h"
#include "waveletsampleblock.h"
#include "transforms.h"

void usage()
{
  cerr << "testSFWTmixed  [wavelet-type]   [numstages]   [infile]   [sigspec]\n";
  cerr << "               --------------   -----------   --------   ---------\n";
  cerr << "               D2 (Haar)= 0     # stages in   sample      approx &\n";
  cerr << "               D4       = 1       decomp       per        details\n";
  cerr << "               D6       = 2       ( > 0 )      line\n";
  cerr << "               D8       = 3\n";
  cerr << "               D10      = 4\n";
  cerr << "               D12      = 5\n";
  cerr << "               D14      = 6\n";
  cerr << "               D16      = 7\n";
  cerr << "               D18      = 8\n";
  cerr << "               D20      = 9\n";
}

void print() {
}

int main(int argc, char *argv[])
{
  if (argc!=5) {
    usage();
    exit(-1);
  }

  int type = atoi(argv[1]);
  if ((type < 0) || (type >= NUM_WAVELET_TYPES)) {
    usage();
    exit(-1);
  }

  cerr << "WaveletType: " << type << endl;

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

  ifstream sigspecfile(argv[4]);
  if (!sigspecfile) {
    cerr << "Cannot open input file.\n";
    exit(-1);
  }

  // Need to create a parser here, but for testing, the levels have been hard
  //  coded
  numstages = 7;
  const unsigned numDetailLevels = 4;
  int levels[numDetailLevels] = {1, 3, 4, 5};

  SignalSpec spec;
  spec.approximations.push_back(6);
  for (unsigned l=0; l<numDetailLevels; l++) {
    spec.details.push_back(levels[l]);
  }

  typedef WaveletInputSample<double> wisd;
  typedef WaveletOutputSample<double> wosd;

  WaveletType wt = (WaveletType) type;

  unsigned i;

  // Read the data from file into an input vector
  deque<wisd> samples;
  double sample;
  while (infile >> sample) {
    wisd wavesample;
    wavesample.SetSampleValue(sample);
    samples.push_back(wavesample);
  }
  infile.close();

  // Instantiate a static forward wavelet transform
  cerr << "StaticForwardWaveletTransform instantiation" << endl;
  StaticForwardWaveletTransform<double, wosd, wisd> sfwt(numstages,wt,2,2,0);

#if 0
  cerr << "The Samples of the input file: " << endl;
  for (i=0; i<samples.size(); i++) {
    cerr << "\t" << samples[i];
  }
#endif

  // Since we are working in block transforms, create an input block of samples
  WaveletInputSampleBlock<wisd> inputblock(samples);

  vector<WaveletOutputSampleBlock<wosd> > approxoutput;
  vector<WaveletOutputSampleBlock<wosd> > detailoutput;

  sfwt.StreamingMixedBlockOperation(approxoutput,
				    detailoutput,
				    inputblock,
				    spec);

  // Print the outputs
  cerr << "The number of approximation levels: " << approxoutput.size() << endl;
  cerr << "The number of detail levels: " << detailoutput.size() << endl;

  cerr << "The size of each of the levels:" << endl;
  cerr << "  Approximations" << endl;
  for (i=0; i<approxoutput.size(); i++) {
    cerr << "\tLevel " << approxoutput[i].GetBlockLevel() << " size = " 
	 << approxoutput[i].GetBlockSize() << endl;
  }
  cerr << endl;

  cerr << "  Details" << endl;
  for (i=0; i<detailoutput.size(); i++) {
    cerr << "\tLevel " << detailoutput[i].GetBlockLevel() << " size = " 
	 << detailoutput[i].GetBlockSize() << endl;
  }

  return 0;
}
