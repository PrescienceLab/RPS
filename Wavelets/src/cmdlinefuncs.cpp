#include "cmdlinefuncs.h"

#if defined(WIN32) && !defined(__CYGWIN__)
#define strcasecmp stricmp
#endif

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

void ParseZeroSpec(vector<int> &spec, ifstream &file)
{
  const char pound = '#';
  const char space = ' ';
  const char zero = 'Z';
  char c;

  unsigned i;
  unsigned numlevels;
  int levelnum;

  while ( (c=file.get()) != EOF) {
    if (c == pound) {
      file.ignore(SHRT_MAX, '\n');
    } else if (c == zero) {
      file.ignore(SHRT_MAX, space);
      file >> numlevels;
      for (i=0; i<numlevels; i++) {
	file >> levelnum;
	spec.push_back(levelnum);
      }
    }
  }
}

void OutputWaveletCoefsNonFlat(ostream &os,
			       const vector<deque<wosd> *> &levels,
			       const unsigned numlevels)
{
  unsigned i;

  os << "The size of each level:" << endl;
  for (i=0; i<numlevels; i++) {
    os << "\tLevel " << i << " size = " << levels[i]->size() << endl;
  }
  os << endl;
  
  os << "Index     ";
  for (i=0; i<numlevels; i++) {
    os << "Level " << i << "        " ;
  }
  os << endl << "-----     ";
  for (i=0; i<numlevels; i++) {
    os << "-------        ";
  }
  os << endl;
  
  unsigned loopsize = levels[0]->size();
  for (i=0; i<loopsize; i++) {
    os << i << "\t";
    
    for (unsigned j=0; j<numlevels; j++) {
      if (!levels[j]->empty()) {
	wosd wos;
	wos = levels[j]->back();
	os << wos.GetSampleValue() << "\t";
	levels[j]->pop_back();
      }
    }
    os << endl;
  }
}

void OutputWaveletCoefsNonFlat(ostream &os,
			       vector<WaveletOutputSampleBlock<wosd> > &fwdout,
			       const unsigned numlevels)
{
  unsigned i;
  os << "The size of each level:" << endl;
  for (i=0; i<numlevels; i++) {
    os << "\tLevel " << i << " size = " 
	      << fwdout[i].GetBlockSize() << endl;
  }
  os << endl;

  os << "Index     ";
  for (i=0; i<numlevels; i++) {
    os << "Level " << i << "        " ;
  }
  os << endl << "-----     ";
  for (i=0; i<numlevels; i++) {
    os << "-------        ";
  }
  os << endl;

  unsigned loopsize = fwdout[0].GetBlockSize();
  for (i=0; i<loopsize; i++) {
    os << i << "\t";

    // Find number of samples for this line
    unsigned numsamples=0;
	unsigned j;
    for (j=0; j<numlevels; j++) {
      if (!fwdout[j].Empty()) {
	numsamples++;
      }
    }

    for (j=0; j<numsamples; j++) {
      if (!fwdout[j].Empty()) {
	wosd wos;
	wos = fwdout[j].Front();

	os << wos.GetSampleValue() << "\t";
	fwdout[j].PopSampleFront();
      }
    }
    os << endl;
  }
}

void OutputWaveletCoefsFlat(ostream &os,
			    vector<WaveletOutputSampleBlock<wosd> > &fwdout,
			    const unsigned numlevels)
{
  unsigned loopsize = fwdout[0].GetBlockSize();
  for (unsigned i=0; i<loopsize; i++) {
    os << i << "\t";

    // Find number of samples for this line
    unsigned numsamples=0;
	unsigned j;
    for (j=0; j<numlevels; j++) {
      if (!fwdout[j].Empty()) {
	numsamples++;
      }
    }

    os << numsamples << "\t";

    for (j=0; j<numsamples; j++) {
      if (!fwdout[j].Empty()) {
	wosd wos;
	wos = fwdout[j].Front();
	os << wos.GetSampleLevel() << " ";
	os << wos.GetSampleValue() << "\t";
	fwdout[j].PopSampleFront();
      }
    }
    os << endl;
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
    os << i << "\t";

    // Find number of samples for this line
    unsigned numsamples=0;
	unsigned j;
    for (j=0; j<mraout.size(); j++) {
      if (!mraout[j].Empty()) {
	numsamples++;
      }
    }

    os << ch << " " << numsamples << "\t";

    for (j=0; j<numsamples; j++) {
      if (!mraout[j].Empty()) {
	wosd wos;
	wos = mraout[j].Front();
	os << wos.GetSampleLevel() << " ";
	os << wos.GetSampleValue() << "\t";
	mraout[j].PopSampleFront();
      }
    }
    os << endl;
  }
}

//------------------------------------------------------------------------------

void OutputWaveletCoefs(ostream &os, vector<vector<wosd> > &levels)
{
  for (unsigned i=0; i<levels.size(); i++) {
    os << i << "\t" << levels[i].size() << "\t";
    vector<wosd> &sampleout = levels[i];
    for (unsigned j=0; j<sampleout.size(); j++) {
      os << sampleout[j].GetSampleLevel() << " "
		<< sampleout[j].GetSampleValue() << "\t";
    }
    os << endl;
  }
}

void OutputWaveletCoefs(ostream &os,
			vector<WaveletOutputSampleBlock<wosd> > &levels,
			const TransformType tt)
{
  unsigned i, j, k;
  unsigned numlevels=levels.size();

  // Find maximum blocksize and level
  unsigned maxblock=0;
  unsigned level=0;
  for (i=0; i<numlevels; i++) {
    if (levels[i].GetBlockSize() > maxblock) {
      maxblock = levels[i].GetBlockSize();
      level = i;
    }
  }
  
  vector<unsigned> indices;
  for (i=0; i<maxblock*(2<<level); i++) {
    os << i << "\t";

    unsigned numsamples=0;
    if (tt==TRANSFORM) {
      for (j=0; j<numlevels-1; j++) {
	if (!levels[j].Empty() && (i % (2 << j)) == 0) {
	  numsamples++;
	  indices.push_back(j);

	  if (j==numlevels-2) {
	    numsamples++;
	    indices.push_back(numlevels-1);
	  }
	}
      }
    } else {
      for (j=0; j<numlevels; j++) {
	if (!levels[j].Empty() && (i % (2 << j)) == 0) {
	  numsamples++;
	  indices.push_back(j);
	}
      }
    }

    os << numsamples << "\t";
    for (k=0; k<indices.size(); k++) {
      wosd wos;
      wos = levels[indices[k]].Front();
	os << wos.GetSampleLevel() << " ";
	os << wos.GetSampleValue() << "\t";
	levels[indices[k]].PopSampleFront();
    }
    os << endl;
    indices.clear();
  }
}

void OutputWaveletCoefs(ostream &os,
			const DiscreteWaveletOutputSampleBlock<wosd> &dwosb,
			const TransformType tt,
			const bool flat)
{
  unsigned i, j;
  unsigned numlevels=dwosb.GetNumberLevels();
  int lowlevel=dwosb.GetLowestLevel();

  for (i=0; i<numlevels; i++) {
    int plevel=lowlevel+numlevels-1-i;
    if (!flat) {
      os << endl;
      os << "Level " << plevel << endl;
      os << "---------\n";
    }
    deque<wosd> leveldata;
    dwosb.GetSamplesAtLevel(leveldata, plevel);
    for (j=0; j<leveldata.size(); j++) {
      os << plevel << " " << leveldata[j].GetSampleValue() << endl;
    }
  }
}


unsigned OutputWaveletCoefs(ostream &os,
			    vector<WaveletOutputSampleBlock<wosd> > &levels,
			    const TransformType tt,
			    const unsigned start_index)
{
  unsigned i, j, k;
  unsigned numlevels=levels.size();

  // Find maximum blocksize and level
  unsigned maxblock=0;
  unsigned level=0;
  for (i=0; i<numlevels; i++) {
    if (levels[i].GetBlockSize() > maxblock) {
      maxblock = levels[i].GetBlockSize();
      level = i;
    }
  }
  
  vector<unsigned> indices;
  for (i=0; i<maxblock*(2<<level); i++) {
    os << i + start_index << "\t";

    unsigned numsamples=0;
    if (tt==TRANSFORM) {
      for (j=0; j<numlevels-1; j++) {
	if (!levels[j].Empty() && (i % (2 << j)) == 0) {
	  numsamples++;
	  indices.push_back(j);

	  if (j==numlevels-2) {
	    numsamples++;
	    indices.push_back(numlevels-1);
	  }
	}
      }
    } else {
      for (j=0; j<numlevels; j++) {
	if (!levels[j].Empty() && (i % (2 << j)) == 0) {
	  numsamples++;
	  indices.push_back(j);
	}
      }
    }

    os << numsamples << "\t";
    for (k=0; k<indices.size(); k++) {
      wosd wos;
      wos = levels[indices[k]].Front();
	os << wos.GetSampleLevel() << " ";
	os << wos.GetSampleValue() << "\t";
	levels[indices[k]].PopSampleFront();
    }
    os << endl;
    indices.clear();
  }
  return i + start_index;
}

void OutputMRACoefs(ostream &os,
		    vector<vector<wosd> > &approxlevels,
		    vector<vector<wosd> > &detaillevels)
{
  for (unsigned i=0; i<MAX(approxlevels.size(), detaillevels.size()); i++) {
    if (i < approxlevels.size()) {
      os << endl << i << "\tA\t" << approxlevels[i].size();
      vector<wosd> &approxout = approxlevels[i];
      for (unsigned j=0; j<approxout.size(); j++) {
	os << "\t" << approxout[j].GetSampleLevel() << " "
		  << approxout[j].GetSampleValue();
      }
    }

    if (i < detaillevels.size()) {
      os << endl << i << "\tD\t" << detaillevels[i].size();
      vector<wosd> &detailout = detaillevels[i];
      for (unsigned j=0; j<detailout.size(); j++) {
	os << "\t" << detailout[j].GetSampleLevel() << " "
		  << detailout[j].GetSampleValue();
      }
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
  unsigned level=0;
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

    os << i << "\tA\t" << numsamples << "\t";
    for (k=0; k<indices.size(); k++) {
      wosd wos;
      wos = approx[indices[k]].Front();
      os << wos.GetSampleLevel() << " ";
      os << wos.GetSampleValue() << "\t";
      approx[indices[k]].PopSampleFront();
    }
    os << endl;
    indices.clear();

    // DETAILS
    numsamples=0;
    for (j=0; j<dlevels; j++) {
      if (!detail[j].Empty() && (i % (2 << j)) == 0) {
	numsamples++;
	indices.push_back(j);
      }
    }

    os << i << "\tD\t" << numsamples << "\t";
    for (k=0; k<indices.size(); k++) {
      wosd wos;
      wos = detail[indices[k]].Front();
      os << wos.GetSampleLevel() << " ";
      os << wos.GetSampleValue() << "\t";
      detail[indices[k]].PopSampleFront();
    }
    os << endl;
    indices.clear();
  }
}

unsigned OutputMRACoefs(ostream &os,
			vector<WaveletOutputSampleBlock<wosd> > &approx,
			vector<WaveletOutputSampleBlock<wosd> > &detail,
			const unsigned index)
{
  unsigned i, j, k;
  unsigned alevels=approx.size();
  unsigned dlevels=detail.size();

  // Find maximum blocksize and level
  unsigned maxblock=0;
  unsigned level=0;
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

    os << i+index << "\tA\t" << numsamples << "\t";
    for (k=0; k<indices.size(); k++) {
      wosd wos;
      wos = approx[indices[k]].Front();
      os << wos.GetSampleLevel() << " ";
      os << wos.GetSampleValue() << "\t";
      approx[indices[k]].PopSampleFront();
    }
    os << endl;
    indices.clear();

    // DETAILS
    numsamples=0;
    for (j=0; j<dlevels; j++) {
      if (!detail[j].Empty() && (i % (2 << j)) == 0) {
	numsamples++;
	indices.push_back(j);
      }
    }

    os << i+index << "\tD\t" << numsamples << "\t";
    for (k=0; k<indices.size(); k++) {
      wosd wos;
      wos = detail[indices[k]].Front();
      os << wos.GetSampleLevel() << " ";
      os << wos.GetSampleValue() << "\t";
      detail[indices[k]].PopSampleFront();
    }
    os << endl;
    indices.clear();
  }
  return i+index;
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

  os << "The size of each level:" << endl;
  for (i=0; i<numlevels; i++) {
    os << "\tLevel " << i << " size = " 
	      << sizes[i] << endl;
  }
  os << endl;
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

  os << "The size of each level:" << endl;
  for (i=0; i<levels.size(); i++) {
    size[levels[i].GetBlockLevel()] = levels[i].GetBlockSize();
  }

  for (i=0; i<numlevels; i++) {
    os << "\tLevel " << i << " size = " 
	      << size[i] << endl;
  }
  os << endl;
}

void OutputLevelMetaData(ostream &os,
			 const unsigned *levelsize,
			 const unsigned levelcnt)
{
  unsigned i;

  os << "The size of each level:" << endl;
  for (i=0; i<levelcnt; i++) {
    os << "\tLevel " << i << " size = " 
	      << levelsize[i] << endl;
  }
  os << endl;
}

void OutputLevelMetaData(ostream &os,
			 const DiscreteWaveletOutputSampleBlock<wosd> &dwosb,
			 const TransformType tt)
{
  int i;
  unsigned levels=dwosb.GetNumberLevels();
  unsigned M=levels-1;

  os << "The size of each level:" << endl;
    
  for (i=M; i>=0; i--) {
    unsigned size=1;
    if ((unsigned)i!= M) {
      size = 0x1 << ((tt!=TRANSFORM) ? (M-i) : (M-i-1));
    }

    os << "\tLevel " << i << " size = " 
	      << size << endl;
  }
  os << endl;
}

// Performance

void GetRusage(double &systime, double &usrtime)
{
#if defined(WIN32) && !defined(__CYGWIN__)
	systime=0;
	usrtime=0;
#else 
  struct rusage x;

  if (getrusage(RUSAGE_SELF,&x)) {
    perror("getrusage");
    exit(-1);
  }

  systime=x.ru_stime.tv_sec + x.ru_stime.tv_usec/1.0e6;
  usrtime=x.ru_utime.tv_sec + x.ru_utime.tv_usec/1.0e6;
#endif
}

double GetTimeDuration(const timeval &stime, const timeval &etime)
{
  double start = ((double)(stime.tv_sec)) + ((double)(stime.tv_usec)/1e6);
  double end = ((double)(etime.tv_sec)) + ((double)(etime.tv_usec)/1e6);
  return start - end;
}

void GetNextBlock(vector<WaveletOutputSampleBlock<wosd> > &outblock,
		  const unsigned blocknumber,
		  const vector<WaveletOutputSampleBlock<wosd> > &inblock,
		  const int approxlevel,
		  const unsigned blocksize)
{
  outblock.clear();
  for (unsigned i=0; i<inblock.size(); i++) {
    int lvl = inblock[i].GetBlockLevel();
    unsigned size = (lvl == approxlevel) ?
      (blocksize >> (lvl)) :
      (blocksize >> (1+lvl));

    unsigned start_indx = blocknumber*size;
    deque<wosd> levelbuf;
    inblock[i].GetSamples(levelbuf, start_indx, start_indx+size);

    WaveletOutputSampleBlock<wosd> block(levelbuf, start_indx);
    block.SetBlockLevel(lvl);

    outblock.push_back(block);
  }
}
