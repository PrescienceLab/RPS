#ifndef _waveletinfo
#define _waveletinfo

// Increment as a new wavelet type is added, also update coefTable and 
//  numCoefTable with the coefficients and number of coefficients
const int NUM_WAVELET_TYPES=10;

enum WaveletType {DAUB2,
		  DAUB4,
		  DAUB6,
		  DAUB8,
		  DAUB10,
		  DAUB12,
		  DAUB14,
		  DAUB16,
		  DAUB18,
		  DAUB20};

const unsigned MAX_STAGES=20;

#endif
