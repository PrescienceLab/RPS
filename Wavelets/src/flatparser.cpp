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


bool FlatParser::ParseWaveletCoefsSample(vector<wosd> &wavecoefs, istream &in)
{
  if (in.eof()) {
    return false;
  }

  unsigned sampletime, numsamples;
  int levelnum;
  double sampvalue;
  in >> sampletime >> numsamples;
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
  return true;
}


void FlatParser::
ParseWaveletCoefsBlock(vector<WaveletOutputSampleBlock<wosd> > &wavecoefs,
		       istream &in)
{
  unsigned indextime, numsamples;
  int levelnum;
  double sampvalue;
  while(!in.eof()) {
    in >> indextime >> numsamples;
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

bool FlatParser::ParseMRACoefsSample(const SignalSpec &spec,
				     vector<wosd> &acoefs,
				     vector<wosd> &dcoefs,
				     istream &in)
{
  //  if (in.eof()) {
  //    return false;
  //  }
  
  bool moredata=true;
  unsigned sampletime, numsamples;
  char mratype;
  int levelnum;
  double sampvalue;

  in >> sampletime >> mratype >> numsamples;
  for (unsigned i=0; i<numsamples; i++) {
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
      moredata=false;
    }
  }
  return moredata;
}


bool FlatParser::ParseMRACoefsSample(vector<wosd> &acoefs,
				     vector<wosd> &dcoefs,
				     istream &in)
{
  if (in.eof()) {
    return false;
  }
   
  unsigned sampletime, numsamples;
  char mratype;
  int levelnum;
  double sampvalue;

  in >> sampletime >> mratype >> numsamples;
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
  return true;
}

void FlatParser::ParseMRACoefsBlock(vector<WaveletOutputSampleBlock<wosd> > &acoefs,
				    vector<WaveletOutputSampleBlock<wosd> > &dcoefs,
				    istream &in)
{
  unsigned indextime, numsamples;
  char mratype;
  int levelnum;
  double sampvalue;
  while (!in.eof()) {
    in >> indextime >> mratype >> numsamples;

    if (mratype == 'A') {
      for (unsigned i=0; i<numsamples; i++) {
	in >> levelnum >> sampvalue;
	if (a_indices.find(levelnum) == a_indices.end()) {
	  a_indices[levelnum] = 0;
	} else {
	  a_indices[levelnum] += 1;
	}
	wosd sample(sampvalue, levelnum, a_indices[levelnum]);
	acoefs[levelnum].PushSampleBack(sample);
      }
    } else if (mratype == 'D') {
      for (unsigned i=0; i<numsamples; i++) {
	in >> levelnum >> sampvalue;
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
