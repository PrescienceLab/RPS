#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits.h>

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

  cerr << " block_dynamic_mixed_sfwt [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [specification-file] [wavelet-type-new]\n";
  cerr << "  [numstages-new] [change-interval] [flat] [output-file]\n\n";
  cerr << "--------------------------------------------------------------\n";
  cerr << "\n";
  cerr << "[input-file]         = The name of the file containing time-\n";
  cerr << "                       domain samples.  Can also be stdin.\n";
  cerr << "\n";
  cerr << "[wavelet-type-init]  = The type of wavelet.  The choices are\n";
  cerr << "                       {DAUB2 (Haar), DAUB4, DAUB6, DAUB8,\n";
  cerr << "                       DAUB10, DAUB12, DAUB14, DAUB16, DAUB18,\n";
  cerr << "                       DAUB20}.  The 'DAUB' stands for\n";
  cerr << "                       Daubechies wavelet types and the order\n";
  cerr << "                       is the number of coefficients.\n";
  cerr << "\n";
  cerr << "[numstages-init]     = The number of stages to use in the\n";
  cerr << "                       decomposition.  The number of levels is\n";
  cerr << "                       equal to the number of stages + 1.\n";
  cerr << "\n";
  cerr << "[specification-file] = Mixed signal specification.\n";
  cerr << "\n";
  cerr << "[wavelet-type-new]   = The new type of wavelet.  The choices\n";
  cerr << "                       are {DAUB2 (Haar), DAUB4, DAUB6, DAUB8,\n";
  cerr << "                       DAUB10, DAUB12, DAUB14, DAUB16, DAUB18,\n";
  cerr << "                       DAUB20}.  The 'DAUB' stands for\n";
  cerr << "                       Daubechies wavelet types and the order\n";
  cerr << "                       is the number of coefficients.\n";
  cerr << "\n";
  cerr << "[numstages-new]      = The new number of stages to use in the\n";
  cerr << "                       decomposition.  The number of levels is\n";
  cerr << "                       equal to the number of stages + 1.\n";
  cerr << "\n";
  cerr << "[change-interval]    = The amount of time in samples before\n";
  cerr << "                       changing to the new wavelet types and\n";
  cerr << "                       number of stage\n";
  cerr << "\n";
  cerr << "[flat]               = Whether the output is flat or human\n";
  cerr << "                       readable.  flat | noflat to choose.\n";
  cerr << "\n";
  cerr << "[output-file]        = Which file to write the output.  This may\n";
  cerr << "                       also be stdout or stderr.\n\n";
  cerr << "\n";
  cerr << tb << endl;
  cerr << b << endl;
  delete [] tb;
  delete [] b;
}

int main(int argc, char *argv[])
{
  if (argc!=10) {
    usage();
    exit(-1);
  }

  istream *is = &cin;
  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "block_dynamic_mixed_sfwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    is = &infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "block_dynamic_mixed_sfwt: Number of stages must be positive.\n";
    exit(-1);
  }

  ifstream specfile;
  specfile.open(argv[4]);
  if (!specfile) {
    cerr << "sample_dynamic_mixed_sfwt: Cannot open specification file " << argv[4] << ".\n";
    exit(-1);
  }

  WaveletType wtnew = GetWaveletType(argv[5], argv[0]);

  int numstages_new = atoi(argv[6]);
  if (numstages_new <= 0) {
    cerr << "sample_dynamic_mixed_sfwt: Number of stages must be positive.\n";
    exit(-1);
  }

  int change_interval = atoi(argv[7]);
  if (change_interval <= 0) {
    cerr << "sample_dynamic_mixed_sfwt: Change interval must be positive.\n";
    exit(-1);
  }

  bool flat=true;
  if (toupper(argv[8][0])=='N') {
    flat = false;
  } else if (toupper(argv[8][0])!='F') {
    cerr << "block_dynamic_mixed_sfwt: Need to choose flat or noflat for human readable.\n";
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
      cerr << "block_dynamic_mixed_sfwt: Cannot open output file " << argv[9] << ".\n";
      exit(-1);
    }
    outstr = &outfile;
  }

  unsigned i;

  SignalSpec sigspec;
  ParseSignalSpec(sigspec, specfile);
  specfile.close();

  // Read the data from file into an input vector
  deque<wisd> samples;
  FlatParser fp;
  fp.ParseTimeDomain(samples, *is);
  infile.close();

  WaveletInputSampleBlock<wisd> inputblock(samples);

  // Instantiate a static forward wavelet transform
  DynamicForwardWaveletTransform<double, wosd, wisd> dfwt(numstages,wt,2,2,0);

  // Create result buffers
  vector<WaveletOutputSampleBlock<wosd> > approxout;
  vector<WaveletOutputSampleBlock<wosd> > detailout;

  vector<vector<WaveletOutputSampleBlock<wosd> > > approxlevels;
  vector<vector<WaveletOutputSampleBlock<wosd> > > detaillevels;

  // Dynamic bookkeeping...sigh
  bool orig_struct=true;
  int samplecnt=0;
  deque<wisd> buf;

  // Structure for printing out level metadata
  unsigned *a_levelsize=0;
  unsigned *d_levelsize=0;
  unsigned levelcnt=MAX(numstages, numstages_new);

  a_levelsize = new unsigned[levelcnt];
  d_levelsize = new unsigned[levelcnt];

  for (i=0; i<levelcnt; i++) {
    a_levelsize[i]=0;
    d_levelsize[i]=0;
  }

  while (inputblock.GetBlockSize() - samplecnt > 0) {
    if ( (unsigned)(samplecnt + change_interval) <= inputblock.GetBlockSize()) {
      inputblock.GetSamples(buf, samplecnt, samplecnt+change_interval);
      samplecnt += change_interval;
    } else {
      inputblock.GetSamples(buf, samplecnt, inputblock.GetBlockSize()-1);
      samplecnt += inputblock.GetBlockSize() - samplecnt - 1;
    }

    dfwt.StreamingMixedBlockOperation(approxout,
				      detailout,
				      WaveletInputSampleBlock<wisd>(buf),
				      sigspec);

    approxlevels.push_back(approxout);
    detaillevels.push_back(detailout);

    // Update counts
	unsigned j;
    for (j=0; j<approxout.size(); j++) {
      a_levelsize[j] += approxout[j].GetBlockSize();
    }
    for (j=0; j<detailout.size(); j++) {
      d_levelsize[j] += detailout[j].GetBlockSize();
    }
    approxout.clear();
    detailout.clear();

    // Toggle the structure
    bool success = (orig_struct) ? dfwt.ChangeStructure(numstages_new, wtnew) :
      dfwt.ChangeStructure(numstages, wt);
    if (!success) {
      cerr << "block_dynamic_sfwt: Structure failure.\n";
    }
    (orig_struct) ? (orig_struct=false) : (orig_struct=true);
  }

  // Human readable output
  if (!flat) {
    *outstr << "APPROXIMATIONS" << endl;
    *outstr << "--------------" << endl;
    OutputLevelMetaData(*outstr, a_levelsize, levelcnt);

    *outstr << endl << "DETAILS" << endl;
    *outstr << "-------" << endl;
    OutputLevelMetaData(*outstr, d_levelsize, levelcnt);
  }

  unsigned count=0;
  for (i=0; i<MIN(approxlevels.size(), detaillevels.size()); i++) {
    count=OutputMRACoefs(*outstr, approxlevels[i], detaillevels[i], count);
  }

  if (a_levelsize != 0) {
    delete[] a_levelsize;
    a_levelsize=0;
  }
  if (d_levelsize != 0) {
    delete[] d_levelsize;
    d_levelsize=0;
  }

  return 0;
}
