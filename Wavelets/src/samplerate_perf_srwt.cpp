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

  cerr << " samplerate_perf_sfwt [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [transform-type] [sample-or-block]\n";
  cerr << "  [blocksize] [sleep-rate] [numtests] [flat] [output-file]\n\n";
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
      cerr << "samplerate_perf_srwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    is = &infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "samplerate_perf_srwt: Number of stages must be positive.\n";
    exit(-1);
  }

  TransformType tt=TRANSFORM;
  if (toupper(argv[4][0])!='T') {
    cerr << "samplerate_perf_srwt: Invalid transform type.  Must be type TRANSFORM.\n";
    exit(-1);
  }

  bool sample;
  if (toupper(argv[5][0])=='S') {
    sample=true;
  } else if (toupper(argv[5][0])=='B') {
    sample=false;
  } else {
    cerr << "samplerate_perf_sfwt: Operation type.  Choose SAMPLE | BLOCK.\n";
    exit(-1);
  }

  unsigned blocksize = atoi(argv[6]);
  if (blocksize == 0) {
    cerr << "samplerate_perf_sfwt: Must be greater than 0.\n";
    exit(-1);
  }

  bool sleep=true;
  unsigned long sleeptime_us = atoi(argv[7]);
  if (sleeptime_us == 0) {
    sleep=false;
  }

  unsigned numtests = atoi(argv[8]);


  bool flat=true;
  if (toupper(argv[9][0])=='N') {
    flat = false;
  } else if (toupper(argv[9][0])!='F') {
    cerr << "samplerate_perf_srwt: Need to choose flat or noflat for human readable.\n";
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
      cerr << "samplerate_perf_srwt: Cannot open output file " << argv[10] << ".\n";
      exit(-1);
    }
    outstr = &outfile;
  }

  unsigned i;

  // Parameterize and instantiate the delay block
  unsigned wtcoefnum = numberOfCoefs[wt];
  int *delay = new int[numstages+1];
  CalculateWaveletDelayBlock(wtcoefnum, numstages+1, delay);
  DelayBlock<wosd> dlyblk(numstages+1, 0, delay);

  // Instantiate a static reverse wavelet transform
  StaticReverseWaveletTransform<double, wisd, wosd> srwt(numstages,wt,2,2,0);

  double usrbegin, sysbegin, usrend, sysend;

  FlatParser fp;
  if (sample) {

    // Result buffers
    vector<vector<wosd> > samples;

    vector<wosd> waveletcoefs;
    vector<wosd> delaysamples;
    vector<wisd> currentoutput;
    vector<wisd> reconst;

    // Read all of the sampletime data into a vector
    while ( fp.ParseWaveletCoefsSample(waveletcoefs, *is) ) {
      samples.push_back(waveletcoefs);
      waveletcoefs.clear();
    }

    for (unsigned test=0; test<numtests; test++) {

      GetRusage(sysbegin, usrbegin);
      for (i=0; i<samples.size(); i++) {
	if (sleep) {
	  usleep(sleeptime_us);
	}
	dlyblk.StreamingSampleOperation(delaysamples, samples[i]);
	if (srwt.StreamingTransformSampleOperation(currentoutput, delaysamples)) {
	  for (unsigned j=0; j<currentoutput.size(); j++) {
	    reconst.push_back(currentoutput[j]);
	  }
	  currentoutput.clear();
	}
      }
      GetRusage(sysend, usrend);

      // Print the output with appropriate tag
      if (flat) {
	*outstr << sleeptime_us << " " << wt << " " << numstages << " " << tt << " "
		<< "SAMPLE " << usrend - usrbegin << " "
		<< sysend - sysbegin;
      } else {
	*outstr << "Sleeptime (us) = " << sleeptime_us << endl;
	*outstr << "Wavelet type = " << wt << endl;
	*outstr << "Number stages = " << numstages << endl;
	*outstr << "Transform type = " << tt << endl;
	*outstr << "Operation type = SAMPLE" << endl;
	*outstr << "User time = " << usrend - usrbegin << endl;
	*outstr << "System time = " << sysend - sysbegin << endl;
      }
      *outstr << endl;
    }

  } else { //Block mode

    vector<vector<WaveletOutputSampleBlock<wosd> > > blocks;

    vector<WaveletOutputSampleBlock<wosd> > waveletcoefs;
    for (i=0; i<(unsigned)numstages+1; i++) {
      waveletcoefs.push_back( WaveletOutputSampleBlock<wosd>(i) );
    }

    vector<WaveletOutputSampleBlock<wosd> > delayoutput;
    WaveletInputSampleBlock<wisd> reconst;
    WaveletInputSampleBlock<wisd> finaloutput;

    while( fp.ParseWaveletCoefsBlock(waveletcoefs, *is, blocksize)) {

      blocks.push_back(waveletcoefs);
      for (i=0; i<(unsigned)numstages+1; i++) {
	waveletcoefs[i].ClearBlock();
      }
    }

    for (unsigned test=0; test<numtests; test++) {

      GetRusage(sysbegin, usrbegin);
      for (i=0; i<blocks.size(); i++) {
	if (sleep) {
	  usleep(sleeptime_us);
	}
	// The operations
	dlyblk.StreamingBlockOperation(delayoutput, blocks[i]);
	srwt.StreamingTransformBlockOperation(reconst, delayoutput);
	finaloutput.AppendBlockBack(reconst);

	reconst.ClearBlock();
      }
      GetRusage(sysend, usrend);

      // Print the output with appropriate tag
      if (flat) {
	*outstr << sleeptime_us << " " << wt << " " << numstages << " " << tt << " "
		<< "BLOCK " << usrend - usrbegin << " "
		<< sysend - sysbegin;
      } else {
	*outstr << "Sleeptime (us) = " << sleeptime_us << endl;
	*outstr << "Wavelet type = " << wt << endl;
	*outstr << "Number stages = " << numstages << endl;
	*outstr << "Transform type = " << tt << endl;
	*outstr << "Operation type = " << "BLOCK" << endl;
	*outstr << "User time = " << usrend - usrbegin << endl;
	*outstr << "System time = " << sysend - sysbegin << endl;
      }
      *outstr << endl;
    }
  }

  if (delay != 0) {
    delete[] delay;
    delay=0;
  }

  return 0;
}
