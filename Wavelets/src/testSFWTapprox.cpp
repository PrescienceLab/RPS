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
  cerr << "testSFWTapprox [wavelet-type]   [numstages]   [infile]\n";
  cerr << "               --------------   -----------   --------\n";
  cerr << "               D2 (Haar)= 0     # stages in   file formatted\n";
  cerr << "               D4       = 1       decomp       as sample per\n";
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

  WaveletType wt = (WaveletType) type;

  // Instantiate a static forward wavelet transform
  cerr << "StaticForwardWaveletTransform instantiation" << endl;
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

  unsigned numlevels=numstages+1;
  unsigned i;

  // Create vectors for the level outputs
  vector<deque<WaveletOutputSample> *> levels;
  for (i=0; i<numlevels; i++) {
    deque<WaveletOutputSample>* pwos = new deque<WaveletOutputSample>();
    levels.push_back(pwos);
  }

#if 0
  cerr << "The Samples of the input file: " << endl;
  for (i=0; i<samples.size(); i++) {
    cerr << "\t" << samples[i];
  }
#endif

  for (i=0; i<samples.size(); i++) {
    // Copy the input signal to the level 0 approximation signal
    WaveletOutputSample tempsamp(samples[i].GetSampleValue(),0,i);
    levels[0]->push_front(tempsamp);

    sfwt.StreamingSampleOperation(outsamples, samples[i], APPROX);

    // Print the new samples
    cerr << "Output for input sample " << i << ":" << endl;
    for (unsigned j=0; j<outsamples.size(); j++) {
      cerr << outsamples[j];

      int samplelevel = outsamples[j].GetSampleLevel();
      levels[samplelevel]->push_front(outsamples[j]);
    }

    outsamples.clear();
  }

  cerr << "The size of each level:" << endl;
  for (i=0; i<numlevels; i++) {
    cerr << "\tLevel " << i << " size = " << levels[i]->size() << endl;
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
  cerr << endl;

  unsigned loopsize = levels[0]->size();
  for (i=0; i<loopsize; i++) {
    cout << i << "\t";

    for (unsigned j=0; j<numlevels; j++) {
      if (!levels[j]->empty()) {
	WaveletOutputSample wos;
	wos = levels[j]->back();
	cout << wos.GetSampleValue() << "\t";
	levels[j]->pop_back();
      }
    }
    cout << endl;
  }

  for (i=0; i<numlevels; i++) {
    CHK_DEL(levels[i]);
  }
  levels.clear();

  return 0;
}
