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
  char *tb=GetTsunamiBanner();
  char *b=GetRPSBanner();

  cerr << " block_static_sfwt [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [transform-type] [output-file] [flat]\n\n";
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

WaveletType GetWaveletType(const char *x, const char *filename)
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
     fprintf(stderr,"%s: Unknown wavelet type\n", filename);
     exit(-1);
   }
}

int main(int argc, char *argv[])
{
  if (argc!=7) {
    usage();
    exit(-1);
  }

  ifstream infile;
  if (!strcasecmp(argv[1],"stdin")) {
  } else {
    infile.open(argv[1]);
    if (!infile) {
      cerr << "block_static_sfwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    cin = infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "block_static_sfwt: Number of stages must be positive.\n";
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
    cerr << "block_static_sfwt: Invalid transform type.  Choose APPROX | DETAIL | TRANSFORM.\n";
    usage();
    exit(-1);
  }

  ostream outstr;
  ofstream outfile;
  if (!strcasecmp(argv[5],"stdout")) {
    outstr.tie(&cout);
  } else if (!strcasecmp(argv[5],"stderr")) {
    outstr.tie(&cerr);
  } else {
    outfile.open(argv[5]);
    if (!outfile) {
      cerr << "block_static_sfwt: Cannot open output file " << argv[5] << ".\n";
      exit(-1);
    }
    outstr.tie(&outfile);
  }

  bool flat=true;
  if (toupper(argv[6][0])=='N') {
    flat = false;
  } else if (toupper(argv[6][0])!='F') {
    cerr << "block_static_sfwt: Need to choose flat or noflat for human readable.\n";
    exit(-1);
  }

  unsigned i;
  typedef WaveletInputSample<double> wisd;
  typedef WaveletOutputSample<double> wosd;

  // Read the data from file into an input vector
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
  StaticForwardWaveletTransform<double, wosd, wisd> sfwt(numstages,wt,2,2,0);

  // Create result buffers
  vector<WaveletOutputSampleBlock<wosd> > forwardoutput;

  switch(tt) {
  case APPROX: {
    sfwt.StreamingApproxBlockOperation(forwardoutput, inputblock);
    numlevels -= 1;
    break;
  }
  case DETAIL: {
    sfwt.StreamingDetailBlockOperation(forwardoutput, inputblock);
    numlevels -= 1;
    break;
  }
  case TRANSFORM: {
    sfwt.StreamingTransformBlockOperation(forwardoutput, inputblock);
    break;
  }
  default:
    break;
  }

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

    unsigned loopsize = forwardoutput[0].GetBlockSize();
    for (i=0; i<loopsize; i++) {
      *outstr.tie() << i << "\t";

      for (unsigned j=0; j<numlevels; j++) {
	if (!forwardoutput[j].Empty()) {
	  wosd wos;
	  wos = forwardoutput[j].Front();
	  *outstr.tie() << wos.GetSampleValue() << "\t";
	  levels[j]->pop_back();
	}
      }
      *outstr.tie() << endl;
    }
  }
  
  for (i=0; i<numlevels; i++) {
    CHK_DEL(levels[i]);
  }
  levels.clear();

  return 0;
}
