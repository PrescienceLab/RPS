#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "banner.h"
#include "waveletsample.h"
#include "waveletsampleblock.h"
#include "transforms.h"
#include "delay.h"
#include "cmdlinefuncs.h"
#include "flatparser.h"

void usage()
{
  char *tb=GetTsunamiBanner();
  char *b=GetRPSBanner();

  cerr << " sample_static_streaming_test [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [transform-type] [output-file]\n\n";
  cerr << "--------------------------------------------------------------\n";
  cerr << "\n";
  cerr << "[input-file]        = The name of the file containing time-\n";
  cerr << "                      domain samples.  Can also be stdin.\n";
  cerr << "\n";
  cerr << "[wavelet-type-init] = The type of wavelet.  The choices are\n";
  cerr << "                      {DAUB2 (Haar), DAUB4, DAUB6, DAUB8,\n";
  cerr << "                      DAUB10, DAUB12, DAUB14, DAUB16, DAUB18,\n";
  cerr << "                      DAUB20}.  The 'DAUB' stands for\n";
  cerr << "                      Daubechies wavelet types and the order\n";
  cerr << "                      is the number of coefficients.\n";
  cerr << "\n";
  cerr << "[numstages-init]    = The number of stages to use in the\n";
  cerr << "                      decomposition.  The number of levels is\n";
  cerr << "                      equal to the number of stages + 1.\n";
  cerr << "\n";
  cerr << "[transform-type]    = The transform type may only be of type\n";
  cerr << "                      TRANSFORM for this test.\n";
  cerr << "\n";
  cerr << "[output-file]       = Which file to write the output.  This may\n";
  cerr << "                      also be stdout or stderr.\n\n";
  cerr << tb << endl;
  cerr << b << endl;
  delete [] tb;
  delete [] b;
}

int main(int argc, char *argv[])
{
  if (argc!=6) {
    usage();
    exit(-1);
  }

  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "sample_static_streaming_test: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    cin = infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "sample_static_streaming_test: Number of stages must be positive.\n";
    exit(-1);
  }

  if (toupper(argv[4][0])!='T') {
    cerr << "sample_static_streaming_test: For streaming tests, only TRANSFORM type allowed.\n";
    exit(-1);
  }

  ostream outstr;
  ofstream outfile;
  if (!strcasecmp(argv[5],"stdout")) {
    outstr.tie(&cout);
  } else if (!strcasecmp(argv[5],"stderr")) {
    outstr.tie(&cerr);
  } else {
    outfile.open(argv[5]);
    if (!outfile) {
      cerr << "sample_static_streaming_test: Cannot open output file " << argv[5] << ".\n";
      exit(-1);
    }
    outstr.tie(&outfile);
  }

  // Read the data from file into an input vector
  vector<wisd> samples;
  FlatParser fp;
  fp.ParseTimeDomain(samples, cin);
  infile.close();

  // Instantiate a static forward wavelet transform
  StaticForwardWaveletTransform<double, wosd, wisd> sfwt(numstages,wt,2,2,0);

  // Parameterize and instantiate the delay block
  unsigned wtcoefnum = numberOfCoefs[wt];
  int *delay = new int[numstages+1];
  CalculateWaveletDelayBlock(wtcoefnum, numstages+1, delay);
  DelayBlock<wosd> dlyblk(numstages+1, 0, delay);

  // Instantiate a static forward wavelet transform
  StaticReverseWaveletTransform<double, wisd, wosd> srwt(numstages,wt,2,2,0);

  // Create result buffers
  vector<wosd> outsamples;
  vector<wosd> delaysamples;
  vector<wisd> finaloutput;
  vector<wisd> outsamp;

  for (unsigned i=0; i<samples.size(); i++) {
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

  for (unsigned i=0; i<MIN(finaloutput.size(), samples.size()); i++) {
    *outstr.tie() << i << "\t" << samples[i].GetSampleValue() << "\t"
		  << finaloutput[i].GetSampleValue() << endl;
  }
  *outstr.tie() << endl;

  // Calculate the error between input and output
  double error=0;
  unsigned sampledelay = CalculateStreamingRealTimeDelay(wtcoefnum,numstages)-1;
  unsigned i=0;
  for (unsigned j=sampledelay; j<MIN(finaloutput.size(), samples.size()); i++, j++) {
    error += samples[i].GetSampleValue() - finaloutput[j].GetSampleValue();
  }
  
  *outstr.tie() << "Mean error: " << error/(double)i << endl;

  if (delay != 0) {
    delete[] delay;
    delay=0;
  }

  return 0;
}
