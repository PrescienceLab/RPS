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

  cerr << " samplerate_perf_sfwt [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [transform-type] [sample-or-block]\n";
  cerr << "  [blocksize] [first-sleep-rate] [multiplier] [numrates] [flat]\n";
  cerr << "  [output-file]\n\n";
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
  cerr << "[first-sleep-rate]  = This rate is the first sleep rate after\n";
  cerr << "                      the max rate is determined.\n";
  cerr << "\n";
  cerr << "[multiplier]        = The newrate = oldrate x multiplier.\n";
  cerr << "\n";
  cerr << "[numrates]          = This is the number of samplerates to test\n";
  cerr << "                      before terminating the test.\n";
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
  if (argc!=12) {
    usage();
    exit(-1);
  }

  istream *is=&cin;
  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "samplerate_perf_sfwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    is = &infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "samplerate_perf_sfwt: Number of stages must be positive.\n";
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
    cerr << "samplerate_perf_sfwt: Invalid transform type.  Choose APPROX | DETAIL | TRANSFORM.\n";
    usage();
    exit(-1);
  }

  bool sample;
  if (toupper(argv[5][0])=='S') {
    sample=true;
  } else if (toupper(argv[5][0])=='B') {
    sample=false;
  } else {
    cerr << "samplerate_perf_sfwt: Operation type.  Choose SAMPLE | BLOCK.\n";
    usage();
    exit(-1);
  }

  unsigned blocksize = atoi(argv[6]);
  double sleeprate = atof(argv[7]);
  double multiplier = atof(argv[8]);
  unsigned numrates = atoi(argv[9]);

  bool flat=true;
  if (toupper(argv[10][0])=='N') {
    flat = false;
  } else if (toupper(argv[10][0])!='F') {
    cerr << "samplerate_perf_sfwt: Need to choose flat or noflat for human readable.\n";
    exit(-1);
  }

  ostream *outstr = &cout;
  ofstream outfile;
  if (!strcasecmp(argv[11],"stdout")) {
  } else if (!strcasecmp(argv[11],"stderr")) {
    outstr = &cerr;
  } else {
    outfile.open(argv[11]);
    if (!outfile) {
      cerr << "samplerate_perf_sfwt: Cannot open output file " << argv[11] << ".\n";
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

  // Block mode input
  WaveletInputSampleBlock<wisd> inputblock(samples);
  vector<WaveletInputSampleBlock<wisd> > blocks;
  unsigned numblocks = samples.size() / blocksize;
  for (i=0; i<numblocks; i++) {
    deque<wisd> dwisd;
    inputblock.GetSamples(dwisd, i*blocksize, i*blocksize+blocksize-1);
    blocks.push_back(WaveletInputSampleBlock<wisd>(dwisd));
    dwisd.clear();
  }

  // Instantiate a static forward wavelet transform
  StaticForwardWaveletTransform<double, wosd, wisd> sfwt(numstages,wt,2,2,0);

  // Create result buffers
  vector<wosd> outsamples;
  vector<WaveletOutputSampleBlock<wosd> > forwardoutput;

  double usrbegin, sysbegin, usrend, sysend;

  // Max rate test
  if (sample) {
    switch(tt) {
    case APPROX: {
      GetRusage(sysbegin, usrbegin);
      for (i=0; i<samples.size(); i++) {
	sleep(1);
	sfwt.StreamingApproxSampleOperation(outsamples, samples[i]);
	outsamples.clear();
      }
      GetRusage(sysend, usrend);
      break;
    }
    case DETAIL: {
      GetRusage(sysbegin, usrbegin);
      for (i=0; i<samples.size(); i++) {
	sleep(1);
	sfwt.StreamingDetailSampleOperation(outsamples, samples[i]);
	outsamples.clear();
      }
      GetRusage(sysend, usrend);
      break;
    }
    case TRANSFORM: {
      GetRusage(sysbegin, usrbegin);
      for (i=0; i<samples.size(); i++) {
	sleep(1);
	sfwt.StreamingTransformSampleOperation(outsamples, samples[i]);
	outsamples.clear();
      }
      GetRusage(sysend, usrend);
      break;
    }
    default:
      break;
    }
  } else { // block modes
    switch(tt) {
    case APPROX: {
      GetRusage(sysbegin, usrbegin);
      for (i=0; i<blocks.size(); i++) {
	sleep(1);
	sfwt.StreamingApproxBlockOperation(forwardoutput, blocks[i]);
	forwardoutput.clear();
      }
      GetRusage(sysend, usrend);
      break;
    }
    case DETAIL: {
      GetRusage(sysbegin, usrbegin);
      for (i=0; i<blocks.size(); i++) {
	sleep(1);
	sfwt.StreamingDetailBlockOperation(forwardoutput, blocks[i]);
	forwardoutput.clear();
      }
      GetRusage(sysend, usrend);
      break;
    }
    case TRANSFORM: {
      GetRusage(sysbegin, usrbegin);
      for (i=0; i<blocks.size(); i++) {
	sleep(1);
	sfwt.StreamingTransformBlockOperation(forwardoutput, blocks[i]);
	forwardoutput.clear();
      }
      GetRusage(sysend, usrend);
      break;
    }
    default:
      break;
    }
  }
  // Print the output with appropriate tag
  if (flat) {

  } else {

  }


  // ugly code, but trying to maximize performance here!
  for (unsigned test=0; test<numrates; test++) {
    if (sample) {
      switch(tt) {
      case APPROX: {
	GetRusage(sysbegin, usrbegin);
	for (i=0; i<samples.size(); i++) {
	  sleep(1);
	  sfwt.StreamingApproxSampleOperation(outsamples, samples[i]);
	  outsamples.clear();
	}
	GetRusage(sysend, usrend);
	break;
      }
      case DETAIL: {
	GetRusage(sysbegin, usrbegin);
	for (i=0; i<samples.size(); i++) {
	  sleep(1);
	  sfwt.StreamingDetailSampleOperation(outsamples, samples[i]);
	  outsamples.clear();
	}
	GetRusage(sysend, usrend);
	break;
      }
      case TRANSFORM: {
	GetRusage(sysbegin, usrbegin);
	for (i=0; i<samples.size(); i++) {
	  sleep(1);
	  sfwt.StreamingTransformSampleOperation(outsamples, samples[i]);
	  outsamples.clear();
	}
	GetRusage(sysend, usrend);
	break;
      }
      default:
	break;
      }
    } else { // block modes
      switch(tt) {
      case APPROX: {
	GetRusage(sysbegin, usrbegin);
	for (i=0; i<blocks.size(); i++) {
	  sleep(1);
	  sfwt.StreamingApproxBlockOperation(forwardoutput, blocks[i]);
	  forwardoutput.clear();
	}
	GetRusage(sysend, usrend);
	break;
      }
      case DETAIL: {
	GetRusage(sysbegin, usrbegin);
	for (i=0; i<blocks.size(); i++) {
	  sleep(1);
	  sfwt.StreamingDetailBlockOperation(forwardoutput, blocks[i]);
	  forwardoutput.clear();
	}
	GetRusage(sysend, usrend);
	break;
      }
      case TRANSFORM: {
	GetRusage(sysbegin, usrbegin);
	for (i=0; i<blocks.size(); i++) {
	  sleep(1);
	  sfwt.StreamingTransformBlockOperation(forwardoutput, blocks[i]);
	  forwardoutput.clear();
	}
	GetRusage(sysend, usrend);
	break;
      }
      default:
	break;
      }
    }
    // Print the output with appropriate tag
  }

  return 0;
}
