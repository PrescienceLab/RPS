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

  cerr << " block_static_mixed_srwt [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [specification-file] [output-file] [flat]\n\n";
  cerr << "--------------------------------------------------------------\n";
  cerr << "\n";
  cerr << "[input-file]         = The name of the file containing wavelet\n";
  cerr << "                       coefficients.  Can NOT be stdin because\n";
  cerr << "                       a particular file format is expected.\n";
  cerr << "\n";
  cerr << "[wavelet-type-init]  = The type of wavelet.  The choices are\n";
  cerr << "                       {DAUB2 (Haar), DAUB4, DAUB6, DAUB8,\n";
  cerr << "                       DAUB10, DAUB12, DAUB14, DAUB16, DAUB18,\n";
  cerr << "                       DAUB20}.  The 'DAUB' stands for\n";
  cerr << "                       Daubechies wavelet types and the order\n";
  cerr << "                       is the number of coefficients.\n";
  cerr << "\n";
  cerr << "[numstages-init]     = The number of stages to use in the\n";
  cerr << "                       reconstruction.  The number of levels\n";
  cerr << "                       is equal to the number of stages + 1.\n";
  cerr << "\n";
  cerr << "[specification-file] = Mixed signal specification.\n";
  cerr << "\n";
  cerr << "[output-file]        = Which file to write the output.  This may\n";
  cerr << "                       also be stdout or stderr.\n";
  cerr << "\n";
  cerr << "[flat]               = Whether the output is flat or human\n";
  cerr << "                       readable.  flat | noflat to choose.\n";
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
    cerr << "block_static_mixed_srwt: stdin is not allowed in this utility.\n";
    exit(-1);
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "block_static_mixed_srwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    cin = infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "block_static_mixed_srwt: Number of stages must be positive.\n";
    exit(-1);
  }

  ostream outstr;
  ofstream outfile;
  if (!strcasecmp(argv[4],"stdout")) {
    outstr.tie(&cout);
  } else if (!strcasecmp(argv[4],"stderr")) {
    outstr.tie(&cerr);
  } else {
    outfile.open(argv[4]);
    if (!outfile) {
      cerr << "block_static_mixed_srwt: Cannot open output file " << argv[4] << ".\n";
      exit(-1);
    }
    outstr.tie(&outfile);
  }

  ifstream specfile;
  specfile.open(argv[5]);
  if (!specfile) {
    cerr << "block_static_mixed_srwt: Cannot open specification file " << argv[5] << ".\n";
    exit(-1);
  }

  bool flat=true;
  if (toupper(argv[6][0])=='N') {
    flat = false;
  } else if (toupper(argv[6][0])!='F') {
    cerr << "sample_static_srwt: Need to choose flat or noflat for human readable.\n";
    exit(-1);
  }

  SignalSpec sigspec;
  ParseSignalSpec(sigspec, specfile);
  specfile.close();

  vector<WaveletOutputSampleBlock<wosd> > approxcoefs;
  vector<WaveletOutputSampleBlock<wosd> > detailcoefs;
  for (unsigned i=0; i<(unsigned)numstages; i++) {
    approxcoefs.push_back( WaveletOutputSampleBlock<wosd>(i) );
    detailcoefs.push_back( WaveletOutputSampleBlock<wosd>(i) );
  }

  // Read in the MRA coefficients
  FlatParser fp;
  fp.ParseMRACoefsBlock(approxcoefs, detailcoefs, cin);

  // Parameterize and instantiate the delay block
  unsigned wtcoefnum = numberOfCoefs[wt];
  int *delay = new int[numstages];
  CalculateMRADelayBlock(wtcoefnum, numstages, delay);
  DelayBlock<wosd> approx_dlyblk(numstages, 0, delay);
  DelayBlock<wosd> detail_dlyblk(numstages, 0, delay);

  // Instantiate a static reverse wavelet transform
  StaticReverseWaveletTransform<double, wisd, wosd> srwt(numstages,wt,2,2,0);

  // Create output buffers
  vector<WaveletOutputSampleBlock<wosd> > approx_dlyoutput;
  vector<WaveletOutputSampleBlock<wosd> > detail_dlyoutput;
  WaveletInputSampleBlock<wisd> reconst;

  // The operations
  approx_dlyblk.StreamingBlockOperation(approx_dlyoutput, approxcoefs);
  detail_dlyblk.StreamingBlockOperation(detail_dlyoutput, detailcoefs);
  srwt.StreamingMixedBlockOperation(reconst,
				    approx_dlyoutput,
				    detail_dlyoutput,
				    sigspec);

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
