#ifndef _cmdlinefuncs
#define _cmdlinefuncs

#include <cstdlib>
#include <iostream>
#include <fstream>

#if !defined(WIN32) || defined(__CYGWIN__)
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#endif

#include "waveletinfo.h"
#include "transforms.h"

typedef WaveletInputSample<double> wisd;
typedef WaveletOutputSample<double> wosd;

WaveletType GetWaveletType(const char *x, const char *filename);
void ParseSignalSpec(SignalSpec &spec, ifstream &file);
void ParseZeroSpec(vector<int> &spec, ifstream &file);

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

void OutputWaveletCoefs(ostream &os,
			const DiscreteWaveletOutputSampleBlock<wosd> &dwosb,
			const TransformType tt,
			const bool flat);

unsigned OutputWaveletCoefs(ostream &os,
			    vector<WaveletOutputSampleBlock<wosd> > &levels,
			    const TransformType tt,
			    const unsigned start_index);

void OutputMRACoefs(ostream &os,
		    vector<vector<wosd> > &approxlevels,
		    vector<vector<wosd> > &detaillevels);

void OutputMRACoefs(ostream &os,
		    vector<WaveletOutputSampleBlock<wosd> > &approx,
		    vector<WaveletOutputSampleBlock<wosd> > &detail);

unsigned OutputMRACoefs(ostream &os,
			vector<WaveletOutputSampleBlock<wosd> > &approx,
			vector<WaveletOutputSampleBlock<wosd> > &detail,
			const unsigned index);

void OutputLevelMetaData(ostream &os,
			 vector<vector<wosd> > &levels,
			 const unsigned numlevels);

void OutputLevelMetaData(ostream &os,
			 vector<WaveletOutputSampleBlock<wosd> > &levels,
			 const unsigned numlevels);

void OutputLevelMetaData(ostream &os,
			 const unsigned *levelsize,
			 const unsigned levelcnt);

void OutputLevelMetaData(ostream &os,
			 const DiscreteWaveletOutputSampleBlock<wosd> &dwosb,
			 const TransformType tt);

// Performance routines
void GetRusage(double &systime, double &usrtime);

#if defined(WIN32) && !defined(__CYGWIN__)
struct timeval {
	int tv_sec;
	int tv_usec;
};
#endif

double GetTimeDuration(const timeval &stime, const timeval &etime);

void GetNextBlock(vector<WaveletOutputSampleBlock<wosd> > &outblock,
		  const unsigned blocknumber,
		  const vector<WaveletOutputSampleBlock<wosd> > &inblock,
		  const int approxlevel,
		  const unsigned blocksize);

#endif
