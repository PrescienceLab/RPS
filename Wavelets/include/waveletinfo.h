#ifndef _waveletinfo
#define _waveletinfo

// Increment as a new wavelet type is added, also update coefTable and 
//  numCoefTable with the coefficients and number of coefficients
const int NUM_WAVELET_TYPES=12;

enum WaveletType {DAUB2,
		  DAUB4,
		  DAUB6,
		  DAUB8,
		  DAUB10,
		  DAUB12,
		  DAUB14,
		  DAUB16,
		  DAUB18,
		  DAUB20,
                  FILTERBANK_NBS_G8_D1,
                  FILTERBANK_NBS_H8_D1};

const unsigned numberOfCoefs[NUM_WAVELET_TYPES] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 8, 8};

const unsigned MAX_STAGES=20;

#endif
