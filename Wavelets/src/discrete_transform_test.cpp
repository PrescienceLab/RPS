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

  cerr << " discrete_transform_test [input-file] [wavelet-type-init]\n";
  cerr << "  [transform-type] [output-file]\n\n";
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
  if (argc!=5) {
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

  if (toupper(argv[3][0])!='T') {
    cerr << "block_static_streaming_test: For streaming tests, only TRANSFORM type allowed.\n";
    exit(-1);
  }

  ostream *outstr = &cout;
  ofstream outfile;
  if (!strcasecmp(argv[4],"stdout")) {
  } else if (!strcasecmp(argv[4],"stderr")) {
    outstr = &cerr;
  } else {
    outfile.open(argv[4]);
    if (!outfile) {
      cerr << "block_static_streaming_test: Cannot open output file " << argv[4] << ".\n";
      exit(-1);
    }
    outstr = &outfile;
  }

  unsigned i;

  deque<wisd> samples;
  FlatParser fp;
  fp.ParseTimeDomain(samples, *is);
  infile.close();

  WaveletInputSampleBlock<wisd> inputblock(samples);

  // Instantiate a forward discrete wavelet transform
  ForwardDiscreteWaveletTransform<double, wosd, wisd> fdwt(wt,0);

  // Instantiate a reverse discrete wavelet transform
  ReverseDiscreteWaveletTransform<double, wisd, wosd> rdwt(wt);

  // Create result buffers
  DiscreteWaveletOutputSampleBlock<wosd> forwardoutput;
  WaveletInputSampleBlock<wisd> reconst;

  // The operations
  fdwt.DiscreteWaveletTransformOperation(forwardoutput, inputblock);
  rdwt.DiscreteWaveletTransformOperation(reconst, forwardoutput);


  for (i=0; i<MIN(inputblock.GetBlockSize(), reconst.GetBlockSize()); i++) {
    *outstr << i << "\t" << inputblock[i].GetSampleValue() << "\t"
	    << reconst[i].GetSampleValue() << endl;
  }
  *outstr << endl;

  // Calculate the error between input and output
  double error=0;
  for (i=0; i<MIN(reconst.GetBlockSize(), inputblock.GetBlockSize()); i++) {
    error += inputblock[i].GetSampleValue() - reconst[i].GetSampleValue();
  }
  
  *outstr << "Mean error: " << error/(double)i << endl;

  return 0;
}
