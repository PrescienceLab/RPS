#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <unistd.h>

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

  cerr << " perf_sfwt1 [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [transform-type] [sample-or-block]\n";
  cerr << "  [blocksize] [sleeprate] [flat] [output-file]\n\n";
  cerr << "----------------------------------------------------------------\n";
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
  cerr << "[transform-type]    = The transform type may be of type\n";
  cerr << "                      APPROX | DETAIL | TRANSFORM.\n";
  cerr << "\n";
  cerr << "[sample-or-block]   = This parameter may be SAMPLE | BLOCK.\n";
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
  cerr << "                      also be stdout or stderr.\n\n";
  cerr << "\n";
  cerr << tb << endl;
  cerr << b << endl;
  delete [] tb;
  delete [] b;
}

void GetNextBlock(WaveletInputSampleBlock<wisd> &block,
		  WaveletInputSampleBlock<wisd> &inputsamples,
		  const unsigned blknum,
		  const unsigned blocksize)
{
  block.ClearBlock();
  deque<wisd> dwisd;
  inputsamples.GetSamples(dwisd, blknum*blocksize, blknum*blocksize+blocksize);
  block.SetSamples(dwisd);
}

int main(int argc, char *argv[])
{
  if (argc!=10) {
    usage();
    exit(-1);
  }

  istream *is=&cin;
  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "perf_sfwt1: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    is = &infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "perf_sfwt1: Number of stages must be positive.\n";
    exit(-1);
  }

  TransformType tt;
  if (toupper(argv[4][0])=='A') {
    tt = APPROX;
  } else if (toupper(argv[4][0])=='D') {
    tt = DETAIL;
  } else if (toupper(argv[4][0])=='T') {
    tt = TRANSFORM;
  } else {
    cerr << "perf_sfwt1: Invalid transform type.  Choose APPROX | DETAIL | TRANSFORM.\n";
    exit(-1);
  }

  bool sample;
  if (toupper(argv[5][0])=='S') {
    sample=true;
  } else if (toupper(argv[5][0])=='B') {
    sample=false;
  } else {
    cerr << "perf_sfwt1: Operation type.  Choose SAMPLE | BLOCK.\n";
    exit(-1);
  }

  unsigned blocksize = atoi(argv[6]);
  if (blocksize == 0) {
    cerr << "perf_sfwt1: Must be greater than 0.\n";
    exit(-1);
  }

  bool sleep=true;
  unsigned long sleeptime_us = atoi(argv[7]);
  if (sleeptime_us == 0) {
    sleep=false;
  }

  bool flat=true;
  if (toupper(argv[8][0])=='N') {
    flat = false;
  } else if (toupper(argv[8][0])!='F') {
    cerr << "perf_sfwt1: Need to choose flat or noflat for human readable.\n";
    exit(-1);
  }

  ostream *outstr = &cout;
  ofstream outfile;
  if (!strcasecmp(argv[9],"stdout")) {
  } else if (!strcasecmp(argv[9],"stderr")) {
    outstr = &cerr;
  } else {
    outfile.open(argv[9]);
    if (!outfile) {
      cerr << "perf_sfwt1: Cannot open output file " << argv[9] << ".\n";
      exit(-1);
    }
    outstr = &outfile;
  }

  unsigned i;

  // Get the samples from stream
  deque<wisd> samples;
  FlatParser fp;
  fp.ParseTimeDomain(samples, *is);
  infile.close();

  // Instantiate a static forward wavelet transform
  StaticForwardWaveletTransform<double, wosd, wisd> sfwt(numstages,wt,2,2,0);

  // Create result buffers
  vector<wosd> outsamples;
  vector<WaveletOutputSampleBlock<wosd> > forwardoutput;


  if (sample) {

    WaveletInputSampleBlock<wisd> inputblock(samples);
    WaveletInputSampleBlock<wisd> block;

    const unsigned NUMTESTS = 8;
    const unsigned BLOCKS_IN_TEST = 1024;

    unsigned numblocks = samples.size() / blocksize;

    // Sleep 50 seconds
    usleep(1000000*50);

    timeval sproc, eproc;
    unsigned long proctime = 0;
    unsigned long sleepduration;
    for (unsigned j=0; j<NUMTESTS; j++) {

      if (j == NUMTESTS-1) {
      	sleep = false;
      }

      switch(tt) {
      case APPROX: {
	for (i=0; i<BLOCKS_IN_TEST; i++) {
	  if (sleep) {
	    sleepduration = (sleeptime_us > proctime) ?
	      sleeptime_us - proctime : 0;
	    usleep(sleepduration);
	  }
	  if (gettimeofday(&sproc, 0) < 0) {
	    cerr << "Can't obtain the current time.\n";
	    exit(-1);
	  }
	  GetNextBlock(block, inputblock, i % numblocks, blocksize);
	  for (unsigned k=0; k<blocksize; k++) {
	    sfwt.StreamingApproxSampleOperation(outsamples, block[k]);
	    outsamples.clear();
	  }
	  if (gettimeofday(&eproc, 0) < 0) {
	    cerr << "Can't obtain the current time.\n";
	    exit(-1);
	  }
	  proctime =
	    (eproc.tv_sec - sproc.tv_sec) * 100000 + (eproc.tv_usec - sproc.tv_usec);
	}
	break;
      }
      case DETAIL: {
	for (i=0; i<BLOCKS_IN_TEST; i++) {
	  if (sleep) {
	    sleepduration = (sleeptime_us > proctime) ?
	      sleeptime_us - proctime : 0;
	    usleep(sleepduration);
	  }
	  if (gettimeofday(&sproc, 0) < 0) {
	    cerr << "Can't obtain the current time.\n";
	    exit(-1);
	  }
	  GetNextBlock(block, inputblock, i % numblocks, blocksize);
	  for (unsigned k=0; k<blocksize; k++) {
	    sfwt.StreamingDetailSampleOperation(outsamples, block[k]);
	    outsamples.clear();
	  }
	  if (gettimeofday(&eproc, 0) < 0) {
	    cerr << "Can't obtain the current time.\n";
	    exit(-1);
	  }
	  proctime =
	    (eproc.tv_sec - sproc.tv_sec) * 100000 + (eproc.tv_usec - sproc.tv_usec);
	}
	break;
      }
      case TRANSFORM: {
	for (i=0; i<BLOCKS_IN_TEST; i++) {
	  if (sleep) {
	    sleepduration = (sleeptime_us > proctime) ?
	      sleeptime_us - proctime : 0;
	    usleep(sleepduration);
	  }
	  if (gettimeofday(&sproc, 0) < 0) {
	    cerr << "Can't obtain the current time.\n";
	    exit(-1);
	  }
	  GetNextBlock(block, inputblock, i % numblocks, blocksize);
	  for (unsigned k=0; k<blocksize; k++) {
	    sfwt.StreamingTransformSampleOperation(outsamples, block[k]);
	    outsamples.clear();
	  }
	  if (gettimeofday(&eproc, 0) < 0) {
	    cerr << "Can't obtain the current time.\n";
	    exit(-1);
	  }
	  proctime =
	    (eproc.tv_sec - sproc.tv_sec) * 100000 + (eproc.tv_usec - sproc.tv_usec);
	}
	break;
      }
      default:
	break;
      }
      blocksize *= 2;
      numblocks = samples.size() / blocksize;
    }
  } else { // block modes

    // Block mode input
    WaveletInputSampleBlock<wisd> inputblock(samples);
    vector<WaveletInputSampleBlock<wisd> > blocks;
    unsigned numblocks = samples.size() / blocksize;
    for (i=0; i<numblocks; i++) {
      deque<wisd> dwisd;
      inputblock.GetSamples(dwisd, i*blocksize, i*blocksize+blocksize);
      blocks.push_back(WaveletInputSampleBlock<wisd>(dwisd));
      dwisd.clear();
    }

    switch(tt) {
    case APPROX: {
      for (i=0; i<blocks.size(); i++) {
	if (sleep) {
	  usleep(sleeptime_us);
	}
	sfwt.StreamingApproxBlockOperation(forwardoutput, blocks[i]);
	forwardoutput.clear();
      }
      break;
    }
    case DETAIL: {
      for (i=0; i<blocks.size(); i++) {
	if (sleep) {
	  usleep(sleeptime_us);
	}
	sfwt.StreamingDetailBlockOperation(forwardoutput, blocks[i]);
	forwardoutput.clear();
      }
      break;
    }
    case TRANSFORM: {
      for (i=0; i<blocks.size(); i++) {
	if (sleep) {
	  usleep(sleeptime_us);
	}
	sfwt.StreamingTransformBlockOperation(forwardoutput, blocks[i]);
	forwardoutput.clear();
      }
      break;
    }
    default:
      break;
    }
  }

  // Print the output with appropriate tag
  if (flat) {
    *outstr << sleeptime_us << " " << wt << " " << numstages << " " << tt << " "
	    << ((sample) ? 1 : blocksize);
  } else {
    *outstr << "Sleeptime (us) = " << sleeptime_us << endl;
    *outstr << "Wavelet type = " << wt << endl;
    *outstr << "Number stages = " << numstages << endl;
    *outstr << "Transform type = " << tt << endl;
    *outstr << "Block size (1 = sample op) = " << ((sample) ? 1 : blocksize) << endl;
  }
  *outstr << endl;

  return 0;
}
