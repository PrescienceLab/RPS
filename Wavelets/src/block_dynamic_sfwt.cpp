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

void usage()
{
  char *tb=GetTsunamiBanner();
  char *b=GetRPSBanner();

  cerr << " block_dynamic_sfwt [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [transform-type] [wavelet-type-new]\n";
  cerr << "  [numstages-new] [change-interval] [output-file] [flat]\n\n";
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
  cerr << "[output-file]       = Which file to write the output.  This may\n";
  cerr << "                      also be stdout or stderr.\n\n";
  cerr << "\n";
  cerr << "[flat]              = Whether the output is flat or human\n";
  cerr << "                      readable.  flat | noflat to choose.\n";
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
      cerr << "block_dynamic_sfwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    cin = infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "block_dynamic_sfwt: Number of stages must be positive.\n";
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
    cerr << "block_dynamic_sfwt: Invalid transform type.  Choose APPROX | DETAIL | TRANSFORM.\n";
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

  ostream outstr;
  ofstream outfile;
  if (!strcasecmp(argv[8],"stdout")) {
    outstr.tie(&cout);
  } else if (!strcasecmp(argv[8],"stderr")) {
    outstr.tie(&cerr);
  } else {
    outfile.open(argv[8]);
    if (!outfile) {
      cerr << "block_dynamic_sfwt: Cannot open output file " << argv[8] << ".\n";
      exit(-1);
    }
    outstr.tie(&outfile);
  }

  bool flat=true;
  if (toupper(argv[9][0])=='N') {
    flat = false;
  } else if (toupper(argv[9][0])!='F') {
    cerr << "block_dynamic_sfwt: Need to choose flat or noflat for human readable.\n";
    exit(-1);
  }

  unsigned i;
  typedef WaveletInputSample<double> wisd;
  typedef WaveletOutputSample<double> wosd;

  // Read a block of data from file into an input vector
  deque<wisd> samples;
  double sample;
  unsigned index=0;
  while (cin >> sample) {
    wisd wavesample;
    wavesample.SetSampleValue(sample);
    wavesample.SetSampleIndex(index++);
    samples.push_back(wavesample);
  }
  infile.close();

  WaveletInputSampleBlock<wisd> inputblock(samples);

  // Instantiate a static forward wavelet transform
  DynamicForwardWaveletTransform<double, wosd, wisd> dfwt(numstages,wt,2,2,0);

  // Create result buffers
  vector<WaveletOutputSampleBlock<wosd> > forwardoutput;

  bool orig_struct=true;
  int samplecnt=0;
  deque<wisd> buf;

  unsigned *levelsize=0;
  if (tt==APPROX || tt==DETAIL) {
    levelsize = new unsigned[MAX(numstages, numstages_new)];
  } else {
    levelsize = new unsigned[MAX(numstages+1, numstages_new+1)];
  }

  while (inputblock.GetBlockSize() - samplecnt >= 0) {

    if ( (unsigned)(samplecnt + change_interval) <= inputblock.GetBlockSize()) {
      inputblock.GetSamples(buf, samplecnt, samplecnt+change_interval);
      samplecnt += change_interval;
    } else {
      inputblock.GetSamples(buf, samplecnt, inputblock.GetBlockSize()-1);
      samplecnt += inputblock.GetBlockSize();
    }

    switch(tt) {
    case APPROX: {
      dfwt.StreamingApproxBlockOperation(forwardoutput, buf);
      break;
    }
    case DETAIL: {
      dfwt.StreamingDetailBlockOperation(forwardoutput, buf);
      break;
    }
    case TRANSFORM: {
      dfwt.StreamingTransformBlockOperation(forwardoutput, buf);
    }
    default:
      break;
    }

    // Print the results and clear the buffer

    // Toggle the structure
    bool success = (orig_struct) ? dfwt.ChangeStructure(numstages_new, wtnew) :
      dfwt.ChangeStructure(numstages, wt);
    if (!success) {
      cerr << "block_dynamic_sfwt: Structure failure.\n";
    }
    (orig_struct) ? (orig_struct=false) : (orig_struct=true);
  }

  numlevels = (orig_struct) ? (numlevels-1) : (numlevels_new-1);

  // Human readable output
  if (!flat) {
    *outstr.tie() << "The size of each level:" << endl;
    for (i=0; i<numlevels; i++) {
      *outstr.tie() << "\tLevel " << i << " size = " 
		    << forwardoutput[i].GetBlockSize() << endl;
    }
    *outstr.tie() << endl;

    *outstr.tie() << "Index     ";
    for (i=0; i<numlevels; i++) {
      *outstr.tie() << "Level " << i << "        " ;
    }
    *outstr.tie() << endl << "-----     ";
    for (i=0; i<numlevels; i++) {
      *outstr.tie() << "-------        ";
    }
    *outstr.tie() << endl;
  }


  unsigned loopsize = forwardoutput[0].GetBlockSize();
  for (i=0; i<loopsize; i++) {
    *outstr.tie() << i << "\t";


    // Find number of samples for this line
    unsigned numsamples=0;
    for (unsigned j=0; j<numlevels; j++) {
      if (!forwardoutput[j].Empty()) {
	numsamples++;
      }
    }

    if (flat) {
      *outstr.tie() << numsamples << "\t";
    }

    for (unsigned j=0; j<numsamples; j++) {
      if (!forwardoutput[j].Empty()) {
	wosd wos;
	wos = forwardoutput[j].Front();

	if (flat) {
	  *outstr.tie() << wos.GetSampleLevel() << " ";
	}

	*outstr.tie() << wos.GetSampleValue() << "\t";
	forwardoutput[j].PopSampleFront();
      }
    }
    *outstr.tie() << endl;
  }
  
  return 0;
}
