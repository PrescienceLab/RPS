#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <limits.h>

#include "banner.h"
#include "waveletsample.h"
#include "waveletsampleblock.h"
#include "transforms.h"
#include "delay.h"

void usage()
{
  char *tb=GetTsunamiBanner();
  char *b=GetRPSBanner();

  cerr << " sample_static_mixed_sfwt [input-file] [wavelet-type-init]\n";
  cerr << "  [numstages-init] [output-file] [specification-file]\n";
  cerr << "  [flat]\n\n";
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
  cerr << "[output-file]        = Which file to write the output.  This may\n";
  cerr << "                       also be stdout or stderr.\n\n";
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

void ParseSignalSpec(SignalSpec &spec, ifstream &file)
{
  const char pound = '#';
  const char approx = 'A';
  const char detail = 'D';
  const char space = ' ';
  char c;

  unsigned i;
  unsigned numlevels;
  int levelnum;

  while ( (c=file.get()) != EOF) {
    if (c == pound) {
      file.ignore(SHRT_MAX, '\n');
    } else if (c == approx) {
      file.ignore(SHRT_MAX, space);
      file >> numlevels;
      for (i=0; i<numlevels; i++) {
	file >> levelnum;
	spec.approximations.push_back(levelnum);
      }
    } else if (c == detail) {
      file.ignore(SHRT_MAX, space);
      file >> numlevels;
      for (i=0; i<numlevels; i++) {
	file >> levelnum;
	spec.details.push_back(levelnum);
      }
    } else if (c == space) {
      file.ignore(SHRT_MAX, '\n');
    }
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
      cerr << "sample_static_mixed_sfwt: Cannot open input file " << argv[1] << ".\n";
      exit(-1);
    }
    cin = infile;
  }

  WaveletType wt = GetWaveletType(argv[2], argv[0]);

  int numstages = atoi(argv[3]);
  if (numstages <= 0) {
    cerr << "sample_static_mixed_sfwt: Number of stages must be positive.\n";
    exit(-1);
  }
  unsigned numlevels = numstages + 1;

  ostream outstr;
  ofstream outfile;
  if (!strcasecmp(argv[4],"stdout")) {
    outstr.tie(&cout);
  } else if (!strcasecmp(argv[4],"stderr")) {
    outstr.tie(&cerr);
  } else {
    outfile.open(argv[4]);
    if (!outfile) {
      cerr << "sample_static_mixed_sfwt: Cannot open output file " << argv[5] << ".\n";
      exit(-1);
    }
    outstr.tie(&outfile);
  }

  ifstream specfile;
  specfile.open(argv[5]);
  if (!specfile) {
    cerr << "sample_static_mixed_sfwt: Cannot open specification file " << argv[5] << ".\n";
    exit(-1);
  }

  bool flat=true;
  if (toupper(argv[6][0])=='N') {
    flat = false;
  } else if (toupper(argv[6][0])!='F') {
    cerr << "sample_static_mixed_sfwt: Need to choose flat or noflat for human readable.\n";
    exit(-1);
  }

  SignalSpec sigspec;
  ParseSignalSpec(sigspec, specfile);
  specfile.close();

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
  vector<wosd> approxout;
  vector<wosd> detailout;

  // Create vectors for the level outputs
  vector<deque<wosd> *> approxlevels;
  vector<deque<wosd> *> detaillevels;
  deque<wosd>* pwos;
  for (i=0; i<numlevels; i++) {
    pwos = new deque<wosd>();
    approxlevels.push_back(pwos);

    pwos = new deque<wosd>();
    detaillevels.push_back(pwos);
  }

  for (i=0; i<samples.size(); i++) {
    sfwt.StreamingMixedSampleOperation(approxout, detailout, samples[i], sigspec);

    if (flat) {
      *outstr.tie() << i << "\t" << "A\t" << approxout.size() << "\t";
    }

    for (unsigned j=0; j<approxout.size(); j++) {
      int samplelevel = approxout[j].GetSampleLevel();
      approxlevels[samplelevel]->push_front(approxout[j]);
      if (flat) {
	*outstr.tie() << samplelevel << " " << approxout[j].GetSampleValue() << "\t";
      }
    }

    if (flat) {
      *outstr.tie() << endl << i << "\t" << "D\t" << detailout.size() << "\t";
    }

    for (unsigned j=0; j<detailout.size(); j++) {
      int samplelevel = detailout[j].GetSampleLevel();
      detaillevels[samplelevel]->push_front(detailout[j]);
      if (flat) {
	*outstr.tie() << samplelevel << " " << detailout[j].GetSampleValue() << "\t";
      }
    }
    if (flat) {
      *outstr.tie() << endl;
    }

    detailout.clear();
    approxout.clear();
  }
  numlevels -= 1;

  // Human readable output
  if (!flat) {
    *outstr.tie() << "The size of each approximation level:" << endl;
    for (i=0; i<numlevels; i++) {
      *outstr.tie() << "\tLevel " << i << " size = " << approxlevels[i]->size() << endl;
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

    unsigned loopsize=0;
    for (i=0; i<numlevels; i++) {
      if (approxlevels[i]->size() > loopsize) {
	loopsize = approxlevels[i]->size();
      }
    }

    for (i=0; i<loopsize; i++) {
      *outstr.tie() << i << "\t";

      for (unsigned j=0; j<numlevels; j++) {
	if (!approxlevels[j]->empty()) {
	  wosd wos;
	  wos = approxlevels[j]->back();
	  *outstr.tie() << wos.GetSampleLevel() << " " << wos.GetSampleValue() << "\t";
	  approxlevels[j]->pop_back();
	}
      }
      *outstr.tie() << endl;
    }

    *outstr.tie() << "The size of each detail level:" << endl;
    for (i=0; i<numlevels; i++) {
      *outstr.tie() << "\tLevel " << i << " size = " << detaillevels[i]->size() << endl;
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

    loopsize=0;
    for (i=0; i<numlevels; i++) {
      if (detaillevels[i]->size() > loopsize) {
	loopsize = detaillevels[i]->size();
      }
    }

    for (i=0; i<loopsize; i++) {
      *outstr.tie() << i << "\t";

      for (unsigned j=0; j<numlevels; j++) {
	if (!detaillevels[j]->empty()) {
	  wosd wos;
	  wos = detaillevels[j]->back();
	  *outstr.tie() << wos.GetSampleLevel() << " " << wos.GetSampleValue() << "\t";
	  detaillevels[j]->pop_back();
	}
      }
      *outstr.tie() << endl;
    }
  }
  
  for (i=0; i<numlevels; i++) {
    CHK_DEL(approxlevels[i]);
    CHK_DEL(detaillevels[i]);
  }
  approxlevels.clear();
  detaillevels.clear();

  return 0;
}
