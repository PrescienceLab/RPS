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

  cerr << " discrete_reverse_zerofill_transform [input-file]\n";
  cerr << "  [wavelet-type-init] [zerospec-file] [transform-type] [flat] \n";
  cerr << "  [output-file]\n\n";
  cerr << "--------------------------------------------------------------\n";
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
  cerr << "[zerospec-file]     = Mixed signal specification.\n";
  cerr << "\n";
  cerr << "[transform-type]    = The reconstruction type may be of type\n";
  cerr << "                      TRANSFORM.\n";
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

int main(int argc, char *argv[])
{
  if (argc!=7) {
    usage();
    exit(-1);
  }

  istream *is = &cin;
  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "discrete_reverse_zerofill_transform: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    is = &infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  ifstream specfile;
  specfile.open(argv[3]);
  if (!specfile) {
    cerr << "discrete_reverse_zerofill_transform: Cannot open specification file " << argv[3] << ".\n";
    exit(-1);
  }

  if (toupper(argv[4][0])!='T') {
    cerr << "discrete_reverse_zerofill_transform: Invalid transform type.  Must be type TRANSFORM.\n";
    exit(-1);
  }

  bool flat=true;
  if (toupper(argv[5][0])=='N') {
    flat = false;
  } else if (toupper(argv[5][0])!='F') {
    cerr << "discrete_reverse_zerofill_transform: Need to choose flat or noflat for human readable.\n";
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
      cerr << "discrete_reverse_zerofill_transform: Cannot open output file " << argv[6] << ".\n";
      exit(-1);
    }
    outstr = &outfile;
  }

  vector<int> zerospec;
  ParseZeroSpec(zerospec, specfile);
  specfile.close();

  DiscreteWaveletOutputSampleBlock<wosd> waveletcoefs;

  // Read in the wavelet coefficients
  FlatParser fp;
  fp.ParseWaveletCoefsBlock(waveletcoefs, *is);

  if (!flat) {
    OutputLevelMetaData(*outstr, waveletcoefs, TRANSFORM);
  }

  // Instantiate a reverse discrete wavelet transform
  ReverseDiscreteWaveletTransform<double, wisd, wosd> rdwt(wt);

  // Create output buffers
  WaveletInputSampleBlock<wisd> reconst;

  // The operation
  rdwt.DiscreteWaveletTransformZeroFillOperation(reconst, waveletcoefs, zerospec);

  if (!flat) {
    *outstr << "The real-time system delay is no less than "
		  << waveletcoefs.GetBlockSize() << endl;
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
