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

#define DBG_PRINT(bprint, in, sampletime) \
  if ((bprint)) {                         \
    PrintLevelIndex((in), (sampletime));  \
  }                                       \

template <class SAMPLE>
void PrintLevelIndex(const vector<SAMPLE> &input, const unsigned sampletime) {
  cout << "Sampletime: " << sampletime << endl;
  cout << "  Levels: ";
  for (unsigned i=0; i<input.size(); i++) {
    cout << input[i].GetSampleLevel() << " ";
  }
  cout << endl << "  Indices: ";
  for (unsigned i=0; i<input.size(); i++) {
    cout << input[i].GetSampleIndex() << " ";
  }
  cout << endl;
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
  cerr << "The Samples of the input file: " << endl;
  for (i=0; i<samples.size(); i++) {
    cerr << "\t" << samples[i];
  }

  // Instantiate a static forward wavelet transform
  cerr << "StaticForwardWaveletTransform instantiation" << endl;
  StaticForwardWaveletTransform<double, wosd, wisd> sfwt;
  //  sfwt(numstages,wt,2,2,0);
  cerr << "-----------------------------------------------"<< endl;
  cerr << "DEFAULT SFWT CONSTRUCTION COMPLETE" << endl;
  cerr << "-----------------------------------------------"<< endl;
  sfwt=StaticForwardWaveletTransform<double, wosd, wisd>(numstages,wt,2,2,0);
  cerr << sfwt << endl;


  // Parameterize the delay block
  unsigned wtcoefnum = numcoefs[type];
  cerr << "The number of levels: " << numstages+1 << endl;
  int *delay = new int[numstages+1];
  CalculateWaveletDelayBlock(wtcoefnum, numstages+1, delay);

  // Print the delay components
  cerr << "Delay values: " << endl;
  for (int j=0; j<numstages+1; j++) {
    cerr << "\tLevel " << j << ":\t" << delay[j] << endl;
  }

  // Instantiate a delay block
  DelayBlock<wosd> dlyblk;
  //  dlyblk(numstages+1, 0, delay);
  cerr << "-----------------------------------------------"<< endl;
  cerr << "DEFAULT DLYBLK CONSTRUCTION COMPLETE" << endl;
  cerr << "-----------------------------------------------"<< endl;
  dlyblk = DelayBlock<wosd>(numstages+1,0,delay);
  cerr << dlyblk << endl;

  // Instantiate a static forward wavelet transform
  cerr << "StaticReverseWaveletTransform instantiation" << endl;
  StaticReverseWaveletTransform<double, wisd, wosd> srwt;
  //  srwt(numstages,wt,2,2,0);
  cerr << "-----------------------------------------------"<< endl;
  cerr << "DEFAULT SRWT CONSTRUCTION COMPLETE" << endl;
  cerr << "-----------------------------------------------"<< endl;
  srwt = StaticReverseWaveletTransform<double, wisd, wosd>(numstages,wt,2,2,0);
  cerr << srwt << endl;


  // Create result buffers
  vector<wosd> outsamples;
  vector<wosd> delaysamples;
  vector<wisd> finaloutput;
  vector<wisd> outsamp;

  for (i=0; i<samples.size(); i++) {
    sfwt.StreamingTransformSampleOperation(outsamples, samples[i]);
    DBG_PRINT(0, outsamples, i);

    dlyblk.StreamingSampleOperation(delaysamples, outsamples);
    DBG_PRINT(0, outsamples, i);

    cerr << "Sample time: " << i << endl;
    if (srwt.StreamingTransformSampleOperation(outsamp, delaysamples)) {
      DBG_PRINT(0, outsamples, i);

      for (unsigned j=0; j<outsamp.size(); j++) {
	finaloutput.push_back(outsamp[j]);
	cerr << "\t" << outsamp[j];
      }
    }

    outsamp.clear();
    outsamples.clear();
    delaysamples.clear();
  }

  cout << "The size of the output: " << finaloutput.size() << endl;
  cout << "The final output samples: " << endl;
  for (i=0; i<finaloutput.size(); i++) {
    cout << finaloutput[i].GetSampleValue() << endl;
  }

  if (delay != 0) {
    delete[] delay;
    delay=0;
  }

  return 0;
}
