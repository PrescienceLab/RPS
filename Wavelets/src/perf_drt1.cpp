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

  cerr << " perf_drt [input-file] [wavelet-type-init]\n";
  cerr << "  [transform-type] [blocksize] [sleep-rate]\n";
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
  cerr << "[sleep-rate]        = This rate is the sleep rate.  The value\n";
  cerr << "                      is in microseconds and is long integer.\n";
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

void ParseDWTCoefsWithIgnore(WaveletOutputSampleBlock<wosd> &wavecoefs,
			     istream &in) 
{
  int levelnum;
  double sampvalue;
  map<int, unsigned, less<int> > indices;
  while(in >> levelnum) {
    in >> sampvalue;
    if (indices.find(levelnum) == indices.end()) {
      indices[levelnum] = 0;
    } else {
      indices[levelnum] += 1;
    }
    wosd sample(sampvalue, levelnum, indices[levelnum]);
    wavecoefs.PushSampleBack(sample);
  }
}

void GetNextBlock(DiscreteWaveletOutputSampleBlock<wosd> &block,
		  WaveletOutputSampleBlock<wosd> &wavecoefs,
		  const unsigned blknum,
		  const unsigned blocksize)
{
  block.ClearBlock();
  deque<wosd> dwosd;
  wavecoefs.GetSamples(dwosd, blknum*blocksize, blknum*blocksize+blocksize);
  block.SetSamples(dwosd);
  block.SetLowestLevel(0);
  block.SetNumberLevels(NumberOfLevels(block.GetBlockSize())+1);
  block.SetTransformType(TRANSFORM);
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
      cerr << "perf_drt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    is = &infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  TransformType tt=TRANSFORM;
  if (toupper(argv[3][0])!='T') {
    cerr << "perf_drt: Invalid transform type.  Must be type TRANSFORM.\n";
    exit(-1);
  }

  unsigned blocksize = atoi(argv[4]);
  if (blocksize == 0) {
    cerr << "perf_drt: Must be greater than 0.\n";
    exit(-1);
  }

  bool sleep=true;
  unsigned long sleeptime_us = atoi(argv[5]);
  if (sleeptime_us == 0) {
    sleep=false;
  }

  bool flat=true;
  if (toupper(argv[6][0])=='N') {
    flat = false;
  } else if (toupper(argv[6][0])!='F') {
    cerr << "perf_drt: Need to choose flat or noflat for human readable.\n";
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
      cerr << "perf_drt: Cannot open output file " << argv[7] << ".\n";
      exit(-1);
    }
    outstr = &outfile;
  }

  unsigned i;
  WaveletOutputSampleBlock<wosd> wavecoefs;
  ParseDWTCoefsWithIgnore(wavecoefs, *is);
  infile.close();

  DiscreteWaveletOutputSampleBlock<wosd> block;

  // Instantiate a reverse discrete wavelet transform
  ReverseDiscreteWaveletTransform<double, wisd, wosd> rdwt(wt);

  // Create output buffers
  WaveletInputSampleBlock<wisd> reconst;

  unsigned numblocks = wavecoefs.GetBlockSize() / blocksize;
  const unsigned MINBLOCKS = 512;

  // Find number of tests
  unsigned numtests=0;
  unsigned blocksize_save = blocksize;
  while (numblocks >= MINBLOCKS) {
    numtests++;
    blocksize *= 2;
    numblocks = wavecoefs.GetBlockSize() / blocksize;
  }

  blocksize = blocksize_save;
  numblocks = MINBLOCKS;

  // Sleep 50 seconds
  usleep(1000000*50);

  for (unsigned j=0; j<numtests; j++) {

    if (j == numtests-1) {
      sleep = false;
    }

    // The operations
    for (i=0; i<numblocks; i++) {
      if (sleep) {
	usleep(sleeptime_us);
      }
      GetNextBlock(block, wavecoefs, i, blocksize);
      rdwt.DiscreteWaveletTransformOperation(reconst, block);
      reconst.ClearBlock();
    }
    blocksize *= 2;
  }

  // Print the output with appropriate tag
  if (flat) {
    *outstr << sleeptime_us << " " << wt << " " << blocksize << " " << tt;
  } else {
    *outstr << "Sleeptime (us) = " << sleeptime_us << endl;
    *outstr << "Wavelet type = " << wt << endl;
    *outstr << "Block size= " << blocksize << endl;
    *outstr << "Transform type = " << tt << endl;
  }
  *outstr << endl;

  return 0;
}
