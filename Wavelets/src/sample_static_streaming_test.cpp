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

void usage()
{
  char *b=GetRPSBanner();

  cerr << " sample_static_streaming_test [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [transform-type] [output-file]\n\n";
  cerr << "--------------------------------------------------------------\n";
  cerr << "\n";
  cerr << "[input-file]        = The name of the file containing time-\n";
  cerr << "                      domain samples.\n";
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
  cerr << "[transform-type]    = The transform type may only be of type\n";
  cerr << "                      TRANSFORM for this test.\n";
  cerr << "\n";
  cerr << "[output-file]       = Which file to write the output.  This may\n";
  cerr << "                      also be stdout or stderr.\n\n";
  cerr << b << endl;
  delete [] b;
}

WaveletType GetWaveletType(const char *x)
{
   if (!strcasecmp(x,"DAUB2")) {
     return DAUB2;
   } else if (!strcasecmp(x,"DAUB4")) { 
     return DAUB4;
   } else if (!strcasecmp(x,"DAUB6")) { 
     return DAUB6;
   } else if (!strcasecmp(x,"DAUB8")) { 
     return DAUB8;
   } else if (!strcasecmp(x,"DAUB10")) { 
     return DAUB10;
   } else if (!strcasecmp(x,"DAUB12")) { 
     return DAUB12;
   } else if (!strcasecmp(x,"DAUB14")) { 
     return DAUB14;
   } else if (!strcasecmp(x,"DAUB16")) { 
     return DAUB16;
   } else if (!strcasecmp(x,"DAUB18")) { 
     return DAUB18;
   } else if (!strcasecmp(x,"DAUB20")) { 
     return DAUB20;
   } else {
     fprintf(stderr,"sample_static_streaming_test: Unknown wavelet type\n");
     exit(-1);
   }
}

int main(int argc, char *argv[])
{
  if (argc!=6) {
    usage();
    exit(-1);
  }

  ifstream infile(argv[1]);
  if (!infile) {
    cerr << "Cannot open input file " << argv[1] << ".\n";
    usage();
    exit(-1);
  }

  WaveletType wt = GetWaveletType(argv[2]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "Number of stages must be positive.\n";
    usage();
    exit(-1);
  }

  if (toupper(argv[4][0])!='T') {
    cerr << "For streaming tests, only TRANSFORM type allowed.\n";
    usage();
    exit(-1);
  }

  ostream *prevstr;
  ofstream outfile;
  if (!strcasecmp(argv[5],"stdout")) {
    prevstr = cin.tie(&cout);
  } else if (!strcasecmp(argv[5],"stderr")) {
    prevstr = cin.tie(&cerr);
  } else {
    outfile.open(argv[5]);
    if (!outfile) {
      cerr << "Cannot open output file " << argv[5] << ".\n";
      usage();
      exit(-1);
    }
    prevstr = cin.tie(&outfile);
  }

  typedef WaveletInputSample<double> wisd;
  typedef WaveletOutputSample<double> wosd;

  // Read the data from file into an input vector
  vector<wisd> samples;
  double sample;
  unsigned index=0;
  while (infile >> sample) {
    wisd wavesample;
    wavesample.SetSampleValue(sample);
    wavesample.SetSampleIndex(index++);
    samples.push_back(wavesample);
  }
  infile.close();

  // Instantiate a static forward wavelet transform
  StaticForwardWaveletTransform<double, wosd, wisd> sfwt(numstages,wt,2,2,0);

  // Parameterize and instantiate the delay block
  unsigned wtcoefnum = numberOfCoefs[wt];
  int *delay = new int[numstages+1];
  CalculateWaveletDelayBlock(wtcoefnum, numstages+1, delay);
  DelayBlock<wosd> dlyblk(numstages+1, 0, delay);

  // Instantiate a static forward wavelet transform
  StaticReverseWaveletTransform<double, wisd, wosd> srwt(numstages,wt,2,2,0);

  // Create result buffers
  vector<wosd> outsamples;
  vector<wosd> delaysamples;
  vector<wisd> finaloutput;
  vector<wisd> outsamp;

  for (unsigned i=0; i<samples.size(); i++) {
    sfwt.StreamingTransformSampleOperation(outsamples, samples[i]);
    dlyblk.StreamingSampleOperation(delaysamples, outsamples);
    if (srwt.StreamingTransformSampleOperation(outsamp, delaysamples)) {
      for (unsigned j=0; j<outsamp.size(); j++) {
	finaloutput.push_back(outsamp[j]);
      }
    }

    outsamp.clear();
    outsamples.clear();
    delaysamples.clear();
  }

  for (unsigned i=0; i<MIN(finaloutput.size(), samples.size()); i++) {
    *cin.tie() << i << "\t" << samples[i].GetSampleValue() << "\t"
	    << finaloutput[i].GetSampleValue() << endl;
  }
  *cin.tie() << endl;
  cin.tie(prevstr);

  // Calculate the error between input and output
  

  if (delay != 0) {
    delete[] delay;
    delay=0;
  }

  return 0;
}