#ifndef _cmdlinefuncs
#define _cmdlinefuncs

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "waveletinfo.h"
#include "transforms.h"

typedef WaveletInputSample<double> wisd;
typedef WaveletOutputSample<double> wosd;

WaveletType GetWaveletType(const char *x, const char *filename);
void ParseSignalSpec(SignalSpec &spec, ifstream &file);

void OutputWaveletCoefsNonFlat(ostream &os,
			       const vector<deque<wosd> *> &levels,
			       const unsigned numlevels);
void OutputWaveletCoefsNonFlat(ostream &os,
			       vector<WaveletOutputSampleBlock<wosd> > &fwdout,
			       const unsigned numlevels);



void OutputWaveletCoefsFlat(ostream &os,
			    vector<WaveletOutputSampleBlock<wosd> > &fwdout,
			    const unsigned numlevels);


void OutputMRACoefsFlat(ostream &os,
			vector<WaveletOutputSampleBlock<wosd> > &mraout,
			const char &ch,
			const unsigned numlevels);

//------------------------------------------------------------------------------

void OutputWaveletCoefs(ostream &os, vector<vector<wosd> > &levels);

void OutputWaveletCoefs(ostream &os,
			vector<WaveletOutputSampleBlock<wosd> > &levels,
			const TransformType tt);

void OutputMRACoefs(ostream &os,
		    vector<vector<wosd> > &approxlevels,
		    vector<vector<wosd> > &detaillevels);

void OutputMRACoefs(ostream &os,
		    vector<WaveletOutputSampleBlock<wosd> > &approx,
		    vector<WaveletOutputSampleBlock<wosd> > &detail);

void OutputLevelMetaData(ostream &os,
			 vector<vector<wosd> > &levels,
			 const unsigned numlevels);

void OutputLevelMetaData(ostream &os,
			 vector<WaveletOutputSampleBlock<wosd> > &levels,
			 const unsigned numlevels);
#endif
