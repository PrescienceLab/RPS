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
#include "flatparser.h"

void usage()
{
  char *tb=GetTsunamiBanner();
  char *b=GetRPSBanner();

  cerr << " sample_dynamic_mixed_srwt [input-file] [wavelet-type-init]\n";
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

  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "sample_dynamic_mixed_srwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    cin = infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "sample_dynamic_mixed_srwt: Number of stages must be positive.\n";
    exit(-1);
  }

  ifstream specfile;
  specfile.open(argv[4]);
  if (!specfile) {
    cerr << "sample_dynamic_mixed_srwt: Cannot open specification file " << argv[4] << ".\n";
    exit(-1);
  }

  WaveletType wtnew = GetWaveletType(argv[5], argv[0]);

  int numstages_new = atoi(argv[6]);
  if (numstages_new <= 0) {
    cerr << "sample_dynamic_mixed_srwt: Number of stages must be positive.\n";
    exit(-1);
  }

  int change_interval = atoi(argv[7]);
  if (change_interval <= 0) {
    cerr << "sample_dynamic_mixed_srwt: Change interval must be positive.\n";
    exit(-1);
  }

  bool flat=true;
  if (toupper(argv[8][0])=='N') {
    flat = false;
  } else if (toupper(argv[8][0])!='F') {
    cerr << "sample_dynamic_mixed_srwt: Need to choose flat or noflat for human readable.\n";
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
      cerr << "sample_dynamic_mixed_srwt: Cannot open output file " << argv[9] << ".\n";
      exit(-1);
    }
    outstr.tie(&outfile);
  }

  SignalSpec sigspec;
  ParseSignalSpec(sigspec, specfile);
  specfile.close();

  // Optimize the operations
  SignalSpec optim_spec;
  unsigned optim_stages;
  bool transform=StructureOptimizer(optim_spec,
				    optim_stages,
				    MIN(numstages, numstages_new),
				    0,
				    sigspec);

  // Parameterize and instantiate delay block init
  unsigned wtcoefnum = numberOfCoefs[wt];
  int *delay = new int[optim_stages+1];
  CalculateWaveletDelayBlock(wtcoefnum, optim_stages+1, delay);

  unsigned wtcoefnum_new = numberOfCoefs[wtnew];
  int *delay_new = new int[optim_stages+1];
  CalculateWaveletDelayBlock(wtcoefnum_new, optim_stages+1, delay_new);

  DelayBlock<wosd> dlyblk(optim_stages+1, 0, delay);

  // Instantiate a static reverse wavelet transform
  DynamicReverseWaveletTransform<double, wisd, wosd> drwt(optim_stages,wt,2,2,0);

  // Create buffers
  vector<wosd> approxcoefs;
  vector<wosd> detailcoefs;
  vector<wosd> wavecoefs;
  vector<wosd> dlysamples;
  vector<wisd> currentoutput;
  vector<wisd> reconst;

  // Dynamic bookkeeping
  bool orig_struct=true;
  int current_interval=0;

  FlatParser fp;
  while (fp.ParseMRACoefsSample(optim_spec, approxcoefs, detailcoefs, cin)) {

    // Transfer the coefficients into wavecoefs and change level of approx
    if (approxcoefs.size()) {
      wosd asamp=approxcoefs[0];
      asamp.SetSampleLevel(asamp.GetSampleLevel()+1);
      wavecoefs.push_back(asamp);
    }
    for (unsigned i=0; i<detailcoefs.size(); i++) {
      wavecoefs.push_back(detailcoefs[i]);
    }

    // Toggle the structure if change interval expired
    if (++current_interval == change_interval) {
      bool success1 = (orig_struct) ?
	drwt.ChangeStructure(optim_stages, wtnew) :
	drwt.ChangeStructure(optim_stages, wt);

      bool success2 = (orig_struct) ?
	dlyblk.ChangeDelayConfig(optim_stages+1, 0, delay_new) :
	dlyblk.ChangeDelayConfig(optim_stages+1, 0, delay);

      if (!success1 && !success2) {
	cerr << "sample_dynamic_srwt: Structure change failure.\n";
      }
      current_interval=0;
      (orig_struct) ? (orig_struct=false) : (orig_struct=true);
    }

    dlyblk.StreamingSampleOperation(dlysamples, wavecoefs);

    if (transform) {
      if (drwt.StreamingTransformSampleOperation(currentoutput, dlysamples)) {
	for (unsigned j=0; j<currentoutput.size(); j++) {
	  reconst.push_back(currentoutput[j]);
	}
      }
    } else {
      vector<int> zerospec;
      vector<int> specs;
      FlattenSignalSpec(specs, optim_spec);
      InvertSignalSpec(zerospec, specs, optim_stages+1, 0);
      if (drwt.StreamingTransformZeroFillSampleOperation(currentoutput,
							 dlysamples,
							 zerospec)) {
	for (unsigned j=0; j<currentoutput.size(); j++) {
	  reconst.push_back(currentoutput[j]);
	}
      }
    }
    approxcoefs.clear();
    detailcoefs.clear();
    wavecoefs.clear();
  }

  if (!flat) {
    unsigned sampledelay = CalculateStreamingRealTimeDelay(wtcoefnum,optim_stages)-1;
    if (sampledelay <= (unsigned)change_interval) {
      *outstr.tie() << "The real-time system delay is " << sampledelay << endl;
    } else {
      *outstr.tie() << "The real-time system delay cannot be calculated." << endl;
    }
    *outstr.tie() << endl;
    *outstr.tie() << "Index\tValue\n" << endl;
    *outstr.tie() << "-----\t-----\n" << endl << endl;
  }

  for (unsigned i=0; i<reconst.size(); i++) {
    *outstr.tie() << i << "\t" << reconst[i].GetSampleValue() << endl;
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
