#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits.h>

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

  cerr << " block_static_mixed_sfwt [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [specification-file] [flat] [output-file]\n\n";
  cerr << "--------------------------------------------------------------\n";
  cerr << "\n";
  cerr << "[input-file]         = The name of the file containing time-\n";
  cerr << "                       domain samples.  Can also be stdin.\n";
  cerr << "\n";
  cerr << "[wavelet-type-init]  = The type of wavelet.  The choices are\n";
  cerr << "                       {DAUB2 (Haar), DAUB4, DAUB6, DAUB8,\n";
  cerr << "                       DAUB10, DAUB12, DAUB14, DAUB16, DAUB18,\n";
  cerr << "                       DAUB20}.  The 'DAUB' stands for\n";
  cerr << "                       Daubechies wavelet types and the order\n";
  cerr << "                       is the number of coefficients.\n";
  cerr << "\n";
  cerr << "[numstages-init]     = The number of stages to use in the\n";
  cerr << "                       decomposition.  The number of levels is\n";
  cerr << "                       equal to the number of stages + 1.\n";
  cerr << "\n";
  cerr << "[specification-file] = Mixed signal specification.\n";
  cerr << "\n";
  cerr << "[flat]               = Whether the output is flat or human\n";
  cerr << "                       readable.  flat | noflat to choose.\n";
  cerr << "\n";
  cerr << "[output-file]        = Which file to write the output.  This may\n";
  cerr << "                       also be stdout or stderr.\n\n";
  cerr << "\n";
  cerr << tb << endl;
  cerr << b << endl;
  delete [] tb;
  delete [] b;
}

int main(int argc, char *argv[])
{
  if (argc!=7) {
    usage();
    exit(-1);
  }

  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "block_static_mixed_sfwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    cin = infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "block_static_mixed_sfwt: Number of stages must be positive.\n";
    exit(-1);
  }

  ifstream specfile;
  specfile.open(argv[4]);
  if (!specfile) {
    cerr << "sample_static_mixed_sfwt: Cannot open specification file " << argv[4] << ".\n";
    exit(-1);
  }

  bool flat=true;
  if (toupper(argv[5][0])=='N') {
    flat = false;
  } else if (toupper(argv[5][0])!='F') {
    cerr << "block_static_mixed_sfwt: Need to choose flat or noflat for human readable.\n";
    exit(-1);
  }

  ostream outstr;
  ofstream outfile;
  if (!strcasecmp(argv[6],"stdout")) {
    outstr.tie(&cout);
  } else if (!strcasecmp(argv[6],"stderr")) {
    outstr.tie(&cerr);
  } else {
    outfile.open(argv[6]);
    if (!outfile) {
      cerr << "block_static_mixed_sfwt: Cannot open output file " << argv[6] << ".\n";
      exit(-1);
    }
    outstr.tie(&outfile);
  }

  SignalSpec sigspec;
  ParseSignalSpec(sigspec, specfile);
  specfile.close();

  // Read the data from file into an input vector
  deque<wisd> samples;
  FlatParser fp;
  fp.ParseTimeDomain(samples, cin);
  infile.close();

  WaveletInputSampleBlock<wisd> inputblock(samples);

  // Instantiate a static forward wavelet transform
  StaticForwardWaveletTransform<double, wosd, wisd> sfwt(numstages,wt,2,2,0);

  // Create result buffers
  vector<WaveletOutputSampleBlock<wosd> > approxout;
  vector<WaveletOutputSampleBlock<wosd> > detailout;

  sfwt.StreamingMixedBlockOperation(approxout, detailout, inputblock, sigspec);

  // Approximations
  if (!flat) {
    *outstr.tie() << "APPROXIMATIONS" << endl;
    *outstr.tie() << "--------------" << endl;
    OutputLevelMetaData(outstr, approxout, numstages);

    *outstr.tie() << endl << "DETAILS" << endl;
    *outstr.tie() << "-------" << endl;
    OutputLevelMetaData(outstr, detailout, numstages);
  }

  OutputMRACoefs(outstr, approxout, detailout);
  
  return 0;
}
