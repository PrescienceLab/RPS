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

  cerr << " sample_dynamic_sfwt [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [transform-type] [wavelet-type-new]\n";
  cerr << "  [numstages-new] [change-interval] [flat] [output-file]\n\n";
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
  cerr << "[numstages-init]    = The number of stages to use in the\n";
  cerr << "                      decomposition.  The number of levels is\n";
  cerr << "                      equal to the number of stages + 1.\n";
  cerr << "\n";
  cerr << "[transform-type]    = The transform type may be of type\n";
  cerr << "                      APPROX | DETAIL | TRANSFORM.\n";
  cerr << "\n";
  cerr << "[wavelet-type-new]  = The new type of wavelet.  The choices\n";
  cerr << "                      are {DAUB2 (Haar), DAUB4, DAUB6, DAUB8,\n";
  cerr << "                      DAUB10, DAUB12, DAUB14, DAUB16, DAUB18,\n";
  cerr << "                      DAUB20}.  The 'DAUB' stands for\n";
  cerr << "                      Daubechies wavelet types and the order\n";
  cerr << "                      is the number of coefficients.\n";
  cerr << "\n";
  cerr << "[numstages-new]     = The new number of stages to use in the\n";
  cerr << "                      decomposition.  The number of levels is\n";
  cerr << "                      equal to the number of stages + 1.\n";
  cerr << "\n";
  cerr << "[change-interval]   = The amount of time in samples before\n";
  cerr << "                      changing to the new wavelet types and\n";
  cerr << "                      number of stage\n";
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
  if (argc!=10) {
    usage();
    exit(-1);
  }

  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "sample_dynamic_sfwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    cin = infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "sample_dynamic_sfwt: Number of stages must be positive.\n";
    exit(-1);
  }
  unsigned numlevels = numstages + 1;

  TransformType tt;
  if (toupper(argv[4][0])=='A') {
    tt = APPROX;
  } else if (toupper(argv[4][0])=='D') {
    tt = DETAIL;
  } else if (toupper(argv[4][0])=='T') {
    tt = TRANSFORM;
  } else {
    cerr << "sample_dynamic_sfwt: Invalid transform type.  Choose APPROX | DETAIL | TRANSFORM.\n";
    usage();
    exit(-1);
  }

  WaveletType wtnew = GetWaveletType(argv[5], argv[0]);

  int numstages_new = atoi(argv[6]);
  if (numstages_new <= 0) {
    cerr << "sample_dynamic_sfwt: Number of stages must be positive.\n";
    exit(-1);
  }
  unsigned numlevels_new = numstages_new + 1;

  int change_interval = atoi(argv[7]);
  if (change_interval <= 0) {
    cerr << "sample_dynamic_sfwt: Change interval must be positive.\n";
    exit(-1);
  }

  bool flat=true;
  if (toupper(argv[8][0])=='N') {
    flat = false;
  } else if (toupper(argv[8][0])!='F') {
    cerr << "sample_dynamic_sfwt: Need to choose flat or noflat for human readable.\n";
    exit(-1);
  }

  ostream outstr;
  ofstream outfile;
  if (!strcasecmp(argv[9],"stdout")) {
    outstr.tie(&cout);
  } else if (!strcasecmp(argv[9],"stderr")) {
    outstr.tie(&cerr);
  } else {
    outfile.open(argv[9]);
    if (!outfile) {
      cerr << "sample_dynamic_sfwt: Cannot open output file " << argv[9] << ".\n";
      exit(-1);
    }
    outstr.tie(&outfile);
  }

  unsigned i;

  // Read the data from file into an input vector
  vector<wisd> samples;
  FlatParser fp;
  fp.ParseTimeDomain(samples, cin);
  infile.close();

  // Instantiate a dynamic forward wavelet transform
  DynamicForwardWaveletTransform<double, wosd, wisd> dfwt(numstages,wt,2,2,0);

  // Create result buffers
  vector<wosd> outsamples;

  // Create vectors for the level outputs
  vector<deque<wosd> *> levels;
  for (i=0; i<MAX(numlevels,numlevels_new); i++) {
    deque<wosd>* pwos = new deque<wosd>();
    levels.push_back(pwos);
  }

  bool orig_struct=true;
  int current_interval=0;
  for (i=0; i<samples.size(); i++) {

    // Toggle the structure if change interval expired
    if (++current_interval == change_interval) {
      bool success = (orig_struct) ? dfwt.ChangeStructure(numstages_new, wtnew) :
	dfwt.ChangeStructure(numstages, wt);
      if (!success) {
	cerr << "sample_dynamic_sfwt: Structure failure.\n";
      }
      current_interval=0;
      (orig_struct) ? (orig_struct=false) : (orig_struct=true);
    }

    switch(tt) {
    case APPROX: {
      dfwt.StreamingApproxSampleOperation(outsamples, samples[i]);
      break;
    }
    case DETAIL: {
      dfwt.StreamingDetailSampleOperation(outsamples, samples[i]);
      break;
    }
    case TRANSFORM: {
      dfwt.StreamingTransformSampleOperation(outsamples, samples[i]);
      break;
    }
    default:
      break;
    }

    if (flat) {
      *outstr.tie() << i << "\t" << outsamples.size() << "\t";
    }

    for (unsigned j=0; j<outsamples.size(); j++) {
      int samplelevel = outsamples[j].GetSampleLevel();
      levels[samplelevel]->push_front(outsamples[j]);
      if (flat) {
	*outstr.tie() << samplelevel << " " << outsamples[j].GetSampleValue() << "\t";
      }
    }
    if (flat) {
      *outstr.tie() << endl;
    }
    outsamples.clear();
  }

  if (tt==APPROX || tt==DETAIL) {
    numlevels = MAX(numlevels-1, numlevels_new-1);
  } else {
    numlevels = MAX(numlevels, numlevels_new);
  }

  // Human readable output
  if (!flat) {
    OutputWaveletCoefsNonFlat(outstr, levels, numlevels);
  }
  
  for (i=0; i<numlevels; i++) {
    CHK_DEL(levels[i]);
  }
  levels.clear();

  return 0;
}
