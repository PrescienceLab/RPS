#ifndef _flatparser
#define _flatparser

#include <vector>
#include <deque>
#include <iostream>
#include <fstream>
#include <map>

#include "waveletsample.h"
#include "waveletsampleblock.h"
#include "transforms.h"

class FlatParser {
private:
  // Used for keeping track of wavelet coef indices
  map<int, unsigned, less<int> > indices;

  // Used for keeping track of MRA indices
  map<int, unsigned, less<int> > a_indices;
  map<int, unsigned, less<int> > d_indices;

  bool LevelInSpec(const vector<int> &mra, const int levelnum);

public:
  typedef WaveletInputSample<double> wisd;
  typedef WaveletOutputSample<double> wosd;

  FlatParser();
  virtual ~FlatParser();

  void ParseTimeDomain(vector<wisd> &samples, istream &in);
  void ParseTimeDomain(deque<wisd> &samples, istream &in);
  unsigned ParseTimeDomain(deque<wisd> &samples,
		       istream &in,
		       unsigned &index,
		       const unsigned parsenum);

  bool ParseWaveletCoefsSample(vector<wosd> &wavecoefs, istream &in);
  void ParseWaveletCoefsBlock(vector<WaveletOutputSampleBlock<wosd> > &wavecoefs,
			      istream &in);
  void ParseWaveletCoefsBlock(DiscreteWaveletOutputSampleBlock<wosd> &wavecoefs,
			      istream &in);

  unsigned ParseWaveletCoefsBlock(vector<WaveletOutputSampleBlock<wosd> > &wavecoefs,
				  istream &in,
				  const unsigned parsenum);

  unsigned ParseWaveletCoefsBlock(DiscreteWaveletOutputSampleBlock<wosd> &wavecoefs,
			      istream &in,
			      const unsigned parsenum);

  bool ParseMRACoefsSample(const SignalSpec &spec,
			   vector<wosd> &acoefs,
			   vector<wosd> &dcoefs,
			   istream &in);

  bool ParseMRACoefsSample(vector<wosd> &acoefs, vector<wosd> &dcoefs, istream &in);

  void ParseMRACoefsBlock(const SignalSpec &spec,
			  vector<WaveletOutputSampleBlock<wosd> > &acoefs,
			  vector<WaveletOutputSampleBlock<wosd> > &dcoefs,
			  istream &in);

  unsigned ParseMRACoefsBlock(const SignalSpec &spec,
			      vector<WaveletOutputSampleBlock<wosd> > &acoefs,
			      vector<WaveletOutputSampleBlock<wosd> > &dcoefs,
			      istream &in,
			      const unsigned parsenum);

  void ParseMRACoefsBlock(vector<WaveletOutputSampleBlock<wosd> > &acoefs,
			  vector<WaveletOutputSampleBlock<wosd> > &dcoefs,
			  istream &in);
};

#endif
