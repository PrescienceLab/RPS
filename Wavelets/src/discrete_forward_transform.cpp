#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "banner.h"
#include "waveletsample.h"
#include "waveletsampleblock.h"
#include "transforms.h"
#include "cmdlinefuncs.h"
#include "flatparser.h"

void usage()
{
  char *tb=GetTsunamiBanner();
  char *b=GetRPSBanner();

  cerr << " discrete_forward_transform [input-file] [wavelet-type-init]\n";
  cerr << "  [transform-type] [flat] [output-file]\n\n";
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
  cerr << "[transform-type]    = The transform type may be of type\n";
  cerr << "                      APPROX | DETAIL | TRANSFORM.\n";
  cerr << "\n";
  cerr << "[flat]              = Whether the output is flat or human\n";
  cerr << "                      readable.  flat | noflat to choose.\n";
  cerr << "\n";
  cerr << "[output-file]       = Which file to write the output.  This may\n";
  cerr << "                      also be stdout or stderr.\n\n";
  cerr << "\n";
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
      cerr << "block_static_sfwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    cin = infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  TransformType tt;
  if (toupper(argv[3][0])=='A') {
    tt = APPROX;
  } else if (toupper(argv[3][0])=='D') {
    tt = DETAIL;
  } else if (toupper(argv[3][0])=='T') {
    tt = TRANSFORM;
  } else {
    cerr << "block_static_sfwt: Invalid transform type.  Choose APPROX | DETAIL | TRANSFORM.\n";
    usage();
    exit(-1);
  }

  bool flat=true;
  if (toupper(argv[4][0])=='N') {
    flat = false;
  } else if (toupper(argv[4][0])!='F') {
    cerr << "block_static_sfwt: Need to choose flat or noflat for human readable.\n";
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
      cerr << "block_static_sfwt: Cannot open output file " << argv[5] << ".\n";
      exit(-1);
    }
    outstr.tie(&outfile);
  }

  deque<wisd> samples;
  FlatParser fp;
  fp.ParseTimeDomain(samples, cin);
  infile.close();

  WaveletInputSampleBlock<wisd> inputblock(samples);

  // Instantiate a static forward wavelet transform
  DiscreteForwardWaveletTransform<double, wosd, wisd> dfwt(wt,0);

  // Create result buffers
  DiscretWaveletOutputSampleBlock<wosd> forwardoutput;

  switch(tt) {
  case APPROX: {
    dfwt.StreamingApproxBlockOperation(forwardoutput, inputblock);
    numlevels -= 1;
    break;
  }
  case DETAIL: {
    dfwt.StreamingDetailBlockOperation(forwardoutput, inputblock);
    numlevels -= 1;
    break;
  }
  case TRANSFORM: {
    dfwt.StreamingTransformBlockOperation(forwardoutput, inputblock);
    break;
  }
  default:
    break;
  }

#if 0
  // Human readable output
  if (!flat) {
    OutputLevelMetaData(outstr, forwardoutput, numlevels);
  }

  OutputWaveletCoefs(outstr, forwardoutput, tt);
#endif

  return 0;
}
