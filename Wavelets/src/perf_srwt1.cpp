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

  cerr << " perf_srwt [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [transform-type] [sample-or-block]\n";
  cerr << "  [blocksize] [numblocks] [sleep-rate] [flat] [output-file]\n\n";
  cerr << "----------------------------------------------------------------\n";
  cerr << "\n";
  cerr << "[input-file]        = The name of the file containing wavelet\n";
  cerr << "                      coefficients.  Can also be stdin.\n";
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
  cerr << "[transform-type]    = The reconstruction type may be of type\n";
  cerr << "                      TRANSFORM.\n";
  cerr << "\n";
  cerr << "[sample-or-block]   = This parameter may be SAMPLE | BLOCK.\n";
  cerr << "\n";
  cerr << "[blocksize]         = The size of the blocks to be used in the\n";
  cerr << "                      analysis.\n";
  cerr << "\n";
  cerr << "[numblocks]         = The number of blocks to be processed.\n";
  cerr << "\n";
  cerr << "[sleep-rate]        = This rate is the sleep rate.  The value\n";
  cerr << "                      is in microseconds and is long integer.\n";
  cerr << "\n";
  cerr << "[flat]              = Whether the output is flat or human\n";
  cerr << "                      readable.  flat | noflat to choose.\n";
  cerr << "\n";
  cerr << "[output-file]       = Which file to write the output.  This may\n";
  cerr << "                      also be stdout or stderr.\n\n";
  cerr << "\n";
  delete [] tb;
  delete [] b;
}

int main(int argc, char *argv[])
{
  if (argc!=11) {
    usage();
    exit(-1);
  }

  istream *is = &cin;
  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "perf_srwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    is = &infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "perf_srwt: Number of stages must be positive.\n";
    exit(-1);
  }

  TransformType tt=TRANSFORM;
  if (toupper(argv[4][0])!='T') {
    cerr << "perf_srwt: Invalid transform type.  Must be type TRANSFORM.\n";
    exit(-1);
  }

  bool sample;
  if (toupper(argv[5][0])=='S') {
    sample=true;
  } else if (toupper(argv[5][0])=='B') {
    sample=false;
  } else {
    cerr << "perf_sfwt: Operation type.  Choose SAMPLE | BLOCK.\n";
    exit(-1);
  }

  unsigned blocksize = atoi(argv[6]);
  if (blocksize == 0) {
    cerr << "perf_sfwt: Must be greater than 0.\n";
    exit(-1);
  }

  unsigned numblocks = atoi(argv[7]);
  if (numblocks == 0) {
    cerr << "perf_sfwt: Number of blocks must be greater than 0.\n";
    exit(-1);
  }

  bool sleep=true;
  unsigned long sleeptime_us = atoi(argv[8]);
  if (sleeptime_us == 0) {
    sleep=false;
  }

  bool flat=true;
  if (toupper(argv[9][0])=='N') {
    flat = false;
  } else if (toupper(argv[9][0])!='F') {
    cerr << "perf_srwt: Need to choose flat or noflat for human readable.\n";
    exit(-1);
  }

  ostream *outstr = &cout;
  ofstream outfile;
  if (!strcasecmp(argv[10],"stdout")) {
  } else if (!strcasecmp(argv[10],"stderr")) {
    outstr = &cerr;
  } else {
    outfile.open(argv[10]);
    if (!outfile) {
      cerr << "perf_srwt: Cannot open output file " << argv[10] << ".\n";
      exit(-1);
    }
    outstr = &outfile;
  }

  // Parameterize and instantiate the delay block
  unsigned wtcoefnum = numberOfCoefs[wt];
  int *delay = new int[numstages+1];
  CalculateWaveletDelayBlock(wtcoefnum, numstages+1, delay);
  DelayBlock<wosd> dlyblk(numstages+1, 0, delay);

  // Instantiate a static reverse wavelet transform
  StaticReverseWaveletTransform<double, wisd, wosd> srwt(numstages,wt,2,2,0);

  if (sample) {

    vector<wosd> samplecoefs;
    vector<vector<wosd> > waveletcoefs;
    vector<wosd> delaysamples;
    vector<wisd> currentoutput;

    FlatParser fp;
    while ( fp.ParseWaveletCoefsSample(samplecoefs, *is) ) {
      waveletcoefs.push_back(samplecoefs);
      samplecoefs.clear();
    }
    infile.close();

    // Find the number of tests
    //    unsigned numblocks = waveletcoefs.size() / blocksize;
    unsigned datasize = waveletcoefs.size();
    const unsigned MINBLOCKS = 32;

    // Find number of tests
    unsigned numtests=0;
    unsigned blocksize_save = blocksize;
    while (numblocks >= MINBLOCKS) {
      numtests++;
      blocksize *= 2;
      numblocks = datasize / blocksize;
    }

    //  datasize = datasize >> (numtests-1);
    blocksize = blocksize_save;
    numblocks = datasize / blocksize;

    for (unsigned j=0; j<numtests; j++) {

      if (j == numtests-1) {
	sleep = false;
      }

      for (unsigned i=0; i<numblocks; i++) {
	if (sleep) {
	  usleep(sleeptime_us);
	}
	for (unsigned k=0; k<blocksize; k++) {
	  dlyblk.StreamingSampleOperation(delaysamples, waveletcoefs[i*blocksize+k]);
	  srwt.StreamingTransformSampleOperation(currentoutput, delaysamples);
	}
      }
      blocksize *= 2;
      numblocks = datasize/blocksize;
    }
  } else { //Block mode

    vector<WaveletOutputSampleBlock<wosd> > waveletcoefs;
    for (unsigned i=0; i<(unsigned)numstages+1; i++) {
      waveletcoefs.push_back( WaveletOutputSampleBlock<wosd>(i) );
    }

    // Read in the wavelet coefficients
    FlatParser fp;
    fp.ParseWaveletCoefsBlock(waveletcoefs, *is);

    vector<WaveletOutputSampleBlock<wosd> > block;
    vector<vector<WaveletOutputSampleBlock<wosd> > > blocks;
    vector<WaveletOutputSampleBlock<wosd> > delayoutput;
    WaveletInputSampleBlock<wisd> reconst;

    for (unsigned i=0; i<numblocks; i++) {
      GetNextBlock(block, i, waveletcoefs, numstages, blocksize);
      blocks.push_back(block);
      block.clear();
    }
    waveletcoefs.clear();

    // Perform tests
    for (unsigned i=0; i<numblocks; i++) {
      if (sleep) {
	usleep(sleeptime_us);
      }
      dlyblk.StreamingBlockOperation(delayoutput, blocks[i]);
      srwt.StreamingTransformBlockOperation(reconst, delayoutput);

      delayoutput.clear();
      reconst.ClearBlock();
    }
  }

  // Print the output with appropriate tag
  if (flat) {
    *outstr << sleeptime_us << " " << wt << " " << numstages << " " << tt << " " << 1;
  } else {
    *outstr << "Sleeptime (us) = " << sleeptime_us << endl;
    *outstr << "Wavelet type = " << wt << endl;
    *outstr << "Number stages = " << numstages << endl;
    *outstr << "Transform type = " << tt << endl;
    *outstr << "Block size (1 = sample op) = 1" << endl;
  }
  *outstr << endl;

  if (delay != 0) {
    delete[] delay;
    delay=0;
  }

  return 0;
}
