#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <map>
#include <limits.h>

#include "banner.h"
#include "waveletsample.h"
#include "waveletsampleblock.h"
#include "transforms.h"
#include "delay.h"
#include "cmdlinefuncs.h"

void usage()
{
  char *tb=GetTsunamiBanner();
  char *b=GetRPSBanner();

  cerr << " sample_static_mixed_srwt [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [specification-file] [output-file] [flat]\n\n";
  cerr << "--------------------------------------------------------------\n";
  cerr << "\n";
  cerr << "[input-file]         = The name of the file containing wavelet\n";
  cerr << "                       coefficients.  Can NOT be stdin because\n";
  cerr << "                       a particular file format is expected.\n";
  cerr << "\n";
  cerr << "[wavelet-type-init]  = The type of wavelet.  The choices are\n";
  cerr << "                       {DAUB2 (Haar), DAUB4, DAUB6, DAUB8,\n";
  cerr << "                       DAUB10, DAUB12, DAUB14, DAUB16, DAUB18,\n";
  cerr << "                       DAUB20}.  The 'DAUB' stands for\n";
  cerr << "                       Daubechies wavelet types and the order\n";
  cerr << "                       is the number of coefficients.\n";
  cerr << "\n";
  cerr << "[numstages-init]     = The number of stages to use in the\n";
  cerr << "                       reconstruction.  The number of levels\n";
  cerr << "                       is equal to the number of stages + 1.\n";
  cerr << "\n";
  cerr << "[output-file]        = Which file to write the output.  This may\n";
  cerr << "                       also be stdout or stderr.\n";
  cerr << "\n";
  cerr << "[specification-file] = Mixed signal specification.\n";
  cerr << "\n";
  cerr << "[flat]               = Whether the output is flat or human\n";
  cerr << "                       readable.  flat | noflat to choose.\n";
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

  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
    cerr << "sample_static_mixed_srwt: stdin is not allowed in this utility.\n";
    exit(-1);
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "sample_static_mixed_srwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    cin = infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "sample_static_mixed_srwt: Number of stages must be positive.\n";
    exit(-1);
  }

  ostream outstr;
  ofstream outfile;
  if (!strcasecmp(argv[4],"stdout")) {
    outstr.tie(&cout);
  } else if (!strcasecmp(argv[4],"stderr")) {
    outstr.tie(&cerr);
  } else {
    outfile.open(argv[4]);
    if (!outfile) {
      cerr << "sample_static_mixed_srwt: Cannot open output file " << argv[4] << ".\n";
      exit(-1);
    }
    outstr.tie(&outfile);
  }

  ifstream specfile;
  specfile.open(argv[5]);
  if (!specfile) {
    cerr << "sample_static_mixed_srwt: Cannot open specification file " << argv[5] << ".\n";
    exit(-1);
  }

  bool flat=true;
  if (toupper(argv[6][0])=='N') {
    flat = false;
  } else if (toupper(argv[6][0])!='F') {
    cerr << "sample_static_mixed_srwt: Need to choose flat or noflat for human readable.\n";
    exit(-1);
  }

  SignalSpec sigspec;
  ParseSignalSpec(sigspec, specfile);
  specfile.close();

  typedef WaveletInputSample<double> wisd;
  typedef WaveletOutputSample<double> wosd;

  // Parameterize and instantiate the delay block
  unsigned wtcoefnum = numberOfCoefs[wt];
  int *delay = new int[numstages];
  CalculateMRADelayBlock(wtcoefnum, numstages, delay);
  DelayBlock<wosd> approx_dlyblk(numstages, 0, delay);
  DelayBlock<wosd> detail_dlyblk(numstages, 0, delay);

  if (!flat) {
    unsigned sampledelay = CalculateStreamingRealTimeDelay(wtcoefnum,numstages)-1;
    *outstr.tie() << "The real-time system delay is " << sampledelay << endl;
    *outstr.tie() << endl;
    *outstr.tie() << "Index\tValue\n" << endl;
    *outstr.tie() << "-----\t-----\n" << endl << endl;
  }

  // Instantiate a static reverse wavelet transform
  StaticReverseWaveletTransform<double, wisd, wosd> srwt(numstages,wt,2,2,0);

  // Create buffers
  vector<wosd> approxcoefs;
  vector<wosd> detailcoefs;
  vector<wosd> approx_dlysamples;
  vector<wosd> detail_dlysamples;
  vector<wisd> currentoutput;
  vector<wisd> reconst;

  unsigned sampletime, numsamples;
  char mratype;
  int levelnum;
  double sampvalue;
  map<int, unsigned, less<int> > indices;
  while (!cin.eof()) {
    cin >> sampletime >> mratype >> numsamples;
    for (unsigned i=0; i<numsamples; i++) {
      cin >> levelnum >> sampvalue;
      if (indices.find(levelnum) == indices.end()) {
	indices[levelnum] = 0;
      } else {
	indices[levelnum] += 1;
      }
      wosd sample(sampvalue, levelnum, indices[levelnum]);
      if (mratype == 'A') {
	approxcoefs.push_back(sample);
      } else if (mratype == 'D') {
	detailcoefs.push_back(sample);
      } else {
	cerr << "sample_static_mixed_srwt: Invalid MRA type.\n";
      }
    }

    approx_dlyblk.StreamingSampleOperation(approx_dlysamples, approxcoefs);
    detail_dlyblk.StreamingSampleOperation(detail_dlysamples, detailcoefs);

    if (srwt.StreamingMixedSampleOperation(currentoutput, 
					   approx_dlysamples,
					   detail_dlysamples,
					   sigspec)) {
      for (unsigned j=0; j<currentoutput.size(); j++) {
	reconst.push_back(currentoutput[j]);
      }
      approxcoefs.clear();
      detailcoefs.clear();
    }
  }

  for (unsigned i=0; i<reconst.size(); i++) {
    *outstr.tie() << i << "\t" << reconst[i].GetSampleValue() << endl;
  }
  *outstr.tie() << endl;

  if (delay != 0) {
    delete[] delay;
    delay=0;
  }

  return 0;
}