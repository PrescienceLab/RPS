#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <map>

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

  cerr << " block_dynamic_srwt [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [transform-type] [wavelet-type-new]\n";
  cerr << "  [numstages-new] [change-interval] [flat] [output-file]\n\n";
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
      cerr << "block_dynamic_srwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    cin = infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "block_dynamic_srwt: Number of stages must be positive.\n";
    exit(-1);
  }

  if (toupper(argv[4][0])!='T') {
    cerr << "block_static_srwt: Invalid transform type.  Must be type TRANSFORM.\n";
    exit(-1);
  }

  WaveletType wtnew = GetWaveletType(argv[5], argv[0]);

  int numstages_new = atoi(argv[6]);
  if (numstages_new <= 0) {
    cerr << "block_dynamic_srwt: Number of stages must be positive.\n";
    exit(-1);
  }

  int change_interval = atoi(argv[7]);
  if (change_interval <= 0) {
    cerr << "block_dynamic_srwt: Change interval must be positive.\n";
    exit(-1);
  }

  bool flat=true;
  if (toupper(argv[8][0])=='N') {
    flat = false;
  } else if (toupper(argv[8][0])!='F') {
    cerr << "block_dynamic_srwt: Need to choose flat or noflat for human readable.\n";
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
      cerr << "block_dynamic_srwt: Cannot open output file " << argv[9] << ".\n";
      exit(-1);
    }
    outstr.tie(&outfile);
  }

  unsigned i;

  // Parameterize and instantiate the delay block
  unsigned wtcoefnum = numberOfCoefs[wt];
  int *delay = new int[numstages+1];
  CalculateWaveletDelayBlock(wtcoefnum, numstages+1, delay);

  unsigned wtcoefnum_new = numberOfCoefs[wtnew];
  int *delay_new = new int[numstages_new+1];
  CalculateWaveletDelayBlock(wtcoefnum_new, numstages_new+1, delay_new);

  DelayBlock<wosd> dlyblk(numstages+1, 0, delay);

  // Instantiate a static reverse wavelet transform
  DynamicReverseWaveletTransform<double, wisd, wosd> drwt(numstages,wt,2,2,0);

  // Create input/output buffers
  vector<WaveletOutputSampleBlock<wosd> > delayoutput;
  WaveletInputSampleBlock<wisd> reconst;
  WaveletInputSampleBlock<wisd> finaloutput;

  unsigned numlevels=numstages+1;
  vector<WaveletOutputSampleBlock<wosd> > waveletcoefs;
  for (i=0; i<numlevels; i++) {
    waveletcoefs.push_back( WaveletOutputSampleBlock<wosd>(i) );
  }

  unsigned diff=0;
  bool action_dir=true;  // True means add levels to waveletcoefs

  if (numstages < numstages_new) {
    diff = numstages_new - numstages;
  } else {
    diff = numstages - numstages_new;
    action_dir=false;
  }

  // Dynamic bookkeeping
  bool orig_struct=true;
  unsigned phasenum=0;

  FlatParser fp;
  while( fp.ParseWaveletCoefsBlock(waveletcoefs, cin, change_interval)) {

    if (!flat) {
      *outstr.tie() << "Phase " << phasenum << ":" << endl;
      OutputLevelMetaData(outstr, waveletcoefs, numlevels);
    }

    // The operations
    dlyblk.StreamingBlockOperation(delayoutput, waveletcoefs);
    drwt.StreamingTransformBlockOperation(reconst, delayoutput);

    bool success1 = (orig_struct) ?
      drwt.ChangeStructure(numstages_new, wtnew) :
      drwt.ChangeStructure(numstages, wt);

    bool success2 = (orig_struct) ?
      dlyblk.ChangeDelayConfig(numstages_new+1, 0, delay_new) :
      dlyblk.ChangeDelayConfig(numstages+1, 0, delay);

    if (!success1 && !success2) {
      cerr << "block_dynamic_srwt: Structure change failure.\n";
    }

    finaloutput.AppendBlockBack(reconst);

    for (i=0; i<numlevels; i++) {
      waveletcoefs[i].ClearBlock();
    }
    reconst.ClearBlock();

    // Either add stages or remove from input data structure to accomodate structure
    //  change
    if (diff) {
      if (action_dir) {
	// Increase levels in wavecoefs
	for (i=0; i<diff; i++) {
	  waveletcoefs.push_back( WaveletOutputSampleBlock<wosd>(numlevels+i) );
	}
      } else {
	// Decrease levels in wavecoefs
	for (i=0; i<diff; i++) {
	  waveletcoefs.pop_back();
	}
      }
      action_dir = (action_dir) ? false : true;
    }

    if (orig_struct) {
      numlevels=numstages_new+1;
      orig_struct=false;
    } else {
      numlevels=numlevels=numstages+1;
      orig_struct=true;
    }
    phasenum++;
  }

  if (!flat) {
    unsigned sampledelay = CalculateStreamingRealTimeDelay(wtcoefnum,numstages)-1;
    if (sampledelay <= (unsigned)change_interval) {
      *outstr.tie() << "The real-time system delay is " << sampledelay << endl;
    } else {
      *outstr.tie() << "The real-time system delay cannot be calculated." << endl;
    }
    *outstr.tie() << endl;
    *outstr.tie() << "Index\tValue\n" << endl;
    *outstr.tie() << "-----\t-----\n" << endl << endl;
  }

  for (unsigned i=0; i<finaloutput.GetBlockSize(); i++) {
    *outstr.tie() << i << "\t" << finaloutput[i].GetSampleValue() << endl;
  }
  *outstr.tie() << endl;

  if (delay != 0) {
    delete[] delay;
    delay=0;
  }
  if (delay_new != 0) {
    delete[] delay_new;
    delay_new=0;
  }

  return 0;
}
