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

  cerr << " perf_dft [input-file] [wavelet-type-init]\n";
  cerr << "  [transform-type] [blocksize] [sleep-rate]\n";
  cerr << "  [flat] [output-file]\n\n";
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
      cerr << "perf_dft: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    is = &infile;
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
    cerr << "perf_dft: Invalid transform type.  Choose APPROX | DETAIL | TRANSFORM.\n";
    usage();
    exit(-1);
  }

  unsigned blocksize = atoi(argv[4]);
  if (blocksize == 0) {
    cerr << "perf_dft: Must be greater than 0.\n";
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
    cerr << "perf_dft: Need to choose flat or noflat for human readable.\n";
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
      cerr << "perf_dft: Cannot open output file " << argv[7] << ".\n";
      exit(-1);
    }
    outstr = &outfile;
  }

  unsigned i;

  deque<wisd> samples;
  FlatParser fp;
  fp.ParseTimeDomain(samples, *is);
  infile.close();

  // Instantiate a forward discrete wavelet transform
  ForwardDiscreteWaveletTransform<double, wosd, wisd> fdwt(wt,0);

  // Create result buffers
  DiscreteWaveletOutputSampleBlock<wosd> forwardoutput;

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
	forwardoutput.SetTransformType(APPROX);
	fdwt.DiscreteWaveletApproxOperation(forwardoutput, block);
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
	forwardoutput.SetTransformType(DETAIL);
	fdwt.DiscreteWaveletDetailOperation(forwardoutput, block);
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
	fdwt.DiscreteWaveletTransformOperation(forwardoutput, block);
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
