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

  cerr << " sample_static_sfwt [input-file] [wavelet-type-init]\n";
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
  cerr << "                      APPROX | DETAIL | TRANSFORM for this\n";
  cerr << "                      test.\n";
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
     fprintf(stderr,"sample_static_sfwt: Unknown wavelet type\n");
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
      cerr << "Cannot open input file " << argv[1] << ".\n";
      usage();
      exit(-1);
    }
    cin = infile;
  }

  WaveletType wt = GetWaveletType(argv[2]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "Number of stages must be positive.\n";
    usage();
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
    cerr << "For streaming tests, only TRANSFORM type allowed.\n";
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
      cerr << "Cannot open output file " << argv[5] << ".\n";
      usage();
      exit(-1);
    }
    outstr.tie(&outfile);
  }

  bool flat=true;
  if (toupper(argv[6][0])=='N') {
    flat = false;
  } else if (toupper(argv[6][0])!='F') {
    cerr << "Need to choose flat or noflat for human readable.\n";
    usage();
    exit(-1);
  }


  unsigned i;
  typedef WaveletInputSample<double> wisd;
  typedef WaveletOutputSample<double> wosd;

  // Read the data from file into an input vector
  vector<wisd> samples;
  double sample;
  unsigned index=0;
  while (cin >> sample) {
    wisd wavesample;
    wavesample.SetSampleValue(sample);
    wavesample.SetSampleIndex(index++);
    samples.push_back(wavesample);
  }
  infile.close();

  // Instantiate a static forward wavelet transform
  StaticForwardWaveletTransform<double, wosd, wisd> sfwt(numstages,wt,2,2,0);

  // Create result buffers
  vector<wosd> outsamples;

  // Create vectors for the level outputs
  vector<deque<wosd> *> levels;
  for (i=0; i<numlevels; i++) {
    deque<wosd>* pwos = new deque<wosd>();
    levels.push_back(pwos);
  }

  switch(tt) {
  case APPROX: {
    for (i=0; i<samples.size(); i++) {
      sfwt.StreamingApproxSampleOperation(outsamples, samples[i]);

      for (unsigned j=0; j<outsamples.size(); j++) {
	int samplelevel = outsamples[j].GetSampleLevel();
	levels[samplelevel]->push_front(outsamples[j]);
      }

      outsamples.clear();
    }
    numlevels -= 1;
  }
  break;

  case DETAIL: {
    for (i=0; i<samples.size(); i++) {
      sfwt.StreamingDetailSampleOperation(outsamples, samples[i]);

      for (unsigned j=0; j<outsamples.size(); j++) {
	int samplelevel = outsamples[j].GetSampleLevel();
	levels[samplelevel]->push_front(outsamples[j]);
      }

      outsamples.clear();
    }
    numlevels -= 1;
  }
  break;

  case TRANSFORM: {
    for (i=0; i<samples.size(); i++) {
      sfwt.StreamingTransformSampleOperation(outsamples, samples[i]);

      for (unsigned j=0; j<outsamples.size(); j++) {
	int samplelevel = outsamples[j].GetSampleLevel();
	levels[samplelevel]->push_front(outsamples[j]);
      }

      outsamples.clear();
    }
  }
  break;

  default:
    break;
  }

  // Human readable output
  if (!flat) {
    *outstr.tie() << "The size of each level:" << endl;
    for (i=0; i<numlevels; i++) {
      *outstr.tie() << "\tLevel " << i << " size = " << levels[i]->size() << endl;
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

    unsigned loopsize = levels[0]->size();
    for (i=0; i<loopsize; i++) {
      *outstr.tie() << i << "\t";

      for (unsigned j=0; j<numlevels; j++) {
	if (!levels[j]->empty()) {
	  wosd wos;
	  wos = levels[j]->back();
	  *outstr.tie() << wos.GetSampleValue() << "\t";
	  levels[j]->pop_back();
	}
      }
      *outstr.tie() << endl;
    }
  } else {  // Machine readable output
    *outstr.tie() << "Level     Size     Coefs (in order)" << endl;
    for (i=0; i<numlevels; i++) {
      *outstr.tie() << i << "\t" << levels[i]->size() << "\t";
      for (unsigned j=0; j<levels[i]->size(); j++) {
	wosd wos;
	wos = levels[i]->back();
	*outstr.tie() << wos.GetSampleValue() << "\t";
	levels[i]->pop_back();
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
