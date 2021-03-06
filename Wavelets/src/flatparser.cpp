#include "flatparser.h"

FlatParser::FlatParser() {}
FlatParser::~FlatParser() {}

void FlatParser::ParseTimeDomain(vector<wisd> &samples, istream &in)
{
  double sample;
  unsigned index=0;
  while (in >> sample) {
    wisd wavesample;
    wavesample.SetSampleValue(sample);
    wavesample.SetSampleIndex(index++);
    samples.push_back(wavesample);
  }
}

void FlatParser::ParseTimeDomain(deque<wisd> &samples, istream &in)
{
  double sample;
  unsigned index=0;
  while (in >> sample) {
    wisd wavesample;
    wavesample.SetSampleValue(sample);
    wavesample.SetSampleIndex(index++);
    samples.push_back(wavesample);
  }
}

unsigned FlatParser::ParseTimeDomain(deque<wisd> &samples,
				     istream &in,
				     unsigned &index,
				     const unsigned parsenum)
{
  double sample;
  unsigned parsecount=0;
  while (in >> sample) {
    parsecount++;
    wisd wavesample;
    wavesample.SetSampleValue(sample);
    wavesample.SetSampleIndex(index++);
    samples.push_back(wavesample);
    if ((index+1) % parsenum == 0) {
      break;
    }
  }
  return parsecount;
}

bool FlatParser::ParseWaveletCoefsSample(vector<wosd> &wavecoefs, istream &in)
{
  bool read=true;
  unsigned sampletime, numsamples;
  int levelnum;
  double sampvalue;
  if (in >> sampletime) {
    in >> numsamples;
    for (unsigned i=0; i<numsamples; i++) {
      in >> levelnum >> sampvalue;
      if (indices.find(levelnum) == indices.end()) {
	indices[levelnum] = 0;
      } else {
	indices[levelnum] += 1;
      }
      wosd sample(sampvalue, levelnum, indices[levelnum]);
      wavecoefs.push_back(sample);
    }
  } else {
    read=false;
  }
  return read; 
}


void FlatParser::
ParseWaveletCoefsBlock(vector<WaveletOutputSampleBlock<wosd> > &wavecoefs,
		       istream &in)
{
  unsigned indextime, numsamples;
  int levelnum;
  double sampvalue;
  while(in >> indextime) {
    in >> numsamples;
    for (unsigned i=0; i<numsamples; i++) {
      in >> levelnum >> sampvalue;
      if (indices.find(levelnum) == indices.end()) {
	indices[levelnum] = 0;
      } else {
	indices[levelnum] += 1;
      }
      wosd sample(sampvalue, levelnum, indices[levelnum]);
      wavecoefs[levelnum].PushSampleBack(sample);
    }
  }
}

void FlatParser::
ParseWaveletCoefsBlock(DiscreteWaveletOutputSampleBlock<wosd> &wavecoefs,
		       istream &in)
{
  int levelnum, lowlevel=0;
  double sampvalue;
  while(in >> levelnum) {
    if (lowlevel > levelnum) {
      lowlevel = levelnum;
    }
    in >> sampvalue;
    if (indices.find(levelnum) == indices.end()) {
      indices[levelnum] = 0;
    } else {
      indices[levelnum] += 1;
    }
    wosd sample(sampvalue, levelnum, indices[levelnum]);
    wavecoefs.PushSampleBack(sample);
  }

  // Fill in meta data for the discrete block
  wavecoefs.SetLowestLevel(lowlevel);
  wavecoefs.SetNumberLevels(NumberOfLevels(wavecoefs.GetBlockSize())+1);
  wavecoefs.SetTransformType(TRANSFORM);
}

unsigned FlatParser::
ParseWaveletCoefsBlock(vector<WaveletOutputSampleBlock<wosd> > &wavecoefs,
		       istream &in,
		       const unsigned parsenum)
{
  unsigned indextime, numsamples;
  int levelnum;
  double sampvalue;
  unsigned parsecount=0;
  while(in >> indextime) {
    parsecount++;
    in >> numsamples;
    for (unsigned i=0; i<numsamples; i++) {
      in >> levelnum >> sampvalue;
      if (indices.find(levelnum) == indices.end()) {
	indices[levelnum] = 0;
      } else {
	indices[levelnum] += 1;
      }
      wosd sample(sampvalue, levelnum, indices[levelnum]);
      wavecoefs[levelnum].PushSampleBack(sample);
    }
    if ((indextime+1) % parsenum == 0) {
      break;
    }
  }
  return parsecount;
}

unsigned FlatParser::
ParseWaveletCoefsBlock(DiscreteWaveletOutputSampleBlock<wosd> &wavecoefs,
		       istream &in,
		       const unsigned parsenum)
{
  int levelnum, lowlevel=0;
  double sampvalue;
  unsigned parsecount=0;
  while(in >> levelnum) {
    if (lowlevel > levelnum) {
      lowlevel = levelnum;
    }
    in >> sampvalue;
    if (indices.find(levelnum) == indices.end()) {
      indices[levelnum] = 0;
    } else {
      indices[levelnum] += 1;
    }
    wosd sample(sampvalue, levelnum, indices[levelnum]);
    wavecoefs.PushSampleBack(sample);

    parsecount++;
    if (parsecount == parsenum) {
      break;
    }
  }

  // Fill in meta data for the discrete block
  if (parsecount != 0) {
    wavecoefs.SetLowestLevel(lowlevel);
    wavecoefs.SetNumberLevels(NumberOfLevels(wavecoefs.GetBlockSize())+1);
    wavecoefs.SetTransformType(TRANSFORM);
  }
  return parsecount;
}

bool FlatParser::ParseMRACoefsSample(const SignalSpec &spec,
				     vector<wosd> &acoefs,
				     vector<wosd> &dcoefs,
				     istream &in)
{
  const unsigned MRA_COUNT=2;
  bool read=true;
  unsigned sampletime, numsamples;
  char mratype;
  int levelnum;
  double sampvalue;

  for (unsigned i=0; i<MRA_COUNT; i++) {
    if (in >> sampletime) {
      in >> mratype >> numsamples;
      for (unsigned j=0; j<numsamples; j++) {
	in >> levelnum >> sampvalue;

	if (mratype == 'A') {
	  if (LevelInSpec(spec.approximations, levelnum)) {
	    if (a_indices.find(levelnum) == a_indices.end()) {
	      a_indices[levelnum] = 0;
	    } else {
	      a_indices[levelnum] += 1;
	    }
	    wosd sample(sampvalue, levelnum, indices[levelnum]);
	    acoefs.push_back(sample);
	  }
	} else if (mratype == 'D') {
	  if (LevelInSpec(spec.details, levelnum)) {
	    if (d_indices.find(levelnum) == d_indices.end()) {
	      d_indices[levelnum] = 0;
	    } else {
	      d_indices[levelnum] += 1;
	    }
	    wosd sample(sampvalue, levelnum, indices[levelnum]);
	    dcoefs.push_back(sample);
	  }
	} else {
	  cerr << "Invalid MRA type.\n";
	}
      }
    } else {
      read=false;
      break;
    }
  }
  return read;
}

bool FlatParser::ParseMRACoefsSample(vector<wosd> &acoefs,
				     vector<wosd> &dcoefs,
				     istream &in)
{
  bool read=true;
  unsigned sampletime, numsamples;
  char mratype;
  int levelnum;
  double sampvalue;

  if (in >> sampletime) {
    in >> mratype >> numsamples;
    for (unsigned i=0; i<numsamples; i++) {
      in >> levelnum >> sampvalue;

      if (mratype == 'A') {
	if (a_indices.find(levelnum) == a_indices.end()) {
	  a_indices[levelnum] = 0;
	} else {
	  a_indices[levelnum] += 1;
	}
	wosd sample(sampvalue, levelnum, indices[levelnum]);
	acoefs.push_back(sample);
      } else if (mratype == 'D') {
	if (d_indices.find(levelnum) == d_indices.end()) {
	  d_indices[levelnum] = 0;
	} else {
	  d_indices[levelnum] += 1;
	}
	wosd sample(sampvalue, levelnum, indices[levelnum]);
	dcoefs.push_back(sample);
      } else {
	cerr << "Invalid MRA type.\n";
      }
    }
  } else {
    read=false;
  }
  return read;
}

void FlatParser::ParseMRACoefsBlock(const SignalSpec &spec,
				    vector<WaveletOutputSampleBlock<wosd> > &acoefs,
				    vector<WaveletOutputSampleBlock<wosd> > &dcoefs,
				    istream &in)
{
  unsigned indextime, numsamples;
  char mratype;
  int levelnum;
  double sampvalue;
  while (in >> indextime) {
    in >> mratype >> numsamples;

    for (unsigned i=0; i<numsamples; i++) {
      in >> levelnum >> sampvalue;

      if (mratype == 'A') {
	if (LevelInSpec(spec.approximations, levelnum)) {
	  if (a_indices.find(levelnum) == a_indices.end()) {
	    a_indices[levelnum] = 0;
	  } else {
	    a_indices[levelnum] += 1;
	  }
	  wosd sample(sampvalue, levelnum, a_indices[levelnum]);
	  acoefs[levelnum].PushSampleBack(sample);
	}
      } else if (mratype == 'D') {
	if (LevelInSpec(spec.details, levelnum)) {
	  if (d_indices.find(levelnum) == d_indices.end()) {
	    d_indices[levelnum] = 0;
	  } else {
	    d_indices[levelnum] += 1;
	  }
	  wosd sample(sampvalue, levelnum, d_indices[levelnum]);
	  dcoefs[levelnum].PushSampleBack(sample);
	}
      } else {
	cerr << "Invalid MRA type.\n";
      }
    }
  }
}

unsigned
FlatParser::ParseMRACoefsBlock(const SignalSpec &spec,
			       vector<WaveletOutputSampleBlock<wosd> > &acoefs,
			       vector<WaveletOutputSampleBlock<wosd> > &dcoefs,
			       istream &in,
			       const unsigned parsenum)
{
  unsigned indextime, numsamples;
  char mratype;
  int levelnum;
  double sampvalue;
  unsigned parsecount=0;
  while (in >> indextime) {
    parsecount++;
    in >> mratype;

    if (mratype == 'A') {
      in >> numsamples;
      for (unsigned i=0; i<numsamples; i++) {
	in >> levelnum >> sampvalue;

	if (LevelInSpec(spec.approximations, levelnum)) {
	  if (a_indices.find(levelnum) == a_indices.end()) {
	    a_indices[levelnum] = 0;
	  } else {
	    a_indices[levelnum] += 1;
	  }
	  wosd sample(sampvalue, levelnum, a_indices[levelnum]);
	  acoefs[levelnum].PushSampleBack(sample);
	}
      }
    } else if (mratype == 'D') {
      in >> numsamples;
      for (unsigned i=0; i<numsamples; i++) {
	in >> levelnum >> sampvalue;
	if (LevelInSpec(spec.details, levelnum)) {
	  if (d_indices.find(levelnum) == d_indices.end()) {
	    d_indices[levelnum] = 0;
	  } else {
	    d_indices[levelnum] += 1;
	  }
	  wosd sample(sampvalue, levelnum, d_indices[levelnum]);
	  dcoefs[levelnum].PushSampleBack(sample);
	}
      }

      // Dependency is approx then detail ***always***
      if ((indextime+1) % parsenum == 0) {
	break;
      }
    } else {
      cerr << "Invalid MRA type.\n";
    }
  }
  parsecount = parsecount >> 0x1;
  return parsecount;
}

void FlatParser::ParseMRACoefsBlock(vector<WaveletOutputSampleBlock<wosd> > &acoefs,
				    vector<WaveletOutputSampleBlock<wosd> > &dcoefs,
				    istream &in)
{
  unsigned indextime, numsamples;
  char mratype;
  int levelnum;
  double sampvalue;
  while (in >> indextime) {
    in >> mratype >> numsamples;

    for (unsigned i=0; i<numsamples; i++) {
      in >> levelnum >> sampvalue;
   
      if (mratype == 'A') {
   	if (a_indices.find(levelnum) == a_indices.end()) {
	  a_indices[levelnum] = 0;
	} else {
	  a_indices[levelnum] += 1;
	}
	wosd sample(sampvalue, levelnum, a_indices[levelnum]);
	acoefs[levelnum].PushSampleBack(sample);
      } else if (mratype == 'D') {
	if (d_indices.find(levelnum) == d_indices.end()) {
	  d_indices[levelnum] = 0;
	} else {
	  d_indices[levelnum] += 1;
	}
	wosd sample(sampvalue, levelnum, d_indices[levelnum]);
	dcoefs[levelnum].PushSampleBack(sample);
      } else {
	cerr << "Invalid MRA type.\n";
      }
    }
  }
}

// Private functions
bool FlatParser::LevelInSpec(const vector<int> &mra, const int levelnum)
{
  bool inspec=false;
  for (unsigned i=0; i<mra.size(); i++) {
    if (levelnum==mra[i]) {
      inspec=true;
      break;
    }
  }
  return inspec;
}
