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

  cerr << " scal_perf_drt [input-file] [wavelet-type-init]\n";
  cerr << "  [transform-type] [blocksize] [numtests]\n";
  cerr << "  [flat] [output-file]\n\n";
  cerr << "--------------------------------------------------------------\n";
  cerr << "\n";
  cerr << "[input-file]        = The name of the file containing time-\n";
  cerr << "                      domain samples.  (The DWT is run first\n";
  cerr << "                      in order to obtain the input blocks).\n";
  cerr << "                      Input file can also be stdin.\n";
  cerr << "\n";
  cerr << "[wavelet-type-init] = The type of wavelet.  The choices are\n";
  cerr << "                      {DAUB2 (Haar), DAUB4, DAUB6, DAUB8,\n";
  cerr << "                      DAUB10, DAUB12, DAUB14, DAUB16, DAUB18,\n";
  cerr << "                      DAUB20}.  The 'DAUB' stands for\n";
  cerr << "                      Daubechies wavelet types and the order\n";
  cerr << "                      is the number of coefficients.\n";
  cerr << "\n";
  cerr << "[transform-type]    = The reconstruction type may be of type\n";
  cerr << "                      TRANSFORM.\n";
  cerr << "\n";
  cerr << "[blocksize]         = The size of the blocks to be used in the\n";
  cerr << "                      analysis.\n";
  cerr << "\n";
  cerr << "[numtests]          = This is the number of tests to run.\n";
  cerr << "\n";
  cerr << "[flat]              = Whether the output is flat or human\n";
  cerr << "                      readable.  flat | noflat to choose.\n";
  cerr << "\n";
  cerr << "[output-file]       = Which file to write the output.  This may\n";
  cerr << "                      also be stdout or stderr.\n";
  cerr << "\n";
  cerr << tb << endl;
  cerr << b << endl;
  delete [] tb;
  delete [] b;
}

int main(int argc, char *argv[])
{
  if (argc!=8) {
    usage();
    exit(-1);
  }

  istream *is = &cin;
  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "scal_perf_drt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    is = &infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  TransformType tt=TRANSFORM;
  if (toupper(argv[3][0])!='T') {
    cerr << "scal_perf_drt: Invalid transform type.  Must be type TRANSFORM.\n";
    exit(-1);
  }

  unsigned blocksize = atoi(argv[4]);
  if (blocksize == 0) {
    cerr << "scal_perf_drt: Must be greater than 0.\n";
    exit(-1);
  }

  unsigned numtests = atoi(argv[5]);

  bool flat=true;
  if (toupper(argv[6][0])=='N') {
    flat = false;
  } else if (toupper(argv[6][0])!='F') {
    cerr << "scal_perf_drt: Need to choose flat or noflat for human readable.\n";
    exit(-1);
  }

  ostream *outstr = &cout;
  ofstream outfile;
  if (!strcasecmp(argv[7],"stdout")) {
  } else if (!strcasecmp(argv[7],"stderr")) {
    outstr = &cerr;
  } else {
    outfile.open(argv[7]);
    if (!outfile) {
      cerr << "scal_perf_drt: Cannot open output file " << argv[7] << ".\n";
      exit(-1);
    }
    outstr = &outfile;
  }

  unsigned i;
  FlatParser fp;
  DiscreteWaveletOutputSampleBlock<wosd> waveletcoefs;
  vector<DiscreteWaveletOutputSampleBlock<wosd> > blocks;
  while (fp.ParseWaveletCoefsBlock(waveletcoefs, *is, blocksize)) {
    blocks.push_back(waveletcoefs);
    waveletcoefs.ClearBlock();
  }
  infile.close();

  // Instantiate a reverse discrete wavelet transform
  ReverseDiscreteWaveletTransform<double, wisd, wosd> rdwt(wt);

  // Create output buffers
  WaveletInputSampleBlock<wisd> reconst, finaloutput;

  // The operations
  double usrbegin, sysbegin, usrend, sysend;
  for (unsigned test=0; test<numtests; test++) {

    GetRusage(sysbegin, usrbegin);
    for (i=0; i<blocks.size(); i++) {
      rdwt.DiscreteWaveletTransformOperation(reconst, blocks[i]);
      reconst.ClearBlock();
    }
    GetRusage(sysend, usrend);

    // Print the output with appropriate tag
    if (flat) {
      *outstr << wt << " " << blocksize << " " << tt << " "
	      << usrend - usrbegin << " "
	      << sysend - sysbegin;
    } else {
      *outstr << "Wavelet type = " << wt << endl;
      *outstr << "Block size= " << blocksize << endl;
      *outstr << "Transform type = " << tt << endl;
      *outstr << "User time = " << usrend - usrbegin << endl;
      *outstr << "System time = " << sysend - sysbegin << endl;
    }
    *outstr << endl;
  }

  return 0;
}
