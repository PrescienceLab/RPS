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
  cerr << "  [numstages-init] [specification-file] [flat] [output-file]\n\n";
  cerr << "--------------------------------------------------------------\n";
  cerr << "\n";
  cerr << "[input-file]         = The name of the file containing wavelet\n";
  cerr << "                       coefficients.  Can also be stdin.\n";
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
  cerr << "[flat]               = Whether the output is flat or human\n";
  cerr << "                       readable.  flat | noflat to choose.\n";
  cerr << "\n";
  cerr << "[output-file]        = Which file to write the output.  This may\n";
  cerr << "                       also be stdout or stderr.\n";
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

  istream *is = &cin;
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
    is = &infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "block_static_mixed_srwt: Number of stages must be positive.\n";
    exit(-1);
  }

  ifstream specfile;
  specfile.open(argv[4]);
  if (!specfile) {
    cerr << "block_static_mixed_srwt: Cannot open specification file " << argv[4] << ".\n";
    exit(-1);
  }

  bool flat=true;
  if (toupper(argv[5][0])=='N') {
    flat = false;
  } else if (toupper(argv[5][0])!='F') {
    cerr << "sample_static_srwt: Need to choose flat or noflat for human readable.\n";
    exit(-1);
  }

  ostream *outstr = &cout;
  ofstream outfile;
  if (!strcasecmp(argv[6],"stdout")) {
  } else if (!strcasecmp(argv[6],"stderr")) {
    outstr = &cerr;
  } else {
    outfile.open(argv[6]);
    if (!outfile) {
      cerr << "block_static_mixed_srwt: Cannot open output file " << argv[6] << ".\n";
      exit(-1);
    }
    outstr = &outfile;
  }

  SignalSpec sigspec;
  ParseSignalSpec(sigspec, specfile);
  specfile.close();

  // Optimize the operations
  SignalSpec optim_spec;
  unsigned optim_stages;
  bool transform=StructureOptimizer(optim_spec, optim_stages, numstages, 0, sigspec);

  // Parameterize and instantiate the delay block
  unsigned wtcoefnum = numberOfCoefs[wt];
  int *delay = new int[optim_stages+1];
  CalculateWaveletDelayBlock(wtcoefnum, optim_stages+1, delay);
  DelayBlock<wosd> dlyblk(optim_stages+1, 0, delay);

  // Instantiate a static reverse wavelet transform
  StaticReverseWaveletTransform<double, wisd, wosd> srwt(optim_stages,wt,2,2,0);

  // Create output buffers
  vector<WaveletOutputSampleBlock<wosd> > approxcoefs;
  vector<WaveletOutputSampleBlock<wosd> > detailcoefs;
  vector<WaveletOutputSampleBlock<wosd> > wavecoefs;
  for (unsigned i=0; i<optim_stages; i++) {
    approxcoefs.push_back( WaveletOutputSampleBlock<wosd>(i) );
    detailcoefs.push_back( WaveletOutputSampleBlock<wosd>(i) );
    wavecoefs.push_back( WaveletOutputSampleBlock<wosd>(i) );
  }
  wavecoefs.push_back( WaveletOutputSampleBlock<wosd>(optim_stages));

  vector<WaveletOutputSampleBlock<wosd> > dlysamples;
  WaveletInputSampleBlock<wisd> reconst;

  // Read in the MRA coefficients
  FlatParser fp;
  fp.ParseMRACoefsBlock(optim_spec, approxcoefs, detailcoefs, *is);

  // Transform the coefficients into wavecoefs and change level of approx
  for (unsigned i=0; i<optim_stages; i++) {
    if (approxcoefs[i].GetBlockSize() > 0) {
      approxcoefs[i].SetBlockLevel( approxcoefs[i].GetBlockLevel()+1);
      wavecoefs[approxcoefs[i].GetBlockLevel()] = approxcoefs[i];
    }

    if (detailcoefs[i].GetBlockSize() > 0) {
      wavecoefs[i] = detailcoefs[i];
    }
  }

  if (!flat) {
    OutputLevelMetaData(*outstr, wavecoefs, optim_stages+1);
  }

  // The operations
  dlyblk.StreamingBlockOperation(dlysamples, wavecoefs);

  if (transform) {
    srwt.StreamingTransformBlockOperation(reconst, dlysamples);
  } else {
    vector<int> zerospec;
    vector<int> specs;
    FlattenSignalSpec(specs, optim_spec);
    InvertSignalSpec(zerospec, specs, optim_stages+1, 0);
    srwt.StreamingTransformZeroFillBlockOperation(reconst, dlysamples, zerospec);
  }

  if (!flat) {
    unsigned sampledelay = CalculateStreamingRealTimeDelay(wtcoefnum,optim_stages)-1;
    *outstr << "The real-time system delay is " << sampledelay << endl;
    *outstr << endl;
    *outstr << "Index\tValue\n" << endl;
    *outstr << "-----\t-----\n" << endl << endl;
  }

  for (unsigned i=0; i<reconst.GetBlockSize(); i++) {
    *outstr << i << "\t" << reconst[i].GetSampleValue() << endl;
  }
  *outstr << endl;

  if (delay != 0) {
    delete[] delay;
    delay=0;
  }

  return 0;
}
