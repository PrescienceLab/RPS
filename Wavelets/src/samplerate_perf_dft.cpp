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
  cerr << "  [transform-type] [blocksize] [sleep-rate] [numtests]\n";
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
  cerr << "[numtests]          = This is the number of tests to run.\n";
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
  if (argc!=9) {
    usage();
    exit(-1);
  }

  istream *is = &cin;
  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "discrete_forward_transform: Cannot open input file " << argv[1] << ".\n";
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
    cerr << "discrete_forward_transform: Invalid transform type.  Choose APPROX | DETAIL | TRANSFORM.\n";
    usage();
    exit(-1);
  }

  unsigned blocksize = atoi(argv[4]);
  if (blocksize == 0) {
    cerr << "samplerate_perf_sfwt: Must be greater than 0.\n";
    exit(-1);
  }

  bool sleep=true;
  unsigned long sleeptime_us = atoi(argv[5]);
  if (sleeptime_us == 0) {
    sleep=false;
  }

  unsigned numtests = atoi(argv[6]);

  bool flat=true;
  if (toupper(argv[7][0])=='N') {
    flat = false;
  } else if (toupper(argv[7][0])!='F') {
    cerr << "discrete_forward_transform: Need to choose flat or noflat for human readable.\n";
    exit(-1);
  }

  ostream *outstr = &cout;
  ofstream outfile;
  if (!strcasecmp(argv[8],"stdout")) {
  } else if (!strcasecmp(argv[8],"stderr")) {
    outstr = &cerr;
  } else {
    outfile.open(argv[8]);
    if (!outfile) {
      cerr << "discrete_forward_transform: Cannot open output file " << argv[8] << ".\n";
      exit(-1);
    }
    outstr = &outfile;
  }

  unsigned i;

  deque<wisd> samples;
  FlatParser fp;
  fp.ParseTimeDomain(samples, *is);
  infile.close();

  WaveletInputSampleBlock<wisd> inputblock(samples);
  vector<WaveletInputSampleBlock<wisd> > blocks;
  unsigned numblocks = samples.size() / blocksize;
  for (i=0; i<numblocks; i++) {
    deque<wisd> dwisd;
    inputblock.GetSamples(dwisd, i*blocksize, i*blocksize+blocksize);
    blocks.push_back(WaveletInputSampleBlock<wisd>(dwisd));
    dwisd.clear();
  }

  // Instantiate a forward discrete wavelet transform
  ForwardDiscreteWaveletTransform<double, wosd, wisd> fdwt(wt,0);

  // Create result buffers
  DiscreteWaveletOutputSampleBlock<wosd> forwardoutput;

  double usrbegin, sysbegin, usrend, sysend;
  for (unsigned test=0; test<numtests; test++) {
    switch(tt) {
    case APPROX: {
      GetRusage(sysbegin, usrbegin);
      for (i=0; i<blocks.size(); i++) {
	if (sleep) {
	  usleep(sleeptime_us);
	}
	forwardoutput.SetTransformType(APPROX);
	fdwt.DiscreteWaveletApproxOperation(forwardoutput, blocks[i]);
      }
      GetRusage(sysend, usrend);
      break;
    }
    case DETAIL: {
      GetRusage(sysbegin, usrbegin);
      for (i=0; i<blocks.size(); i++) {
	if (sleep) {
	  usleep(sleeptime_us);
	}
	forwardoutput.SetTransformType(DETAIL);
	fdwt.DiscreteWaveletDetailOperation(forwardoutput, blocks[i]);
      }
      GetRusage(sysend, usrend);
      break;
    }
    case TRANSFORM: {
      GetRusage(sysbegin, usrbegin);
      for (i=0; i<blocks.size(); i++) {
	if (sleep) {
	  usleep(sleeptime_us);
	}
	fdwt.DiscreteWaveletTransformOperation(forwardoutput, blocks[i]);
      }
      GetRusage(sysend, usrend);
      break;
    }
    default:
      break;
    }
    // Print the output with appropriate tag
    if (flat) {
      *outstr << sleeptime_us << " " << wt << " " << blocksize << " " << tt << " "
	      << usrend - usrbegin << " "
	      << sysend - sysbegin;
    } else {
      *outstr << "Sleeptime (us) = " << sleeptime_us << endl;
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
