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

  cerr << " block_static_streaming_test [input-file] [wavelet-type-init]\n";
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

  istream *is = &cin;
  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "block_static_streaming_test: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    is = &infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "block_static_streaming_test: Number of stages must be positive.\n";
    exit(-1);
  }

  if (toupper(argv[4][0])!='T') {
    cerr << "block_static_streaming_test: For streaming tests, only TRANSFORM type allowed.\n";
    exit(-1);
  }

  ostream *outstr = &cout;
  ofstream outfile;
  if (!strcasecmp(argv[5],"stdout")) {
  } else if (!strcasecmp(argv[5],"stderr")) {
    outstr = &cerr;
  } else {
    outfile.open(argv[5]);
    if (!outfile) {
      cerr << "block_static_streaming_test: Cannot open output file " << argv[5] << ".\n";
      exit(-1);
    }
    outstr = &outfile;
  }

  deque<wisd> samples;
  FlatParser fp;
  fp.ParseTimeDomain(samples, *is);
  infile.close();

  WaveletInputSampleBlock<wisd> inputblock(samples);

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
  vector<WaveletOutputSampleBlock<wosd> > forwardoutput;
  vector<WaveletOutputSampleBlock<wosd> > delayoutput;
  WaveletInputSampleBlock<wisd> reconst;

  // The operations
  sfwt.StreamingTransformBlockOperation(forwardoutput, inputblock);
  dlyblk.StreamingBlockOperation(delayoutput, forwardoutput);
  srwt.StreamingTransformBlockOperation(reconst, delayoutput);
  unsigned i;

  for (i=0; i<MIN(inputblock.GetBlockSize(), reconst.GetBlockSize()); i++) {
    *outstr << i << "\t" << inputblock[i].GetSampleValue() << "\t"
	    << reconst[i].GetSampleValue() << endl;
  }
  *outstr << endl;

  // Calculate the error between input and output
  double error=0;
  unsigned sampledelay = CalculateStreamingRealTimeDelay(wtcoefnum,numstages)-1;
  i=0;
  for (unsigned j=sampledelay; j<MIN(reconst.GetBlockSize(), inputblock.GetBlockSize()); i++, j++) {
    error += inputblock[i].GetSampleValue() - reconst[j].GetSampleValue();
  }
  
  *outstr << "Mean error: " << error/(double)i << endl;

  if (delay != 0) {
    delete[] delay;
    delay=0;
  }

  return 0;
}
