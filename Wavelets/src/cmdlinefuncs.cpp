#include "cmdlinefuncs.h"

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
    }
  }
}

void OutputWaveletCoefsNonFlat(ostream &os,
			       const vector<deque<wosd> *> &levels,
			       const unsigned numlevels)
{
  unsigned i;

  *os.tie() << "The size of each level:" << endl;
  for (i=0; i<numlevels; i++) {
    *os.tie() << "\tLevel " << i << " size = " << levels[i]->size() << endl;
  }
  *os.tie() << endl;
  
  *os.tie() << "Index     ";
  for (i=0; i<numlevels; i++) {
    *os.tie() << "Level " << i << "        " ;
  }
  *os.tie() << endl << "-----     ";
  for (i=0; i<numlevels; i++) {
    *os.tie() << "-------        ";
  }
  *os.tie() << endl;
  
  unsigned loopsize = levels[0]->size();
  for (i=0; i<loopsize; i++) {
    *os.tie() << i << "\t";
    
    for (unsigned j=0; j<numlevels; j++) {
      if (!levels[j]->empty()) {
	wosd wos;
	wos = levels[j]->back();
	*os.tie() << wos.GetSampleValue() << "\t";
	levels[j]->pop_back();
      }
    }
    *os.tie() << endl;
  }
}

void OutputWaveletCoefsNonFlat(ostream &os,
			       vector<WaveletOutputSampleBlock<wosd> > &fwdout,
			       const unsigned numlevels)
{
  unsigned i;
  *os.tie() << "The size of each level:" << endl;
  for (i=0; i<numlevels; i++) {
    *os.tie() << "\tLevel " << i << " size = " 
	      << fwdout[i].GetBlockSize() << endl;
  }
  *os.tie() << endl;

  *os.tie() << "Index     ";
  for (i=0; i<numlevels; i++) {
    *os.tie() << "Level " << i << "        " ;
  }
  *os.tie() << endl << "-----     ";
  for (i=0; i<numlevels; i++) {
    *os.tie() << "-------        ";
  }
  *os.tie() << endl;

  unsigned loopsize = fwdout[0].GetBlockSize();
  for (i=0; i<loopsize; i++) {
    *os.tie() << i << "\t";

    // Find number of samples for this line
    unsigned numsamples=0;
    for (unsigned j=0; j<numlevels; j++) {
      if (!fwdout[j].Empty()) {
	numsamples++;
      }
    }

    for (unsigned j=0; j<numsamples; j++) {
      if (!fwdout[j].Empty()) {
	wosd wos;
	wos = fwdout[j].Front();

	*os.tie() << wos.GetSampleValue() << "\t";
	fwdout[j].PopSampleFront();
      }
    }
    *os.tie() << endl;
  }
}

void OutputWaveletCoefsFlat(ostream &os,
			    vector<WaveletOutputSampleBlock<wosd> > &fwdout,
			    const unsigned numlevels)
{
  unsigned loopsize = fwdout[0].GetBlockSize();
  for (unsigned i=0; i<loopsize; i++) {
    *os.tie() << i << "\t";

    // Find number of samples for this line
    unsigned numsamples=0;
    for (unsigned j=0; j<numlevels; j++) {
      if (!fwdout[j].Empty()) {
	numsamples++;
      }
    }

    *os.tie() << numsamples << "\t";

    for (unsigned j=0; j<numsamples; j++) {
      if (!fwdout[j].Empty()) {
	wosd wos;
	wos = fwdout[j].Front();
	*os.tie() << wos.GetSampleLevel() << " ";
	*os.tie() << wos.GetSampleValue() << "\t";
	fwdout[j].PopSampleFront();
      }
    }
    *os.tie() << endl;
  }
}


void OutputMRACoefsFlat(ostream &os,
			vector<WaveletOutputSampleBlock<wosd> > &mraout,
			const char &ch,
			const unsigned numlevels)
{
  unsigned i;
  unsigned loopsize=0;
  for (i=0; i<mraout.size(); i++) {
    if (mraout[i].GetBlockSize() > loopsize) {
      loopsize = mraout[i].GetBlockSize();
    }
  }

  for (i=0; i<loopsize; i++) {
    *os.tie() << i << "\t";

    // Find number of samples for this line
    unsigned numsamples=0;
    for (unsigned j=0; j<mraout.size(); j++) {
      if (!mraout[j].Empty()) {
	numsamples++;
      }
    }

    *os.tie() << ch << " " << numsamples << "\t";

    for (unsigned j=0; j<numsamples; j++) {
      if (!mraout[j].Empty()) {
	wosd wos;
	wos = mraout[j].Front();
	*os.tie() << wos.GetSampleLevel() << " ";
	*os.tie() << wos.GetSampleValue() << "\t";
	mraout[j].PopSampleFront();
      }
    }
    *os.tie() << endl;
  }
}

//------------------------------------------------------------------------------

void OutputWaveletCoefs(ostream &os, vector<vector<wosd> > &levels)
{
  for (unsigned i=0; i<levels.size(); i++) {
    *os.tie() << i << "\t" << levels[i].size() << "\t";
    vector<wosd> &sampleout = levels[i];
    for (unsigned j=0; j<sampleout.size(); j++) {
      *os.tie() << sampleout[j].GetSampleLevel() << " "
		<< sampleout[j].GetSampleValue() << "\t";
    }
    *os.tie() << endl;
  }
}

void OutputWaveletCoefs(ostream &os,
			vector<WaveletOutputSampleBlock<wosd> > &levels)
{
  unsigned i, j, k;
  unsigned numlevels=levels.size();

  // Find maximum blocksize and level
  unsigned maxblock=0;
  unsigned level;
  for (i=0; i<numlevels; i++) {
    if (levels[i].GetBlockSize() > maxblock) {
      maxblock = levels[i].GetBlockSize();
      level = i;
    }
  }
  
  vector<unsigned> indices;
  for (i=0; i<maxblock*(2<<level); i++) {
    *os.tie() << i << "\t";

    unsigned numsamples=0;
    for (j=0; j<numlevels; j++) {
      if (!levels[j].Empty() && (i % (2 << j)) == 0) {
	numsamples++;
	indices.push_back(j);
      }
    }

    *os.tie() << numsamples << "\t";
    for (k=0; k<indices.size(); k++) {
      wosd wos;
      wos = levels[indices[k]].Front();
	*os.tie() << wos.GetSampleLevel() << " ";
	*os.tie() << wos.GetSampleValue() << "\t";
	levels[indices[k]].PopSampleFront();
    }
    *os.tie() << endl;
    indices.clear();
  }
}

void OutputMRACoefs(ostream &os,
		    vector<vector<wosd> > &approxlevels,
		    vector<vector<wosd> > &detaillevels)
{
  for (unsigned i=0; i<MAX(approxlevels.size(), detaillevels.size()); i++) {
    if (i < approxlevels.size()) {
      *os.tie() << i << "\tA\t" << approxlevels[i].size() << "\t";
      vector<wosd> &approxout = approxlevels[i];
      for (unsigned j=0; j<approxout.size(); j++) {
	*os.tie() << approxout[j].GetSampleLevel() << " "
		  << approxout[j].GetSampleValue() << "\t";
      }
      *os.tie() << endl;
    }

    if (i < detaillevels.size()) {
      *os.tie() << i << "\tD\t" << detaillevels[i].size() << "\t";
      vector<wosd> &detailout = detaillevels[i];
      for (unsigned j=0; j<detailout.size(); j++) {
	*os.tie() << detailout[j].GetSampleLevel() << " "
		  << detailout[j].GetSampleValue() << "\t";
      }
      *os.tie() << endl;
    }
  }
}

void OutputMRACoefs(ostream &os,
		    vector<WaveletOutputSampleBlock<wosd> > &approx,
		    vector<WaveletOutputSampleBlock<wosd> > &detail)
{
  unsigned i, j, k;
  unsigned alevels=approx.size();
  unsigned dlevels=detail.size();

  // Find maximum blocksize and level
  unsigned maxblock=0;
  unsigned level;
  for (i=0; i<alevels; i++) {
    if (approx[i].GetBlockSize() > maxblock) {
      maxblock = approx[i].GetBlockSize();
      level = i;
    }
  }
  for (i=0; i<dlevels; i++) {
    if (detail[i].GetBlockSize() > maxblock) {
      maxblock = detail[i].GetBlockSize();
      level = i;
    }
  }

  vector<unsigned> indices;
  for (i=0; i<maxblock*(2<<level); i++) {

    // APPROXIMATIONS
    unsigned numsamples=0;
    for (j=0; j<alevels; j++) {
      if (!approx[j].Empty() && (i % (2 << j)) == 0) {
	numsamples++;
	indices.push_back(j);
      }
    }

    *os.tie() << i << "\tA\t" << numsamples << "\t";
    for (k=0; k<indices.size(); k++) {
      wosd wos;
      wos = approx[indices[k]].Front();
      *os.tie() << wos.GetSampleLevel() << " ";
      *os.tie() << wos.GetSampleValue() << "\t";
      approx[indices[k]].PopSampleFront();
    }
    *os.tie() << endl;
    indices.clear();

    // DETAILS
    numsamples=0;
    for (j=0; j<dlevels; j++) {
      if (!detail[j].Empty() && (i % (2 << j)) == 0) {
	numsamples++;
	indices.push_back(j);
      }
    }

    *os.tie() << i << "\tD\t" << numsamples << "\t";
    for (k=0; k<indices.size(); k++) {
      wosd wos;
      wos = detail[indices[k]].Front();
      *os.tie() << wos.GetSampleLevel() << " ";
      *os.tie() << wos.GetSampleValue() << "\t";
      detail[indices[k]].PopSampleFront();
    }
    *os.tie() << endl;
    indices.clear();
  }
}


void OutputLevelMetaData(ostream &os,
			 vector<vector<wosd> > &levels,
			 const unsigned numlevels)
{
  unsigned i, j;
  vector<unsigned> sizes;
  for (i=0; i<numlevels; i++) {
    sizes.push_back(0);
  }

  for (i=0; i<levels.size(); i++) {
    vector<wosd> &sampleout = levels[i];
    for (j=0; j<sampleout.size(); j++) {
      sizes[sampleout[j].GetSampleLevel()]++;
    }
  }

  *os.tie() << "The size of each level:" << endl;
  for (i=0; i<numlevels; i++) {
    *os.tie() << "\tLevel " << i << " size = " 
	      << sizes[i] << endl;
  }
  *os.tie() << endl;
}

void OutputLevelMetaData(ostream &os,
			 vector<WaveletOutputSampleBlock<wosd> > &levels,
			 const unsigned numlevels)
{
  unsigned i;

  vector<unsigned> size;
  for (i=0; i<numlevels; i++) {
    size.push_back(0);
  }

  *os.tie() << "The size of each level:" << endl;
  for (i=0; i<levels.size(); i++) {
    size[levels[i].GetBlockLevel()] = levels[i].GetBlockSize();
  }

  for (i=0; i<numlevels; i++) {
    *os.tie() << "\tLevel " << i << " size = " 
	      << size[i] << endl;
  }
  *os.tie() << endl;
}
