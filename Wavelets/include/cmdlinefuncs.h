#ifndef _cmdlinefuncs
#define _cmdlinefuncs

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "waveletinfo.h"
#include "transforms.h"

WaveletType GetWaveletType(const char *x, const char *filename);
void ParseSignalSpec(SignalSpec &spec, ifstream &file);

#endif
