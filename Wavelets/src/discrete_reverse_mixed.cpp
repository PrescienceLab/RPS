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

  cerr << " discrete_reverse_mixed [input-file] [wavelet-type-init]\n";
  cerr << "  [specification-file] [flat] [output-file]\n\n";
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
  if (argc!=6) {
    usage();
    exit(-1);
  }

  istream *is = &cin;
  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
    cerr << "discrete_reverse_mixed: stdin is not allowed in this utility.\n";
    exit(-1);
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "discrete_reverse_mixed: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    is = &infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  ifstream specfile;
  specfile.open(argv[3]);
  if (!specfile) {
    cerr << "discrete_reverse_mixed: Cannot open specification file " << argv[3] << ".\n";
    exit(-1);
  }

  bool flat=true;
  if (toupper(argv[4][0])=='N') {
    flat = false;
  } else if (toupper(argv[4][0])!='F') {
    cerr << "discrete_reverse_mixed: Need to choose flat or noflat for human readable.\n";
    exit(-1);
  }

  ostream *outstr = &cout;
  ofstream outfile;
  if (!strcasecmp(argv[5],"stdout")) {
  } else if (!strcasecmp(argv[5],"stderr")) {
    outstr = &cerr;
  } else {
    outfile.open(argv[5]);
    if (!outfile) {
      cerr << "discrete_reverse_mixed: Cannot open output file " << argv[5] << ".\n";
      exit(-1);
    }
    outstr = &outfile;
  }

  SignalSpec sigspec;
  ParseSignalSpec(sigspec, specfile);
  specfile.close();

  // Find the number of stages represented in the signal specification
  unsigned numstages=0;
  for (unsigned i=0; i<sigspec.approximations.size(); i++) {
    if ((unsigned)sigspec.approximations[i] > numstages) {
      numstages = sigspec.approximations[i];
    }
  }
  for (unsigned i=0; i<sigspec.details.size(); i++) {
    if ((unsigned)sigspec.details[i] > numstages) {
      numstages = sigspec.details[i];
    }
  }

  // Instantiate a reverse discrete wavelet transform
  ReverseDiscreteWaveletTransform<double, wisd, wosd> rdwt(wt);

  // Create output buffers
  vector<WaveletOutputSampleBlock<wosd> > approxcoefs;
  vector<WaveletOutputSampleBlock<wosd> > detailcoefs;
  for (unsigned i=0; i<numstages+1; i++) {
    approxcoefs.push_back( WaveletOutputSampleBlock<wosd>(i) );
    detailcoefs.push_back( WaveletOutputSampleBlock<wosd>(i) );
  }
  WaveletInputSampleBlock<wisd> reconst;

  // Read in the MRA coefficients
  FlatParser fp;
  fp.ParseMRACoefsBlock(sigspec, approxcoefs, detailcoefs, *is);

  // Could possible look at available approximation levels and optimize
  // that way, but for now if the one sample approximation is not in
  // the representation, then no approximations are used.

  if (!flat) {
    *outstr << "APPROX LEVELS:" << endl;
    OutputLevelMetaData(*outstr, approxcoefs, numstages+1);
    *outstr << "DETAIL LEVELS:" << endl;
    OutputLevelMetaData(*outstr, detailcoefs, numstages+1);
  }

  // The operations
  rdwt.DiscreteWaveletMixedOperation(reconst,
				     approxcoefs,
				     detailcoefs,
				     numstages+1);

  if (!flat) {
    *outstr << "The real-time system delay is no less than "
		  << reconst.GetBlockSize() << endl;
    *outstr << endl;
    *outstr << "Index\tValue\n" << endl;
    *outstr << "-----\t-----\n" << endl << endl;
  }

  for (unsigned i=0; i<reconst.GetBlockSize(); i++) {
    *outstr << i << "\t" << reconst[i].GetSampleValue() << endl;
  }
  *outstr << endl;

  return 0;
}
