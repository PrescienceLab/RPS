#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "waveletsample.h"
#include "waveletsampleblock.h"
#include "transforms.h"
#include "delay.h"
#include "filterbankinfo.h"

void usage()
{
  cerr << "testfilterbank [wavelet-type]   [delay]   [numstages]   [infile]\n";
  cerr << "               --------------   -------   -----------   --------\n";
  cerr << "               NBS-1-8 = 10     2-band    # stages in   file formatted\n";
  cerr << "                                 delay     decomp        as sample per\n";
  cerr << "                                           ( > 0 )       line\n";
}

const unsigned numcoefs[1] = {8};

int main(int argc, char *argv[])
{
  if (argc!=5) {
    usage();
    exit(-1);
  }

  int type = atoi(argv[1]);
  if ((type != 10)) {
    usage();
    exit(-1);
  }
  cerr << "FilterBankType: " << type << endl;

  int twobanddelay = atoi(argv[2]);
  if (twobanddelay <= 0) {
    cerr << "The two band delay must be >= 0.\n";
    usage();
    exit(-1);
  }

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "Number of stages must be positive.\n";
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
  CalculateFilterBankDelayBlock(wtcoefnum, numstages+1, twobanddelay, delay);

  // Print the delay components
  cout << "Delay values: " << endl;
  for (int j=0; j<numstages+1; j++) {
    cout << "\tLevel " << j << ":\t" << delay[j] << endl;
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

    dlyblk.StreamingSampleOperation(delaysamples, outsamples);

    cerr << "Sample time: " << i << endl;
    if (srwt.StreamingTransformSampleOperation(outsamp, delaysamples)) {

      for (unsigned j=0; j<outsamp.size(); j++) {
	finaloutput.push_back(outsamp[j]*2);
	cerr << "\t" << outsamp[j];
      }
    }

    outsamp.clear();
    outsamples.clear();
    delaysamples.clear();
  }

  cout << "The size of the output: " << finaloutput.size() << endl;
  cout << "Index\tInput samples\tFinal output samples" << endl;
  cout << "-----\t-------------\t--------------------" << endl;
  for (i=0; i<finaloutput.size(); i++) {
    cout<<i<<"\t"<<samples[i].GetSampleValue()<<"\t"<<finaloutput[i].GetSampleValue();
    cout << endl;
  }

  if (delay != 0) {
    delete[] delay;
    delay=0;
  }

  return 0;
}
