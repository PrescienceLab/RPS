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
  cerr << "testSFWT [wavelet-type]   [numstages]   [type]    [infile]\n";
  cerr << "         --------------   -----------   ------    --------\n";
  cerr << "         D2 (Haar)= 0     # stages in   approx=0  file formatted\n";
  cerr << "         D4       = 1       decomp      detail=1   as sample per\n";
  cerr << "         D6       = 2       ( > 0 )     transf=2   line\n";
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

  int operation = atoi(argv[3]);
  if (operation > 2 || operation < 0) {
    cerr << "Type must be less than 2 and greater than 0.\n";
    usage();
    exit(-1);
  }


  ifstream infile(argv[4]);
  if (!infile) {
    cerr << "Cannot open input file.\n";
    exit(-1);
  }

  typedef WaveletInputSample<double> wisd;
  typedef WaveletOutputSample<double> wosd;

  WaveletType wt = (WaveletType) type;

  // Instantiate a static forward wavelet transform
  cerr << "StaticForwardWaveletTransform instantiation" << endl;
  StaticForwardWaveletTransform
    <double, wosd, wisd>
    sfwt(numstages,wt,2,2,0);

  vector<wosd> outsamples;

  // Read the data from file into an input vector
  vector<wisd> samples;
  double sample;
  while (infile >> sample) {
    wisd wavesample;
    wavesample.SetSampleValue(sample);
    samples.push_back(wavesample);
  }
  infile.close();

  unsigned numlevels=numstages+1;
  unsigned i;

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

  switch(operation) {
  case 0: {
    for (i=0; i<samples.size(); i++) {
      sfwt.StreamingApproxSampleOperation(outsamples, samples[i]);

      // Print the new samples
      cerr << "Output for input sample " << i << ":" << endl;
      for (unsigned j=0; j<outsamples.size(); j++) {
	cerr << outsamples[j];

	int samplelevel = outsamples[j].GetSampleLevel();
	levels[samplelevel]->push_front(outsamples[j]);
      }

      outsamples.clear();
    }
  }
  break;

  case 1: {
    for (i=0; i<samples.size(); i++) {
      sfwt.StreamingDetailSampleOperation(outsamples, samples[i]);

      // Print the new samples
      cerr << "Output for input sample " << i << ":" << endl;
      for (unsigned j=0; j<outsamples.size(); j++) {
	cerr << outsamples[j];

	int samplelevel = outsamples[j].GetSampleLevel();
	levels[samplelevel]->push_front(outsamples[j]);
      }

      outsamples.clear();
    }
  }
  break;

  case 2: {
    for (i=0; i<samples.size(); i++) {
      sfwt.StreamingTransformSampleOperation(outsamples, samples[i]);

      // Print the new samples
      cerr << "Output for input sample " << i << ":" << endl;
      for (unsigned j=0; j<outsamples.size(); j++) {
	cerr << outsamples[j];

	int samplelevel = outsamples[j].GetSampleLevel();
	levels[samplelevel]->push_front(outsamples[j]);
      }

      outsamples.clear();
    }
  }
  break;

  default:
    break;
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
  cout << endl;

  unsigned loopsize = levels[0]->size();
  for (i=0; i<loopsize; i++) {
    cout << i << "\t";

    for (unsigned j=0; j<numlevels; j++) {
      if (!levels[j]->empty()) {
	wosd wos;
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
