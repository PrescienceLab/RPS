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

  cerr << " block_static_srwt [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [transform-type] [flat] [output-file]\n\n";
  cerr << "--------------------------------------------------------------\n";
  cerr << "\n";
  cerr << "[input-file]        = The name of the file containing wavelet\n";
  cerr << "                      coefficients.  Can NOT be stdin because\n";
  cerr << "                      a particular file format is expected.\n";
  cerr << "\n";
  cerr << "[wavelet-type-init] = The type of wavelet.  The choices are\n";
  cerr << "                      {DAUB2 (Haar), DAUB4, DAUB6, DAUB8,\n";
  cerr << "                      DAUB10, DAUB12, DAUB14, DAUB16, DAUB18,\n";
  cerr << "                      DAUB20}.  The 'DAUB' stands for\n";
  cerr << "                      Daubechies wavelet types and the order\n";
  cerr << "                      is the number of coefficients.\n";
  cerr << "\n";
  cerr << "[numstages-init]    = The number of stages to use in the\n";
  cerr << "                      reconstruction.  The number of levels\n";
  cerr << "                      is equal to the number of stages + 1.\n";
  cerr << "\n";
  cerr << "[transform-type]    = The reconstruction type may be of type\n";
  cerr << "                      TRANSFORM.\n";
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
  if (argc!=7) {
    usage();
    exit(-1);
  }

  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
    cerr << "block_static_srwt: stdin is not allowed in this utility.\n";
    exit(-1);
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "block_static_srwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    cin = infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "block_static_srwt: Number of stages must be positive.\n";
    exit(-1);
  }
  unsigned numlevels=numstages+1;

  if (toupper(argv[4][0])!='T') {
    cerr << "block_static_srwt: Invalid transform type.  Must be type TRANSFORM.\n";
    exit(-1);
  }

  bool flat=true;
  if (toupper(argv[5][0])=='N') {
    flat = false;
  } else if (toupper(argv[5][0])!='F') {
    cerr << "sample_static_srwt: Need to choose flat or noflat for human readable.\n";
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
      cerr << "block_static_srwt: Cannot open output file " << argv[6] << ".\n";
      exit(-1);
    }
    outstr.tie(&outfile);
  }

  vector<WaveletOutputSampleBlock<wosd> > waveletcoefs;
  for (unsigned i=0; i<numlevels; i++) {
    waveletcoefs.push_back( WaveletOutputSampleBlock<wosd>(i) );
  }

  // Read in the wavelet coefficients
  FlatParser fp;
  fp.ParseWaveletCoefsBlock(waveletcoefs, cin);

  if (!flat) {
    OutputLevelMetaData(outstr, waveletcoefs, numlevels);
  }

  // Parameterize and instantiate the delay block
  unsigned wtcoefnum = numberOfCoefs[wt];
  int *delay = new int[numstages+1];
  CalculateWaveletDelayBlock(wtcoefnum, numstages+1, delay);
  DelayBlock<wosd> dlyblk(numstages+1, 0, delay);

  // Instantiate a static reverse wavelet transform
  StaticReverseWaveletTransform<double, wisd, wosd> srwt(numstages,wt,2,2,0);

  // Create output buffers
  vector<WaveletOutputSampleBlock<wosd> > delayoutput;
  WaveletInputSampleBlock<wisd> reconst;

  // The operations
  dlyblk.StreamingBlockOperation(delayoutput, waveletcoefs);
  srwt.StreamingTransformBlockOperation(reconst, delayoutput);

  if (!flat) {
    unsigned sampledelay = CalculateStreamingRealTimeDelay(wtcoefnum,numstages)-1;
    *outstr.tie() << "The real-time system delay is " << sampledelay << endl;
    *outstr.tie() << endl;
    *outstr.tie() << "Index\tValue\n" << endl;
    *outstr.tie() << "-----\t-----\n" << endl << endl;
  }

  for (unsigned i=0; i<reconst.GetBlockSize(); i++) {
    *outstr.tie() << i << "\t" << reconst[i].GetSampleValue() << endl;
  }
  *outstr.tie() << endl;

  if (delay != 0) {
    delete[] delay;
    delay=0;
  }

  return 0;
}
