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
  cerr << "testSFWTblock [wavelet-type]   [numstages]   [infile]\n";
  cerr << "              --------------   -----------   --------\n";
  cerr << "              D2 (Haar)= 0     # stages in   file formatted\n";
  cerr << "              D4       = 1       decomp       as sample per\n";
  cerr << "              D6       = 2       ( > 0 )      line\n";
  cerr << "              D8       = 3\n";
  cerr << "              D10      = 4\n";
  cerr << "              D12      = 5\n";
  cerr << "              D14      = 6\n";
  cerr << "              D16      = 7\n";
  cerr << "              D18      = 8\n";
  cerr << "              D20      = 9\n";
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

  typedef WaveletInputSample<double> wisd;
  typedef WaveletOutputSample<double> wosd;

  WaveletType wt = (WaveletType) type;

  unsigned i, numlevels=numstages+1;

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

  // Create vectors for the level outputs
  vector<deque<wosd> *> levels;
  for (i=0; i<numlevels; i++) {
    deque<wosd>* pwos = new deque<wosd>();
    levels.push_back(pwos);
  }

#if 0
  cerr << "The Samples of the input file: " << endl;
  for (i=0; i<samples.size(); i++) {
    cerr << "\t" << samples[i];
  }
#endif

  // Since we are working in block transforms, create an input block of samples
  WaveletInputSampleBlock<wisd> inputblock(samples);

  vector<WaveletOutputSampleBlock<wosd> > forwardoutput;

  sfwt.StreamingTransformBlockOperation(forwardoutput, inputblock);

  // Print the outputs
  cerr << "The size of each level:" << endl;
  for (i=0; i<numlevels; i++) {
    cerr << "\tLevel " << i << " size = " 
	 << forwardoutput[i].GetBlockSize() << endl;
  }
  cerr << endl;

  cerr << "Index     ";
  for (i=0; i<numlevels; i++) {
    cerr << "Level " << i << "        " ;
  }
  cerr << endl << "-----     ";
  for (i=0; i<numlevels; i++) {
    cerr << "-------        ";
  }
  cout << endl;

  unsigned loopsize = forwardoutput[0].GetBlockSize();
  for (i=0; i<loopsize; i++) {
    cout << i << "\t";

    for (unsigned j=0; j<numlevels; j++) {
      if (!forwardoutput[j].Empty()) {
	wosd wos;
	wos = forwardoutput[j].Front();
	cout << wos.GetSampleValue() << "\t";
	forwardoutput[j].PopSampleFront();
      }
    }
    cout << endl;
  }

  return 0;
}
