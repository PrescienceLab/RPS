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

  cerr << " block_dynamic_streaming_test [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [wavelet-type-new] [numstages-new]\n";
  cerr << "  [change-interval] [output-file]\n\n";
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
  cerr << tb << endl;
  cerr << b << endl;
  delete [] tb;
  delete [] b;
}

int main(int argc, char *argv[])
{
  if (argc!=8) {
    usage();
    exit(-1);
  }

  istream *is = &cin;
  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "block_dynamic_streaming_test: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    is = &infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "block_dynamic_streaming_test: Number of stages must be positive.\n";
    exit(-1);
  }

  WaveletType wtnew = GetWaveletType(argv[4], argv[0]);

  int numstages_new = atoi(argv[5]);
  if (numstages_new <= 0) {
    cerr << "block_dynamic_streaming_test: Number of stages must be positive.\n";
    exit(-1);
  }

  int change_interval = atoi(argv[6]);
  if (change_interval <= 0) {
    cerr << "block_dynamic_streaming_test: Change interval must be positive.\n";
    exit(-1);
  }

  ostream *outstr = &cout;
  ofstream outfile;
  if (!strcasecmp(argv[7],"stdout")) {
  } else if (!strcasecmp(argv[7],"stderr")) {
    outstr = &cerr;
  } else {
    outfile.open(argv[7]);
    if (!outfile) {
      cerr << "block_dynamic_streaming_test: Cannot open output file " << argv[7] << ".\n";
      exit(-1);
    }
    outstr = &outfile;
  }

  unsigned i;

  // Read a block of data from file into an input vector
  deque<wisd> samples;
  FlatParser fp;
  fp.ParseTimeDomain(samples, *is);
  infile.close();

  WaveletInputSampleBlock<wisd> inputblock(samples);

  // Instantiate a static forward wavelet transform
  DynamicForwardWaveletTransform<double, wosd, wisd> dfwt(numstages,wt,2,2,0);

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
  vector<WaveletOutputSampleBlock<wosd> > waveletcoefs;
  vector<WaveletOutputSampleBlock<wosd> > delayoutput;
  WaveletInputSampleBlock<wisd> reconst;
  WaveletInputSampleBlock<wisd> finaloutput;

  // Dynamic bookkeeping
  bool orig_struct=true;
  int samplecnt=0;
  deque<wisd> buf;

  while ((int)inputblock.GetBlockSize() - samplecnt > 0) {
    if ( (unsigned)(samplecnt + change_interval) <= inputblock.GetBlockSize()) {
      inputblock.GetSamples(buf, samplecnt, samplecnt+change_interval);
      samplecnt += change_interval;
    } else {
      inputblock.GetSamples(buf, samplecnt, inputblock.GetBlockSize());
      samplecnt = inputblock.GetBlockSize();
    }

    // The operations
    dfwt.StreamingTransformBlockOperation(waveletcoefs, buf);
    dlyblk.StreamingBlockOperation(delayoutput, waveletcoefs);
    drwt.StreamingTransformBlockOperation(reconst, delayoutput);

    // Change structures
    bool success = (orig_struct) ?
      dfwt.ChangeStructure(numstages_new, wtnew) :
      dfwt.ChangeStructure(numstages, wt);

    bool success1 = (orig_struct) ?
      drwt.ChangeStructure(numstages_new, wtnew) :
      drwt.ChangeStructure(numstages, wt);

    bool success2 = (orig_struct) ?
      dlyblk.ChangeDelayConfig(numstages_new+1, 0, delay_new) :
      dlyblk.ChangeDelayConfig(numstages+1, 0, delay);

    if (!success && !success1 && !success2) {
      cerr << "block_dynamic_streaming_test: Structure change failure.\n";
    }
    (orig_struct) ? (orig_struct=false) : (orig_struct=true);

    finaloutput.AppendBlockBack(reconst);
    waveletcoefs.clear();
    reconst.ClearBlock();
  }

  unsigned sampledelay = CalculateStreamingRealTimeDelay(wtcoefnum,numstages)-1;
  if (sampledelay <= (unsigned)change_interval) {
    *outstr << "The real-time system delay is " << sampledelay << endl;
  } else {
    *outstr << "The real-time system delay cannot be calculated." << endl;
  }
  *outstr << endl;
  *outstr << "Index\tValue\n" << endl;
  *outstr << "-----\t-----\n" << endl << endl;

  for (i=0; i<MIN(finaloutput.GetBlockSize(), inputblock.GetBlockSize()); i++) {
    *outstr << i << "\t" << inputblock[i].GetSampleValue() << "\t"
		  << finaloutput[i].GetSampleValue() << endl;
  }
  *outstr << endl;

  // Calculate the error between input and output
  double error=0;
  i=0;
  for (unsigned j=sampledelay; j<MIN(finaloutput.GetBlockSize(), inputblock.GetBlockSize()); i++, j++) {
    error += inputblock[i].GetSampleValue() - finaloutput[j].GetSampleValue();
  }
  
  *outstr << "Mean error: " << error/(double)i << endl;

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
