#ifndef _flatparser
#define _flatparser

#include <vector>
#include <deque>
#include <iostream>
#include <fstream>
#include <map>

#include "waveletsample.h"
#include "waveletsampleblock.h"

//typedef WaveletInputSample<double> wisd;
//typedef WaveletOutputSample<double> wosd;

class FlatParser {
private:
  // Used for keeping track of wavelet coef indices
  map<int, unsigned, less<int> > indices;

  // Used for keeping track of MRA indices
  map<int, unsigned, less<int> > a_indices;
  map<int, unsigned, less<int> > d_indices;
public:
  typedef WaveletInputSample<double> wisd;
  typedef WaveletOutputSample<double> wosd;

  FlatParser();
  virtual ~FlatParser();

  void ParseTimeDomain(vector<wisd> &samples, istream &in);
  void ParseTimeDomain(deque<wisd> &samples, istream &in);

  bool ParseWaveletCoefsSample(vector<wosd> &wavecoefs, istream &in);
  void ParseWaveletCoefsBlock(vector<WaveletOutputSampleBlock<wosd> > &wavecoefs,
			      istream &in);

  bool ParseMRACoefsSample(vector<wosd> &acoefs, vector<wosd> &dcoefs, istream &in);
  void ParseMRACoefsBlock(vector<WaveletOutputSampleBlock<wosd> > &acoefs,
			  vector<WaveletOutputSampleBlock<wosd> > &dcoefs,
			  istream &in);
};

#endif
